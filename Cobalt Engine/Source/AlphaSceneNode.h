/*
	AlphaSceneNode.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <list>

#include "interfaces.h"
#include "Matrix.h"

/**
	A scene node that must be drawn in the alpha pass.
*/
struct AlphaSceneNode
{
	shared_ptr<ISceneNode> m_pNode;
	Mat4x4 m_Concat;
	float m_ScreenZ;

	// less than operator for stl sort
	bool const operator <(const AlphaSceneNode& other) { return m_ScreenZ < other.m_ScreenZ; }
};

typedef std::list<AlphaSceneNode*> AlphaSceneNodes;