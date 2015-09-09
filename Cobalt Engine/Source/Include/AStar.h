/*
	AStar.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <list>
#include <unordered_map>

#include "PathingNode.h"
#include "PathPlan.h"

typedef std::unordered_map<PathingNode*, PathPlanNode*> PathingNodeToPathPlanNodeMap;
typedef std::list<PathPlanNode*> PathPlanNodeList;

/**
	This class implements the A* algorithm to find a path between 
	two nodes in a pathing graph. It will build a PathPlan object.
*/
class AStar
{
public:
	/// Default constructor
	AStar();

	/// Default destructor
	~AStar();

	/// Release all the objects associated with this AStar object
	void Destroy();

	/// Build a path plan between two nodes using the A* algorithm
	PathPlan* operator()(PathingNode* pStartNode, PathingNode* pGoalNode);

private:
	/// Add a node to the open set of nodes that have not been processed yet
	PathPlanNode* AddToOpenSet(PathingNode* pNode, PathPlanNode* pPrevNode);

	/// Mark a node as closed (processed)
	void AddToClosedSet(PathPlanNode* pNode);

	/// Insert a node into the open set
	void InsertNode(PathPlanNode* pNode);

	/// Remove and reinsert a node into the open set
	void ReinsertNode(PathPlanNode* pNode);

	/// Given a goal node of an already built path, rebuild and return the path
	PathPlan* RebuildPath(PathPlanNode* pGoalNode);

private:
	/// Map of nodes mapping actual path nodes to the path plan nodes
	PathingNodeToPathPlanNodeMap m_Nodes;

	/// Starting node for the path
	PathingNode* m_pStartNode;

	/// Goal node -- where to end the path
	PathingNode* m_pGoalNode;

	/// List of path plan nodes that have not been processed yet
	PathPlanNodeList m_OpenSet;
};
