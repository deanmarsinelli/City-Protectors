/*
	LightManager.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "LightManager.h"
#include "LightNode.h"
#include "Logger.h"
#include "Material.h"
#include "Scene.h"

void LightManager::CalcLighting(Scene* pScene)
{
	pScene->GetRenderer()->CalcLighting(&m_Lights, MAXIMUM_LIGHTS_SUPPORTED);

	int count = 0;

	CB_ASSERT(m_Lights.size() < MAXIMUM_LIGHTS_SUPPORTED);
	for (Lights::iterator it = m_Lights.begin(); it != m_Lights.end(); ++it, ++count)
	{
		shared_ptr<LightNode> light = *it;

		if (count == 0)
		{
			// light 0 is the only one we use for ambient lighting
			Color ambient = light->Get()->GetMaterial().GetAmbient();
			m_LightAmbient = D3DXVECTOR4(ambient.r, ambient.g, ambient.b, 1.0f);
		}

		Vec3 lightDir = light->GetDirection();
		m_LightDir[count] = D3DXVECTOR4(lightDir.x, lightDir.y, lightDir.z, 1.0f);
		m_LightDiffuse[count] = light->Get()->GetMaterial().GetDiffuse();
	}
}

void LightManager::CalcLighting(ConstantBuffer_Lighting* pLighting, SceneNode* pNode)
{
	int count = GetLightCount(pNode);
	if (count)
	{
		// fill out the cbuffer struct with lighting data for the shader
		pLighting->m_LightAmbient = *GetLightAmbient(pNode);
		memcpy(pLighting->m_LightDir, GetLightDirection(pNode), sizeof(Vec4) * count);
		memcpy(pLighting->m_LightDiffuse, GetLightDiffuse(pNode), sizeof(Vec4) * count);
		pLighting->m_NumLights = count;
	}
}

int LightManager::GetLightCount(const SceneNode* node)
{
	return m_Lights.size();
}

const Vec4* LightManager::GetLightAmbient(const SceneNode* node)
{
	return &m_LightAmbient;
}

const Vec4* LightManager::GetLightDirection(const SceneNode* node)
{
	return m_LightDir;
}

const Color* LightManager::GetLightDiffuse(const SceneNode* node)
{
	return m_LightDiffuse;
}
