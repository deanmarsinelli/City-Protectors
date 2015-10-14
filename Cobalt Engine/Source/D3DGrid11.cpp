/*
	D3DGrid11.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3D11Vertex.h"
#include "D3DGrid11.h"
#include "EngineStd.h"
#include "Logger.h"
#include "RenderComponent.h"

D3DGrid11::D3DGrid11(GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, const Mat4x4* pMatrix) :
SceneNode(objectId, renderComponent, RenderPass_0, pMatrix)
{
	m_TextureHasAlpha = false;
	m_NumPolys = 0;
	m_NumVerts = 0;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

D3DGrid11::~D3DGrid11()
{
	CB_COM_RELEASE(m_pVertexBuffer);
	CB_COM_RELEASE(m_pIndexBuffer);
}

HRESULT D3DGrid11::OnRestore(Scene* pScene)
{
	HRESULT hr;

	V_RETURN(SceneNode::OnRestore(pScene));

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);

	V_RETURN(m_VertexShader.OnRestore(pScene));
	V_RETURN(m_PixelShader.OnRestore(pScene));

	GridRenderComponent* grc = static_cast<GridRenderComponent*>(m_RenderComponent);

	int squares = grc->GetDivision();

	SetRadius(sqrt(squares * squares / 2.0f));

	// Create the vertex buffer - we'll need enough verts
	// to populate the grid. If we want a 2x2 grid, we'll
	// need 3x3 set of verts.
	m_NumVerts = (squares + 1)*(squares + 1);    // Create vertex buffer

	// Fill the vertex buffer. We are setting the tu and tv texture
	// coordinates, which range from 0.0 to 1.0
	D3D11Vertex_UnlitTextured *pVerts = CB_NEW D3D11Vertex_UnlitTextured[m_NumVerts];
	CB_ASSERT(pVerts && "Out of memory in D3DGrid11::OnRestore()");
	if (!pVerts)
		return E_FAIL;

	for (int j = 0; j < (squares + 1); j++)
	{
		for (int i = 0; i < (squares + 1); i++)
		{
			// Which vertex are we setting?
			int index = i + (j * (squares + 1));
			D3D11Vertex_UnlitTextured *vert = &pVerts[index];

			// Default position of the grid is centered on the origin, flat on
			// the XZ plane.
			float x = (float)i - (squares / 2.0f);
			float y = (float)j - (squares / 2.0f);
			vert->Pos = Vec3(x, 0.f, y);
			vert->Normal = Vec3(0.0f, 1.0f, 0.0f);

			// The texture coordinates are set to x,y to make the
			// texture tile along with units - 1.0, 2.0, 3.0, etc.
			vert->Uv.x = x;
			vert->Uv.y = y;
		}
	}

	// The number of indicies equals the number of polygons times 3
	// since there are 3 indicies per polygon. Each grid square contains
	// two polygons. The indicies are 16 bit, since our grids won't
	// be that big!

	m_NumPolys = squares * squares * 2;

	WORD *pIndices = CB_NEW WORD[m_NumPolys * 3];

	CB_ASSERT(pIndices && "Out of memory in D3DGrid11::VOnRestore()");
	if (!pIndices)
		return E_FAIL;

	// Loop through the grid squares and calc the values
	// of each index. Each grid square has two triangles:
	//
	//		A - B
	//		| / |
	//		C - D

	WORD *current = pIndices;
	for (int j = 0; j < squares; j++)
	{
		for (int i = 0; i < squares; i++)
		{
			// Triangle #1  ACB
			*(current) = WORD(i + (j * (squares + 1)));
			*(current + 1) = WORD(i + ((j + 1) * (squares + 1)));
			*(current + 2) = WORD((i + 1) + (j * (squares + 1)));

			// Triangle #2  BCD
			*(current + 3) = WORD((i + 1) + (j * (squares + 1)));
			*(current + 4) = WORD(i + ((j + 1) * (squares + 1)));
			*(current + 5) = WORD((i + 1) + ((j + 1) * (squares + 1)));
			current += 6;
		}
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(D3D11Vertex_UnlitTextured)* m_NumVerts;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = pVerts;
	hr = DXUTGetD3D11Device()->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
	if (SUCCEEDED(hr))
	{
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD)* m_NumPolys * 3;        // 36 vertices needed for 12 triangles in a triangle list
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = pIndices;
		hr = DXUTGetD3D11Device()->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
	}

	CB_SAFE_DELETE_ARRAY(pVerts);
	CB_SAFE_DELETE_ARRAY(pIndices);

	return hr;
}

HRESULT D3DGrid11::Render(Scene* pScene)
{
	HRESULT hr;

	GridRenderComponent* grid = static_cast<GridRenderComponent*>(m_RenderComponent);
	m_PixelShader.SetTexture(grid->GetTextureResource());

	V_RETURN(m_VertexShader.SetupRender(pScene, this));
	V_RETURN(m_PixelShader.SetupRender(pScene, this));

	// set vertex buffer
	UINT stride = sizeof(D3D11Vertex_UnlitTextured);
	UINT offset = 0;
	DXUTGetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// set index buffer
	DXUTGetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// set primitive topology
	DXUTGetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the polygon
	DXUTGetD3D11DeviceContext()->DrawIndexed(m_NumPolys * 3, 0, 0);

	return S_OK;
}
