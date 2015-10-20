/*
	D3DShaderMeshNode9.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <DXUT.h>
#include <SDKmisc.h>

#include "D3DShaderMeshNode9.h"
#include "LightManager.h"
#include "Raycast.h"
#include "Resource.h"
#include "ResourceHandle.h"
#include "ResourceCache.h"
#include "Scene.h"

//====================================================
//	D3DMeshNode9 Definitions
//====================================================
D3DMeshNode9::D3DMeshNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
	ID3DXMesh* mesh, RenderPass renderPass, const Mat4x4* t) :
	D3DSceneNode9(objectId, renderComponent, renderPass, t)
{
	m_pMesh = mesh;
	if (m_pMesh)
	{
		m_pMesh->AddRef();
	}
}

D3DMeshNode9::D3DMeshNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
	std::wstring meshFileName, RenderPass renderPass, const Mat4x4* t) :
	D3DSceneNode9(objectId, renderComponent, renderPass, t)
{
	m_pMesh = nullptr;
	m_FileName = meshFileName;
}

HRESULT D3DMeshNode9::Render(Scene* pScene)
{
	if (S_OK != D3DSceneNode9::Render(pScene))
		return E_FAIL;

	return m_pMesh->DrawSubset(0);
}

HRESULT D3DMeshNode9::OnRestore(Scene* pScene)
{
	if (m_FileName.empty())
	{
		SetRadius(CalcBoundingSphere());
		return D3DSceneNode9::OnRestore(pScene);
	}

	// release the mesh if we have a valid mesh file name to load
	// Otherwise we likely created it on our own, and needs to be kept.
	SAFE_RELEASE(m_pMesh);

	WCHAR str[MAX_PATH];
	HRESULT hr;

	// Load the Mesh with D3DX and get back a ID3DXMesh*.  For this
	// sample we'll ignore the X file's embedded materials since we know 
	// exactly the model we're loading.  See the Mesh samples such as
	// "OptimizedMesh" for a more generic Mesh loading example.
	V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, m_FileName.c_str()));

	V_RETURN(D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, DXUTGetD3D9Device(), NULL, NULL, NULL, NULL, &m_pMesh));

	DWORD *rgdwAdjacency = NULL;

	// Make sure there are normals which are required for lighting
	if (!(m_pMesh->GetFVF() & D3DFVF_NORMAL))
	{
		ID3DXMesh* pTempMesh;
		V(m_pMesh->CloneMeshFVF(m_pMesh->GetOptions(),
			m_pMesh->GetFVF() | D3DFVF_NORMAL,
			DXUTGetD3D9Device(), &pTempMesh));
		V(D3DXComputeNormals(pTempMesh, NULL));

		CB_COM_RELEASE(m_pMesh);
		m_pMesh = pTempMesh;
	}

	// Optimize the Mesh for this graphics card's vertex cache 
	// so when rendering the Mesh's triangle list the vertices will 
	// cache hit more often so it won't have to re-execute the vertex shader 
	// on those vertices so it will improve perf.     

	rgdwAdjacency = CB_NEW DWORD[m_pMesh->GetNumFaces() * 3];
	if (rgdwAdjacency == NULL)
		return E_OUTOFMEMORY;
	V(m_pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency));
	V(m_pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL));

	CB_SAFE_DELETE_ARRAY(rgdwAdjacency);

	SetRadius(CalcBoundingSphere());

	return D3DSceneNode9::OnRestore(pScene);
}

HRESULT D3DMeshNode9::Pick(Scene* pScene, RayCast* pRayCast)
{
	if (SceneNode::Pick(pScene, pRayCast) == E_FAIL)
		return E_FAIL;

	pScene->PushAndSetMatrix(m_Properties.ToWorld());
	HRESULT hr = pRayCast->Pick(pScene, m_Properties.ObjectId(), m_pMesh);
	pScene->PopMatrix();

	return hr;
}

float D3DMeshNode9::CalcBoundingSphere()
{
	D3DXVECTOR3* pData;
	D3DXVECTOR3 center;
	FLOAT objectRadius;
	HRESULT hr;
	V(m_pMesh->LockVertexBuffer(0, (LPVOID*)&pData));
	V(D3DXComputeBoundingSphere(pData, m_pMesh->GetNumVertices(),
		D3DXGetFVFVertexSize(m_pMesh->GetFVF()), &center, &objectRadius));
	V(m_pMesh->UnlockVertexBuffer());

	return objectRadius;
}


//====================================================
//	D3DShaderMeshNode9 Definitions
//====================================================
D3DShaderMeshNode9::D3DShaderMeshNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
	ID3DXMesh* mesh, std::string fxFileName, RenderPass renderPass, const Mat4x4* t) :
	D3DMeshNode9(objectId, renderComponent, mesh, renderPass, t)
{
	m_pEffect = nullptr;
	m_FXFileName = fxFileName;
}

D3DShaderMeshNode9::D3DShaderMeshNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
	std::wstring meshFileName, std::string fxFileName, RenderPass renderPass, const Mat4x4* t) :
	D3DMeshNode9(objectId, renderComponent, meshFileName, renderPass, t)
{
	m_pEffect = nullptr;
	m_FXFileName = fxFileName;
}

HRESULT D3DShaderMeshNode9::OnRestore(Scene *pScene)
{
	CB_COM_RELEASE(m_pEffect);

	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_DEBUG | D3DXSHADER_NO_PRESHADER;
	HRESULT hr;

	Resource resource(m_FXFileName);
	// load the actual xml file from the zip file
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource); 

	// create the d3d effect
	V(D3DXCreateEffect(DXUTGetD3D9Device(), pResourceHandle->Buffer(), pResourceHandle->Size(), NULL, NULL, dwShaderFlags, NULL, &m_pEffect, NULL));
	
	return D3DMeshNode9::OnRestore(pScene);
}

HRESULT D3DShaderMeshNode9::OnLostDevice(Scene *pScene)
{
	CB_COM_RELEASE(m_pEffect);

	HRESULT hr;
	V_RETURN(D3DMeshNode9::OnLostDevice(pScene));
	return S_OK;
}

HRESULT D3DShaderMeshNode9::Render(Scene *pScene)
{
	if (S_OK != D3DSceneNode9::Render(pScene))
		return E_FAIL;

	HRESULT hr;

	// Update the effect's variables.  Instead of using strings, it would 
	// be more efficient to cache a handle to the parameter by calling 
	// ID3DXEffect::GetParameterByName

	Mat4x4 worldViewProj = pScene->GetCamera()->GetWorldViewProjection(pScene);
	Mat4x4 world = pScene->GetTopMatrix();

	D3DXCOLOR ambient = m_Properties.GetMaterial().GetAmbient();
	V_RETURN(m_pEffect->SetValue("g_MaterialAmbientColor", &ambient, sizeof(D3DXCOLOR)));
	D3DXCOLOR diffuse = m_Properties.GetMaterial().GetDiffuse();
	V_RETURN(m_pEffect->SetValue("g_MaterialDiffuseColor", &diffuse, sizeof(D3DXCOLOR)));

	V(m_pEffect->SetMatrix("g_mWorldViewProjection", &worldViewProj));
	V(m_pEffect->SetMatrix("g_mWorld", &world));
	V(m_pEffect->SetFloat("g_fTime", (float)1.0f));

	int count = pScene->GetLightManager()->GetLightCount(this);
	if (count)
	{
		// Light 0 is the only one we use for ambient lighting. The rest are ignored in this simple shader.
		V(m_pEffect->SetValue("g_LightAmbient", pScene->GetLightManager()->GetLightAmbient(this), sizeof(D3DXVECTOR4)* 1));
		V(m_pEffect->SetValue("g_LightDir", pScene->GetLightManager()->GetLightDirection(this), sizeof(D3DXVECTOR4)* MAXIMUM_LIGHTS_SUPPORTED));
		V(m_pEffect->SetValue("g_LightDiffuse", pScene->GetLightManager()->GetLightDiffuse(this), sizeof(D3DXVECTOR4)* MAXIMUM_LIGHTS_SUPPORTED));
		V(m_pEffect->SetInt("g_nNumLights", count));
	}

	float alpha = m_Properties.GetMaterial().GetAlpha();
	V(m_pEffect->SetFloat("g_fAlpha", alpha));
	V(m_pEffect->SetTechnique((alpha < 1.0f) ? "RenderSceneWithAlpha" : "RenderScene"));

	// Apply the technique contained in the effect 
	UINT iPass, cPasses;

	V(m_pEffect->Begin(&cPasses, 0));

	for (iPass = 0; iPass < cPasses; iPass++)
	{
		V(m_pEffect->BeginPass(iPass));

		// The effect interface queues up the changes and performs them 
		// with the CommitChanges call. You do not need to call CommitChanges if 
		// you are not setting any parameters between the BeginPass and EndPass.
		// V( g_pEffect->CommitChanges() );

		// Render the Mesh with the applied technique
		V(m_pMesh->DrawSubset(0));

		V(m_pEffect->EndPass());
	}
	V(m_pEffect->End());

	return S_OK;
}
