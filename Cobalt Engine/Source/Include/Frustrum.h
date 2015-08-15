/*
	Frustrum.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "Plane.h"
#include "Vector.h"

/**
	Represents the view frustrum of a camera. This will be used for camera persepctive. For more 
	information on view frustrums check out https://en.wikipedia.org/wiki/Viewing_frustum
*/
class Frustrum
{
public:
	/// Possible sides of the frustrum
	enum Side { Near, Far, Top, Right, Bottom, Left, NumPlanes };

public:
	/// Default constructor
	Frustrum();

	/// Initialize a frustrum
	void Init(const float fov, const float aspect, const float nearClip, const float farClip);

	/// Render the view frustrum
	void Render();

	/// Return whether a point is inside the view frustrum
	bool Inside(const Vec3& point) const;

	/// Return whether a sphere is inside the view frustrum
	bool Inside(const Vec3& point, const float radius) const;

	/// Return a specific plane
	const Plane& Get(Side side);

	/// Set the field of view
	void SetFOV(float fov);

	/// Set the aspect ratio
	void SetAspect(float aspect);
	
	/// Set the near clipping plane distance
	void SetNear(float nearClip);

	/// Set the far clipping plane distance
	void SetFar(float farClip);

public:
	/// Planes of the frustrum in camera space
	Plane m_Planes[NumPlanes];

	/// Vertices of the near clipping plane in camera space
	Vec3 m_NearClip[4];

	/// Vertices of the far clipping plane in camera space
	Vec3 m_FarClip[4];

	/// Field of view in radians
	float m_Fov;

	/// Aspect ratio (width / height)
	float m_Aspect;

	/// Near clipping plane distance
	float m_Near;

	/// Far clipping plane distance
	float m_Far;
};