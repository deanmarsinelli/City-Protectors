/*
	D3DLineDrawer11.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <DXUT.h>
#include <memory>

#include "D3D11Vertex.h"
#include "D3DLineDrawer11.h"
#include "EngineStd.h"
#include "HumanView.h"
#include "Scene.h"

D3DLineDrawer11::D3DLineDrawer11()
{
	m_pVertexBuffer = nullptr;
}

D3DLineDrawer11::~D3DLineDrawer11()
{
	CB_SAFE_DELETE(m_pVertexBuffer);
}

void D3DLineDrawer11::DrawLine(const Vec3& from, const Vec3& to, const Color& color)
{
	HRESULT hr;
	shared_ptr<Scene> pScene = g_pApp->GetHumanView()->m_pScene;
	shared_ptr<IRenderer> pRenderer = pScene->GetRenderer();

	if (FAILED(m_LineDrawerShader.SetupRender(&(*pScene))))
		return;

	m_LineDrawerShader.SetDiffuse("art\\grid.dds", color);

	UINT stride = sizeof(Vec3);
	UINT offset = 0;

	// map the vertex buffer into a D3D11 mapped resource
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	V(DXUTGetD3D11DeviceContext()->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	// fill in the vertex buffer with the from and to params
	Vec3* pVertices = (Vec3*)mappedResource.pData;
	pVertices[0] = from;
	pVertices[1] = to;

	// store the vertices
	m_Verts[0] = from;
	m_Verts[1] = to;

	DXUTGetD3D11DeviceContext()->Unmap(m_pVertexBuffer, 0);

	// send the vertices to the GPU and draw
	DXUTGetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	DXUTGetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	DXUTGetD3D11DeviceContext()->Draw(2, 0);
}

HRESULT D3DLineDrawer11::OnRestore()
{
	// create a d3d11 buffer description
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(D3D11Vertex_PositionColored) * 2; // holds 2 vertices
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// create a d3d resource for initial data
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &m_Verts;

	// using the buffer description and initial data, create a vertex buffer and store it in
	// m_pVertexBuffer
	HRESULT hr;
	V_RETURN(DXUTGetD3D11Device()->CreateBuffer(&bd, &initData, &m_pVertexBuffer));
	shared_ptr<Scene> pScene = g_pApp->GetHumanView()->m_pScene;
	V_RETURN(m_LineDrawerShader.OnRestore(&(*pScene)));

	return S_OK;
}