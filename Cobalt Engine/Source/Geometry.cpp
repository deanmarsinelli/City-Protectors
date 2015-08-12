/*
	Geometry.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DGeometry.h"

const Mat4x4 Mat4x4::Identity(D3DXMATRIX(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));

Mat4x4::Mat4x4() :
D3DXMATRIX()
{}

Mat4x4::Mat4x4(D3DXMATRIX& matrix) :
D3DXMATRIX()
{
	memcpy(&m, &matrix.m, sizeof(matrix.m));
}