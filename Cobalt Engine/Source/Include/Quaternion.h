/*
	Quaternion.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3dx9math.h>

#include "Vector.h"

/**
	Represents a rotation using a quaternion.
*/
class Quaternion : public D3DXQUATERNION
{
public:
	/// Default constructor
	Quaternion();

	/// Copy constructor
	Quaternion(D3DXQUATERNION& q);
	
	/// Normalize the quaternion
	void Normalize();

	/// Spherical linear interpolation between two Quaternions. Coeff should be set between 0.0f - 1.0f
	void Slerp(const Quaternion& begin, const Quaternion& end, float coeff);

	/// Compute a quaternions axis and angle
	void GetAxisAngle(Vec3& axis, float& angle) const;

	/// Build a quaternion from a rotation matrix
	void Build(const class Mat4x4& mat);

	/// Build a quaternion from yaw, pitch, and roll angles
	void BuildRotYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians);

	/// Build a quaternion from an axis vector and an angle
	void BuildAxisAngle(const Vec3& axis, const float radians);

	/// Multiply operator
	Quaternion operator*(const Quaternion& rhs);

public:
	/// Identity quaternion
	static const Quaternion Identity;
};