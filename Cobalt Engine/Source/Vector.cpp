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


Vec3 BarycentricToVec3(const Vec3& v0, const Vec3& v1, const Vec3& v2, float u, float v)
{
	// V1 + U(V2-V1) + V(V3-V1)
	Vec3 result = v0 + u * (v1 - v0) + v * (v2 - v0);
	return result;
}


bool IntersectTriangle(const Vec3& rayOrigin, const Vec3& rayDir, const Vec3& v0,
	const Vec3& v1, const Vec3& v2, FLOAT* t, FLOAT* u, FLOAT* v)
{
	// find the vectors for two edges sharing vert0
	Vec3 edge1 = v1 - v0;
	Vec3 edge2 = v2 - v0;

	// get the cross product of the ray and edge 2
	Vec3 pvec;
	D3DXVec3Cross(&pvec, &rayDir, &edge2);

	// if the determinant is near zero, the ray lies in the plane of the triangle
	FLOAT det = D3DXVec3Dot(&edge1, &pvec);

	Vec3 tvec;
	if (det > 0)
	{
		tvec = rayOrigin - v0;
	}
	else
	{
		tvec = v0 - rayOrigin;
		det = -det;
	}

	if (det < 0.0001f)
		return false;

	// calculate u parameter and test bounds
	*u = D3DXVec3Dot(&tvec, &pvec);
	if (*u < 0.0f || *u > det)
		return false;

	Vec3 qvec;
	D3DXVec3Cross(&qvec, &tvec, &edge1);

	// calculate v parameter and test bounds
	*v = D3DXVec3Dot(&rayDir, &qvec);
	if (*v < 0.0f || *u + *v > det)
		return false;

	// calculate t, scale parameters, and the ray intersects the tri
	*t = D3DXVec3Dot(&edge2, &qvec);
	FLOAT invDet = 1.0f / det;
	*t *= invDet;
	*u *= invDet;
	*v *= invDet;

	return true;
}


Vec3 g_Right(1.0f, 0.0f, 0.0f);
Vec3 g_Up(0.0f, 1.0f, 0.0f);
Vec3 g_Forward(0.0f, 0.0f, 1.0f);

Vec4 g_Up4(g_Up.x, g_Up.y, g_Up.z, 0.0f);
Vec4 g_Right4(g_Right.x, g_Right.y, g_Right.z, 0.0f);
Vec4 g_Forward4(g_Forward.x, g_Forward.y, g_Forward.z, 0.0f);
