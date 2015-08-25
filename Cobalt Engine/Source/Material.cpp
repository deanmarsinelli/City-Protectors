/*
	Material.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Material.h"

#include "EngineStd.h"

Material::Material()
{
	// zero out memory and set the starting color to white
	ZeroMemory(&m_D3DMaterial, sizeof(D3DMATERIAL9));
	m_D3DMaterial.Diffuse = g_White;
	m_D3DMaterial.Ambient = Color(0.1f, 0.1f, 0.1f, 1.0f);
	m_D3DMaterial.Specular = g_White;
	// no emissive light
	m_D3DMaterial.Emissive = g_Black; 
}

void Material::SetAmbient(const Color& color)
{
	m_D3DMaterial.Ambient = color;
}

const Color Material::GetAmbient() const
{
	return m_D3DMaterial.Ambient;
}

void Material::SetDiffuse(const Color& color)
{
	m_D3DMaterial.Diffuse = color;
}

const Color Material::GetDiffuse() const
{
	return m_D3DMaterial.Diffuse;
}

void Material::SetSpecular(const Color& color, const float power)
{
	m_D3DMaterial.Specular = color;
	m_D3DMaterial.Power = power;
}

void Material::GetSpecular(Color& color, float& power) const
{
	color = m_D3DMaterial.Specular;
	power = m_D3DMaterial.Power;
}

void Material::SetEmissive(const Color& color)
{
	m_D3DMaterial.Emissive = color;
}

const Color Material::GetEmissive() const
{
	return m_D3DMaterial.Emissive;
}

void Material::SetAlpha(const float alpha)
{
	m_D3DMaterial.Diffuse.a = alpha;
}

bool Material::HasAlpha() const
{
	return GetAlpha() != fOPAQUE;
}

float Material::GetAlpha() const
{
	return m_D3DMaterial.Diffuse.a;
}

void Material::D3DUse9()
{
	DXUTGetD3D9Device()->SetMaterial(&m_D3DMaterial);
}
