/*
	PhysicsComponent.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "Component.h"

/**
	Allows a game object to be controlled by physics, interact with
	other game objects, and detect collisions and triggers.
*/
class PhysicsComponent : public Component
{
public:
	/// Default constructor
	PhysicsComponent();

	/// Virtual destructor
	virtual ~PhysicsComponent();

	/// Serialze the component to Xml
	virtual TiXmlElement* GenerateXml() override;

	// Component interface
	/// Initialize the physics component from xml data
	virtual bool Init(TiXmlElement* pData) override;

	/// Logic executed post initialization
	virtual void PostInit() override;

	/// Update method called once per frame
	virtual void Update(float deltaTime) override;

	// Physics functions
	/// Apply physics force to a game object
	void ApplyForce(const Vec3& direction, float forceNewtons);

	/// Apply physics torque to a game object
	void ApplyTorque(const Vec3& direction, float forceNewtons);

	/// Move the object through kinematics instead of forces
	bool KinematicMove(const Mat4x4& transform);

	// Velocity, Acceleration, and Position
	/// Apply physics acceleration to a game object
	void ApplyAcceleration(float acceleration);

	/// Remove physics acceleration from a game object
	void RemoveAcceleration();

	/// Apply physics angular acceleration to a game object
	void ApplyAngularAcceleration(float acceleration);

	/// Remove physics angular acceleration from a game object
	void RemoveAngularAcceleration();

	/// Return the velocity of the game object
	Vec3 GetVelocity();

	/// Manually set the velocity of the game object
	void SetVelocity(const Vec3& velocity);

	/// Rotate the game object around the Y axis
	void RotateY(float angleRadians);

	/// Manually set position of the object
	void SetPosition(float x, float y, float z);
	
	/// Stop all motion of the object
	void Stop();

	/// Return the name of the component
	virtual const char* GetName() const override { return PhysicsComponent::g_Name; }

protected:
	/// Build a rigid body from xml data
	void BuildRigidBodyTransform(TiXmlElement* pTransformElement);

public:
	/// Name of the component
	static const char* g_Name;

protected:
	float m_Acceleration;
	float m_AngularAcceleration;
	float m_MaxVelocity;
	float m_MaxAngularVelocity;

	// Properties from xml
	std::string m_Shape;
	std::string m_Density;
	std::string m_Material;
	
	// Not world position. These are offsets for how the rigidbody differs from the objects transform
	Vec3 m_RigidBodyLocation;
	Vec3 m_RigidBodyOrientation;
	Vec3 m_RigidBodyScale;

	/// Pointer to the physics system
	shared_ptr<IGamePhysics> m_pGamePhysics;
};
