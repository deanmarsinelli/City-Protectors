/*
	PathingNode.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <list>

#include "Vector.h"

const float PATHING_DEFAULT_NODE_TOLERANCE = 5.0f;

class PathingArc;
class PathingNode;
typedef std::list<PathingArc*> PathingArcList;
typedef std::list<PathingNode*> PathingNodeList;

/**
	Represents a single node in a pathing graph. This is a spot in the
	world that an AI agent can path to.
*/
class PathingNode
{
public:
	/// Constructor
	explicit PathingNode(const Vec3& pos, float tolerance = PATHING_DEFAULT_NODE_TOLERANCE);

	/// Return the world space position of the node
	const Vec3& GetPos() const;

	/// Return the tolerance for the node
	float GetTolerance() const;

	/// Add a pathing arc to this node
	void AddArc(PathingArc* pArc);

	/// Get adjacent nodes to this node by passing a list of node ptrs by reference
	void GetNeighbors(PathingNodeList& neighbors);

	/// Return the cost of travelling from a node to this node
	/// This is the arc weight * actual distance between the nodes
	float GetCostFromNode(PathingNode* pFromNode);

private:
	/// Returns the arc between this node and a given node
	PathingArc* FindArc(PathingNode* pLinkedNode);

private:
	/// Tolerance around the node where an agent is "at" the node
	float m_Tolerance;

	/// World space position of the node
	Vec3 m_Pos;

	/// List of edges (arcs) from this node to other nodes
	PathingArcList m_Arcs;
};
