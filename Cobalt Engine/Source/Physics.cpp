/*
	Physics.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <algorithm>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include "BaseGameLogic.h"
#include "EventManager.h"
#include "Events.h"
#include "GameObject.h"
#include "Logger.h"
#include "Matrix.h"
#include "Physics.h"
#include "PhysicsDebugDrawer.h"
#include "PhysicsEvents.h"
#include "XmlResource.h"

/**
	Physics material properties.
*/
struct MaterialData
{
	float m_Restitution;
	float m_Friction;

	MaterialData(float restitution, float friction) :
		m_Restitution(restitution),
		m_Friction(friction)
	{ }

	MaterialData(const MaterialData& other)
	{
		m_Restitution = other.m_Restitution;
		m_Friction = other.m_Friction;
	}
};


/**
	Helper function to convert a Vec3 to a btVector3
*/
static btVector3 Vec3_to_btVector3(const Vec3& vec3)
{
	return btVector3(vec3.x, vec3.y, vec3.z);
}


/**
	Helper function to convert a btVector3 to a Vec3.
*/
static Vec3 btVector3_to_Vec3(const btVector3& btvec)
{
	return Vec3(btvec.x(), btvec.y(), btvec.z());
}


/**
	Helper function to convert a Mat4x4 to a btTransform.
*/
static btTransform Mat4x4_to_btTransform(const Mat4x4& mat)
{
	btMatrix3x3 bulletRotation;
	btVector3 bulletPosition;

	// copy rotation matrix
	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 3; column++)
		{
			// opposite because Mat4x4's are row major matrices
			bulletRotation[row][column] = mat.m[column][row];
		}
	}
	
	// copy position
	for (int column = 0; column < 3; column++)
	{
		bulletPosition[column] = mat.m[3][column];
	}

	return btTransform(bulletRotation, bulletPosition);
}


/**
	Helper function to convert a btTransform to a Mat4x4.
*/
static Mat4x4 btTransform_to_Mat4x4(const btTransform& trans)
{
	Mat4x4 returnMatrix = Mat4x4::Identity;

	const btMatrix3x3& bulletRotation = trans.getBasis();
	const btVector3& bulletPosition = trans.getOrigin();

	// copy rotation matrix
	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 3; column++)
		{
			returnMatrix.m[row][column] = bulletRotation[column][row];
		}
	}

	// copy position
	for (int column = 0; column < 3; column++)
	{
		returnMatrix.m[3][column] = bulletPosition[column];
	}

	return returnMatrix;
}

/**
	Used to send position and orientation changes from the physics
	world back to the game (rendering). This assumes the objects
	center of mass and world position are the same point.
*/
struct ObjectMotionState : public btMotionState
{
	ObjectMotionState(const Mat4x4& startingTransform) :
		m_WorldToPositionTransform(startingTransform)
	{ }

	// called by bullet sdk
	virtual void getWorldTransform(btTransform& worldTrans) const
	{
		worldTrans = Mat4x4_to_btTransform(m_WorldToPositionTransform);
	}

	virtual void setWorldTransform(const btTransform& worldTrans)
	{
		m_WorldToPositionTransform = btTransform_to_Mat4x4(worldTrans);
	}

	Mat4x4 m_WorldToPositionTransform;
};


/**
	Private implementation of the IGamePhysics interface. This class
	uses the 3rd Party BulletPhysics SDK.
*/
class BulletPhysics : public IGamePhysics
{
	// typedefs
	typedef std::unordered_map<std::string, float> DensityTable;
	typedef std::unordered_map<std::string, MaterialData> MaterialTable;
	typedef std::unordered_map<GameObjectId, btRigidBody*> ObjectIDToRigidBodyMap;
	typedef std::unordered_map<const btRigidBody*, GameObjectId> RigidBodyToObjectIDMap;
	typedef std::pair<const btRigidBody*, const btRigidBody*> CollisionPair;
	typedef std::set<CollisionPair> CollisionPairs;

public:
	BulletPhysics();
	virtual ~BulletPhysics();

	// interface methods
	// init and maintenance of the physics world
	virtual bool Initialize() override;
	virtual void SyncVisibleScene() override;
	virtual void OnUpdate(float deltaTime) override;

	// initialize physics objects
	virtual void AddSphere(float radius, WeakGameObjectPtr pGameObject, const std::string& densityStr, const std::string& physicsMaterial) override;
	virtual void AddBox(const Vec3& dimensions, WeakGameObjectPtr pGameObject, const std::string& densityStr, const std::string& physicsMaterial) override;
	virtual void AddPointCloud(Vec3* verts, int numPoints, WeakGameObjectPtr pGameObject, const std::string& densityStr, const std::string& physicsMaterial) override;
	virtual void RemoveGameObject(GameObjectId id) override;

	// debugging
	virtual void RenderDiagnostics() override;

	// physics world modifiers
	virtual void CreateTrigger(WeakGameObjectPtr pGameObject, const Vec3& position, const float dim);
	virtual void ApplyForce(const Vec3& dir, float newtons, GameObjectId id);
	virtual void ApplyTorque(const Vec3& dir, float newtons, GameObjectId id);
	virtual bool KinematicMove(const Mat4x4& mat, GameObjectId id);

	virtual void RotateY(GameObjectId id, float angleRadians, float time);
	virtual float GetOrientationY(GameObjectId id);
	virtual void StopGameObject(GameObjectId id);
	virtual Vec3 GetVelocity(GameObjectId id);
	virtual void SetVelocity(GameObjectId id, const Vec3& vel);
	virtual Vec3 GetAngularVelocity(GameObjectId id);
	virtual void SetAngularVelocity(GameObjectId id, const Vec3& vel);
	virtual void Translate(GameObjectId id, const Vec3& vec);
	virtual void SetTransform(const GameObjectId id, const Mat4x4& mat);
	virtual Mat4x4 GetTransform(const GameObjectId id);

private:
	void LoadXml();
	float LookupSpecificGravity(const std::string& densityStr);
	MaterialData LookupMaterialData(const std::string& materialStr);
	btRigidBody* FindBulletRigidBody(GameObjectId id) const;
	GameObjectId FindObjectId(const btRigidBody* body) const;

	// helpers for sending events relating to collision pairs
	void SendCollisionPairAddEvent(const btPersistentManifold* manifold, const btRigidBody* body0, const btRigidBody* body1);
	void SendCollisionPairRemoveEvent(const btRigidBody* body0, const btRigidBody* body1);

	// add a shape to the physics world
	void AddShape(StrongGameObjectPtr pGameObject, btCollisionShape* shape, float mass, const std::string& physicsMaterial);

	void RemoveCollisionObject(btCollisionObject* obj);

	// callback from bullet sdk for each physics setp
	static void BulletInternalTickCallback(btDynamicsWorld* world, btScalar timeStep);

private:
	// objects used by Bullet SDK
	unique_ptr<btDynamicsWorld>			m_DynamicsWorld;
	unique_ptr<btBroadphaseInterface>	m_Broadphase;
	unique_ptr<btCollisionDispatcher>	m_Dispatcher;
	unique_ptr<btConstraintSolver>		m_Solver;
	unique_ptr<btDefaultCollisionConfiguration> m_CollisionConfiguration;
	unique_ptr<BulletDebugDrawer> m_DebugDrawer;

	// tables read from the XML
	DensityTable m_DensityTable;
	MaterialTable m_MaterialTable;

	// keep track of rigid bodies
	ObjectIDToRigidBodyMap m_ObjectIdToRigidBody;
	RigidBodyToObjectIDMap m_RigidBodyToObjectId;
	
	// store pairs of bodies that are colliding. when bodies first collide they
	// are stored here and an event is sent. when they stop colliding, they are removed
	// and another event is sent
	CollisionPairs m_PreviousTickCollisionPairs;
};


BulletPhysics::BulletPhysics()
{
	// register physics events in the event factory
	REGISTER_EVENT(Event_PhysTriggerEnter);
	REGISTER_EVENT(Event_PhysTriggerLeave);
	REGISTER_EVENT(Event_PhysCollision);
	REGISTER_EVENT(Event_PhysSeparation);
}


BulletPhysics::~BulletPhysics()
{
	// delete any physics objects still in the world
	// iterate backwards so the vector doesnt have to move objects
	for (int i = m_DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = m_DynamicsWorld->getCollisionObjectArray[i];
		RemoveCollisionObject(obj);
	}

	m_RigidBodyToObjectId.clear();
	
	// physics objects are smart ptr's and will clean themselves up
}


bool BulletPhysics::Initialize()
{
	LoadXml();

	// this object controls bullet SDK's internal memory management during collision pass
	m_CollisionConfiguration.reset(CB_NEW btDefaultCollisionConfiguration);

	// this manages how bullet detects precise collisions between pairs of objects
	m_Dispatcher.reset(CB_NEW btCollisionDispatcher(m_CollisionConfiguration.get()));

	// used to quickly detect collisions between object, imprecisely. 
	// if a collision passes this phase it will be passed to the dispatcher
	m_Broadphase.reset(CB_NEW btDbvtBroadphase);

	m_Solver.reset(CB_NEW btSequentialImpulseConstraintSolver);

	// this is the main interface point for the physics world
	m_DynamicsWorld.reset(CB_NEW btDiscreteDynamicsWorld(m_Dispatcher.get(), 
		m_Broadphase.get(), m_Solver.get(), m_CollisionConfiguration.get()));

	m_DebugDrawer.reset(CB_NEW BulletDebugDrawer);
	m_DebugDrawer->SetOptions(btIDebugDraw::DebugDrawModes::DBG_MAX_DEBUG_DRAW_MODE);

	if (!m_CollisionConfiguration || !m_Dispatcher || !m_Broadphase || !m_Solver
		|| !m_DynamicsWorld /*|| !m_DebugDrawer*/)
	{
		CB_ERROR("BulletPhysics::Initialize failed");
		return false;
	}

	m_DynamicsWorld->setDebugDrawer(m_DebugDrawer.get());

	// set internal tick callback to our own method
	m_DynamicsWorld->setInternalTickCallback(BulletPhysics::BulletInternalTickCallback);
	m_DynamicsWorld->setWorldUserInfo(this);

	return true;
}


void BulletPhysics::SyncVisibleScene()
{
	// keep graphics and physics in sync

	// check all existing object bodies for changes
	// if there is a change, send appropriate event to game system
	for (auto it = m_ObjectIdToRigidBody.begin(); it != m_ObjectIdToRigidBody.end(); ++it)
	{
		const GameObjectId id = it->first;

		// get the motion state that is updated by bullet sdk
		const ObjectMotionState* objMotionState = static_cast<ObjectMotionState*>(it->second->getMotionState());
		CB_ASSERT(objMotionState);

		StrongGameObjectPtr pGameObject = MakeStrongPtr(g_pApp->m_pGame->GetGameObject(id));
		if (pGameObject && objMotionState)
		{
			// if the objects world position and physics position are different, update
			if (pGameObject->transform.GetTransform() != objMotionState->m_WorldToPositionTransform)
			{
				// sync the transform and dispatch an event that an object has moved
				pGameObject->transform.SetTransform(objMotionState->m_WorldToPositionTransform);
				shared_ptr<Event_MoveGameObject> pEvent(CB_NEW Event_MoveGameObject(id, objMotionState->m_WorldToPositionTransform));
				IEventManager::Get()->QueueEvent(pEvent);
			}
		}
	}
}


void BulletPhysics::OnUpdate(float deltaTime)
{
	// step the physics sim with a max of 4 sub steps
	m_DynamicsWorld->stepSimulation(deltaTime, 4);
}


void BulletPhysics::AddSphere(float radius, WeakGameObjectPtr pGameObject, const std::string& densityStr, const std::string& physicsMaterial)
{
	StrongGameObjectPtr pStrongObject = MakeStrongPtr(pGameObject);
	if (!pStrongObject)
	{
		CB_ERROR("Must attach a game object to the sphere");
		return;
	}

	// create a sphere collider
	btSphereShape* sphereShape = new btSphereShape(radius);

	// calculate mass using specific gravity (density)
	float specificGravity = LookupSpecificGravity(densityStr);
	const float volume = (4.0f / 3.0f) * CB_PI * radius * radius * radius;
	const btScalar mass = volume * specificGravity;

	AddShape(pStrongObject, sphereShape, mass, physicsMaterial);
}


void BulletPhysics::AddBox(const Vec3& dimensions, WeakGameObjectPtr pGameObject, const std::string& densityStr, const std::string& physicsMaterial)
{
	StrongGameObjectPtr pStrongObject = MakeStrongPtr(pGameObject);
	if (!pStrongObject)
	{
		CB_ERROR("Must attach a game object to the box");
		return;
	}

	// create box collider
	btBoxShape* boxShape = new btBoxShape(Vec3_to_btVector3(dimensions));

	// calculate mass using specific gravity (density)
	float specificGravity = LookupSpecificGravity(densityStr);
	const float volume = dimensions.x * dimensions.y * dimensions.z;
	const btScalar mass = volume * specificGravity;

	AddShape(pStrongObject, boxShape, mass, physicsMaterial);
}


void BulletPhysics::AddPointCloud(Vec3* verts, int numPoints, WeakGameObjectPtr pGameObject, const std::string& densityStr, const std::string& physicsMaterial)
{
	// used to create a convex mesh shape
	StrongGameObjectPtr pStrongObject = MakeStrongPtr(pGameObject);
	if (!pStrongObject)
	{
		CB_ERROR("Must attach a game object to the point cloud");
		return;
	}

	btConvexHullShape* shape = new btConvexHullShape();
	
	// add points to the shape one at a time
	for (int i = 0; i < numPoints; i++)
	{
		shape->addPoint(Vec3_to_btVector3(verts[i]));
	}

	// approximate mass using bounding box
	btVector3 aabbMin(0, 0, 0), aabbMax(0, 0, 0);
	shape->getAabb(btTransform::getIdentity(), aabbMin, aabbMax);

	const btVector3 aabbExtents = aabbMax - aabbMin;
	float specificGravity = LookupSpecificGravity(densityStr);
	const float volume = aabbExtents.x() * aabbExtents.y() * aabbExtents.z();
	const btScalar mass = volume * specificGravity;

	AddShape(pStrongObject, shape, mass, physicsMaterial);
}


void BulletPhysics::RemoveGameObject(GameObjectId id)
{
	// remove an object from the physics sim
	btRigidBody* body = FindBulletRigidBody(id);
	if (body)
	{
		RemoveCollisionObject(body);
		m_ObjectIdToRigidBody.erase(id);
		m_RigidBodyToObjectId.erase(body);
	}
}


void BulletPhysics::RenderDiagnostics()
{
	m_DynamicsWorld->debugDrawWorld();
}


void BulletPhysics::CreateTrigger(WeakGameObjectPtr pGameObject, const Vec3& position, const float dim)
{
	StrongGameObjectPtr pStrongObject = MakeStrongPtr(pGameObject);
	if (!pStrongObject)
	{
		CB_ERROR("Must attach a game object to the trigger");
		return;
	}

	// create the collision body
	btBoxShape* boxShape = new btBoxShape(Vec3_to_btVector3(Vec3(dim, dim, dim)));

	// 0 mass, this trigger is not moveable
	const btScalar mass = 0;

	// set the initial position of the trigger from the object
	Mat4x4 triggerTransform = Mat4x4::Identity;
	triggerTransform.SetPosition(position);
	ObjectMotionState* motionState = CB_NEW ObjectMotionState(triggerTransform);

	// create the rigid body
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, boxShape, btVector3(0, 0, 0));
	btRigidBody* body = new btRigidBody(rbInfo);

	// add the body to the world
	m_DynamicsWorld->addRigidBody(body);

	// mark the body as a trigger so it does not physically collide with object
	body->setCollisionFlags(body->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);
	
	// update the maps
	m_ObjectIdToRigidBody[pStrongObject->GetId()] = body;
	m_RigidBodyToObjectId[body] = pStrongObject->GetId();
}


void BulletPhysics::ApplyForce(const Vec3& dir, float newtons, GameObjectId id)
{
	btRigidBody* body = FindBulletRigidBody(id);
	if (body)
	{
		body->setActivationState(DISABLE_DEACTIVATION);
		btVector3 force(dir.x * newtons, dir.y * newtons, dir.z * newtons);

		body->applyCentralImpulse(force);
	}
}


void BulletPhysics::ApplyTorque(const Vec3& dir, float newtons, GameObjectId id)
{
	btRigidBody* body = FindBulletRigidBody(id);
	if (body)
	{
		body->setActivationState(DISABLE_DEACTIVATION);
		btVector3 torque(dir.x * newtons, dir.y * newtons, dir.z * newtons);

		body->applyTorqueImpulse(torque);
	}
}


bool BulletPhysics::KinematicMove(const Mat4x4& mat, GameObjectId id)
{
	// forces a physics object to a new location
	btRigidBody* body = FindBulletRigidBody(id);
	if (body)
	{
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setWorldTransform(Mat4x4_to_btTransform(mat));
		return true;
	}

	return false;
}


void BulletPhysics::RotateY(GameObjectId id, float angleRadians, float time)
{
	btRigidBody* pRigidBody = FindBulletRigidBody(id);
	CB_ASSERT(pRigidBody);

	btTransform angleTransform;
	angleTransform.setIdentity();
	// rotate about y axis
	angleTransform.getBasis().setEulerYPR(0, angleRadians, 0);

	// multiply the transforms
	pRigidBody->setCenterOfMassTransform(pRigidBody->getCenterOfMassTransform() * angleTransform);
}


float BulletPhysics::GetOrientationY(GameObjectId id)
{
	btRigidBody* pRigidBody = FindBulletRigidBody(id);
	CB_ASSERT(pRigidBody);

	const btTransform& objectTramsform = pRigidBody->getCenterOfMassTransform();
	btMatrix3x3 objectRotationMatrix(objectTramsform.getBasis());

	btVector3 startingVec(0, 0, 1);
	btVector3 endingVec = objectRotationMatrix * startingVec;
	endingVec.setY(0); // we only want the XZ plane

	const float endingVecLength = endingVec.length();
	if (endingVecLength < 0.001)
	{
		return 0;
	}
	else
	{
		btVector3 cross = startingVec.cross(endingVec);
		float sign = cross.getY() > 0 ? 1.0f : -1.0f;
		return (acosf(startingVec.dot(endingVec) / endingVecLength) * sign);
	}

	// fail case
	return FLT_MAX;
}


void BulletPhysics::StopGameObject(GameObjectId id)
{
	// set the velocity of the object to 0
	SetVelocity(id, Vec3(0.0f, 0.0f, 0.0f));
}


Vec3 BulletPhysics::GetVelocity(GameObjectId id)
{
	btRigidBody* pRigidBody = FindBulletRigidBody(id);
	CB_ASSERT(pRigidBody);
	if (!pRigidBody)
		return Vec3();

	btVector3 btVelocity = pRigidBody->getLinearVelocity();
	return btVector3_to_Vec3(btVelocity);
}


void BulletPhysics::SetVelocity(GameObjectId id, const Vec3& vel)
{
	btRigidBody* pRigidBody = FindBulletRigidBody(id);
	CB_ASSERT(pRigidBody);
	if (!pRigidBody)
		return;

	btVector3 btVelocity = Vec3_to_btVector3(vel);
	pRigidBody->setLinearVelocity(btVelocity);
}


Vec3 BulletPhysics::GetAngularVelocity(GameObjectId id)
{
	btRigidBody* pRigidBody = FindBulletRigidBody(id);
	CB_ASSERT(pRigidBody);
	if (!pRigidBody)
		return Vec3();

	btVector3 btVelocity = pRigidBody->getAngularVelocity();
	return btVector3_to_Vec3(btVelocity);
}


void BulletPhysics::SetAngularVelocity(GameObjectId id, const Vec3& vel)
{
	btRigidBody* pRigidBody = FindBulletRigidBody(id);
	CB_ASSERT(pRigidBody);
	if (!pRigidBody)
		return;

	btVector3 btVelocity = Vec3_to_btVector3(vel);
	pRigidBody->setAngularVelocity(btVelocity);
}


void BulletPhysics::Translate(GameObjectId id, const Vec3& vec)
{
	btRigidBody* pRigidBody = FindBulletRigidBody(id);
	CB_ASSERT(pRigidBody);
	btVector3 btVec = Vec3_to_btVector3(vec);
	pRigidBody->translate(btVec);
}


void BulletPhysics::SetTransform(const GameObjectId id, const Mat4x4& mat)
{
	// force the object into a position
	KinematicMove(mat, id);
}


Mat4x4 BulletPhysics::GetTransform(const GameObjectId id)
{
	btRigidBody* pRigidBody = FindBulletRigidBody(id);
	CB_ASSERT(pRigidBody);

	const btTransform& objectTransform = pRigidBody->getCenterOfMassTransform();
	return btTransform_to_Mat4x4(objectTransform);
}


void BulletPhysics::LoadXml()
{
	// load physics config file
	TiXmlElement* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement("config\\Physics.xml");
	CB_ASSERT(pRoot);

	// load materials
	TiXmlElement* pParentNode = pRoot->FirstChildElement("PhysicsMaterials");
	CB_ASSERT(pParentNode);
	for (TiXmlElement* pNode = pParentNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
	{
		double restitution = 0.0;
		double friction = 0.0;
		pNode->Attribute("restitution", &restitution);
		pNode->Attribute("friction", &friction);
		m_MaterialTable.insert(std::make_pair(pNode->Value(), MaterialData((float)restitution, (float)friction)));
	}

	// load densities
	pParentNode = pRoot->FirstChildElement("DensityTable");
	CB_ASSERT(pParentNode);
	for (TiXmlElement* pNode = pParentNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
	{
		m_DensityTable.insert(std::make_pair(pNode->Value(), (float)atof(pNode->FirstChild()->Value())));
	}
}


float BulletPhysics::LookupSpecificGravity(const std::string& densityStr)
{
	// return the density of an object
	float density = 0;
	auto it = m_DensityTable.find(densityStr);
	if (it != m_DensityTable.end())
	{
		density = it->second;
	}
	else
	{
		CB_ERROR("Could not find density");
	}

	return density;
}


MaterialData BulletPhysics::LookupMaterialData(const std::string& materialStr)
{
	// return material data
	auto it = m_MaterialTable.find(materialStr);
	if (it != m_MaterialTable.end())
	{
		return it->second;
	}
	else
	{
		return MaterialData(0.0f, 0.0f);
	}
}


btRigidBody* BulletPhysics::FindBulletRigidBody(GameObjectId id) const
{
	// find a rigid body given an object's id
	auto it = m_ObjectIdToRigidBody.find(id);
	if (it != m_ObjectIdToRigidBody.end())
	{
		return it->second;
	}

	return nullptr;
}


GameObjectId BulletPhysics::FindObjectId(const btRigidBody* body) const
{
	// return a game object id given a rigid body
	auto it = m_RigidBodyToObjectId.find(body);
	if (it != m_RigidBodyToObjectId.end())
	{
		return it->second;
	}

	return GameObjectId();
}


void BulletPhysics::SendCollisionPairAddEvent(const btPersistentManifold* manifold, const btRigidBody* body0, const btRigidBody* body1)
{
	// send an event for 2 bodies colliding
	if (body0->getUserPointer() || body1->getUserPointer())
	{
		// only triggers have non NULL user pointers, so this is a trigger collision
		const btRigidBody* triggerBody;
		const btRigidBody* otherBody;

		if (body0->getUserPointer())
		{
			// body0 is the trigger
			triggerBody = body0;
			otherBody = body1;
		}
		else
		{
			// body1 is the trigger
			otherBody = body0;
			triggerBody = body1;
		}
		
		// send trigger event
		const int triggerId = *static_cast<int*>(triggerBody->getUserPointer());
		shared_ptr<Event_PhysTriggerEnter> pEvent(CB_NEW Event_PhysTriggerEnter(triggerId, FindObjectId(otherBody)));
		IEventManager::Get()->QueueEvent(pEvent);
	}
	else
	{
		// two non-trigger bodies
		const GameObjectId id0 = FindObjectId(body0);
		const GameObjectId id1 = FindObjectId(body1);

		if (id0 == INVALID_GAMEOBJECT_ID || id1 == INVALID_GAMEOBJECT_ID)
		{
			// something collided with a non game object, ignore it
			return;
		}

		// send collision began event
		Vec3List collisionPoints;
		Vec3 sumNormalForce;
		Vec3 sumFrictionForce;
		for (int i = 0; i < manifold->getNumContacts(); i++)
		{
			const btManifoldPoint& point = manifold->getContactPoint(i);

			collisionPoints.push_back(btVector3_to_Vec3(point.getPositionWorldOnB()));
			sumNormalForce += btVector3_to_Vec3(point.m_combinedRestitution * point.m_normalWorldOnB);
			sumFrictionForce += btVector3_to_Vec3(point.m_combinedFriction * point.m_lateralFrictionDir1);
		}

		// send game event
		shared_ptr<Event_PhysCollision> pEvent(CB_NEW Event_PhysCollision(id0, id1, sumNormalForce, sumFrictionForce, collisionPoints));
		IEventManager::Get()->QueueEvent(pEvent);
	}
}


void BulletPhysics::SendCollisionPairRemoveEvent(const btRigidBody* body0, const btRigidBody* body1)
{
	// send an event for 2 bodies colliding
	if (body0->getUserPointer() || body1->getUserPointer())
	{
		// only triggers have non NULL user pointers, so this is a trigger collision
		const btRigidBody* triggerBody;
		const btRigidBody* otherBody;

		if (body0->getUserPointer())
		{
			// body0 is the trigger
			triggerBody = body0;
			otherBody = body1;
		}
		else
		{
			// body1 is the trigger
			otherBody = body0;
			triggerBody = body1;
		}

		// send trigger event
		const int triggerId = *static_cast<int*>(triggerBody->getUserPointer());
		shared_ptr<Event_PhysTriggerLeave> pEvent(CB_NEW Event_PhysTriggerLeave(triggerId, FindObjectId(otherBody)));
		IEventManager::Get()->QueueEvent(pEvent);
	}
	else
	{
		// two non-trigger bodies
		const GameObjectId id0 = FindObjectId(body0);
		const GameObjectId id1 = FindObjectId(body1);

		if (id0 == INVALID_GAMEOBJECT_ID || id1 == INVALID_GAMEOBJECT_ID)
		{
			// something collided with a non game object, ignore it
			return;
		}

		// send the game event
		shared_ptr<Event_PhysSeparation> pEvent(CB_NEW Event_PhysSeparation(id0, id1));
		IEventManager::Get()->QueueEvent(pEvent);
	}
}


void BulletPhysics::AddShape(StrongGameObjectPtr pGameObject, btCollisionShape* shape, float mass, const std::string& physicsMaterial)
{
	CB_ASSERT(pGameObject);

	// make sure this object is only added once
	GameObjectId id = pGameObject->GetId();
	CB_ASSERT(m_ObjectIdToRigidBody.find(id) == m_ObjectIdToRigidBody.end() && "GameObjects can only have one physics body");

	// look up material
	MaterialData material(LookupMaterialData(physicsMaterial));

	// local inertia defines how the objects mass is distributed
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (mass > 0.0f)
	{
		shape->calculateLocalInertia(mass, localInertia);
	}

	// get the objects transform
	Mat4x4 transform = pGameObject->transform.GetTransform();

	ObjectMotionState* motionState = CB_NEW ObjectMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

	// set up material properties
	rbInfo.m_restitution = material.m_Restitution;
	rbInfo.m_friction = material.m_Friction;

	// add body to physics world
	btRigidBody* body = new btRigidBody(rbInfo);
	m_DynamicsWorld->addRigidBody(body);

	// update the maps
	m_ObjectIdToRigidBody[id] = body;
	m_RigidBodyToObjectId[body] = id;
}


void BulletPhysics::RemoveCollisionObject(btCollisionObject* obj)
{
	// remove a collision object from the physics world
	m_DynamicsWorld->removeCollisionObject(obj);

	// remove the pointer from the collision contacts list
	for (auto it = m_PreviousTickCollisionPairs.begin(); it != m_PreviousTickCollisionPairs.end(); )
	{
		// step the iterator by 1
		auto next = it;
		++next;

		// remove the object from any currently happening collision
		if (it->first == obj || it->second == obj)
		{
			SendCollisionPairRemoveEvent(it->first, it->second);
			m_PreviousTickCollisionPairs.erase(it);
		}

		it = next;
	}

	// if the object was a rigid body
	if (btRigidBody* body = btRigidBody::upcast(obj))
	{
		// delete the components of the body
		delete body->getMotionState();
		delete body->getCollisionShape();
		delete body->getUserPointer();

		for (int i = body->getNumConstraintRefs() - 1; i >= 0; i--)
		{
			btTypedConstraint* constraint = body->getConstraintRef(i);
			m_DynamicsWorld->removeConstraint(constraint);
			delete constraint;
		}
	}

	delete obj;
}


void BulletPhysics::BulletInternalTickCallback(btDynamicsWorld* world, btScalar timeStep)
{
	// called after bullet sdk performs its internal step

	CB_ASSERT(world);
	CB_ASSERT(world->getWorldUserInfo());

	BulletPhysics* bulletPhysics = static_cast<BulletPhysics*>(world->getWorldUserInfo());
	CollisionPairs currentTickCollisionPairs;

	// look at all existing collisions
	btDispatcher* dispatcher = world->getDispatcher();
	const int numManifolds = dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		// get the manifold which is the data corresponding to a contact point between two colliders
		const btPersistentManifold* manifold = dispatcher->getManifoldByIndexInternal(i);
		CB_ASSERT(manifold);

		// get the colliding bodies
		const btRigidBody* body0 = static_cast<const btRigidBody *>(manifold->getBody0());
		const btRigidBody* body1 = static_cast<const btRigidBody *>(manifold->getBody1());

		// swap them if need be
		const bool swapped = body0 > body1;
		const btRigidBody* sortedBody0 = swapped ? body1 : body0;
		const btRigidBody* sortedBody1 = swapped ? body0 : body1;

		// insert the collision pair into the set
		const CollisionPair pair = std::make_pair(sortedBody0, sortedBody1);
		currentTickCollisionPairs.insert(pair);
		
		// if this is a new contact, send an event
		if (bulletPhysics->m_PreviousTickCollisionPairs.find(pair) == bulletPhysics->m_PreviousTickCollisionPairs.end())
		{
			bulletPhysics->SendCollisionPairAddEvent(manifold, body0, body1);
		}
	}

	CollisionPairs removedCollisionPairs;

	// use set difference to see which collisions existed last tick but are no longer colliding
	std::set_difference(bulletPhysics->m_PreviousTickCollisionPairs.begin(), bulletPhysics->m_PreviousTickCollisionPairs.end(),
		currentTickCollisionPairs.begin(), currentTickCollisionPairs.end(),
		std::inserter(removedCollisionPairs, removedCollisionPairs.begin()));

	// send collision exit events
	for (auto it = removedCollisionPairs.begin(); it != removedCollisionPairs.end(); ++it)
	{
		const btRigidBody* body0 = it->first;
		const btRigidBody* body1 = it->second;

		bulletPhysics->SendCollisionPairRemoveEvent(body0, body1);
	}

	// update the collision pairs
	bulletPhysics->m_PreviousTickCollisionPairs = currentTickCollisionPairs;
}


// C function to create the physics world
IGamePhysics* CreateGamePhysics()
{
	unique_ptr<IGamePhysics> gamePhysics;
	gamePhysics.reset(CB_NEW BulletPhysics);

	if (gamePhysics.get() && !gamePhysics->Initialize())
	{
		// failed to init
		CB_ERROR("Failed to initialize physics");
		gamePhysics.reset();
	}

	// return the raw pointer to the game physics
	return gamePhysics.release();
}
