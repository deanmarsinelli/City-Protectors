/*
	Raycast.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3d9.h>
#include <vector>

#include "interfaces.h"
#include "Matrix.h"
#include "types.h"
#include "Vector.h"

struct Intersection;
typedef std::vector<Intersection> IntersectionArray;

class CDXUTSDKMesh;

/**
	A simple ray cast class.
*/
class RayCast
{
public:
	/// Constructor takes in an screen space origin point and max intersections
	RayCast(Point point, DWORD maxIntersections = 16);

	// Different overloads to send out a ray cast
	HRESULT Pick(Scene* pScene, GameObjectId objectId, ID3DXMesh* pMesh);
	HRESULT Pick(Scene* pScene, GameObjectId objectId, CDXUTSDKMesh* pMesh);
	HRESULT Pick(Scene* pScene, GameObjectId objectId, LPDIRECT3DVERTEXBUFFER9 pVerts, LPDIRECT3DINDEXBUFFER9 pIndices, DWORD numPolys);

	/// Sort intersections
	void Sort();

public:
	/// Maximum number of intersections a ray can hit
	DWORD m_MaxIntersections;

	/// Number of intersections a ray has hit
	DWORD m_NumIntersections;

	/// Whether or not to use D3DXIntersect
	bool m_UseD3DXIntersect;

	/// Whether to get the first hit or all hits
	bool m_AllHits;

	/// Screen space point origin for the ray cast
	Point m_Point;

	/// Direction vector for the ray cast
	D3DXVECTOR3 m_PickRayDir;

	/// Origin point of the ray cast
	D3DXVECTOR3 m_PickRayOrigin;

	IntersectionArray m_IntersectionArray;

protected:
	/// Pointer to a d3d9 vertex buffer
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
};


/**
	Holds intersection data of a ray cast.
*/
struct Intersection
{
	/// Distance from the ray origin to the intersection point
	FLOAT m_Dist;

	/// The face index of the intersection
	DWORD m_Face;

	/// Barcentric coordinates of the intersection
	FLOAT m_Bary1, m_Bary2;

	/// Texture coordinates of the intersection
	FLOAT m_tu, m_tv;

	/// Id of the game object that was intersected with
	GameObjectId m_ObjectId;

	/// World location of the intersection
	Vec3 m_WorldLoc;

	/// Local object coordinates of the intersection
	Vec3 m_ObjectLoc;

	/// Normal of the intersection
	Vec3 m_Normal;

	/// Less than comparitor operator
	const bool operator <(const Intersection& rhs)
	{
		return m_Dist < rhs.m_Dist;
	}
};

/// Initialize an intersection structure
template<typename T>
void InitIntersection(Intersection& intersection, DWORD faceIndex, FLOAT dist, FLOAT u, FLOAT v,
	GameObjectId objectId, WORD* pIndices, T* pVertices, const Mat4x4& matWorld)
{
	intersection.m_Face = faceIndex;
	intersection.m_Dist = dist;
	intersection.m_Bary1 = u;
	intersection.m_Bary2 = v;

	T* v0 = &pVertices[pIndices[3 * faceIndex + 0]];
	T* v1 = &pVertices[pIndices[3 * faceIndex + 1]];
	T* v2 = &pVertices[pIndices[3 * faceIndex + 2]];

	// get texture coordinates of the intersection
	FLOAT dtu1 = v1->tu - v0->tu;
	FLOAT dtu2 = v2->tu - v0->tu;
	FLOAT dtv1 = v1->tv - v0->tv;
	FLOAT dtv2 = v2->tv - v0->tv;
	intersection.m_tu = v0->tu + intersection.m_Bary1 * dtu1 + intersection.m_Bary2 * dtu2;
	intersection.m_tv = v0->tv + intersection.m_Bary1 * dtv1 + intersection.m_Bary2 * dtv2;

	Vec3 a = v0->position - v1->position;
	Vec3 b = v2->position - v1->position;

	// get the cross product and normalize it to get the surface normal
	Vec3 cross = a.Cross(b);
	cross /= cross.Length();
	intersection.m_Normal = cross;

	// get object information
	Vec3 objectLoc = BarycentricToVec3(v0->position, v1->position, v2->position, intersection.m_Bary1, intersection.m_Bary2);
	intersection.m_ObjectLoc = objectLoc;
	intersection.m_WorldLoc = matWorld.Transform(objectLoc);
	intersection.m_ObjectId = objectId;
}