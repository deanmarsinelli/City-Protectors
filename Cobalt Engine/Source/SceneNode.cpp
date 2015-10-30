/*
	SceneNode.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "AlphaSceneNode.h"
#include "BaseGameLogic.h"
#include "EngineStd.h"
#include "Frustrum.h"
#include "GameObject.h"
#include "Logger.h"
#include "RenderComponent.h"
#include "Scene.h"
#include "SceneNode.h"
#include "templates.h"
#include "TransformComponent.h"
#include "Vector.h"

SceneNode::SceneNode(GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, RenderPass renderPass, const Mat4x4* to, const Mat4x4* from)
{
	m_pParent = nullptr;
	m_Properties.m_ObjectId = objectId;
	// if the object has a render component, get the name of that, otherwise just generic scene node
	m_Properties.m_Name = (renderComponent) ? renderComponent->GetName() : "SceneNode";
	m_Properties.m_RenderPass = renderPass;
	m_Properties.m_AlphaType = AlphaOpaque;
	m_RenderComponent = renderComponent;
	SetTransform(to, from);
	SetRadius(0.0f);
}

SceneNode::~SceneNode()
{ }

void SceneNode::SetAlpha(float alpha)
{
	// set the alpha on this node and all children
	m_Properties.SetAlpha(alpha);
	for (SceneNodeList::const_iterator it = m_Children.begin(); it != m_Children.end(); ++it)
	{
		shared_ptr<SceneNode> sceneNode = static_pointer_cast<SceneNode>(*it);
		sceneNode->SetAlpha(alpha);
	}
}

float SceneNode::GetAlpha() const
{
	return m_Properties.Alpha();
}

Vec3 SceneNode::GetPosition() const
{
	return m_Properties.m_ToWorld.GetPosition();
}

void SceneNode::SetPosition(const Vec3& posistion)
{
	m_Properties.m_ToWorld.SetPosition(posistion);
}

const Vec3 SceneNode::GetWorldPosition() const
{
	Vec3 pos = GetPosition();
	if (m_pParent)
	{
		pos += m_pParent->GetWorldPosition();
	}

	return pos;
}

Vec3 SceneNode::GetDirection() const
{
	return m_Properties.m_ToWorld.GetDirection();
}

void SceneNode::SetRadius(const float radius)
{
	m_Properties.m_Radius = radius;
}

void SceneNode::SetMaterial(const Material& mat)
{
	m_Properties.m_Material = mat;
}

const SceneNodeProperties* SceneNode::Get() const
{
	return &m_Properties;
}

void SceneNode::SetTransform(const Mat4x4* toWorld, const Mat4x4* fromWorld)
{
	m_Properties.m_ToWorld = *toWorld;
	if (!fromWorld)
	{
		m_Properties.m_FromWorld = m_Properties.m_ToWorld.Inverse();
	}
	else
	{
		m_Properties.m_FromWorld = *fromWorld;
	}
}

HRESULT SceneNode::OnUpdate(Scene* pScene, float deltaTime)
{
	// this is meant to be called by any class subclassing SceneNode
	SceneNodeList::iterator it = m_Children.begin();
	SceneNodeList::iterator endIt = m_Children.end();
	while (it != endIt)
	{
		(*it)->OnUpdate(pScene, deltaTime);
		++it;
	}

	return S_OK;
}

HRESULT SceneNode::OnRestore(Scene* pScene)
{
	Color color = (m_RenderComponent) ? m_RenderComponent->GetColor() : g_White;
	m_Properties.m_Material.SetDiffuse(color);

	// this is meant to be called by any class subclassing SceneNode
	SceneNodeList::iterator it = m_Children.begin();
	SceneNodeList::iterator endIt = m_Children.end();
	while (it != endIt)
	{
		(*it)->OnRestore(pScene);
		++it;
	}

	return S_OK;
}

HRESULT SceneNode::PreRender(Scene* pScene)
{
	StrongGameObjectPtr pObject = MakeStrongPtr(g_pApp->GetGameLogic()->GetGameObject(m_Properties.m_ObjectId));
	if (pObject)
	{
		shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(pObject->GetComponent<TransformComponent>(TransformComponent::g_Name));
		if (pTransformComponent)
		{
			m_Properties.m_ToWorld = pTransformComponent->GetTransform();
		}
	}

	pScene->PushAndSetMatrix(m_Properties.m_ToWorld);
	return S_OK;
}

bool SceneNode::IsVisible(Scene* pScene) const
{
	// transform the location of this node into camera space and do a visibility check
	Mat4x4 toWorld, fromWorld;
	pScene->GetCamera()->Get()->Transform(&toWorld, &fromWorld);

	Vec3 position = GetWorldPosition();
	Vec3 fromWorldPosition = fromWorld.Transform(position);

	const Frustrum& frustrum = pScene->GetCamera()->GetFrustrum();

	bool isVisible = frustrum.Inside(fromWorldPosition, Get()->Radius());

	return isVisible;
}

HRESULT SceneNode::Render(Scene* pScene)
{
	return S_OK;
}

HRESULT SceneNode::RenderChildren(Scene* pScene)
{
	SceneNodeList::iterator it = m_Children.begin();
	SceneNodeList::iterator endIt = m_Children.end();

	while (it != endIt)
	{
		// make sure each child passes pre render and is visible
		if ((*it)->PreRender(pScene) == S_OK)
		{
			if ((*it)->IsVisible(pScene))
			{
				float alpha = (*it)->Get()->m_Material.GetAlpha();
				if (alpha == fOPAQUE)
				{
					// if an object is opaque, render!
					(*it)->Render(pScene);
				}
				else if (alpha != fTRANSPARENT)
				{
					// if an object isn't fully transparent
					AlphaSceneNode* alphaNode = CB_NEW AlphaSceneNode;
					CB_ASSERT(alphaNode);
					alphaNode->m_pNode = *it;
					alphaNode->m_Concat = pScene->GetTopMatrix();

					Vec4 worldPos(alphaNode->m_Concat.GetPosition());
					Mat4x4 fromWorld = pScene->GetCamera()->Get()->FromWorld();
					Vec4 screenPos = fromWorld.Transform(worldPos);

					alphaNode->m_ScreenZ = screenPos.z;
					pScene->AddAlphaSceneNode(alphaNode);
				}
				// render children if this node was visible
				(*it)->RenderChildren(pScene);
			}
		}
		(*it)->PostRender(pScene);
		++it;
	}

	return S_OK;
}

HRESULT SceneNode::PostRender(Scene* pScene)
{
	pScene->PopMatrix();
	return S_OK;
}

bool SceneNode::AddChild(shared_ptr<ISceneNode> child)
{
	m_Children.push_back(child);
	shared_ptr<SceneNode> tempChild = static_pointer_cast<SceneNode>(child);

	// set "this" as the childs parent
	tempChild->m_pParent = this;

	// get the childs radiu, and create a new radius for this node including the child
	Vec3 childPos = tempChild->Get()->ToWorld().GetPosition();
	float newRadius = childPos.Length() + child->Get()->Radius();
	if (newRadius > m_Properties.m_Radius)
	{
		m_Properties.m_Radius = newRadius;
	}
	
	return true;
}

bool SceneNode::RemoveChild(GameObjectId id)
{
	// if an object is destroyed, it should be removed from the scene graph
	// generally the humanview will receive a message saying the object was
	// destroyed and will call RemoveChild() to remove it from the scene graph
	for (SceneNodeList::iterator it = m_Children.begin(); it != m_Children.end(); ++it)
	{
		const SceneNodeProperties* pProperties = (*it)->Get();
		if (pProperties->ObjectId() != INVALID_GAMEOBJECT_ID && pProperties->ObjectId() == id)
		{
			it = m_Children.erase(it);
			return true;
		}
	}

	return false;
}

HRESULT SceneNode::OnLostDevice(Scene* pScene)
{
	// this is meant to be called by any class subclassing SceneNode
	SceneNodeList::iterator it = m_Children.begin();
	SceneNodeList::iterator endIt = m_Children.end();
	while (it != endIt)
	{
		(*it)->OnLostDevice(pScene);
		++it;
	}

	return S_OK;
}

HRESULT SceneNode::Pick(Scene* pScene, RayCast* pRayCast)
{
	for (SceneNodeList::const_iterator it = m_Children.begin(); it != m_Children.end(); ++it)
	{
		HRESULT hr = (*it)->Pick(pScene, pRayCast);

		if (hr == E_FAIL)
			return E_FAIL;
	}

	return S_OK;
}
