/*
	RootNode.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "EngineStd.h"
#include "Logger.h"
#include "RootNode.h"
#include "Scene.h"

RootNode::RootNode() :
SceneNode(INVALID_GAMEOBJECT_ID, WeakBaseRenderComponentPtr(), RenderPass::RenderPass_0, &Mat4x4::Identity)
{
	m_Children.reserve(RenderPass::RenderPass_Last);

	// split children into different groups

	shared_ptr<SceneNode> staticGroup(CB_NEW SceneNode(INVALID_GAMEOBJECT_ID, WeakBaseRenderComponentPtr(), RenderPass::RenderPass_Static, &Mat4x4::Identity));
	m_Children.push_back(staticGroup);

	shared_ptr<SceneNode> objectGroup(CB_NEW SceneNode(INVALID_GAMEOBJECT_ID, WeakBaseRenderComponentPtr(), RenderPass::RenderPass_Object, &Mat4x4::Identity));
	m_Children.push_back(objectGroup);

	shared_ptr<SceneNode> skyGroup(CB_NEW SceneNode(INVALID_GAMEOBJECT_ID, WeakBaseRenderComponentPtr(), RenderPass::RenderPass_Sky, &Mat4x4::Identity));
	m_Children.push_back(skyGroup);

	shared_ptr<SceneNode> invisibleGroup(CB_NEW SceneNode(INVALID_GAMEOBJECT_ID, WeakBaseRenderComponentPtr(), RenderPass::RenderPass_NotRendered, &Mat4x4::Identity));
	m_Children.push_back(staticGroup);

}

bool RootNode::AddChild(shared_ptr<ISceneNode> child)
{
	// add scene nodes to the correct group off of the root based on its render pass
	RenderPass pass = child->Get()->GetRenderPass();
	if ((unsigned int)pass >= m_Children.size() || !m_Children[pass])
	{
		CB_ASSERT(0 && L"There is no render pass for this node");
		return false;
	}

	return m_Children[pass]->AddChild(child);
}

bool RootNode::RemoveChild(GameObjectId id)
{
	bool anythingRemoved = false;
	// search each render pass bucket for the object to be removed
	for (int i = RenderPass::RenderPass_0; i < RenderPass::RenderPass_Last; ++i)
	{
		if (m_Children[i]->RemoveChild(id))
		{
			anythingRemoved = true;
		}
	}

	return anythingRemoved;
}

bool RootNode::IsVisible(Scene* pScene) const
{
	return true;
}

HRESULT RootNode::RenderChildren(Scene* pScene)
{
	for (int pass = RenderPass::RenderPass_0; pass < RenderPass::RenderPass_Last; ++pass)
	{
		switch (pass)
		{
		case RenderPass_Static:
		case RenderPass_Object:
			m_Children[pass]->RenderChildren(pScene);
			break;

		case RenderPass_Sky:
			{
				shared_ptr<IRenderState> skyPass = pScene->GetRenderer()->PrepareSkyBoxPass();
				m_Children[pass]->RenderChildren(pScene);
				break;
			}
		}
	}

	return S_OK;
}