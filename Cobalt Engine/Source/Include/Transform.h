/**
	Transform.h
*/

#pragma once

#include "Geometry.h"

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