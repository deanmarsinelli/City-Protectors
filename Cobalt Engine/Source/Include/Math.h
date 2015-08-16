/*
	Math.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "types.h"
#include <vector>

typedef std::vector<Point> Poly;

/// Determine if a point exists inside a polygon. Algorithm from Graphics Gems
extern bool PointInPoly(const Point& test, const Poly& polygon);