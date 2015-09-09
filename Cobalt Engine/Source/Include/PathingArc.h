/*
	PathingArc.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

const float PATHING_DEFAULT_ARC_WEIGHT = 1.0f;

class PathingNode;

/**
	Represents an edge between two pathing nodes allowing travel between them.
*/
class PathingArc
{
public:
	/// Constructor
	explicit PathingArc(float weight = PATHING_DEFAULT_ARC_WEIGHT);

	/// Return the weight of this arc
	float GetWeight() const;

	/// Link two nodes together with this arc
	void LinkNodes(PathingNode* pNodeA, PathingNode* pNodeB);

	/// Given a node, return the other node attached to this arc
	PathingNode* GetNeighbor(PathingNode* pNode);

private:
	/// The cost to travel along this arc
	float m_Weight;

	/// The two nodes at the ends of this arc
	PathingNode* m_pNodes[2];
};