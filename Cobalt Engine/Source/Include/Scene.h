/*
	Scene.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <unordered_map>

#include "AlphaSceneNode.h"
#include "CameraNode.h"
#include "interfaces.h"

class LightManager;
class SceneNode;
class RayCast;

/// A hash map that allows fast lookup of a scene node given an id
typedef std::unordered_map<GameObjectId, shared_ptr<ISceneNode>> SceneObjectMap;

/**
	A hierarchical container of scene nodes.
*/
class Scene
{
public:
	/// Constructor taking in a renderer
	Scene(shared_ptr<IRenderer> renderer);

	/// Virtual destructor
	virtual ~Scene();

	/// Render the scene
	HRESULT OnRender();

	/// Called when the device is restored
	HRESULT OnRestore();

	/// Called when the device is lost
	HRESULT OnLostDevice();
	
	/// Update the scene called once per frame
	HRESULT OnUpdate(float deltaTime);

	/// Return a pointer to a scene node by giving a game object id
	shared_ptr<ISceneNode> FindObject(GameObjectId id);

	/// Add a child node to the root node in the scene
	bool AddChild(GameObjectId id, shared_ptr<ISceneNode> child);

	/// Remove a child node from the scene
	bool RemoveChild(GameObjectId id);

	// event delegates
	void NewRenderComponentDelegate(IEventPtr pEvent);
	void ModifiedRenderComponentDelegate(IEventPtr pEvent);
	void DestroyObjectDelegate(IEventPtr pEvent);
	void MoveObjectDelegate(IEventPtr pEvent);

	/// Set the camera node of the scene
	void SetCamera(shared_ptr<CameraNode> camera);

	/// Return a pointer to the camera node in the scene
	const shared_ptr<CameraNode> GetCamera() const;

	/// Push a new matrix onto the stack and set the new world transform matrix
	void PushAndSetMatrix(const Mat4x4& toWorld);

	/// Pop a transformation matrix from the matrix stack
	void PopMatrix();

	/// Return the current top matrix from the matrix stack
	Mat4x4 GetTopMatrix();

	/// Return a pointer to the light manager
	LightManager* GetLightManager();

	/// Add a node to the scene graph that has transparency
	void AddAlphaSceneNode(AlphaSceneNode* alphaNode);

	/// Check if this raycast collides with an object in the scene
	HRESULT Pick(RayCast* pRayCast);

	/// Return the camera
	shared_ptr<IRenderer> GetRenderer();

protected:
	/// Render the nodes that have transparency
	void RenderAlphaPass();

protected:
	/// Root node in the scene graph
	shared_ptr<SceneNode> m_Root;

	/// Camera for the scene
	shared_ptr<CameraNode> m_Camera;

	/// The renderer for the scene
	shared_ptr<IRenderer> m_Renderer;

	/// Manages the stack of transform matrices and holds the current world transform matrix
	ID3DXMatrixStack* m_MatrixStack;

	/// List of structs that holds information necessary to draw transparent scene nodes
	AlphaSceneNodes m_AlphaSceneNodes;

	/// Map of scene nodes to game object id's
	SceneObjectMap m_ObjectMap;

	/// A helper object to manage multiple directional lights
	LightManager* m_LightManager;
};