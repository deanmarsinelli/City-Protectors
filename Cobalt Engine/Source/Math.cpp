/*
	Math.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Math.h"

bool PointInPoly(const Point& test, const Poly& polygon)
{
	Point newPoint, oldPoint;
	Point left, right;

	bool inside = false;

	size_t points = polygon.size();

	// the polygon must be at least a triangle
	if (points < 3)
		return false;

	oldPoint = polygon[points - 1];

	for (unsigned int i = 0; i < points; i++)
	{
		newPoint = polygon[i];
		if (newPoint.x > oldPoint.x)
		{
			left = oldPoint;
			right = newPoint;
		}
		else
		{
			left = newPoint;
			right = oldPoint;
		}

		if ((newPoint.x < test.x) == (test.x <= oldPoint.x) &&
			(test.y - left.y) * (right.x - left.x) <
			(right.y - left.y) * (test.x - left.x))
		{
			inside = !inside;
		}

		oldPoint = newPoint;
	}

	return inside;
}