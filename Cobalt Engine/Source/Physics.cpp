/*
	Physics.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

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
#include "PhysicsEvents.h"

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
	void SendCollisionPairRemovedEvent(const btRigidBody* body0, const btRigidBody* body1);

	// add a shape to the physics world
	void AddShape(StrongGameObjectPtr pGameObject, btCollisionShape* shape, float mass, const std::string& physicsMaterial);

	void RemoveCollisionObject(btCollisionObject* obj);

	// callback from bullet sdk for each physics setp
	static void BulletInternalTickCallback(btDynamicsWorld* world, btScalar timeStep);

private:
	// objects used by Bullet SDK
	shared_ptr<btDynamicsWorld>			m_DynamicsWorld;
	shared_ptr<btBroadphaseInterface>	m_Broadphase;
	shared_ptr<btCollisionDispatcher>	m_Dispatcher;
	shared_ptr<btConstraintSolver>		m_Solver;
	shared_ptr<btDefaultCollisionConfiguration> m_CollisionConfiguration;
	// shared_ptr<BulletDebugDrawer> m_DebugDrawer;

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
	m_CollisionConfiguration = shared_ptr<btDefaultCollisionConfiguration>(CB_NEW btDefaultCollisionConfiguration);

	// this manages how bullet detects precise collisions between pairs of objects
	m_Dispatcher = shared_ptr<btCollisionDispatcher>(CB_NEW btCollisionDispatcher(m_CollisionConfiguration.get()));

	// used to quickly detect collisions between object, imprecisely. 
	// if a collision passes this phase it will be passed to the dispatcher
	m_Broadphase = shared_ptr<btDbvtBroadphase>(CB_NEW btDbvtBroadphase);

	m_Solver = shared_ptr<btSequentialImpulseConstraintSolver>(CB_NEW btSequentialImpulseConstraintSolver);

	// this is the main interface point for the physics world
	m_DynamicsWorld = shared_ptr<btDiscreteDynamicsWorld>(CB_NEW btDiscreteDynamicsWorld(
		m_Dispatcher.get(), m_Broadphase.get(), m_Solver.get(), m_CollisionConfiguration.get()));

	// m_DebugDrawer = shared_ptr<BulletDebugDrawer>(CB_NEW DebugDrawer);
	// m_DebugDrawer->ReadOptions();

	if (!m_CollisionConfiguration || !m_Dispatcher || !m_Broadphase || !m_Solver
		|| !m_DynamicsWorld /*|| !m_DebugDrawer*/)
	{
		CB_ERROR("BulletPhysics::Initialize failed");
		return false;
	}

	//m_DynamicsWorld->setDebugDrawer(m_DebugDrawer);

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

// C function to create the physics world
IGamePhysics* CreateGamePhysics()
{

}