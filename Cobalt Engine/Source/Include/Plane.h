/*
	Plane.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3dx9math.h>

#include "Vector.h"

/**
	Represents a plane in 3D space. Normals face away from you (viewer) if the 
	vertices are declared in counter clockwise order. General plane equation:
	ax + by + cz + dw = 0.
*/
class Plane : public D3DXPLANE
{
public:
	/// Initialize a plane
	void Init(const Vec3& p0, const Vec3& p1, const Vec3& p2);

	/// Normalize the plane
	void Normalize();

	/// Return whether or not a point is inside the plane. This is defined as the direction the normal is facing.
	bool Inside(const Vec3& point) const;

	/// Return whether or not a sphere is inside the plane. This is defined as the direction the normal is facing.
	bool Inside(const Vec3& point, const float radius) const;
};
