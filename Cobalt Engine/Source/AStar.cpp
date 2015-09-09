/*
	AStar.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "AStar.h"
#include "EngineStd.h"
#include "Logger.h"

AStar::AStar()
{
	m_pStartNode = nullptr;
	m_pGoalNode = nullptr;
}

AStar::~AStar()
{
	Destroy();
}

void AStar::Destroy()
{
	// destroy all PathPlanNode objects and clear the map
	// the actual PathingNode objects are not released, they live in the graph
	for (auto it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
	{
		delete it->second; // delete PathPlanNode object
		it->second = nullptr;
	}
	m_Nodes.clear();

	// clear the open set
	m_OpenSet.clear();

	// reset the start and end nodes
	m_pStartNode = nullptr;
	m_pGoalNode = nullptr;
}

PathPlan* AStar::operator()(PathingNode* pStartNode, PathingNode* pGoalNode)
{
	CB_ASSERT(pStartNode);
	CB_ASSERT(pGoalNode);

	// make sure the start and end nodes are not the same
	if (pStartNode == pGoalNode)
		return nullptr;

	m_pStartNode = pStartNode;
	m_pGoalNode = pGoalNode;

	// the open set is a priority queue of nodes that have not been evauluated yet.
	// if it is empty, no path was found to the goal. the start node is the only node
	// initially in the open set
	AddToOpenSet(m_pStartNode, nullptr);

	while (!m_OpenSet.empty())
	{
		// get the most likely candidate to be the next node
		PathPlanNode* pNode = m_OpenSet.front();

		// if this node is the goal node, rebuild and return the path from here
		if (pNode->GetPathingNode() == m_pGoalNode)
			return RebuildPath(pNode);

		// process this node
		m_OpenSet.pop_front();
		AddToClosedSet(pNode);

		// get all neighbors to this node (adjacent nodes in the graph)
		PathingNodeList neighbors;
		pNode->GetPathingNode()->GetNeighbors(neighbors);

		// loop through all neighboring nodes and evaluate each one
		for (auto it = neighbors.begin(); it != neighbors.end(); ++it)
		{
			PathingNode* pNodeToEvaluate = *it;

			// try to find a PathPlanNode helper for this node, if one exists
			// and it is closed then skip this node (it has already been evaluated)
			auto findIt = m_Nodes.find(pNodeToEvaluate);
			if (findIt != m_Nodes.end() && findIt->second->IsClosed())
			{
				continue;
			}

			// calculate the cost of the route through this node to be evaluated
			// cost = cost up to this point (goal) + cost to the next node
			float cost = pNode->GetGoal() + pNodeToEvaluate->GetCostFromNode(pNode->GetPathingNode());
			bool isPathBetter = false;

			// get the PathPlanNode helper if there is one
			PathPlanNode* pPathPlaneNodeToEvaluate = nullptr;
			if (findIt != m_Nodes.end())
			{
				pPathPlaneNodeToEvaluate = findIt->second;
			}

			// if there is no PathPlanNode helper, it means we've never added this node to the open set
			// before and that this is the best path through this node that we've found (these nodes should
			// be linked together in the path. Adding it to the open set will set heuristic data and insert
			// it appropriately.
			if (!pPathPlaneNodeToEvaluate)
			{
				pPathPlaneNodeToEvaluate = AddToOpenSet(pNodeToEvaluate, pNode);
			}
			else if (cost < pPathPlaneNodeToEvaluate->GetGoal())
			{
				// if the PathPlanNode helper exists, it is already in the open set, check to see if this 
				// new route through it is better than the last route through it (its last goal)
				isPathBetter = true;
			}

			// if the path was better, link the nodes and reinsert evaluate node into the open set
			// where it will have its heuristics updated and be sorted into the priority queue properly
			if (isPathBetter)
			{
				pPathPlaneNodeToEvaluate->UpdatePrevNode(pNode);
				ReinsertNode(pPathPlaneNodeToEvaluate);
			}
		}
	}

	// no path to goal
	return nullptr;
}

PathPlanNode* AStar::AddToOpenSet(PathingNode* pNode, PathPlanNode* pPrevNode)
{
	CB_ASSERT(pNode);

	// check if this node exists in the open set already, if not add it
	auto it = m_Nodes.find(pNode);
	PathPlanNode* pThisNode = nullptr;

	if (it == m_Nodes.end())
	{
		// node does not exist in the open set
		// create a new PathPlanNode and add the pair to the open set
		pThisNode = CB_NEW PathPlanNode(pNode, pPrevNode, m_pGoalNode);
		m_Nodes.insert(std::make_pair(pNode, pThisNode));
	}
	else
	{
		CB_WARNING("Adding existing PathPlanNode to the open set");
		pThisNode = it->second;
		pThisNode->SetClosed(false);
	}

	// insert the PathPlanNode into the priority queue
	InsertNode(pThisNode);

	return pThisNode;
}

void AStar::AddToClosedSet(PathPlanNode* pNode)
{
	CB_ASSERT(pNode);
	// mark the node as closed
	pNode->SetClosed();
}

void AStar::InsertNode(PathPlanNode* pNode)
{
	CB_ASSERT(pNode);

	// if the set is empty, add the node
	if (m_OpenSet.empty())
	{
		m_OpenSet.push_back(pNode);
		return;
	}

	// otherwise, insert the node into the list by priority
	auto it = m_OpenSet.begin();
	PathPlanNode* pCompare = *it;
	while (pCompare->IsBetterChoiceThan(pNode))
	{
		++it;
		if (it != m_OpenSet.end())
		{
			// comparitor node = the next node as long as we're not at the end
			pCompare = *it;
		}
		else
		{
			// break when the current node is a worse choice than our node to be inserted
			break;
		}
	}

	// insert our node before the last node we compared with (it)
	m_OpenSet.insert(it, pNode);
}

void AStar::ReinsertNode(PathPlanNode* pNode)
{
	CB_ASSERT(pNode);

	// iterate the list looking for the node
	for (auto it = m_OpenSet.begin(); it != m_OpenSet.end(); ++it)
	{
		// if the node is found, remove it and reinsert it
		if (pNode == *it)
		{
			m_OpenSet.erase(it);
			InsertNode(pNode);
			return;
		}
	}

	// the node was not in the open list to begin with
	CB_WARNING("Attempting to reinsert a node that was not in the open list");
	InsertNode(pNode);
}

PathPlan* AStar::RebuildPath(PathPlanNode* pGoalNode)
{
	CB_ASSERT(pNode);

	PathPlan* pPlan = CB_NEW PathPlan;

	// start at the goal node
	PathPlanNode* pNode = pGoalNode;
	while (pNode)
	{
		// keep adding the current mode to the front of the path
		pPlan->AddNode(pNode->GetPathingNode());
		pNode = pNode->GetPrev();
	}

	return pPlan;
}
