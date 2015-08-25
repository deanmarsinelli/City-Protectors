/*
	SceneNode.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <vector>

#include "interfaces.h"
#include "Material.h"
#include "SceneNodeProperties.h"

typedef std::vector<shared_ptr<ISceneNode>> SceneNodeList;
class BaseRenderComponent;
typedef BaseRenderComponent* WeakBaseRenderComponentPtr;

/**
	View represenation of a game object in the 3d world. Game objects exist in
	the logic layer of the engine and scene node's are their counter part in 
	the view layer. Other than GetWorldPosition(), transform related methods are
	in relation to the node's parent.
*/
class SceneNode : public ISceneNode
{
	friend class Scene;

public:
	/// Constructor to build a scene node
	SceneNode(GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, RenderPass renderPass, const Mat4x4* to, const Mat4x4* from = nullptr);
	
	/// Virtual destructor
	virtual ~SceneNode();

	/// Set the alpha value for this node's material
	void SetAlpha(float alpha);

	/// Get the alpha value for this node's material
	float GetAlpha() const;

	/// Get the position of a node in relation to it's parent's position
	Vec3 GetPosition() const;

	/// Set the position of the node in relation to it's parent's position
	void SetPosition(const Vec3& posistion);

	/// Get world position of the object 
	const Vec3 GetWorldPosition() const;

	/// Return the direction the node is facing
	Vec3 GetDirection() const;

	/// Set the radius of a sphere that encapsulates the node's geometry
	void SetRadius(const float radius);

	/// Set the material of the node
	void SetMaterial(const Material& mat);


	//==============================
	//	ISceneNode interface
	//==============================
	/// Return the properties of the scene node
	virtual const SceneNodeProperties* Get() const;

	/// Calculate the inverse transform matrix (world space to object space)
	virtual void SetTransform(const Mat4x4* toWorld, const Mat4x4* fromWorld = nullptr);

	/// Update method called once per frame
	virtual HRESULT OnUpdate(Scene* pScene, float deltaTime);

	/// Restore when the device is lost
	virtual HRESULT OnRestore(Scene* pScene);

	/// Set up the node for rendering
	virtual HRESULT PreRender(Scene* pScene);

	/// Return whether or not the node is visible
	virtual bool IsVisible(Scene* pScene) const;

	/// Render an object
	virtual HRESULT Render(Scene* pScene);

	/// Render all children objects (used for animation)
	virtual HRESULT RenderChildren(Scene* pScene);

	/// Perform any post render actions
	virtual HRESULT PostRender(Scene* pScene);

	/// Add a child node
	virtual bool AddChild(shared_ptr<ISceneNode> child);

	/// Remove a child node
	virtual bool RemoveChild(GameObjectId id);

	/// Called when the device is lost
	virtual HRESULT OnLostDevice(Scene* pScene);

	virtual HRESULT Pick(Scene* pScene, RayCast* pRayCast);

protected:
	/// List of scene node children
	SceneNodeList m_Children;

	/// Pointer to parent scene node
	SceneNode* m_pParent;

	/// Properties of this scene node
	SceneNodeProperties m_Properties;

	/// Pointer to the render component
	WeakBaseRenderComponentPtr m_RenderComponent;
};
