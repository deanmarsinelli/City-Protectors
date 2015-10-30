/*
	AIView.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <Logger.h>

#include "AIView.h"

AIView::AIView(shared_ptr<PathingGraph> pPathingGraph) :
IGameView(),
m_pPathingGraph(pPathingGraph)
{
	//
}

AIView::~AIView()
{
	CB_LOG("AI", "Destroying AIView");
}