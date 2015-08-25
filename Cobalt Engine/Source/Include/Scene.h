#pragma once

#include "interfaces.h"

class Scene
{
public:
	Scene(shared_ptr<IRenderer> renderer) { }
	shared_ptr<IRenderer> GetRenderer();
};