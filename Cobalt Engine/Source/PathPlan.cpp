/*
	PathPlan.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Logger.h"
#include "PathPlan.h"


//====================================================
//	PathPlan definitions
//====================================================

PathPlan::PathPlan()
{ 
	// set the index to the first node
	m_Index = m_Path.begin();
}

void PathPlan::ResetPath()
{
	m_Index = m_Path.begin();
}

const Vec3& PathPlan::GetCurrentNodePosition() const
{
	// make sure the current node is valid
	CB_ASSERT(m_Index != m_Path.end());

	// return world position for the current node
	return (*m_Index)->GetPos();
}

bool PathPlan::CheckForNextNode(const Vec3& pos)
{
	// this function is used to see if we are "at" the next node 
	// yet or not

	if (m_Index == m_Path.end())
	{
		return false;
	}

	// get the vector from the point to the next node's position
	Vec3 diff = pos - (*m_Index)->GetPos();

	// if the length of the vector is less than the nodes tollerance,
	// then the position is "within" the node, and we increment the index
	if (diff.Length() <= (*m_Index)->GetTollerance())
	{
		++m_Index;
		return true;
	}

	return false;
}

bool PathPlan::CheckForEnd()
{
	// return true if the path has been completed
	if (m_Index == m_Path.end())
	{
		return true;
	}
	return false;
}

void PathPlan::AddNode(PathingNode* pNode)
{
	CB_ASSERT(pNode);
	m_Path.push_front(pNode);
}


//====================================================
//	PathPlanNode definitions
//====================================================
PathPlanNode::PathPlanNode(PathingNode* pNode, PathPlanNode* pPrevNode, PathingNode* pGoalNode)
{
	CB_ASSERT(pNode);

	m_pPathingNode = pNode;
	m_pPrev = pPrevNode;
	m_pGoalNode = pGoalNode;
	m_Closed = false;
	UpdateHeuristics();
}

PathPlanNode* PathPlanNode::GetPrev() const
{
	return m_pPrev;
}

PathingNode* PathPlanNode::GetPathingNode() const
{
	return m_pPathingNode;
}

bool PathPlanNode::IsClosed() const
{
	return m_Closed;
}

float PathPlanNode::GetGoal() const
{
	return m_Goal;
}

float PathPlanNode::GetHeuristic() const
{
	return m_Heuristic;
}

float PathPlanNode::GetFitness() const
{
	return m_Fitness;
}

void PathPlanNode::UpdatePrevNode(PathPlanNode* pPrev)
{
	CB_ASSERT(pPrev);
	m_pPrev = pPrev;
	UpdateHeuristics();
}

void PathPlanNode::SetClosed(bool toClose)
{
	m_Closed = toClose;
}

bool PathPlanNode::IsBetterChoiceThan(PathPlanNode* pNode)
{
	// return whether this node has a better fitness than the other node
	return (m_Fitness < pNode->GetFitness());
}

void PathPlanNode::UpdateHeuristics()
{
	// calculate goal: cost up to this node (g)
	if (m_pPrev)
	{
		// goal = prevGoal + cost from prev to this node
		m_Goal = m_pPrev->GetGoal() + m_pPathingNode->GetCostFromNode(m_pPrev->GetPathingNode());
	}
	else
	{
		m_Goal = 0.0f;
	}

	// calculate heuristic: estimation from this node to goal node (f)
	// this is an estimation so we'll use the world distance since it is
	// guaranteed to be less than the pathing distance
	Vec3 diff = m_pPathingNode->GetPos() - m_pGoalNode->GetPos();
	m_Heuristic = diff.Length();

	// calculate fitness: total cost from start to finished (f)
	m_Fitness = m_Goal + m_Heuristic;
}
