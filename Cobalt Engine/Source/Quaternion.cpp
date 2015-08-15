/*
	Quaternion.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Quaternion.h"

#include "Matrix.h"

const Quaternion Quaternion::Identity(D3DXQUATERNION(0, 0, 0, 1));

Quaternion::Quaternion()
{ }

Quaternion::Quaternion(D3DXQUATERNION& q) :
D3DXQUATERNION(q)
{}

void Quaternion::Normalize()
{
	D3DXQuaternionNormalize(this, this);
}

void Quaternion::Slerp(const Quaternion& begin, const Quaternion& end, float coeff)
{
	D3DXQuaternionSlerp(this, &begin, &end, coeff);
}

void Quaternion::GetAxisAngle(Vec3& axis, float& angle) const
{
	D3DXQuaternionToAxisAngle(this, &axis, &angle);
}

void Quaternion::Build(const Mat4x4& mat)
{
	D3DXQuaternionRotationMatrix(this, &mat);
}

void Quaternion::BuildRotYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians)
{
	D3DXQuaternionRotationYawPitchRoll(this, yawRadians, pitchRadians, rollRadians);
}

void Quaternion::BuildAxisAngle(const Vec3& axis, const float radians)
{
	D3DXQuaternionRotationAxis(this, &axis, radians);
}

Quaternion Quaternion::operator*(const Quaternion& rhs)
{
	Quaternion out;
	D3DXQuaternionMultiply(&out, this, &rhs);
	return out;
}
