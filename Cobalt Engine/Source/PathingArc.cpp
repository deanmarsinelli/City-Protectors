/*
	PathingArc.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Logger.h"
#include "PathingArc.h"

PathingArc::PathingArc(float weight) :
m_Weight(weight)
{ }

float PathingArc::GetWeight() const
{
	return m_Weight;
}

void PathingArc::LinkNodes(PathingNode* pNodeA, PathingNode* pNodeB)
{
	CB_ASSERT(pNodeA);
	CB_ASSERT(pNodeB);

	m_pNodes[0] = pNodeA;
	m_pNodes[1] = pNodeB;
}

PathingNode* PathingArc::GetNeighbor(PathingNode* pNode)
{
	CB_ASSERT(pNode);

	// make sure this node is part of this arc, and return the other node
	if (m_pNodes[0] == pNode)
	{
		return m_pNodes[1];
	}
	else if (m_pNodes[1] == pNode)
	{
		return m_pNodes[0];
	}
	else
	{
		// this node is not attached to this arc
		return nullptr;
	}
}
