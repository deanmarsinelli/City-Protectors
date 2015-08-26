/*
	Plane.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "Plane.h"
#include "Vector.h"

void Plane::Init(const Vec3& p0, const Vec3& p1, const Vec3& p2)
{
	D3DXPlaneFromPoints(this, &p0, &p1, &p2);
	Normalize();
}

void Plane::Normalize()
{
	float magnitude;

	magnitude = std::sqrt(a * a + b * b + c * c);
	a = a / magnitude;
	b = b / magnitude;
	c = c / magnitude;
	d = d / magnitude;
}


bool Plane::Inside(const Vec3& point) const
{
	// if the distance from the point to the plane is >= 0, we are inside
	float result = D3DXPlaneDotCoord(this, &point);
	return result >= 0.0f;
}

bool Plane::Inside(const Vec3& point, const float radius) const
{
	// if the distance from the plane to the point is < -radius
	// then we are outside the plane
	float distance = D3DXPlaneDotCoord(this, &point);
	return distance >= -radius;
}
