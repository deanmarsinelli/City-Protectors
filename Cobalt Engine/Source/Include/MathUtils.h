/*
	Math.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <vector>

#include "EngineStd.h"
#include "types.h"

#define RADIANS_TO_DEGREES(x) ((x) * 180.0f / CB_PI)
#define DEGREES_TO_RADIANS(x) ((x) * CB_PI / 180.0f)

class Vec3;
typedef std::vector<Point> Poly;

/// Determine if a point exists inside a polygon. Algorithm from Graphics Gems
extern bool PointInPoly(const Point& test, const Poly& polygon);

float WrapPi(float angle);
Vec3 GetVectorFromYRotation(float angleRadians);
float GetYRotationFromVector(const Vec3& lookAt);


class CBRandom
{
public:
	void Randomize();
};
