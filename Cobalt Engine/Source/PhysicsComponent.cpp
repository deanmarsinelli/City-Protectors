/*
	PhysicsComponent.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "BaseGameLogic.h"
#include "GameObject.h"
#include "Logger.h"
#include "MathUtils.h"
#include "Matrix.h"
#include "PhysicsComponent.h"
#include "Vector.h"

// units per second
const float DEFAULT_MAX_VELOCITY = 7.5f;
const float DEFAULT_MAX_ANGULAR_VELOCITY = 1.2f;

const char* PhysicsComponent::g_Name = "PhysicsComponent";

PhysicsComponent::PhysicsComponent()
{
	// zero out all member variables
	m_RigidBodyLocation = Vec3(0.0f, 0.0f, 0.0f);
	m_RigidBodyOrientation = Vec3(0.0f, 0.0f, 0.0f);
	m_RigidBodyScale = Vec3(0.0f, 0.0f, 0.0f);
	m_Acceleration = 0.0f;
	m_AngularAcceleration = 0.0f;
	m_MaxVelocity = DEFAULT_MAX_VELOCITY;
	m_MaxAngularVelocity = DEFAULT_MAX_ANGULAR_VELOCITY;
}

PhysicsComponent::~PhysicsComponent()
{
	m_pGamePhysics->RemoveGameObject(m_pOwner->GetId());
}

TiXmlElement* PhysicsComponent::GenerateXml()
{
	// serialize this component to xml

	TiXmlElement* pBaseElement = CB_NEW TiXmlElement(GetName());

	// shape
	TiXmlElement* pShape = CB_NEW TiXmlElement("Shape");
	TiXmlText* pShapeText = CB_NEW TiXmlText(m_Shape.c_str());
	pShape->LinkEndChild(pShapeText);
	pBaseElement->LinkEndChild(pShape);

	// density
	TiXmlElement* pDensity = CB_NEW TiXmlElement("Density");
	TiXmlText* pDensityText = CB_NEW TiXmlText(m_Density.c_str());
	pDensity->LinkEndChild(pDensityText);
	pBaseElement->LinkEndChild(pDensity);

	// material
	TiXmlElement* pMaterial = CB_NEW TiXmlElement("Material");
	TiXmlText* pMaterialText = CB_NEW TiXmlText(m_Material.c_str());
	pMaterial->LinkEndChild(pMaterialText);
	pBaseElement->LinkEndChild(pMaterial);

	// rigid body transform
	TiXmlElement* pInitialTransform = CB_NEW TiXmlElement("RigidBodyTransform");

	// initial transform -> position
	TiXmlElement* pPosition = CB_NEW TiXmlElement("Position");
	pPosition->SetAttribute("x", ToStr(m_RigidBodyLocation.x).c_str());
	pPosition->SetAttribute("y", ToStr(m_RigidBodyLocation.y).c_str());
	pPosition->SetAttribute("z", ToStr(m_RigidBodyLocation.z).c_str());
	pInitialTransform->LinkEndChild(pPosition);

	// initial transform -> orientation
	TiXmlElement* pOrientation = CB_NEW TiXmlElement("Orientation");
	pOrientation->SetAttribute("yaw", ToStr(m_RigidBodyOrientation.x).c_str());
	pOrientation->SetAttribute("pitch", ToStr(m_RigidBodyOrientation.y).c_str());
	pOrientation->SetAttribute("roll", ToStr(m_RigidBodyOrientation.z).c_str());
	pInitialTransform->LinkEndChild(pOrientation);

	// initial transform -> scale 
	TiXmlElement* pScale = CB_NEW TiXmlElement("Scale");
	pScale->SetAttribute("x", ToStr(m_RigidBodyScale.x).c_str());
	pScale->SetAttribute("y", ToStr(m_RigidBodyScale.y).c_str());
	pScale->SetAttribute("z", ToStr(m_RigidBodyScale.z).c_str());
	pInitialTransform->LinkEndChild(pScale);

	pBaseElement->LinkEndChild(pInitialTransform);

	return pBaseElement;
}

bool PhysicsComponent::Init(TiXmlElement* pData)
{
	// make sure there is a physics world ready
	m_pGamePhysics = g_pApp->m_pGame->GetGamePhysics();
	if (!m_pGamePhysics)
		return false;

	// shape
	TiXmlElement* pShape = pData->FirstChildElement("Shape");
	if (pShape)
	{
		m_Shape = pShape->FirstChild()->Value();
	}

	// density
	TiXmlElement* pDensity = pData->FirstChildElement("Density");
	if (pDensity)
		m_Density = pDensity->FirstChild()->Value();

	// material
	TiXmlElement* pMaterial = pData->FirstChildElement("PhysicsMaterial");
	if (pMaterial)
		m_Material = pMaterial->FirstChild()->Value();

	// initial transform
	TiXmlElement* pRigidBodyTransform = pData->FirstChildElement("RigidBodyTransform");
	if (pRigidBodyTransform)
		BuildRigidBodyTransform(pRigidBodyTransform);

	return true;
}

void PhysicsComponent::PostInit()
{
	if (m_pOwner)
	{
		if (m_Shape == "Sphere")
		{
			m_pGamePhysics->AddSphere((float)m_RigidBodyScale.x, m_pOwner, m_Density, m_Material);
		}
		else if (m_Shape == "Box")
		{
			m_pGamePhysics->AddBox(m_RigidBodyScale, m_pOwner, m_Density, m_Material);
		}
		else if (m_Shape == "PointCloud")
		{
			CB_ERROR("Unsupported for now");
		}
	}
}

void PhysicsComponent::Update(float deltaTime)
{
	Mat4x4 transform = m_pOwner->transform.GetTransform();

	// if there is acceleration, move the object
	if (m_Acceleration != 0)
	{
		// calculate acceleration for this frame
		float acceleration = m_Acceleration * deltaTime;

		// convert velocity to a scalar speed
		Vec3 velocity(m_pGamePhysics->GetVelocity(m_pOwner->GetId()));
		float speed = velocity.Length();

		Vec3 direction(transform.GetDirection());
		m_pGamePhysics->ApplyForce(direction, acceleration, m_pOwner->GetId());
	}

	// apply angular acceleration if there is any
	if (m_AngularAcceleration != 0)
	{
		float angularAcceleration = m_AngularAcceleration * deltaTime;
		m_pGamePhysics->ApplyTorque(transform.GetUp(), angularAcceleration, m_pOwner->GetId());
	}
}

void PhysicsComponent::ApplyForce(const Vec3& direction, float forceNewtons)
{
	m_pGamePhysics->ApplyForce(direction, forceNewtons, m_pOwner->GetId());
}

void PhysicsComponent::ApplyTorque(const Vec3& direction, float forceNewtons)
{
	m_pGamePhysics->ApplyTorque(direction, forceNewtons, m_pOwner->GetId());
}

bool PhysicsComponent::KinematicMove(const Mat4x4& transform)
{
	return m_pGamePhysics->KinematicMove(transform, m_pOwner->GetId());
}

void PhysicsComponent::ApplyAcceleration(float acceleration)
{
	m_Acceleration = acceleration;
}

void PhysicsComponent::RemoveAcceleration()
{
	m_Acceleration = 0.0f;
}

void PhysicsComponent::ApplyAngularAcceleration(float acceleration)
{
	m_AngularAcceleration = acceleration;
}

void PhysicsComponent::RemoveAngularAcceleration()
{
	m_AngularAcceleration = 0.0f;
}

Vec3 PhysicsComponent::GetVelocity()
{
	return m_pGamePhysics->GetVelocity(m_pOwner->GetId());
}

void PhysicsComponent::SetVelocity(const Vec3& velocity)
{
	m_pGamePhysics->SetVelocity(m_pOwner->GetId(), velocity);
}

void PhysicsComponent::RotateY(float angleRadians)
{
	// rotate the physics body around the y axis
	Mat4x4 transform = m_pOwner->transform.GetTransform();
	Vec3 position = transform.GetPosition();

	Mat4x4 rotateY;
	rotateY.BuildRotationY(angleRadians);
	rotateY.SetPosition(position);

	KinematicMove(rotateY);
}

void PhysicsComponent::SetPosition(float x, float y, float z)
{
	// manually set the physics body
	Mat4x4 transform = m_pOwner->transform.GetTransform();
	Vec3 position = Vec3(x, y, z);
	transform.SetPosition(position);

	KinematicMove(transform);
}

void PhysicsComponent::Stop()
{
	return m_pGamePhysics->StopGameObject(m_pOwner->GetId());
}

void PhysicsComponent::BuildRigidBodyTransform(TiXmlElement* pTransformElement)
{
	CB_ASSERT(pTransformElement);

	TiXmlElement* pPositionElement = pTransformElement->FirstChildElement("Position");
	if (pPositionElement)
	{
		double x = 0.0;
		double y = 0.0;
		double z = 0.0;
		pPositionElement->Attribute("x", &x);
		pPositionElement->Attribute("y", &y);
		pPositionElement->Attribute("z", &z);
		m_RigidBodyLocation = Vec3(x, y, z);
	}

	TiXmlElement* pOrientationElement = pTransformElement->FirstChildElement("Orientation");
	if (pOrientationElement)
	{
		double yaw = 0.0;
		double pitch = 0.0;
		double roll = 0.0;
		pPositionElement->Attribute("yaw", &yaw);
		pPositionElement->Attribute("pitch", &pitch);
		pPositionElement->Attribute("roll", &roll);
		m_RigidBodyOrientation = Vec3((float)DEGREES_TO_RADIANS(yaw), (float)DEGREES_TO_RADIANS(pitch), (float)DEGREES_TO_RADIANS(roll));
	}

	TiXmlElement* pScaleElement = pTransformElement->FirstChildElement("Scale");
	if (pScaleElement)
	{
		double x = 0;
		double y = 0;
		double z = 0;
		pScaleElement->Attribute("x", &x);
		pScaleElement->Attribute("y", &y);
		pScaleElement->Attribute("z", &z);
		m_RigidBodyScale = Vec3((float)x, (float)y, (float)z);
	}
}
