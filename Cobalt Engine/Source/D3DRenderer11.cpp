/*
	D3DRenderer11.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DLineDrawer11.h"
#include "D3DRenderer11.h"
#include "D3DRendererAlphaPass11.h"
#include "D3DRendererSkyBoxPass11.h"
#include "EngineStd.h"

D3DRenderer11::D3DRenderer11()
{
	m_pLineDrawer = nullptr;
}

void D3DRenderer11::Shutdown()
{
	D3DRenderer::Shutdown();
	CB_SAFE_DELETE(m_pLineDrawer);
}

void D3DRenderer11::SetBackgroundColor(BYTE r, BYTE g, BYTE b, BYTE a)
{
	m_BackgroundColor[0] = float(r) / 255.0f;
	m_BackgroundColor[1] = float(g)	/ 255.0f;
	m_BackgroundColor[2] = float(b) / 255.0f;
	m_BackgroundColor[3] = float(a) / 255.0f;
}

bool D3DRenderer11::PreRender()
{
	if (DXUTGetD3D11DeviceContext() && DXUTGetD3D11RenderTargetView())
	{
		DXUTGetD3D11DeviceContext()->ClearRenderTargetView(DXUTGetD3D11RenderTargetView(), m_BackgroundColor);
	
		DXUTGetD3D11DeviceContext()->ClearDepthStencilView(DXUTGetD3D11DepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	return true;
}

bool D3DRenderer11::PostRender()
{
	return true;
}

HRESULT D3DRenderer11::OnRestore()
{
	HRESULT hr;
	V_RETURN(D3DRenderer::OnRestore());
	CB_SAFE_DELETE(D3DRenderer::g_pTextHelper);

	D3DRenderer::g_pTextHelper = CB_NEW CDXUTTextHelper(DXUTGetD3D11Device(), DXUTGetD3D11DeviceContext(), &g_DialogResourceManager, 15);
	return S_OK;
}

shared_ptr<IRenderState> D3DRenderer11::PrepareAlphaPass()
{
	return shared_ptr<IRenderState>(CB_NEW D3DRendererAlphaPass11());
}

shared_ptr<IRenderState> D3DRenderer11::PrepareSkyBoxPass()
{
	return shared_ptr<IRenderState>(CB_NEW D3DRendererSkyBoxPass11());
}

void D3DRenderer11::DrawLine(const Vec3& from, const Vec3& to, const Color& color)
{
	if (!m_pLineDrawer)
	{
		m_pLineDrawer = CB_NEW D3DLineDrawer11();
		m_pLineDrawer->OnRestore();
	}
	m_pLineDrawer->DrawLine(from, to, color);
}

HRESULT D3DRenderer11::CompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	// if debugging, set the debug flag
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	// compile the shader into the blob
	hr = D3DX11CompileFromMemory(pSrcData, SrcDataLen, pFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		shaderFlags, 0, nullptr, ppBlobOut, &pErrorBlob, nullptr);

	// if failed, output the error
	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		}
		CB_COM_RELEASE(pErrorBlob);

		return hr;
	}

	CB_COM_RELEASE(pErrorBlob);

	return S_OK;
}

HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	// if debugging, set the debug flag
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	// compile shader from a file
	hr = D3DX11CompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, shaderFlags,
		0, nullptr, ppBlobOut, &pErrorBlob, nullptr);

	// if failed, output the error
	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		}
		CB_COM_RELEASE(pErrorBlob);

		return hr;
	}

	CB_COM_RELEASE(pErrorBlob);

	return S_OK;
}