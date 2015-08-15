/*
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
	/// Default constructor with a (0,0) point
	Point() { x = y = 0; }

	/// Constructor taking in two coordinates
	Point(const long _x, const long _y) { x = _x; y = _y; }

	/// Copy constructor from a point reference
	Point(const Point& newPoint) { x = newPoint.x; y = newPoint.y; }

	/// Copy constructor taking a point pointer
	Point(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; }

	// overloaded operators
	/// Assignment operator using a point reference
	Point& operator=(const Point& newPoint) { x = newPoint.x; y = newPoint.y; return *this; }
	
	/// Assignment operator using a point pointer
	Point& operator=(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; return *this; }
	
	/// Addition operator
	Point operator+(const Point& other) { Point temp(this); temp += other; return temp; }
	
	/// Addition operator using a point reference
	Point& operator+=(const Point& other) { x += other.x; y += other.y; }
	
	/// Addition assignment using a point pointer
	Point& operator+=(const Point* pOther) { x += pOther->x; y += pOther->y; }
	
	/// Subtraction operator
	Point operator-(const Point& other) { Point temp(this); temp -= other; return temp; }
	
	/// Subtraction assignment using a point reference
	Point& operator-=(const Point& other) { x -= other.x; y -= other.y; }
	
	/// Subtraction assignment using a point pointer
	Point& operator-=(const Point* pOther) { x -= pOther->x; y -= pOther->y; }
	
	/// Comparison operator
	bool operator==(const Point& other) const { return (x == other.x && y == other.y); }
	
	/// Not equal operator
	bool operator!=(const Point& other) const { return !(x == other.x && y == other.y); }

	/// x-coordinate of the point
	long x;

	/// y-coordinate of the point
	long y;
};

/**
	Represents a rectangle box
*/
class Rect
{
public:
	long top, left, right, bottom;
};