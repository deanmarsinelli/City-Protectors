/*
	Matrix.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3dx9math.h>

#include "Vector.h"

/**
	Represents a 4x4 matrix.
*/
class Mat4x4 : public D3DXMATRIX
{
public:
	/// Default constructor
	Mat4x4();

	/// Constructor from another 4x4 matrix
	Mat4x4(D3DXMATRIX& matrix);

	void SetPosition(const Vec3& position);
	void SetPosition(const Vec4& position);
	void SetScale(const Vec3& scale);

	Vec3 GetPosition() const;
	Vec3 GetDirection() const;
	Vec3 GetUp() const;
	Vec3 GetRight() const;
	Vec3 GetYawPitchRoll() const;
	Vec3 GetScale() const;

	/// Transform a Vec4 by this matrix
	Vec4 Transform(Vec4 &vec) const;

	/// Transform a Vec3 by this matrix
	Vec3 Transform(Vec3 &vec) const;

	/// Return the Inverse of this matrix
	Mat4x4 Inverse() const;

public:
	/// The identity matrix
	static const Mat4x4 Identity;
};