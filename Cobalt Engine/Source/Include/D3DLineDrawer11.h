/*
	D3DLineDrawer11.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <D3D11.h>

#include "EngineStd.h"
#include "Geometry.h"
#include "Shaders.h"

/**
	This class will draw 3D lines in a scene using a D3DRenderer11.
*/
class D3DLineDrawer11
{
public:
	/// Default constructor
	D3DLineDrawer11();

	/// Default destructor
	~D3DLineDrawer11();

	/// Draw a line in a 3D scene
	void DrawLine(const Vec3& from, const Vec3& to, const Color& color);

	// Set up the vertex buffer
	HRESULT OnRestore();

protected:
	Vec3					m_Verts[2];
	LineDraw_Hlsl_Shader	m_LineDrawerShader;
	ID3D11Buffer*			m_pVertexBuffer;
};