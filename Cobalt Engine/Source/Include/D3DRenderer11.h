/*
	D3DRenderer11.h

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
	/// Default constructor
	D3DRenderer11();

	/// Shutdown the renderer and release resource
	virtual void Shutdown();

	/// Set the background color
	virtual void SetBackgroundColor(BYTE r, BYTE g, BYTE b, BYTE a);

	/// Set up the scene for rendering
	virtual bool PreRender();

	virtual bool PostRender();

	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc). Can also be used for initialization
	virtual HRESULT OnRestore();

	/// These are all done in the shader not the renderer
	virtual void CalcLighting(Lights* lights, int maximumLights) { }
	virtual void SetWorldTransform(const Mat4x4* mat) { }
	virtual void SetViewTransform(const Mat4x4* mat) { }
	virtual void SetProjectionTransform(const Mat4x4* mat) { }

	/// Prepare for the alpha render pass
	virtual shared_ptr<IRenderState> PrepareAlphaPass();

	/// Prepare for the sky render pass
	virtual shared_ptr<IRenderState> PrepareSkyBoxPass();

	/// Draw a line in the world
	virtual void DrawLine(const Vec3& from, const Vec3& to, const Color& color);

	/// Compile a shader thats loaded into memory -- taken from D3D Samples
	HRESULT CompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	/// Compile a shader from a file -- taken from D3D Samples
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

protected:
	float m_BackgroundColor[4];
	D3DLineDrawer11* m_pLineDrawer;
};
