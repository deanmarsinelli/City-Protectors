/*
	D3DSkyNode9.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "CameraNode.h"
#include "D3D9Vertex.h"
#include "D3DSkyNode9.h"
#include "D3DTextureResourceExtraData.h"
#include "EngineStd.h"
#include "Resource.h"
#include "ResourceCache.h"
#include "ResourceHandle.h"
#include "Scene.h"

D3DSkyNode9::D3DSkyNode9(const char* pTextureBaseName) :
	SkyNode(pTextureBaseName)
{
	for (int i = 0; i < 5; ++i)
	{
		m_pTexture[i] = nullptr;
	}
	m_pVerts = nullptr;
}

D3DSkyNode9::~D3DSkyNode9()
{
	for (int i = 0; i < 5; ++i)
	{
		CB_COM_RELEASE(m_pTexture[i]);
	}
	CB_COM_RELEASE(m_pVerts);
}

HRESULT D3DSkyNode9::OnRestore(Scene* pScene)
{
	// call base class restore
	SceneNode::OnRestore(pScene);

	m_Camera = pScene->GetCamera();
	m_NumVerts = 20;

	CB_COM_RELEASE(m_pVerts);
	if (FAILED(DXUTGetD3D9Device()->CreateVertexBuffer(
		m_NumVerts * sizeof(D3D9Vertex_ColoredTextured),
		D3DUSAGE_WRITEONLY, D3D9Vertex_ColoredTextured::FVF,
		D3DPOOL_MANAGED, &m_pVerts, NULL)))
	{
		return E_FAIL;
	}

	// fill the vertex buffer -- we're setting the tu and tv texture coordinates (0.0 - 1.0)
	// this binds the m_pVerts d3d buffer to pVertices
	D3D9Vertex_ColoredTextured* pVertices;
	if (FAILED(m_pVerts->Lock(0, 0, (void**)&pVertices, 0)))
	{
		return E_FAIL;
	}

	// loop through the grid squares and calculate the values
	// of each index. each grid has two triangles:
	//
	//	A - B
	//	| / |
	//	C - D
	
	D3D9Vertex_ColoredTextured skyVerts[4];
	D3DCOLOR skyVertColor = 0xffffffff;
	float dim = 50.0f;

	skyVerts[0].position = Vec3(dim, dim, dim);
	skyVerts[0].color = skyVertColor;
	skyVerts[0].tu = 1.0f; // top right texture coord
	skyVerts[0].tv = 0.0f;

	skyVerts[1].position = Vec3(-dim, dim, dim);
	skyVerts[1].color = skyVertColor;
	skyVerts[1].tu = 0.0f; // top left texture coord
	skyVerts[1].tv = 0.0f;

	skyVerts[2].position = Vec3(dim, -dim, dim);
	skyVerts[2].color = skyVertColor;
	skyVerts[2].tu = 1.0f; // bottom right texture coord
	skyVerts[2].tv = 1.0f;

	skyVerts[3].position = Vec3(-dim, -dim, dim);
	skyVerts[3].color = skyVertColor;
	skyVerts[3].tu = 0.0f; // bottom left texture coord
	skyVerts[3].tv = 1.0f;

	// triangle
	//	   2
	//	 / |
	//  0--1
	Vec3 triangle[3];
	triangle[0] = Vec3(0.0f, 0.0f, 0.0f);
	triangle[1] = Vec3(5.0f, 0.0f, 0.0f);
	triangle[2] = Vec3(5.0f, 5.0f, 0.0f);
	
	Vec3 edge1 = triangle[1] - triangle[0];
	Vec3 edge2 = triangle[2] - triangle[0];

	Vec3 normal;
	normal = edge1.Cross(edge2);
	normal.Normalize();

	Mat4x4 rotY;
	rotY.BuildRotationY(CB_PI / 2.0f);
	Mat4x4 rotX;
	rotX.BuildRotationX(-CB_PI / 2.0f);

	m_Sides = 5;

	// loop through all the sides of a sky box and set the texture coords
	for (DWORD side = 0; side < m_Sides; side++)
	{
		for (DWORD v = 0; v < 4; v++)
		{
			Vec4 temp;
			if (side < m_Sides - 1)
			{
				temp = rotY.Transform(Vec3(skyVerts[v].position));
			}
			else
			{
				skyVerts[0].tu = 1.0f; skyVerts[0].tv = 1.0f;
				skyVerts[1].tu = 1.0f; skyVerts[1].tv = 0.0f;
				skyVerts[2].tu = 0.0f; skyVerts[2].tv = 1.0f;
				skyVerts[3].tu = 0.0f; skyVerts[3].tv = 0.0f;

				temp = rotX.Transform(Vec3(skyVerts[v].position));
			}
			skyVerts[v].position = Vec3(temp.x, temp.y, temp.z);
		}
		memcpy(&pVertices[side * 4], skyVerts, sizeof(skyVerts));
	}

	m_pVerts->Unlock();
	return S_OK;
}

HRESULT D3DSkyNode9::Render(Scene* pScene)
{
	// blend the texture with the color of the vertices

	DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	DXUTGetD3D9Device()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	DXUTGetD3D9Device()->SetStreamSource(0, m_pVerts, 0, sizeof(D3D9Vertex_ColoredTextured));
	DXUTGetD3D9Device()->SetFVF(D3D9Vertex_ColoredTextured::FVF);

	for (DWORD side = 0; side < m_Sides; side++)
	{
		std::string name = GetTextureName(side);

		Resource resource(name);
		shared_ptr<ResHandle> texture = g_pApp->m_ResCache->GetHandle(&resource);
		shared_ptr<D3DTextureResourceExtraData9> extra = static_pointer_cast<D3DTextureResourceExtraData9>(texture->GetExtra());
		
		DXUTGetD3D9Device()->SetTexture(0, extra->GetTexture());
		DXUTGetD3D9Device()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4 * side, 2);
	}

	DXUTGetD3D9Device()->SetTexture(0, NULL);
	return S_OK;
}
