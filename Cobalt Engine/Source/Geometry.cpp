/*
	Geometry.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Geometry.h"

const double kThreshold = 0.001;

//====================================================
//	Vec3 definitions
//====================================================
inline Vec3 Vec3::Cross(const Vec3& vec) const
{
	Vec3 out;
	D3DXVec3Cross(&out, this, &vec);
	return out;
}

inline Vec3::Vec3(const Vec4 &vec4)
{ 
	x = vec4.x; 
	y = vec4.y; 
	z = vec4.z; 
}


//====================================================
//	Mat4x4 definitions
//====================================================
const Mat4x4 Mat4x4::Identity(D3DXMATRIX(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));

Mat4x4::Mat4x4() :
D3DXMATRIX()
{}

Mat4x4::Mat4x4(D3DXMATRIX& matrix) :
D3DXMATRIX()
{
	memcpy(&m, &matrix.m, sizeof(matrix.m));
}

inline void Mat4x4::SetPosition(const Vec3& position)
{
	m[0][3] = position.x;
	m[1][3] = position.y;
	m[2][3] = position.z;
	m[3][3] = 1.0f;
}

inline void Mat4x4::SetPosition(const Vec4& position)
{
	
	m[0][3] = position.x;
	m[1][3] = position.y;
	m[2][3] = position.z;
	m[3][3] = position.w;
	
}

inline void Mat4x4::SetScale(const Vec3& scale)
{
	m[0][0] = scale.x;
	m[1][1] = scale.y;
	m[2][2] = scale.z;
}

inline Vec3 Mat4x4::GetPosition() const
{
	return Vec3(m[0][3], m[1][3], m[2][3]);
}

inline Vec3 Mat4x4::GetDirection() const
{
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.0f, 0.0f, 0.0f));
	Vec3 forward = justRot.Transform(g_Forward);
	return forward;
}

inline Vec3 Mat4x4::GetUp() const
{
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.f, 0.f, 0.f));
	Vec3 up = justRot.Transform(g_Up);
	return up;
}

inline Vec3 Mat4x4::GetRight() const
{
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.f, 0.f, 0.f));
	Vec3 right = justRot.Transform(g_Right);
	return right;
}

inline Vec3 Mat4x4::GetYawPitchRoll() const
{
	float yaw, pitch, roll;

	pitch = asin(-_32);

	double test = cos(pitch);
	if (test > kThreshold)
	{
		roll = atan2(_12, _22);
		yaw = atan2(_31, _33);
	}
	else
	{
		roll = atan2(-_21, _11);
		yaw = 0.0f;
	}
	
	return Vec3(yaw, pitch, roll);
}

inline Vec3 Mat4x4::GetScale() const
{
	return Vec3(m[0][0], m[1][1], m[2][2]);
}

inline Vec4 Mat4x4::Transform(Vec4 &vec) const
{
	Vec4 temp;
	D3DXVec4Transform(&temp, &vec, this);
	return temp;
}

inline Vec3 Mat4x4::Transform(Vec3 &vec) const
{
	Vec4 temp(vec);
	Vec4 out;
	D3DXVec4Transform(&out, &temp, this);
	return Vec3(out.x, out.y, out.z);
}

inline Mat4x4 Mat4x4::Inverse() const
{
	Mat4x4 out;
	D3DXMatrixInverse(&out, nullptr, this);
	return out;
}


//====================================================
//	extern definitions
//====================================================
Vec3 g_Right(1.0f, 0.0f, 0.0f);
Vec3 g_Up(0.0f, 1.0f, 0.0f);
Vec3 g_Forward(0.0f, 0.0f, 1.0f);


Vec4 g_Up4(g_Up.x, g_Up.y, g_Up.z, 0.0f);
Vec4 g_Right4(g_Right.x, g_Right.y, g_Right.z, 0.0f);
Vec4 g_Forward4(g_Forward.x, g_Forward.y, g_Forward.z, 0.0f);