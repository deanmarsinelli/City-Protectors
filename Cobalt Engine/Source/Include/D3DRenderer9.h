/*
	D3DRenderer9.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3dx9.h>
#include "D3DRenderer.h"

/**
	D3D9 renderer implementation.
*/
class D3DRenderer9 : public D3DRenderer
{
public:
	/// Default constructor
	D3DRenderer9();

	/// Shutdown the renderer and release resource
	virtual void Shutdown();

	/// Set the background color
	virtual void SetBackgroundColor(BYTE r, BYTE g, BYTE b, BYTE a);

	/// Set up the scene for rendering
	virtual bool PreRender();

	virtual bool PostRender();

	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc). Can also be used for initialization
	virtual HRESULT OnRestore();

	virtual void CalcLighting(Lights* lights, int maximumLights);
	virtual void SetWorldTransform(const Mat4x4* mat);
	virtual void SetViewTransform(const Mat4x4* mat);
	virtual void SetProjectionTransform(const Mat4x4* mat);

	/// Prepare for the alpha render pass
	virtual shared_ptr<IRenderState> PrepareAlphaPass();

	/// Prepare for the sky render pass
	virtual shared_ptr<IRenderState> PrepareSkyBoxPass();

	/// Draw a line in the world
	virtual void DrawLine(const Vec3& from, const Vec3& to, const Color& color);

protected:
	D3DCOLOR m_BackgroundColor;
	ID3DXFont* m_pFont;
	ID3DXSprite* m_pTextSprite;
};