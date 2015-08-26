/*
	D3DRenderer9.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3D9Vertex.h"
#include "D3DRenderer9.h"
#include "D3DRendererAlphaPass9.h"
#include "D3DRendererSkyBoxPass9.h"
#include "EngineStd.h"
#include "LightNode.h"
#include "Matrix.h"
#include "Vector.h"

D3DRenderer9::D3DRenderer9()
{
	m_pFont = nullptr;
	m_pTextSprite = nullptr;
}

void D3DRenderer9::Shutdown()
{
	D3DRenderer::Shutdown();
	CB_COM_RELEASE(m_pFont);
	CB_COM_RELEASE(m_pTextSprite);
}

void D3DRenderer9::SetBackgroundColor(BYTE r, BYTE g, BYTE b, BYTE a)
{
	m_BackgroundColor = D3DCOLOR_RGBA(r, g, b, a);
}

bool D3DRenderer9::PreRender()
{
	// clear render target and z buffer
	if (SUCCEEDED(DXUTGetD3D9Device()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_BackgroundColor, 1.0f, 0)))
	{
		return SUCCEEDED(DXUTGetD3D9Device()->BeginScene());
	}

	return false;
}

bool D3DRenderer9::PostRender()
{
	return SUCCEEDED(DXUTGetD3D9Device()->EndScene());
}

HRESULT D3DRenderer9::OnRestore()
{
	HRESULT hr;
	V_RETURN(D3DRenderer::OnRestore());
	V_RETURN(D3DRenderer::g_DialogResourceManager.OnD3D9ResetDevice());

	CB_SAFE_DELETE(D3DRenderer::g_pTextHelper);
	CB_COM_RELEASE(m_pFont);

	V_RETURN(D3DXCreateFont(DXUTGetD3D9Device(), 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"Arial", &m_pFont));

	CB_COM_RELEASE(m_pTextSprite);
	V_RETURN(D3DXCreateSprite(DXUTGetD3D9Device(), &m_pTextSprite));

	D3DRenderer::g_pTextHelper = CB_NEW CDXUTTextHelper(m_pFont, m_pTextSprite, 15);

	return S_OK;
}

void D3DRenderer9::CalcLighting(Lights* lights, int maximumLights)
{
	int count = 1;
	if (lights && lights->size() > 0)
	{
		// iterate all the lights
		for (auto it = lights->begin(); it != lights->end(); ++it, ++count)
		{
			shared_ptr<LightNode> light = *it;
			shared_ptr<D3DLightNode9> light9 = static_pointer_cast<D3DLightNode9>(light);
			D3DLIGHT9* m_pLight = &(light9->m_d3dLight9);
			DXUTGetD3D9Device()->SetLight(count, m_pLight);
			DXUTGetD3D9Device()->LightEnable(count, TRUE);
		}
		DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, TRUE);
	}
	else
	{
		DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, FALSE);
	}

	// turn off other lights if they were on
	for (; count < maximumLights; ++count)
	{
		DXUTGetD3D9Device()->LightEnable(count, FALSE);
	}
}

void D3DRenderer9::SetWorldTransform(const Mat4x4* mat)
{
	DXUTGetD3D9Device()->SetTransform(D3DTS_WORLD, mat);
}

void D3DRenderer9::SetViewTransform(const Mat4x4* mat)
{
	DXUTGetD3D9Device()->SetTransform(D3DTS_VIEW, mat);
}

void D3DRenderer9::SetProjectionTransform(const Mat4x4* mat)
{
	DXUTGetD3D9Device()->SetTransform(D3DTS_PROJECTION, mat);
}

shared_ptr<IRenderState> D3DRenderer9::PrepareAlphaPass()
{
	return shared_ptr<IRenderState>(CB_NEW D3DRendererAlphaPass9());
}

shared_ptr<IRenderState> D3DRenderer9::PrepareSkyBoxPass()
{
	return shared_ptr<IRenderState>(CB_NEW D3DRendererSkyBoxPass9());
}

void D3DRenderer9::DrawLine(const Vec3& from, const Vec3& to, const Color& color)
{
	DWORD oldLightingState;
	// store the original lighting state
	DXUTGetD3D9Device()->GetRenderState(D3DRS_LIGHTING, &oldLightingState);

	// disable lighting for lines
	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, FALSE);

	D3D9Vertex_Colored verts[2];
	verts[0].position = from;
	verts[0].color = color;
	verts[1].position = to;
	verts[1].color = color;

	// draw the line from vertex to vertex
	DXUTGetD3D9Device()->SetFVF(D3D9Vertex_Colored::FVF);
	DXUTGetD3D9Device()->DrawPrimitiveUP(D3DPT_LINELIST, 1, verts, sizeof(D3D9Vertex_Colored));

	// restore original lighting state
	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, oldLightingState);
}
