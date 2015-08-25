/*
	Shaders.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Shaders.h"

#include "D3D11Vertex.h"
#include "D3DRenderer11.h"
#include "EngineStd.h"
#include "Resource.h"
#include "ResourceCache.h"
#include "ResourceHandle.h"
#include "Scene.h"

//====================================================
//	Vertex Shader helper methods
//====================================================
Hlsl_VertexShader::Hlsl_VertexShader()
{
	m_pVertexLayout11 = nullptr;
	m_pVertexShader = nullptr;
	m_pcbVSMatrices = nullptr;
	m_pcbVSMaterial = nullptr;
	m_pcbVSLighting = nullptr;
	m_EnableLights = true;
}

Hlsl_VertexShader::~Hlsl_VertexShader()
{
	// release all resources
	CB_COM_RELEASE(m_pVertexLayout11);
	CB_COM_RELEASE(m_pVertexShader);
	CB_COM_RELEASE(m_pcbVSMatrices);
	CB_COM_RELEASE(m_pcbVSMaterial);
	CB_COM_RELEASE(m_pcbVSLighting);
}

HRESULT Hlsl_VertexShader::OnRestore(Scene* pScene)
{
	HRESULT hr = S_OK;

	// reset all resources
	CB_COM_RELEASE(m_pVertexLayout11);
	CB_COM_RELEASE(m_pVertexShader);
	CB_COM_RELEASE(m_pcbVSMatrices);
	CB_COM_RELEASE(m_pcbVSMaterial);
	CB_COM_RELEASE(m_pcbVSLighting);

	shared_ptr<D3DRenderer11> d3dRenderer11 = static_pointer_cast<D3DRenderer11>(pScene->GetRenderer());

	// set up vertex shader and related constant buffers (cbuffers)
	// compile the vertex shader using the lowest possible profile
	ID3DBlob* pVertexShaderBuffer = nullptr;

	/// get a handle to the vertex shader from the resource cache
	std::string hlslFileName = "Main_VS.hlsl";
	Resource resource(hlslFileName.c_str());
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource); // this loads it from a zip file
	if (FAILED(d3dRenderer11->CompileShader(pResourceHandle->Buffer(), pResourceHandle->Size(), 
		hlslFileName.c_str(), "Main_VS", "vs_4_0_level_9_1", &pVertexShaderBuffer)))
	{
		CB_COM_RELEASE(pVertexShaderBuffer);
		return hr;
	}

	if (FAILED(DXUTGetD3D11Device()->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), nullptr, &m_pVertexShader)))
	{
		CB_COM_RELEASE(pVertexShaderBuffer);
		return hr;
	}
	// set the debugging main function for this shader
	DXUT_SetDebugName(m_pVertexShader, "Main_VS");

	// set the shader input layout
	if (SUCCEEDED(DXUTGetD3D11Device()->CreateInputLayout(D3D11VertexLayout_UnlitTextured, ARRAYSIZE(D3D11VertexLayout_UnlitTextured), 
		pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &m_pVertexLayout11)))
	{
		DXUT_SetDebugName(m_pVertexLayout11, "Primary");

		// set up constant buffers
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		desc.ByteWidth = sizeof(ConstantBuffer_Matrices);
		V_RETURN(DXUTGetD3D11Device()->CreateBuffer(&desc, nullptr, &m_pcbVSMatrices));
		DXUT_SetDebugName(m_pcbVSMatrices, "ConstantBuffer_Matrices");

		desc.ByteWidth = sizeof(ConstantBuffer_Lighting);
		V_RETURN(DXUTGetD3D11Device()->CreateBuffer(&desc, nullptr, &m_pcbVSLighting));
		DXUT_SetDebugName(m_pcbVSMatrices, "ConstantBuffer_Lighting");

		desc.ByteWidth = sizeof(ConstantBuffer_Material);
		V_RETURN(DXUTGetD3D11Device()->CreateBuffer(&desc, nullptr, &m_pcbVSMaterial));
		DXUT_SetDebugName(m_pcbVSMatrices, "ConstantBuffer_Material");
	}

	CB_COM_RELEASE(pVertexShaderBuffer);

	return S_OK;
}

HRESULT Hlsl_VertexShader::SetupRender(Scene* pScene, SceneNode* pNode)
{
	HRESULT hr = S_OK;

	// set the vertex shader and layout
	DXUTGetD3D11DeviceContext()->VSSetShader(m_pVertexShader, nullptr, 0);
	DXUTGetD3D11DeviceContext()->IASetInputLayout(m_pVertexLayout11);

	// get the worldViewProjection matrix from the camera class
	Mat4x4 mWorldViewProjection = pScene->GetCamera()->GetWorldViewProjection(pScene);
	// get the world matrix from the scene
	Mat4x4 mWorld = pScene->GetTopMatrix();

	
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	// fill the d3d buffers used in the shaders with data from the scene and send this data to the graphics card

	// set the transform matrices for the shader matrix cbuffer
	// map the "MappedResource" to the pcbVSMatrices buffer, then get a pointer to its pData
	// and fill the pData members with the transpose of the WVP matrix and world matrix
	V(DXUTGetD3D11DeviceContext()->Map(m_pcbVSMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_Matrices* pVSMatrices = (ConstantBuffer_Matrices*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSMatrices->m_WorldViewProj, &mWorldViewProjection);
	D3DXMatrixTranspose(&pVSMatrices->m_World, &mWorld);
	DXUTGetD3D11DeviceContext()->Unmap(m_pcbVSMatrices, 0);


	// set the lighting data for the shader lighting cbuffer
	// this is the same as above, using the mapped resource to send data to the shader
	V(DXUTGetD3D11DeviceContext()->Map(m_pcbVSLighting, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_Lighting* pLighting = (ConstantBuffer_Lighting*)MappedResource.pData;
	if (m_EnableLights)
	{
		pScene->GetLightManager()->CalcLighting(pLighting, pNode);
	}
	else
	{
		// if lighting is disabled, set the number of lights to 0, with white ambient light
		pLighting->m_NumLights = 0;
		pLighting->m_LightAmbient = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	DXUTGetD3D11DeviceContext()->Unmap(m_pcbVSLighting, 0);


	// set the material data for the shader material cbuffer
	V(DXUTGetD3D11DeviceContext()->Map(m_pcbVSMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	ConstantBuffer_Material* pMaterial = (ConstantBuffer_Material*)MappedResource.pData;
	// get the diffuse material
	Color color = pNode->Get()->GetMaterial().GetDiffuse();
	pMaterial->m_DiffuseObjectColor = Vec4(color.r, color.g, color.b, color.a);
	// if lighting is enabled, get the ambient material otherwise set to white
	color = (m_EnableLights) ? pNode->Get()->GetMaterial().GetAmbient() : Color(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->m_HasTexture = false; // VS doesnt care about texture, so set to false
	DXUTGetD3D11DeviceContext()->Unmap(m_pcbVSMaterial, 0);
	

	// send the data to the graphics card
	DXUTGetD3D11DeviceContext()->VSSetConstantBuffers(0, 1, &m_pcbVSMatrices);
	DXUTGetD3D11DeviceContext()->VSSetConstantBuffers(1, 1, &m_pcbVSLighting);
	DXUTGetD3D11DeviceContext()->VSSetConstantBuffers(2, 1, &m_pcbVSMaterial);

	return S_OK;
}

void Hlsl_VertexShader::EnableLights(bool enableLights)
{
	m_EnableLights = enableLights;
}


//====================================================
//	Pixel Shader helper methods
//====================================================
