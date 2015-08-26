/*
	LightManager.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"
#include "Shaders.h"

class ConstantBuffer_Lighting;
class SceneNode;
class Vec4;

/**
	This class manages up to 8 lights for a given scene. It is used to calculate
	lighting for a vertex/pixel shader.
*/
class LightManager
{
	friend class Scene;

public:
	/// Calculate lighting data based on the scene
	void CalcLighting(Scene* pScene);

	/// Calculate lighting data and fill in the cbuffer structure for a shader
	void CalcLighting(ConstantBuffer_Lighting* pLighting, SceneNode* pNode);

	/// Return the number of lights in the scene
	int GetLightCount(const SceneNode* node);

	/// Get the ambient light color
	const Vec4* GetLightAmbient(const SceneNode* node);

	/// Get a pointer to an array of light direction vectors
	const Vec4* GetLightDirection(const SceneNode* node);

	/// Get a pointer to an array of colors for lights
	const Color* GetLightDiffuse(const SceneNode* node);

protected:
	/// Lights currently managed by this light manager
	Lights m_Lights;

	/// An array of light direction vectors
	Vec4 m_LightDir[MAXIMUM_LIGHTS_SUPPORTED];

	/// An array of light colors
	Color m_LightDiffuse[MAXIMUM_LIGHTS_SUPPORTED];
	
	/// Ambient light color for the scene
	Vec4 m_LightAmbient;
};