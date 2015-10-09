/*
	Math.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "MathUtils.h"

#include "Vector.h"

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

float WrapPi(float wrapMe)
{
	float result = Wrap2Pi(wrapMe + CB_PI);
	return (result - CB_PI);
}

float Wrap2Pi(float wrapMe)
{
	if (wrapMe > CB_2PI)
		wrapMe = fmod(wrapMe, CB_2PI);
	else if (wrapMe < 0)
		wrapMe = CB_2PI - fmod(fabs(wrapMe), CB_2PI);
	return wrapMe;
}

Vec3 GetVectorFromYRotation(float angleRadians)
{
	Vec3 lookAt;
	WrapPi(angleRadians);
	lookAt.x = cos(angleRadians);
	lookAt.y = 0;
	lookAt.z = sin(angleRadians);
	lookAt.Normalize();
	return lookAt;
}

float GetYRotationFromVector(const Vec3& lookAt)
{
	// 0 orientation means staring down the positive Z axis
	Vec3 zUnit(0.f, 0.f, 1.f); 
	float angle = (atan2(lookAt.z, -lookAt.x) - atan2(zUnit.z, zUnit.x));
	return Wrap2Pi(angle);
}

