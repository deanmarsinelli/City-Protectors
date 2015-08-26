/*
	Scene.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <unordered_map>

#include "AlphaSceneNode.h"
#include "CameraNoda.h"
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
	Scene(shared_ptr<IRenderer> renderer);
	virtual ~Scene();

	HRESULT OnRender();
	HRESULT OnRestore();
	HRESULT OnLostDevice();
	HRESULT OnUpdate(float deltaTime);

	shared_ptr<ISceneNode> FindObject(GameObjectId id);
	bool AddChild(GameObjectId id, shared_ptr<ISceneNode> child);
	bool RemoveChild(GameObjectId id);

	// event delegates
	void NewRenderComponentDelegate(IEventPtr pEvent);
	void ModifiedRenderComponentDelegate(IEventPtr pEvent);
	void DestroyObjectDelegate(IEventPtr pEvent);
	void MoveObjectDelegate(IEventPtr pEvent);

	void SetCamera(shared_ptr<CameraNode> camera);
	const shared_ptr<CameraNode> GetCamera() const;

	void PushAndSetMatrix(const Mat4x4& toWorld);

	void PopMatrix();

	Mat4x4 GetTopMatrix();

	LightManager* GetLightManager();

	void AddAlphaSceneNode(AlphaSceneNode* alphaNode);

	HRESULT Pick(RayCast* pRayCast);

	shared_ptr<IRenderer> GetRenderer();

protected:
	void RenderAlphaPass();

protected:
	/// Root node in the scene graph
	shared_ptr<SceneNode> m_Root;

	/// Camera for the scene
	shared_ptr<CameraNode> m_Camera;

	/// The renderer for the scene
	shared_ptr<IRenderer> m_Renderer;

	ID3DXMatrixStack* m_MatrixStack;

	AlphaSceneNodes m_AlphaSceneNodes;
	SceneObjectMap m_ObjectMap;

	LightManager* m_LightManager;
};