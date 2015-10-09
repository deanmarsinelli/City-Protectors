/*
	D3D9Vertex.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "d3dx9.h"

/**
	A vertex containing position and color data.
*/
struct D3D9Vertex_Colored
{
	/// Position of the vertex
	D3DXVECTOR3	position;

	/// Color of the vertex
	D3DCOLOR	color;

	/// Fixed vertex function type
	static const DWORD FVF;
};

/**
	A vertex containing position, color and texture coordinate data.
*/
struct D3D9Vertex_ColoredTextured
{
	/// Position of the vertex
	D3DXVECTOR3	position;

	/// Color of the vertex
	D3DCOLOR	color;

	/// Texture coordinates
	FLOAT		tu, tv;

	/// Fixed vertex function type
	static const DWORD FVF;
};

/**
	A vertex with position, normal, and lighting data.
*/
struct D3D9Vertex_UnlitColored
{
	/// Position of the vertex
	D3DXVECTOR3	position;

	/// Normal of the vertex
	D3DXVECTOR3 normal;

	/// Diffuse color of the vertex
	D3DCOLOR	diffuse;

	/// Specular color of the vertex
	D3DCOLOR	specular;

	/// Fixed vertex function type
	static const DWORD FVF;
};

/**
	A vertex with position, normal and texture data.
*/
struct D3D9Vertex_UnlitTextured
{
	/// Position of the vertex
	D3DXVECTOR3 position;

	/// Normal of the vertex
	D3DXVECTOR3 normal;

	/// Texture coordinates
	FLOAT		tu, tv;

	/// Fixed vertex function type
	static const DWORD FVF;
};

