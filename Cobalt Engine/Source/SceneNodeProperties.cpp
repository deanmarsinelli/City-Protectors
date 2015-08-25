/*
	SceneNodeProperties.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "SceneNodeProperties.h"

SceneNodeProperties::SceneNodeProperties()
{
	m_ObjectId = INVALID_GAMEOBJECT_ID;
	m_Radius = 0;
	m_RenderPass = RenderPass::RenderPass_0;
	m_AlphaType = AlphaType::AlphaOpaque;
}

const GameObjectId& SceneNodeProperties::ObjectId() const
{
	return m_ObjectId;
}

const Mat4x4& SceneNodeProperties::ToWorld() const
{
	return m_ToWorld;
}

const Mat4x4& SceneNodeProperties::FromWorld() const
{
	return m_FromWorld;
}

void SceneNodeProperties::Transform(Mat4x4* toWorld, Mat4x4* fromWorld) const
{
	if (toWorld)
	{
		*toWorld = m_ToWorld;
	}
	if (fromWorld)
	{
		*fromWorld = m_FromWorld;
	}
}

const char* SceneNodeProperties::Name() const
{
	return m_Name.c_str();
}

bool SceneNodeProperties::HasAlpha() const
{
	return m_Material.HasAlpha();
}

float SceneNodeProperties::Alpha() const
{
	return m_Material.GetAlpha();
}

AlphaType SceneNodeProperties::GetAlphaType() const
{
	return m_AlphaType;
}

RenderPass SceneNodeProperties::GetRenderPass() const
{
	return m_RenderPass;
}

float SceneNodeProperties::Radius() const
{
	return m_Radius;
}

Material SceneNodeProperties::GetMaterial()
{
	return m_Material;
}

void SceneNodeProperties::SetAlpha(const float alpha)
{
	m_AlphaType = AlphaMaterial;
	m_Material.SetAlpha(alpha);
}
