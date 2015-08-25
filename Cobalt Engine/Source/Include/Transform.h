/*
	Transform.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "Matrix.h"
#include "Vector.h"

class Transform
{
public:
	Transform() :
		m_Transform(Mat4x4::Identity)
	{}

	Mat4x4 GetTransform() const
	{
		return m_Transform;
	}

	void SetTransform(const Mat4x4& transform)
	{
		m_Transform = transform;
	}
	
	Vec3 GetPosition() const
	{
		return m_Transform.GetPosition();
	}

	void SetPosition(const Vec3& position)
	{
		m_Transform.SetPosition(position);
	}

	Vec3 GetLookAt() const
	{
		return m_Transform.GetDirection();
	}

private:
	Mat4x4 m_Transform;
};
