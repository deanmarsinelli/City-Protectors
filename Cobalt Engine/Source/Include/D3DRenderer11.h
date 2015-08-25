/*
	D3DRenderer.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "D3DRenderer.h"

class D3DLineDrawer11;

/**
	D3D11 renderer implementation.
*/
class D3DRenderer11 : public D3DRenderer
{
public:
	D3DRenderer11();

	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc). Can also be used for initialization
	virtual HRESULT OnRestore();

	/// Compile a shader thats loaded into memory -- taken from D3D Samples
	HRESULT CompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	/// Compile a shader from a file -- taken from D3D Samples
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

protected:
	float m_BackgroundColor[4];
	D3DLineDrawer11* m_pLineDrawer;
};
