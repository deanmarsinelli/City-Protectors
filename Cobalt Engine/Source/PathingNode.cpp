/*
	PathingNode.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Logger.h"
#include "PathingArc.h"
#include "PathingNode.h"

PathingNode::PathingNode(const Vec3& pos, float tolerance) :
m_Pos(pos),
m_Tolerance(tolerance)
{ }

const Vec3& PathingNode::GetPos() const
{
	return m_Pos;
}

float PathingNode::GetTolerance() const
{
	return m_Tolerance;
}

void PathingNode::AddArc(PathingArc* pArc)
{
	// make sure the arc is not null, then add it
	CB_ASSERT(pArc);
	m_Arcs.push_back(pArc);
}

void PathingNode::GetNeighbors(PathingNodeList& neighbors)
{
	// iterate the arcs attached to this node
	for (auto it = m_Arcs.begin(); it != m_Arcs.end(); ++it)
	{
		// get each neighboring (adjacent) node and add it to the list
		neighbors.push_back((*it)->GetNeighbor(this));
	}
}

float PathingNode::GetCostFromNode(PathingNode* pFromNode)
{
	CB_ASSERT(pFromNode);
	// find the arc between the two nodes
	PathingArc* pArc = FindArc(pFromNode);
	CB_ASSERT(pArc);

	// get the vector between the two nodes
	Vec3 diff = m_Pos - pFromNode->GetPos();
	
	// return the arc weight * actual distance between nodes
	return pArc->GetWeight() * diff.Length();
}

PathingArc* PathingNode::FindArc(PathingNode* pLinkedNode)
{
	CB_ASSERT(pLinkedNode);

	// iterate all the arcs attached to this node
	for (auto it = m_Arcs.begin(); it != m_Arcs.end(); ++it)
	{
		PathingArc* pArc = *it;
		// if the node attached at the other end of this arc is the linked node
		// then return this arc
		if (pArc->GetNeighbor(this) == pLinkedNode)
		{
			return pArc;
		}
	}

	return nullptr;
}
