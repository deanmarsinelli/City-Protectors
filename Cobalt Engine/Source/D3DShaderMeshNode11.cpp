/*
	D3DShaderMeshNode11.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DMesh.h"
#include "D3DShaderMeshNode11.h"
#include "Raycast.h"
#include "Resource.h"
#include "ResourceCache.h"
#include "ResourceHandle.h"
#include "Scene.h"
#include "WindowsApp.h"

D3DShaderMeshNode11::D3DShaderMeshNode11(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
	std::string sdkMeshFileName, RenderPass renderPass, const Mat4x4* mat) :
	SceneNode(objectId, renderComponent, renderPass, mat)
{
	m_sdkMeshFileName = sdkMeshFileName;
}

HRESULT D3DShaderMeshNode11::OnRestore(Scene* pScene)
{
	HRESULT hr;

	V_RETURN(SceneNode::OnRestore(pScene));
	V_RETURN(m_VertexShader.OnRestore(pScene));
	V_RETURN(m_PixelShader.OnRestore(pScene));

	// reload the mesh
	Resource resource(m_sdkMeshFileName);
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);
	shared_ptr<D3DSdkMeshResourceExtraData11> extra = static_pointer_cast<D3DSdkMeshResourceExtraData11>(pResourceHandle->GetExtra());

	SetRadius(CalcBoundingSphere(&extra->m_Mesh11));

	return S_OK;
}

HRESULT D3DShaderMeshNode11::Render(Scene* pScene)
{
	HRESULT hr;

	V_RETURN(m_VertexShader.SetupRender(pScene, this));
	V_RETURN(m_PixelShader.SetupRender(pScene, this));

	// get the mesh from cache
	Resource resource(m_sdkMeshFileName);
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);
	shared_ptr<D3DSdkMeshResourceExtraData11> extra = static_pointer_cast<D3DSdkMeshResourceExtraData11>(pResourceHandle->GetExtra());

	// set up buffers
	UINT strides[1];
	UINT offsets[1];
	ID3D11Buffer* pVertexBuffer[1];
	pVertexBuffer[0] = extra->m_Mesh11.GetVB11(0, 0);
	strides[0] = (UINT)extra->m_Mesh11.GetVertexStride(0, 0);
	offsets[0] = 0;
	DXUTGetD3D11DeviceContext()->IASetVertexBuffers(0, 1, pVertexBuffer, strides, offsets);
	DXUTGetD3D11DeviceContext()->IASetIndexBuffer(extra->m_Mesh11.GetIB11(0), extra->m_Mesh11.GetIBFormat11(0), 0);

	// render
	D3D11_PRIMITIVE_TOPOLOGY primType;
	for (UINT subset = 0; subset < extra->m_Mesh11.GetNumSubsets(0); ++subset)
	{
		SDKMESH_SUBSET* pSubset = extra->m_Mesh11.GetSubset(0, subset);

		primType = CDXUTSDKMesh::GetPrimitiveType11((SDKMESH_PRIMITIVE_TYPE)pSubset->PrimitiveType);
		DXUTGetD3D11DeviceContext()->IASetPrimitiveTopology(primType);

		ID3D11ShaderResourceView* pDiffuseRV = extra->m_Mesh11.GetMaterial(pSubset->MaterialID)->pDiffuseRV11;
		DXUTGetD3D11DeviceContext()->PSSetShaderResources(0, 1, &pDiffuseRV);

		DXUTGetD3D11DeviceContext()->DrawIndexed((UINT)pSubset->IndexCount, 0, (UINT)pSubset->VertexStart);
	}

	return S_OK;
}

HRESULT D3DShaderMeshNode11::Pick(Scene* pScene, RayCast* pRayCast)
{
	if (SceneNode::Pick(pScene, pRayCast) == E_FAIL)
	{
		return E_FAIL;
	}

	pScene->PushAndSetMatrix(m_Properties.ToWorld());

	// get the mesh from the resource cache
	Resource resource(m_sdkMeshFileName);
	shared_ptr<ResHandle> pResourceHandle = g_pApp->m_ResCache->GetHandle(&resource);
	shared_ptr<D3DSdkMeshResourceExtraData11> extra = static_pointer_cast<D3DSdkMeshResourceExtraData11>(pResourceHandle->GetExtra());
	
	HRESULT hr = pRayCast->Pick(pScene, m_Properties.ObjectId(), &extra->m_Mesh11);
	pScene->PopMatrix();

	return hr;
}

float D3DShaderMeshNode11::CalcBoundingSphere(CDXUTSDKMesh* mesh11)
{
	float radius = 0.0f;
	for (UINT subset = 0; subset < mesh11->GetNumSubsets(0); ++subset)
	{
		Vec3 extents = mesh11->GetMeshBBoxExtents(subset);
		extents.x = abs(extents.x);
		extents.y = abs(extents.y);
		extents.z = abs(extents.z);
		radius = (radius > extents.x) ? radius : extents.x;
		radius = (radius > extents.y) ? radius : extents.y;
		radius = (radius > extents.z) ? radius : extents.z;
	}

	return radius;
}