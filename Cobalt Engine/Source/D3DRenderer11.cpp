/*
	D3DRenderer11.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DRenderer11.h"

#include "EngineStd.h"

D3DRenderer11::D3DRenderer11()
{
	m_pLineDrawer = nullptr;
}

HRESULT D3DRenderer11::OnRestore()
{
	HRESULT hr;
	V_RETURN(D3DRenderer::OnRestore());
	CB_SAFE_DELETE(D3DRenderer::g_pTextHelper);

	D3DRenderer::g_pTextHelper = CB_NEW CDXUTTextHelper(DXUTGetD3D11Device(), DXUTGetD3D11DeviceContext(), &g_DialogResourceManager, 15);
	return S_OK;
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