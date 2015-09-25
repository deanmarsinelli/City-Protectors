/*
	PathingGraph.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "AStar.h"
#include "BaseGameLogic.h"
#include "EngineStd.h"
#include "Logger.h"
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

void PathingGraph::BuildTestGraph()
{
	if (!m_Nodes.empty())
	{
		DestroyGraph();
	}

	m_Nodes.reserve(81);

	int index = 0;
	for (float x = -45.0f; x < 45.0f; x += 10.0f)
	{
		for (float z = -45.0f; z < 45.0f; z += 10.0f)
		{
			// add new node
			PathingNode* pNode = new PathingNode(Vec3(x, 0, z));
			m_Nodes.push_back(pNode);

			// link it to the previous node
			int tempNode = index - 1;
			if (tempNode >= 0)
			{
				LinkNodes(m_Nodes[tempNode], pNode);
			}

			// link it to the node above it
			tempNode = index - 9;
			if (tempNode >= 0)
			{
				LinkNodes(m_Nodes[tempNode], pNode);
			}

			index++;
		}
	}


}

void PathingGraph::LinkNodes(PathingNode* pNodeA, PathingNode* pNodeB)
{
	CB_ASSERT(pNodeA);
	CB_ASSERT(pNodeB);

	// create an arc to link the nodes
	PathingArc* pArc = CB_NEW PathingArc;
	pArc->LinkNodes(pNodeA, pNodeB);
	pNodeA->AddArc(pArc);
	pNodeB->AddArc(pArc);

	m_Arcs.push_back(pArc);

	// add the nodes to the graph if they dont exist there already
	/*if (std::find(m_Nodes.begin(), m_Nodes.end(), pNodeA) == m_Nodes.end())
	{
		m_Nodes.push_back(pNodeA);
	}
	if (std::find(m_Nodes.begin(), m_Nodes.end(), pNodeB) == m_Nodes.end())
	{
		m_Nodes.push_back(pNodeB);
	}*/
}


PathingGraph* CreatePathingGraph()
{
	PathingGraph* pPathingGraph = CB_NEW PathingGraph;
	
	return pPathingGraph;
}
