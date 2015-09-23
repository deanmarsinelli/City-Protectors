/*
	PathingGraph.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "AStar.h"
#include "BaseGameLogic.h"
#include "EngineStd.h"
#include "PathingGraph.h"

PathingGraph::~PathingGraph()
{
	DestroyGraph();
}

void PathingGraph::DestroyGraph()
{
	// destroy all nodes
	for (auto it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
	{
		delete (*it);
	}
	m_Nodes.clear();

	// destroy all arcs
	for (auto it = m_Arcs.begin(); it != m_Arcs.end(); ++it)
	{
		delete (*it);
	}
	m_Arcs.clear();
}

PathingNode* PathingGraph::FindClosestNode(const Vec3& position)
{
	// O(n) algorithm that does not utilize spatial partitioning
	PathingNode* pClosestNode = m_Nodes.front();
	float length = FLT_MAX;

	// iterate every node finding the distance from the given position
	for (auto it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
	{
		PathingNode* pNode = *it;
		Vec3 diff = position - pNode->GetPos();
		if (diff.Length() < length)
		{
			pClosestNode = pNode;
			length = diff.Length();
		}
	}

	return pClosestNode;
}

PathingNode* PathingGraph::FindFurthestNode(const Vec3& position)
{
	// O(n) algorithm that does not utilize spatial partitioning
	PathingNode* pFurthestNode = m_Nodes.front();
	float length = 0.0f;

	// iterate every node finding the distance from the given position
	for (auto it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
	{
		PathingNode* pNode = *it;
		Vec3 diff = position - pNode->GetPos();
		if (diff.Length() > length)
		{
			pFurthestNode = pNode;
			length = diff.Length();
		}
	}

	return pFurthestNode;
}

PathingNode* PathingGraph::FindRandomNode()
{
	unsigned int numNodes = (unsigned)m_Nodes.size();
	unsigned int node = g_pApp->m_pGame->GetRNG().Random(numNodes);

	// lower half of the node list
	if (node <= numNodes / 2)
	{
		auto it = m_Nodes.begin();
		for (int i = 0; i < node; i++)
		{
			++it;
		}
		return *it;
	}
	// upper half
	else
	{
		auto it = m_Nodes.end();
		for (int i = numNodes; i >= node; i--)
		{
			--it;
		}
		return *it;
	}

}

PathPlan* PathingGraph::FindPath(const Vec3& startPoint, const Vec3& endPoint)
{
	// find the closest nodes to the start and end points
	PathingNode* pStart = FindClosestNode(startPoint);
	PathingNode* pEnd = FindClosestNode(endPoint);

	return FindPath(pStart, pEnd);
}

PathPlan* PathingGraph::FindPath(const Vec3& startPoint, PathingNode* pEndNode)
{
	// find closest node to start point
	PathingNode* pStart = FindClosestNode(startPoint);

	return FindPath(pStart, pEndNode);
}

PathPlan* PathingGraph::FindPath(PathingNode* pStartNode, const Vec3& endPoint)
{
	// find closest node to the end point
	PathingNode* pEnd = FindClosestNode(endPoint);

	return FindPath(pStartNode, pEnd);
}

PathPlan* PathingGraph::FindPath(PathingNode* pStartNode, PathingNode* pEndNode)
{
	// use A* to find a path between nodes
	AStar aStar;

	return aStar(pStartNode, pEndNode);
}
