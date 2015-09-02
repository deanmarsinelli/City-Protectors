/*
	Vector.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3dx9.h>
#include <list>

/**
	Represents a 3D vector.
*/
class Vec3 : public D3DXVECTOR3
{
public:
	/// Default constructor is the 0 vector
	Vec3() : D3DXVECTOR3() { x = 0; y = 0; z = 0; }

	/// Constructor from another vector
	Vec3(D3DXVECTOR3& vec) { x = vec.x; y = vec.y; z = vec.z; }

	/// Constructor from three floats
	Vec3(const float x, const float y, const float z) { this->x = x; this->y = y; this->z = z; }

	/// Constructor from three doubles
	Vec3(const double x, const double y, const double z) { this->x = (float)x; this->y = (float)y; this->z = (float)z; }

	/// Constructor from a Vec4
	Vec3(const class Vec4& vec4);

	/// Return the length of the vector
	float Length() const { return D3DXVec3Length(this); }

	/// Normalize the vector
	Vec3* Normalize() { return static_cast<Vec3*>(D3DXVec3Normalize(this, this)); }

	/// Return the dot product of this vector with another Vec3
	float Dot(const Vec3& vec) const { return D3DXVec3Dot(this, &vec); }

	/// Return the cross product of this vector with another Vec3
	Vec3 Cross(const Vec3& vec) const;
};


/**
	Represents a 4D vector.
*/
class Vec4 : public D3DXVECTOR4
{
public:
	/// Default constructor is the 0 vector
	Vec4() : D3DXVECTOR4() { x = 0; y = 0; z = 0; }

	/// Constructor from another vector
	Vec4(D3DXVECTOR4& vec) { x = vec.x; y = vec.y; z = vec.z; w = vec.w; }

	/// Constructor from four floats
	Vec4(const float x, const float y, const float z, const float w) { this->x = x; this->y = y; this->z = z; this->w = w; }

	/// Constructor from four doubles
	Vec4(const double x, const double y, const double z, const float w) { this->x = (float)x; this->y = (float)y; this->z = (float)z; this->w = (float)w; }

	/// Constructor from a Vec3
	Vec4(const Vec3& vec3) { x = vec3.x; y = vec3.y; z = vec3.z; w = 1.0f; }

	/// Return the length of the vector
	float Length() { return D3DXVec4Length(this); }

	/// Return the dot product of this vector with another Vec4
	Vec4* Normalize() { return static_cast<Vec4*>(D3DXVec4Normalize(this, this)); }

	/// Return the dot product of 
	float Dot(const Vec4& vec) { return D3DXVec4Dot(this, &vec); }
};


/// Calculate the velocity of an object given 2 points and time
inline Vec3 CalcVelocity(const Vec3& p0, const Vec3& p1, float time)
{
	return (p1 - p0) / time;
}


/// Calculate the acceleration of an object given 2 velocities and time
inline Vec3 CalcAcceleration(const Vec3& v0, const Vec3& v1, float time)
{
	return (v1 - v0) / time;
}


/// Update position and velocity vectors given a known acceleration and time
inline Vec3 HandleAccel(Vec3& pos, Vec3& vel, const Vec3& accel, float time)
{
	vel += accel * time;
	pos += vel * time;
}


/// A 2D Vector with x and y coordinates
typedef D3DXVECTOR2 Vec2;

typedef std::list<Vec3> Vec3List;
typedef std::list<Vec4> Vec4List;

extern Vec3 g_Up;
extern Vec3 g_Right;
extern Vec3 g_Forward;

extern Vec4 g_Up4;
extern Vec4 g_Right4;
extern Vec4 g_Forward4;
