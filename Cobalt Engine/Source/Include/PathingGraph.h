/*
	PathingGraph.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <vector>

#include "PathingArc.h"
#include "PathingNode.h"
#include "PathPlan.h"

typedef std::vector<PathingNode*> PathingNodeVec;

/**
	This class holds the pathing graph and owns all the pathing nodes
	and pathing arc objects. There should only be one pathing graph 
	per scene and it is the main interface into the pathing system.
*/
class PathingGraph
{
public:
	/// Default constructor
	PathingGraph() { }

	/// Default destructor destroys the graph
	~PathingGraph();

	/// Destroy the graph
	void DestroyGraph();

	/// Return the closest pathing node to a position coordinate
	PathingNode* FindClosestNode(const Vec3& position);

	/// Return the furthest node from a position coordinate
	PathingNode* FindFurthestNode(const Vec3& position);

	/// Return a random node in the graph
	PathingNode* FindRandomNode();

	/// Find a path between two position coordinates
	PathPlan* FindPath(const Vec3& startPoint, const Vec3& endPoint);

	/// Find a path between a start position and an end node
	PathPlan* FindPath(const Vec3& startPoint, PathingNode* pEndNode);

	/// Find a path between a beginning node and an end position
	PathPlan* FindPath(PathingNode* pStartNode, const Vec3& endPoint);

	/// Find a path between two nodes
	PathPlan* FindPath(PathingNode* pStartNode, PathingNode* pEndNode);

private:
	/// Link two nodes together with an arc
	void LinkNodes(PathingNode* pNodeA, PathingNode* pNodeB);

private:
	/// List of all nodes in the graph
	PathingNodeVec m_Nodes;

	/// List of all arcs between nodes in the graph
	PathingArcList m_Arcs;
};