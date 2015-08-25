/*
	Shaders.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <D3D11.h>
#include <string>

#include "interfaces.h"
#include "Matrix.h"
#include "Vector.h"

class Scene;
class SceneNode;

const int MAXIMUM_LIGHTS_SUPPORTED = 8;

/**
	Struct that defines the shader cbuffer matrix data.
*/
struct ConstantBuffer_Matrices
{
	Mat4x4 m_WorldViewProj;
	Mat4x4 m_World;
};

/**
	Struct that defines the shader cbuffer material data.
*/
struct ConstantBuffer_Material
{
	Vec4 m_DiffuseObjectColor;
	Vec4 m_AmbientObjectColor;
	BOOL m_HasTexture;
	Vec3 m_Unused;
};

/**
	Struct that defines the shader cbuffer lighting data.
*/
struct ConstantBuffer_Lighting
{
	Vec4 m_LightDiffuse[MAXIMUM_LIGHTS_SUPPORTED];
	Vec4 m_LightDir[MAXIMUM_LIGHTS_SUPPORTED];
	Vec4 m_LightAmbient;
	UINT m_NumLights;
	Vec3 m_Unused;
};


/**
	Helper class for getting data to our default vertex shader.
*/
class Hlsl_VertexShader
{
public:
	/// Default constructor
	Hlsl_VertexShader();

	/// Default destructor
	~Hlsl_VertexShader();

	/// Recreate any lost resources or initialize the class
	HRESULT OnRestore(Scene* pScene);

	/// Send scene data to the shader to be rendered, called once per frame
	HRESULT SetupRender(Scene* pScene, SceneNode* pNode);
	
	/// Enable lighting in a shader
	void EnableLights(bool enableLights);

protected:
	/// Pointer to the loaded vertex shader
	ID3D11VertexShader* m_pVertexShader;

	/// Defines the layout of the vertices the shader expects (VS_INPUT)
	ID3D11InputLayout*	m_pVertexLayout11;

	/// Buffer for sending VS matrices to matrix cbuffer
	ID3D11Buffer* m_pcbVSMatrices;

	/// Buffer for sending VS lighting data to lighting cbuffer
	ID3D11Buffer* m_pcbVSLighting;

	/// Buffer for sending VS material data to material cbuffer
	ID3D11Buffer* m_pcbVSMaterial;

	/// Is lighting enabled?
	bool m_EnableLights;
};


class LineDraw_Hlsl_Shader
{
public:
	HRESULT OnRestore(Scene* pScene);
	HRESULT SetupRender(Scene* pScene);
	HRESULT SetDiffuse(const std::string& textureName, const Color& color);
	HRESULT SetTexture(const ID3D11ShaderResourceView** pDiffuseRV, const ID3D11SamplerState** pSamplers);
};