/*
	PathPlan.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Logger.h"
#include "PathPlan.h"

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
