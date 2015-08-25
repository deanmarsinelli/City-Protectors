#pragma once

#include "Component.h"

class PhysicsComponent : public Component
{
public:

	void RotateY(float angleRadians);
	void SetPosition(float x, float y, float z);
	void Stop();

public:
	static const char* g_Name;
};
