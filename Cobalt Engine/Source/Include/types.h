/**
	types.h

	Some basic types used throughout the game.
	All the code is inline.

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

/**
	Represents a 2D point
*/
class Point
{
public:
	// Constructors
	Point() { x = y = 0; }
	Point(const long _x, const long _y) { x = _x; y = _y; }
	Point(const Point& newPoint) { x = newPoint.x; y = newPoint.y; }
	Point(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; }

	// overloaded operators
	// assignment
	Point& operator=(const Point& newPoint) { x = newPoint.x; y = newPoint.y; return *this; }
	Point& operator=(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; return *this; }
	// addition
	Point operator+(const Point& other) { Point temp(this); temp += other; return temp; }
	Point& operator+=(const Point& other) { x += other.x; y += other.y; }
	Point& operator+=(const Point* pOther) { x += pOther->x; y += pOther->y; }
	// subtraction
	Point operator-(const Point& other) { Point temp(this); temp -= other; return temp; }
	Point& operator-=(const Point& other) { x -= other.x; y -= other.y; }
	Point& operator-=(const Point* pOther) { x -= pOther->x; y -= pOther->y; }
	// comparison
	bool operator==(const Point& other) const { return (x == other.x && y == other.y); }
	bool operator!=(const Point& other) const { return !(x == other.x && y == other.y); }

	long x, y;
};


/**
	Represents a rectangle box
*/
class Rect
{
public:
	long top, left, right, bottom;
};