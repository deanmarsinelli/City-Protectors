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

#include "Physics.h"

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
	Private implementation of the IGamePhysics interface. This class
	uses the 3rd Party BulletPhysics SDK.
*/
class BulletPhysics : public IGamePhysics
{
	// typedefs
	typedef std::unordered_map<std::string, float> DensityTable;
	typedef std::unordered_map<std::string, MaterialData> MaterialTable;
	typedef std::unordered_map<GameObjectId, btRigidBody*> ObjectIdToBulletRigidBodyMap;
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
	void LoadXML();
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
	static void BulletInternalTickCallback(const btDynamicsWorld* world, const btScalar timeStep);

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
	ObjectIdToBulletRigidBodyMap m_ObjectIdToRigidBody;
	
	// store pairs of bodies that are colliding. when bodies first collide they
	// are stored here and an event is sent. when they stop colliding, they are removed
	// and another event is sent
	CollisionPairs m_PreviousTickCollisionPairs;
};



// C function to create the physics world
IGamePhysics* CreateGamePhysics()
{

}