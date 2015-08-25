/*
	Vector.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Vector.h"

Vec3 Vec3::Cross(const Vec3& vec) const
{
	Vec3 out;
	D3DXVec3Cross(&out, this, &vec);
	return out;
}

Vec3::Vec3(const Vec4 &vec4)
{
	x = vec4.x;
	y = vec4.y;
	z = vec4.z;
}



Vec3 g_Right(1.0f, 0.0f, 0.0f);
Vec3 g_Up(0.0f, 1.0f, 0.0f);
Vec3 g_Forward(0.0f, 0.0f, 1.0f);

Vec4 g_Up4(g_Up.x, g_Up.y, g_Up.z, 0.0f);
Vec4 g_Right4(g_Right.x, g_Right.y, g_Right.z, 0.0f);
Vec4 g_Forward4(g_Forward.x, g_Forward.y, g_Forward.z, 0.0f);
