/*
	Scene.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <FastDelegate.h>

#include "EngineStd.h"
#include "Events.h"
#include "LightNode.h"
#include "LightManager.h"
#include "Logger.h"
#include "RootNode.h"
#include "Scene.h"
#include "SceneNode.h"
#include "Shaders.h"
#include "StringUtil.h"

Scene::Scene(shared_ptr<IRenderer> renderer)
{
	m_Root.reset(CB_NEW RootNode());
	m_Renderer = renderer;
	m_LightManager = CB_NEW LightManager;

	D3DXCreateMatrixStack(0, &m_MatrixStack);

	IEventManager* pEventMgr = IEventManager::Get();
	pEventMgr->AddListener(fastdelegate::MakeDelegate(this, &Scene::NewRenderComponentDelegate), Event_NewRenderComponent::sk_EventType);
	pEventMgr->AddListener(fastdelegate::MakeDelegate(this, &Scene::DestroyObjectDelegate), Event_DestroyGameObject::sk_EventType);
	pEventMgr->AddListener(fastdelegate::MakeDelegate(this, &Scene::MoveObjectDelegate), Event_MoveGameObject::sk_EventType);
	pEventMgr->AddListener(fastdelegate::MakeDelegate(this, &Scene::ModifiedRenderComponentDelegate), Event_ModifiedRenderComponent::sk_EventType);
}

Scene::~Scene()
{
	IEventManager* pEventMgr = IEventManager::Get();
	pEventMgr->RemoveListener(fastdelegate::MakeDelegate(this, &Scene::NewRenderComponentDelegate), Event_NewRenderComponent::sk_EventType);
	pEventMgr->RemoveListener(fastdelegate::MakeDelegate(this, &Scene::DestroyObjectDelegate), Event_DestroyGameObject::sk_EventType);
	pEventMgr->RemoveListener(fastdelegate::MakeDelegate(this, &Scene::MoveObjectDelegate), Event_MoveGameObject::sk_EventType);
	pEventMgr->RemoveListener(fastdelegate::MakeDelegate(this, &Scene::ModifiedRenderComponentDelegate), Event_ModifiedRenderComponent::sk_EventType);

	CB_COM_RELEASE(m_MatrixStack);
	CB_SAFE_DELETE(m_LightManager);
}

HRESULT Scene::OnRender()
{
	// render passes go like:
	// 1. Static objects and terrain
	// 2. dynamic objects
	// 3. sky
	// 4. anything with alpha

	if (m_Root && m_Camera)
	{
		m_Camera->SetViewTransform(this);

		m_LightManager->CalcLighting(this);

		if (m_Root->PreRender(this) == S_OK)
		{
			m_Root->Render(this);
			m_Root->RenderChildren(this);
			m_Root->PostRender(this);
		}
		RenderAlphaPass();
	}

	return S_OK;
}

HRESULT Scene::OnRestore()
{
	if (!m_Root)
		return S_OK;

	HRESULT hr;
	V_RETURN(m_Renderer->OnRestore());

	return m_Root->OnRestore(this);
}

HRESULT Scene::OnLostDevice()
{
	if (m_Root)
	{
		return m_Root->OnLostDevice(this);
	}

	return S_OK;
}

HRESULT Scene::OnUpdate(float deltaTime)
{
	if (!m_Root)
		return S_OK;

	return m_Root->OnUpdate(this, deltaTime);
}

shared_ptr<ISceneNode> Scene::FindObject(GameObjectId id)
{
	SceneObjectMap::iterator it = m_ObjectMap.find(id);
	if (it == m_ObjectMap.end())
	{
		return shared_ptr<ISceneNode>();
	}
	return it->second;
}

bool Scene::AddChild(GameObjectId id, shared_ptr<ISceneNode> child)
{
	if (id != INVALID_GAMEOBJECT_ID)
	{
		// add the object to the map for fast lookup
		m_ObjectMap[id] = child;
	}

	// if the node is a light, also add it to the light manager
	shared_ptr<LightNode> pLight = dynamic_pointer_cast<LightNode>(child);
	if (pLight != nullptr && m_LightManager->m_Lights.size() + 1 < MAXIMUM_LIGHTS_SUPPORTED)
	{
		m_LightManager->m_Lights.push_back(pLight);
	}

	return m_Root->AddChild(child);
}

bool Scene::RemoveChild(GameObjectId id)
{
	if (id == INVALID_GAMEOBJECT_ID)
		return false;

	shared_ptr<ISceneNode> child = FindObject(id);
	shared_ptr<LightNode> pLight = dynamic_pointer_cast<LightNode>(child);
	if (pLight != nullptr)
	{
		m_LightManager->m_Lights.remove(pLight);
	}
	m_ObjectMap.erase(id);

	return m_Root->RemoveChild(id);
}

void Scene::NewRenderComponentDelegate(IEventPtr pEvent)
{
	shared_ptr<Event_NewRenderComponent> pCastEvent = static_pointer_cast<Event_NewRenderComponent>(pEvent);

	GameObjectId objectId = pCastEvent->GetId();

	shared_ptr<SceneNode> pSceneNode(pCastEvent->GetSceneNode());
	if (FAILED(pSceneNode->OnRestore(this)))
	{
		std::string error = "Failed to restore scene node for object id: " + ToStr(objectId);
		CB_ERROR(error);
		return;
	}

	AddChild(objectId, pSceneNode);
}

void Scene::ModifiedRenderComponentDelegate(IEventPtr pEvent)
{
	shared_ptr<Event_ModifiedRenderComponent> pCastEvent = static_pointer_cast<Event_ModifiedRenderComponent>(pEvent);

	GameObjectId objectId = pCastEvent->GetId();
	if (objectId == INVALID_GAMEOBJECT_ID)
	{
		CB_ERROR("Scene::ModifiedRenderComponentDelegate was given an invalid object id");
		return;
	}

	if (g_pApp->GetGameLogic()->GetState() == BaseGameState::LoadingGameEnvironment)
	{
		return;
	}

	shared_ptr<ISceneNode> pSceneNode = FindObject(objectId);
	if (!pSceneNode || FAILED(pSceneNode->OnRestore(this)))
	{
		CB_ERROR("Failed to restore scene node to the scene for id: " + ToStr(objectId));
	}
}

void Scene::DestroyObjectDelegate(IEventPtr pEvent)
{
	shared_ptr<Event_DestroyGameObject> pCastEvent = static_pointer_cast<Event_DestroyGameObject>(pEvent);
	RemoveChild(pCastEvent->GetId());
}

void Scene::MoveObjectDelegate(IEventPtr pEvent)
{
	shared_ptr<Event_MoveGameObject> pCastEvent = static_pointer_cast<Event_MoveGameObject>(pEvent);

	GameObjectId objectId = pCastEvent->GetId();
	Mat4x4 transform = pCastEvent->GetMatrix();

	shared_ptr<ISceneNode> pNode = FindObject(objectId);
	if (pNode)
	{
		pNode->SetTransform(&transform);
	}
}

void Scene::SetCamera(shared_ptr<CameraNode> camera)
{
	m_Camera = camera;
}

const shared_ptr<CameraNode> Scene::GetCamera() const
{
	return m_Camera;
}

void Scene::PushAndSetMatrix(const Mat4x4& toWorld)
{
	m_MatrixStack->Push();
	m_MatrixStack->MultMatrixLocal(&toWorld);
	Mat4x4 mat = GetTopMatrix();
	m_Renderer->SetWorldTransform(&mat);
}

void Scene::PopMatrix()
{
	m_MatrixStack->Pop();
	Mat4x4 mat = GetTopMatrix();
	m_Renderer->SetWorldTransform(&mat);
}

Mat4x4 Scene::GetTopMatrix()
{
	return static_cast<const Mat4x4>(*m_MatrixStack->GetTop());
}

LightManager* Scene::GetLightManager()
{
	return m_LightManager;
}

void Scene::AddAlphaSceneNode(AlphaSceneNode* alphaNode)
{
	m_AlphaSceneNodes.push_back(alphaNode);
}

HRESULT Scene::Pick(RayCast* pRayCast)
{
	return m_Root->Pick(this, pRayCast);
}

shared_ptr<IRenderer> Scene::GetRenderer()
{
	return m_Renderer;
}

void Scene::RenderAlphaPass()
{
	shared_ptr<IRenderState> alphaPass = m_Renderer->PrepareAlphaPass();

	// sort the alpha nodes front to back
	m_AlphaSceneNodes.sort();
	while (!m_AlphaSceneNodes.empty())
	{
		// render the nodes back to front
		AlphaSceneNodes::reverse_iterator it = m_AlphaSceneNodes.rbegin();
		PushAndSetMatrix((*it)->m_Concat);
		(*it)->m_pNode->Render(this);
		delete (*it);
		PopMatrix();
		m_AlphaSceneNodes.pop_back();
	}
}
