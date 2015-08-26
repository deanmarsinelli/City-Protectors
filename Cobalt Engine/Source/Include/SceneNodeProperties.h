/*
	SceneNodeProperties.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"
#include "Material.h"
#include "Matrix.h"

enum AlphaType
{
	AlphaOpaque,
	AlphaTexture,
	AlphaMaterial,
	AlphaVertex
};

/**
	Properties of a scene node.
*/
class SceneNodeProperties
{
	friend class SceneNode;

public:
	/// Default constructor
	SceneNodeProperties();

	/// Return the Id of the object that this scene node represents
	const GameObjectId& ObjectId() const;

	/// Return the ToWorld matrix
	const Mat4x4& ToWorld() const;

	/// Return the ToWorld matrix
	const Mat4x4& FromWorld() const;

	/// Fill in the toWorld and fromWorld matrices by reference
	void Transform(Mat4x4* toWorld, Mat4x4* fromWorld) const;

	/// Return the name of the node
	const char* Name() const;

	/// Return whether the material has alpha or not
	bool HasAlpha() const;

	/// Return the alpha value of the material
	float Alpha() const;

	/// Return the alpha type of the node
	AlphaType GetAlphaType() const;

	/// Return the pass this node is rendered in
	RenderPass GetRenderPass() const;

	/// Return the radius of the node
	float Radius() const;

	/// Return the material of the node
	Material GetMaterial() const;

protected:
	/// Set the alpha of the node's material
	void SetAlpha(const float alpha);

protected:
	/// Id of an object this node is related to
	GameObjectId m_ObjectId;

	/// Name of an object this node is related to
	std::string m_Name;

	/// Matrix to transform this node from object space to world space
	Mat4x4 m_ToWorld;

	/// Matrix to transform the node from world space to object space
	Mat4x4 m_FromWorld;

	/// Radius of a sphere that includes all visible geometry of a node
	float m_Radius;

	/// Which render pass this node belongs to
	RenderPass m_RenderPass;
	Material m_Material;
	AlphaType m_AlphaType;
};
