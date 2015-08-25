/*
	Matrix.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3dx9.h>

class Quaternion;
class Vec3;
class Vec4;

/**
	Represents a 4x4 matrix.
*/
class Mat4x4 : public D3DXMATRIX
{
public:
	/// Default constructor
	Mat4x4();

	/// Copy constructor
	Mat4x4(D3DXMATRIX& matrix);

	/// Set a 3D Position
	void SetPosition(const Vec3& position);

	/// Set a 4D Position
	void SetPosition(const Vec4& position);

	/// Set the scale elements
	void SetScale(const Vec3& scale);

	/// Return the x, y, z position values
	Vec3 GetPosition() const;

	/// Return the forward direction
	Vec3 GetDirection() const;

	/// Return the up direction
	Vec3 GetUp() const;

	/// Return the "right" direction 
	Vec3 GetRight() const;

	/// Return the yaw, pitch and roll values
	Vec3 GetYawPitchRoll() const;
	
	/// Return the x, y, z scale values
	Vec3 GetScale() const;

	/// Transform a Vec4 by this matrix
	Vec4 Transform(Vec4 &vec) const;

	/// Transform a Vec3 by this matrix
	Vec3 Transform(Vec3 &vec) const;

	/// Return the Inverse of this matrix
	Mat4x4 Inverse() const;

	/// Build a translation matrix from a Vec3
	void BuildTranslation(const Vec3& position);

	/// Build a translation matrix from x, y, and z values
	void BuildTranslation(const float x, const float y, const float z);
	
	/// Build a rotation matrix from an x angle
	void BuildRotationX(const float radians);

	/// Build a rotation matrix from a y angle
	void BuildRotationY(const float radians);

	/// Build a rotation matrix from a z angle
	void BuildRotationZ(const float radians);
	
	/// Build a rotation matrix from yaw, pitch, and roll angles
	void BuildYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians);

	/// Build a rotation matrix from a quaternion
	void BuildRotationQuaternion(const Quaternion& q);

	/// Build a look at matrix from the eye position, the at vector, and the up vector
	void BuildRotationLookAt(const Vec3& eye, const Vec3& at, const Vec3& up);

	/// Build a scaling matrix
	void BuildScale(const float x, const float y, const float z);

public:
	/// The identity matrix
	static const Mat4x4 Identity;
};

/// Overloaded multiply operator for 2 matrices. Represents transformation a followed by transformation b
inline Mat4x4 operator*(const Mat4x4& a, const Mat4x4 &b)
{
	Mat4x4 out;
	D3DXMatrixMultiply(&out, &a, &b);
	return out;
}
