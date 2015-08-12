/*
	D3DGeometry.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3dx9math.h>

class Vec3 : public D3DXVECTOR3
{
public:

};

class Vec4 : public D3DXVECTOR4
{
public:
};

class Mat4x4 : public D3DXMATRIX
{
public:
	Mat4x4();
	Mat4x4(D3DXMATRIX& matrix);

	inline void SetPosition(const Vec3& position);
	inline Vec3 GetPosition() const;
	inline Vec3 GetDirection() const;

public:
	static const Mat4x4 Identity;
};

struct D3D11Vertex_PositionColored
{
	/// Vector 3 for position data
	Vec3 Pos;

	/// Vector3 for color data
	Vec3 Diffuse;
};