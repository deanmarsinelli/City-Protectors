/*
	D3DGrid9.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3D9Vertex.h"
#include "D3DGrid9.h"
#include "D3DTextureResourceExtraData.h"
#include "EngineStd.h"
#include "Raycast.h"
#include "RenderComponent.h"
#include "ResourceCache.h"
#include "Scene.h"
#include "Vector.h"

D3DGrid9::D3DGrid9(GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, const Mat4x4* pMatrix) :
SceneNode(objectId, renderComponent, RenderPass_0, pMatrix)
{
	m_TextureHasAlpha = false;
	m_pVerts = nullptr;
	m_pIndices = nullptr;
	m_NumVerts = 0;
	m_NumPolys = 0;
}

D3DGrid9::~D3DGrid9()
{
	CB_COM_RELEASE(m_pVerts);
	CB_COM_RELEASE(m_pIndices);
}

HRESULT D3DGrid9::OnRestore(Scene* pScene)
{
	// call base class restore
	SceneNode::OnRestore(pScene);

	GridRenderComponent* grid = static_cast<GridRenderComponent*>(m_RenderComponent);

	int squares = grid->GetDivision();

	CB_COM_RELEASE(m_pVerts);
	CB_COM_RELEASE(m_pIndices);

	SetRadius(sqrt(squares * squares / 2.0f));

	// create vertex buffer
	m_NumVerts = (squares + 1) * (squares + 1);
	if (FAILED(DXUTGetD3D9Device()->CreateVertexBuffer(
		m_NumVerts * sizeof(D3D9Vertex_ColoredTextured),
		D3DUSAGE_WRITEONLY, D3D9Vertex_ColoredTextured::FVF,
		D3DPOOL_MANAGED, &m_pVerts, NULL)))
	{
		return E_FAIL;
	}

	// fill the vertex buffer
	D3D9Vertex_ColoredTextured* pVertices;
	if (FAILED(m_pVerts->Lock(0, 0, (void**)&pVertices, 0)))
	{
		return E_FAIL;
	}

	for (int j = 0; j < (squares + 1); j++)
	{
		for (int i = 0; i < (squares + 1); i++)
		{
			// Which vertex are we setting?
			int index = i + (j * (squares + 1));
			D3D9Vertex_ColoredTextured *vert = &pVertices[index];

			// Default position of the grid is centered on the origin, flat on
			// the XZ plane.
			float x = (float)i - (squares / 2.0f);
			float y = (float)j - (squares / 2.0f);
			vert->position = (x * Vec3(1.0f, 0.0f, 0.0f)) + (y * Vec3(0.0f, 0.0f, 1.0f));
			vert->color = m_Properties.GetMaterial().GetDiffuse();

			// The texture coordinates are set to x,y to make the
			// texture tile along with units - 1.0, 2.0, 3.0, etc.
			vert->tu = x;
			vert->tv = y;
		}
	}
	m_pVerts->Unlock();

	// The number of indicies equals the number of polygons times 3
	// since there are 3 indicies per polygon. Each grid square contains
	// two polygons. The indicies are 16 bit, since our grids won't
	// be that big!
	m_NumPolys = squares * squares * 2;
	if (FAILED(DXUTGetD3D9Device()->CreateIndexBuffer(sizeof(WORD) * m_NumPolys * 3,
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndices, NULL)))
	{
		return E_FAIL;
	}

	WORD* pIndices;
	if (FAILED(m_pIndices->Lock(0, 0, (void**)&pIndices, 0)))
		return E_FAIL;

	// Loop through the grid squares and calc the values
	// of each index. Each grid square has two triangles:
	//
	//		A - B
	//		| / |
	//		C - D

	for (int j = 0; j < squares; j++)
	{
		for (int i = 0; i < squares; i++)
		{
			// Triangle #1  ACB
			*(pIndices) = WORD(i + (j * (squares + 1)));
			*(pIndices + 1) = WORD(i + ((j + 1) * (squares + 1)));
			*(pIndices + 2) = WORD((i + 1) + (j * (squares + 1)));

			// Triangle #2  BCD
			*(pIndices + 3) = WORD((i + 1) + (j * (squares + 1)));
			*(pIndices + 4) = WORD(i + ((j + 1) * (squares + 1)));
			*(pIndices + 5) = WORD((i + 1) + ((j + 1) * (squares + 1)));
			pIndices += 6;
		}
	}

	m_pIndices->Unlock();

	return S_OK;
}

HRESULT D3DGrid9::Render(Scene* pScene)
{
	DWORD oldLightMode;
	DXUTGetD3D9Device()->GetRenderState(D3DRS_LIGHTING, &oldLightMode);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, FALSE);

	DWORD oldCullMode;
	DXUTGetD3D9Device()->GetRenderState(D3DRS_CULLMODE, &oldCullMode);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Setup our texture. Using textures introduces the texture stage states,
	// which govern how textures get blended together (in the case of multiple
	// textures) and lighting information. In this case, we are modulating
	// (blending) our texture with the diffuse color of the vertices.

	GridRenderComponent* grc = static_cast<GridRenderComponent*>(m_RenderComponent);

	Resource resource(grc->GetTextureResource());
	shared_ptr<ResHandle> texture = g_pApp->m_ResCache->GetHandle(&resource);
	shared_ptr<D3DTextureResourceExtraData9> extra = static_pointer_cast<D3DTextureResourceExtraData9>(texture->GetExtra());
	DXUTGetD3D9Device()->SetTexture(0, extra->GetTexture());

	if (m_TextureHasAlpha)
	{
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	}
	else
	{
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	}

	DXUTGetD3D9Device()->SetStreamSource(0, m_pVerts, 0, sizeof(D3D9Vertex_ColoredTextured));
	DXUTGetD3D9Device()->SetIndices(m_pIndices);
	DXUTGetD3D9Device()->SetFVF(D3D9Vertex_ColoredTextured::FVF);
	DXUTGetD3D9Device()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_NumVerts, 0, m_NumPolys);


	DXUTGetD3D9Device()->SetTexture(0, NULL);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, oldLightMode);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_CULLMODE, oldCullMode);

	return S_OK;
}

HRESULT D3DGrid9::Pick(Scene* pScene, RayCast* pRayCast)
{
	if (SceneNode::Pick(pScene, pRayCast) == E_FAIL)
		return E_FAIL;

	pScene->PushAndSetMatrix(m_Properties.ToWorld());

	HRESULT hr = pRayCast->Pick(pScene, m_Properties.ObjectId(), m_pVerts, m_pIndices, m_NumPolys);

	pScene->PopMatrix();

	return hr;
}
