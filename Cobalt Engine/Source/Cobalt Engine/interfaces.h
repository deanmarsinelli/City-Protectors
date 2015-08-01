/**
	interfaces.h

	This file contains declaration data and
	type definitions for commonly used interface
	classes in the engine.
*/

#pragma once

#include <memory>

using std::shared_ptr;
using std::weak_ptr;

// forward declarations
class GameObject;
class Component;

typedef unsigned int GameObjectId;
typedef unsigned int ComponentId;

typedef shared_ptr<GameObject> StrongGameObjectPtr;
typedef weak_ptr<GameObject> WeakGameObjectPtr;
typedef shared_ptr<Component> StrongComponentPtr;
typedef weak_ptr<Component> WeakComponentPtr;