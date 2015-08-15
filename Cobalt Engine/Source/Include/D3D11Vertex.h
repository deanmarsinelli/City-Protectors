/*
	D3D11Vertex.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <D3D11.h>

#include "Vector.h"

/**
	A simple vertex with position and normal.
*/
struct D3D11_SimpleVertex
{
	/// Position of the vertex
	Vec3 Pos;

	/// Normal of the vertex
	Vec3 Normal;
};

/// Vertex description for D3D11_SimpleVertex
const D3D11_INPUT_ELEMENT_DESC D3D11VertexLayout_SimpleVertex[] =
{
	// 12 byte offsets since theres 3 floats in each Vec3
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


/**
	A vertex with position, normal, and texture data.
*/
struct D3D11Vertex_UnlitTextured
{
	/// Position of the vertex
	Vec3 Pos;

	/// Normal of the vertex
	Vec3 Normal;

	/// Texture coordinates 
	Vec2 Uv;
};

/// Vertex description for D3D11Vertex_UnlitTextured
const D3D11_INPUT_ELEMENT_DESC D3D11VertexLayout_UnlitTextured[] = 
{
	// 12 byte offsets since theres 3 floats in each Vec3
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


/**
	A vertex with position, texture, and color data.
*/
struct D3D11Vertex_UnlitTexturedColored
{
	/// Position of the vertex
	Vec3 Pos;

	/// Normal of the vertex
	Vec3 Normal;

	/// Color of the vertex
	Vec3 Diffuse;

	/// Texture coordinates 
	Vec2 Uv;
};

/// Vertex description for D3D11Vertex_UnlitTexturedColored
const D3D11_INPUT_ELEMENT_DESC D3D11VertexLayout_UnlitTexturedColored[] =
{
	// 12 byte offsets since theres 3 floats in each Vec3
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


/**
	A Vertex with position and color.
*/
struct D3D11Vertex_PositionColored
{
	/// Position of the vertex
	Vec3 Pos;

	/// Color of the vertex
	Vec3 Diffuse;
};

/// Vertex description for D3D11Vertex_PositionColored
const D3D11_INPUT_ELEMENT_DESC D3D11VertexLayout_PositionColored[] =
{
	// 12 byte offsets since theres 3 floats in each Vec3
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


/// Vertex description for a vertex with position only
const D3D11_INPUT_ELEMENT_DESC D3D11VertexLayout_Position[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};