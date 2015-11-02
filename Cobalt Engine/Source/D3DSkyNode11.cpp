/*
	D3DSkyNode11.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "CameraNode.h"
#include "D3D11Vertex.h"
#include "D3DSkyNode11.h"
#include "EngineStd.h"
#include "Logger.h"
#include "Scene.h"

D3DSkyNode11::D3DSkyNode11(const char* pTextureBaseName) :
	SkyNode(pTextureBaseName)
{
	m_pIndexBuffer = nullptr;
	m_pVertexBuffer = nullptr;
	m_VertexShader.EnableLights(false);
}

D3DSkyNode11::~D3DSkyNode11()
{
	CB_COM_RELEASE(m_pVertexBuffer);
	CB_COM_RELEASE(m_pIndexBuffer);
}

HRESULT D3DSkyNode11::OnRestore(Scene* pScene)
{
	HRESULT hr;

	V_RETURN(SceneNode::OnRestore(pScene));

	m_Camera = pScene->GetCamera();

	CB_COM_RELEASE(m_pVertexBuffer);
	CB_COM_RELEASE(m_pIndexBuffer);

	V_RETURN(m_VertexShader.OnRestore(pScene));
	V_RETURN(m_PixelShader.OnRestore(pScene));

	m_NumVerts = 20;

	// fill the vertex buffer and set texture coordinates
	D3D11Vertex_UnlitTextured* pVertices = CB_NEW D3D11Vertex_UnlitTextured[m_NumVerts];
	CB_ASSERT(pVertices && "Out of memory in D3DSkyNode11::OnRestore()");
	if (!pVertices)
	{
		return E_FAIL;
	}

	// loop through the grid squares and calculate the values
	// of each index. each grid has two triangles:
	//
	//	A - B
	//	| / |
	//	C - D

	D3D11Vertex_UnlitTextured skyVerts[4];
	D3DCOLOR skyVertColor = 0xffffffff;
	float dim = 50.0f;

	skyVerts[0].Pos = Vec3(dim, dim, dim);
	skyVerts[0].Uv = Vec2(1.0f, 0.0f);

	skyVerts[1].Pos = Vec3(-dim, dim, dim);
	skyVerts[1].Uv = Vec2(0.0f, 0.0f);

	skyVerts[2].Pos = Vec3(dim, -dim, dim);
	skyVerts[2].Uv = Vec2(1.0f, 1.0f);

	skyVerts[3].Pos = Vec3(-dim, -dim, dim);
	skyVerts[3].Uv = Vec2(0.0f, 1.0f);

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
				temp = rotY.Transform(Vec3(skyVerts[v].Pos));
			}
			else
			{
				// this might be wrong (TODO)
				skyVerts[0].Uv = Vec2(1.0f, 1.0f);
				skyVerts[1].Uv = Vec2(1.0f, 1.0f);
				skyVerts[2].Uv = Vec2(1.0f, 1.0f);
				skyVerts[3].Uv = Vec2(1.0f, 1.0f);

				temp = rotX.Transform(Vec3(skyVerts[v].Pos));
			}
			skyVerts[v].Pos = Vec3(temp.x, temp.y, temp.z);
		}
		memcpy(&pVertices[side * 4], skyVerts, sizeof(skyVerts));
	}

	D3D11_BUFFER_DESC bDesc;
	ZeroMemory(&bDesc, sizeof(D3D11_BUFFER_DESC));
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.ByteWidth = sizeof(D3D11Vertex_UnlitTextured)* m_NumVerts;
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = pVertices;
	hr = DXUTGetD3D11Device()->CreateBuffer(&bDesc, &initData, &m_pVertexBuffer);

	CB_SAFE_DELETE(pVertices);
	if (FAILED(hr))
		return hr;

	// now loop through the grid squares and calculate the values of each index
	WORD* pIndices = CB_NEW WORD[m_Sides * 2 * 3];
	WORD* current = pIndices;

	//	A - B
	//	| /	|
	//	C - D
	for (DWORD i = 0; i < m_Sides; i++)
	{
		// Triangle 1 ACB
		*(current) = WORD(i * 4);
		*(current + 1) = WORD(i * 4 + 2);
		*(current + 2) = WORD(i * 4 + 1);

		// Triangle 2 BCD
		*(current + 3) = WORD(i * 4 + 1);
		*(current + 4) = WORD(i * 4 + 2);
		*(current + 5) = WORD(i * 4 + 3);
		
		// go to next face
		current += 6;
	}

	// create the index buffer description
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.ByteWidth = sizeof(WORD)* m_Sides * 2 * 3;
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.CPUAccessFlags = 0;
	initData.pSysMem = pIndices;
	hr = DXUTGetD3D11Device()->CreateBuffer(&bDesc, &initData, &m_pIndexBuffer);
	
	CB_SAFE_DELETE_ARRAY(pIndices);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT D3DSkyNode11::Render(Scene* pScene)
{
	HRESULT hr;

	V_RETURN(m_VertexShader.SetupRender(pScene, this));
	V_RETURN(m_PixelShader.SetupRender(pScene, this));

	// set the vertex buffer
	UINT stride = sizeof(D3D11Vertex_UnlitTextured);
	UINT offset = 0;
	DXUTGetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// set the index buffer
	DXUTGetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// set the primitive topology
	DXUTGetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (DWORD side = 0; side < m_Sides; side++)
	{
		std::string name = GetTextureName(side);
		m_PixelShader.SetTexture(name.c_str());

		DXUTGetD3D11DeviceContext()->DrawIndexed(6, side * 6, 0);
	}
	return S_OK;
}
