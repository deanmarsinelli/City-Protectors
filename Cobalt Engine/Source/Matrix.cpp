/*
	Matrix.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Matrix.h"

#include "Quaternion.h"
#include "Vector.h"

const double kThreshold = 0.001;

const Mat4x4 Mat4x4::Identity(D3DXMATRIX(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));

Mat4x4::Mat4x4() :
D3DXMATRIX()
{}

Mat4x4::Mat4x4(D3DXMATRIX& matrix) :
D3DXMATRIX()
{
	memcpy(&m, &matrix.m, sizeof(matrix.m));
}

void Mat4x4::SetPosition(const Vec3& position)
{
	m[3][0] = position.x;
	m[3][1] = position.y;
	m[3][2] = position.z;
	m[3][3] = 1.0f;
}

void Mat4x4::SetPosition(const Vec4& position)
{

	m[3][0] = position.x;
	m[3][1] = position.y;
	m[3][2] = position.z;
	m[3][3] = position.w;

}

void Mat4x4::SetScale(const Vec3& scale)
{
	m[0][0] = scale.x;
	m[1][1] = scale.y;
	m[2][2] = scale.z;
}

Vec3 Mat4x4::GetPosition() const
{
	return Vec3(m[3][0], m[3][1], m[3][2]);
}

Vec3 Mat4x4::GetDirection() const
{
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.0f, 0.0f, 0.0f));
	Vec3 forward = justRot.Transform(g_Forward);
	return forward;
}

Vec3 Mat4x4::GetUp() const
{
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.f, 0.f, 0.f));
	Vec3 up = justRot.Transform(g_Up);
	return up;
}

Vec3 Mat4x4::GetRight() const
{
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.f, 0.f, 0.f));
	Vec3 right = justRot.Transform(g_Right);
	return right;
}

Vec3 Mat4x4::GetYawPitchRoll() const
{
	float yaw, pitch, roll;

	pitch = std::asin(-_32);
	
	double test = std::cos(pitch);
	if (test > kThreshold)
	{
		roll = std::atan2(_12, _22);
		yaw = std::atan2(_31, _33);
	}
	else
	{
		roll = std::atan2(-_21, _11);
		yaw = 0.0f;
	}

	return Vec3(yaw, pitch, roll);
}

Vec3 Mat4x4::GetScale() const
{
	return Vec3(m[0][0], m[1][1], m[2][2]);
}

Vec4 Mat4x4::Transform(Vec4 &vec) const
{
	Vec4 temp;
	D3DXVec4Transform(&temp, &vec, this);
	return temp;
}

Vec3 Mat4x4::Transform(Vec3 &vec) const
{
	Vec4 temp(vec);
	Vec4 out;
	D3DXVec4Transform(&out, &temp, this);
	return Vec3(out.x, out.y, out.z);
}

Mat4x4 Mat4x4::Inverse() const
{
	Mat4x4 out;
	D3DXMatrixInverse(&out, nullptr, this);
	return out;
}

void Mat4x4::BuildTranslation(const Vec3& position)
{
	*this = Mat4x4::Identity;
	m[3][0] = position.x;
	m[3][1] = position.y;
	m[3][2] = position.z;
}

void Mat4x4::BuildTranslation(const float x, const float y, const float z)
{
	*this = Mat4x4::Identity;
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
}

void Mat4x4::BuildRotationX(const float radians)
{
	D3DXMatrixRotationX(this, radians);
}

void Mat4x4::BuildRotationY(const float radians)
{
	D3DXMatrixRotationY(this, radians);
}

void Mat4x4::BuildRotationZ(const float radians)
{
	D3DXMatrixRotationZ(this, radians);
}

void Mat4x4::BuildYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians)
{
	D3DXMatrixRotationYawPitchRoll(this, yawRadians, pitchRadians, rollRadians);
}

void Mat4x4::BuildRotationQuaternion(const Quaternion& q)
{
	D3DXMatrixRotationQuaternion(this, &q);
}

void Mat4x4::BuildRotationLookAt(const Vec3& eye, const Vec3& at, const Vec3& up)
{
	D3DXMatrixLookAtRH(this, &eye, &at, &up);
}

void Mat4x4::BuildScale(const float x, const float y, const float z)
{
	*this = Mat4x4::Identity;
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
}