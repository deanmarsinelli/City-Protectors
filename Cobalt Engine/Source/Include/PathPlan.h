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
