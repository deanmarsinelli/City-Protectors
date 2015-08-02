/**
	Geometry.h
*/

#pragma once

#include <d3dx9math.h>

class Vec3 : public D3DXVECTOR3
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