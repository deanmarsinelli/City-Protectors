/*
	Material.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3d9types.h>

#include "EngineStd.h"

/**
	Represents how a object (or surface) reflects light. Typically the diffuse
	and ambient are set to the same value and the specular is set to white or gray.
*/
class Material
{
public:
	/// Default constructor
	Material();

	/// Set the ambient component of the material
	void SetAmbient(const Color& color);

	/// Return the ambient component of the material
	const Color GetAmbient() const;

	/// Set the diffuse component of the material
	void SetDiffuse(const Color& color);

	/// Return the diffuse component of the material
	const Color GetDiffuse() const;

	/// Set the specular component of the material
	void SetSpecular(const Color& color, const float power);

	/// Return the specular component of the material by passing references
	void GetSpecular(Color& color, float& power) const;

	/// Set the emissive component of the material
	void SetEmissive(const Color& color);

	/// Return the emissive component of the material
	const Color GetEmissive() const;

	/// Set the transparency of the diffuse component
	void SetAlpha(const float alpha);

	/// Return whether or not there is transparency in the diffuse component
	bool HasAlpha();

	/// Return the alpha value of the diffuse component
	float GetAlpha();

	/// Set material properties if using a D3D9 device
	void D3DUse9();

private:
	/// The D3D material
	D3DMATERIAL9 m_D3DMaterial;
};