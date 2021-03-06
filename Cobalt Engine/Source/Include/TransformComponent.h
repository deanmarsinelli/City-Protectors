/*
	TransformComponent.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <tinyxml.h>

#include "Component.h"
#include "Matrix.h"
#include "Vector.h"

/**
	A transform represents position, rotation, and scale.
*/
class TransformComponent : public Component
{
public:
	/// Constructor sets the transform to the identity matrix
	TransformComponent() :
		m_Transform(Mat4x4::Identity)
	{ }

	/// Initialize a transform from xml data
	bool Init(TiXmlElement* pData);

	/// Generate xml from the transform
	TiXmlElement* GenerateXml();

	/// Return the transform matrix
	Mat4x4 GetTransform() const
	{
		return m_Transform;
	}

	/// Set the transform matrix
	void SetTransform(const Mat4x4& transform)
	{
		m_Transform = transform;
	}

	/// Return the position vector
	Vec3 GetPosition() const
	{
		return m_Transform.GetPosition();
	}

	/// Set the position vector
	void SetPosition(const Vec3& position)
	{
		m_Transform.SetPosition(position);
	}

	/// Get the look at vector
	Vec3 GetLookAt() const
	{
		return m_Transform.GetDirection();
	}

	/// Return the name of the component
	virtual const char* GetName() const 
	{ 
		return g_Name; 
	}

public:
	/// Name of the component
	static const char* g_Name;

private:
	/// 4x4 matrix representing position, rotation, and scale
	Mat4x4 m_Transform;
};
