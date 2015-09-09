/*
	PathPlan.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "PathingNode.h"

/**
	Represents a complete path used by AI to determine where to go.
*/
class PathPlan
{
	friend class AStar;

public:
	/// Constructor
	PathPlan();
	
	/// Reset the index of the path
	void ResetPath();

	/// Get the world position of the current node
	const Vec3& GetCurrentNodePosition() const;

	/// Check if the given position is within the next node's tollerance 
	/// i.e. "are we at the next node in the path yet"
	bool CheckForNextNode(const Vec3& pos);

	/// Check to see if the current node is the end node (the goal)
	bool CheckForEnd();

private:
	/// Add a node into the path plan
	void AddNode(PathingNode* pNode);

private:
	/// List of nodes in the path plan
	PathingNodeList m_Path;

	/// An index into the list of pathing nodes marking the next node in the path
	PathingNodeList::iterator m_Index;
};


/**
	Helper node used in the pathing graph when finding a path between a starting node and
	goal node. These nodes track the heuristical and cost data for a given node when building 
	the path.
*/
class PathPlanNode
{
public:
	/// Constructor
	explicit PathPlanNode(PathingNode* pNode, PathPlanNode* pPrevNode, PathingNode* pGoalNode);

	/// Return the previous node
	PathPlanNode* GetPrev() const;

	/// Return the actual pathing node
	PathingNode* GetPathingNode() const;

	/// Has this node been processed yet nor not
	bool IsClosed() const;

	/// Return the goal cost
	float GetGoal() const;

	/// Return the heuristic cost
	float GetHeuristic() const;

	/// Return the fitness cost
	float GetFitness() const;

	/// Update the previous node in the path
	void UpdatePrevNode(PathPlanNode* pPrev);

	/// Set the closed value of this node
	void SetClosed(bool toClose = true);

	/// Return whether this node is a better choice then another node
	/// The node with the lower fitness will be chosen
	bool IsBetterChoiceThan(PathPlanNode* pNode);

private:
	/// Update heuristic estimation
	void UpdateHeuristics();

private:
	/// Previous node in the path where we just came from
	PathPlanNode* m_pPrev;

	/// Pointer to the pathing node from the pathing graph
	PathingNode* m_pPathingNode;

	/// Pointer to the goal node
	PathingNode* m_pGoalNode;

	/// This node is closed if it has already been processed
	bool m_Closed;

	/// Cost of the entire path up to this point: g
	float m_Goal;

	/// Estimated cost from this node to the goal: h
	float m_Heuristic;

	/// Estimated total cost from start to goal through this node: f = g + h
	float m_Fitness;
};
