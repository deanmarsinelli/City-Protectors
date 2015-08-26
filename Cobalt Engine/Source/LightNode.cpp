/*
	LightNode.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "LightNode.h"
#include "RenderComponent.h"
#include "Vector.h"

LightNode::LightNode(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, const LightProperties& properties, const Mat4x4* mat) :
SceneNode(objectId, renderComponent, RenderPass::RenderPass_NotRendered, mat)
{
	m_LightProperties = properties;
}

HRESULT D3DLightNode9::OnRestore(Scene* pScene)
{
	ZeroMemory(&m_d3dLight9, sizeof(D3DLIGHT9));
	m_d3dLight9.Type = D3DLIGHT_DIRECTIONAL;

	// these parameters are constant for the list after the scene is loaded
	m_d3dLight9.Range = m_LightProperties.m_Range;
	m_d3dLight9.Falloff = m_LightProperties.m_Falloff;
	m_d3dLight9.Attenuation0 = m_LightProperties.m_Attenuation[0];
	m_d3dLight9.Attenuation1 = m_LightProperties.m_Attenuation[0];
	m_d3dLight9.Attenuation2 = m_LightProperties.m_Attenuation[0];
	m_d3dLight9.Theta = m_LightProperties.m_Theta;
	m_d3dLight9.Phi = m_LightProperties.m_Phi;

	return S_OK;
}

HRESULT D3DLightNode9::OnUpdate(Scene* pScene, const float deltaTime)
{
	LightRenderComponent* lightComponent = static_cast<LightRenderComponent*>(m_RenderComponent);
	m_Properties.GetMaterial().SetDiffuse(lightComponent->GetColor());

	m_d3dLight9.Diffuse = m_Properties.GetMaterial().GetDiffuse();
	float power;
	Color spec;
	m_Properties.GetMaterial().GetSpecular(spec, power);
	m_d3dLight9.Specular = spec;
	m_d3dLight9.Ambient = m_Properties.GetMaterial().GetAmbient();

	m_d3dLight9.Position = GetPosition();
	m_d3dLight9.Direction = GetDirection();

	return S_OK;
}


HRESULT D3DLightNode11::OnUpdate(Scene* pScene, const float deltaTime)
{
	LightRenderComponent* lightComponent = static_cast<LightRenderComponent*>(m_RenderComponent);
	m_Properties.GetMaterial().SetDiffuse(lightComponent->GetColor());
	return S_OK;
}
