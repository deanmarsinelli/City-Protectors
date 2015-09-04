/*
	Physics.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"

/// Create the physics world and return a pointer to it
extern IGamePhysics* CreateGamePhysics();