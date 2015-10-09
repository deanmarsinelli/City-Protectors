/*
	D3D9Vertex.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3D9Vertex.h"

// d3d9 vertex fixed functions
const DWORD D3D9Vertex_UnlitColored::FVF =
(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR);
const DWORD D3D9Vertex_ColoredTextured::FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
const DWORD D3D9Vertex_Colored::FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE);
const DWORD D3D9Vertex_UnlitTextured::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
