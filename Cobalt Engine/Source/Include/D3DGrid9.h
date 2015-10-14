/*
	D3DGrid9.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "ResourceHandle.h"
#include "SceneNode.h"

/**
	A grid (for a world) rendered with Direct3D 9.
*/
class D3DGrid9 : public SceneNode
{
public:
	/// Constructor constructs the scene node
	D3DGrid9(GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent, const Mat4x4* pMatrix);

	/// Virtual destructor
	virtual ~D3DGrid9();

	/// Set up the grid on restore
	virtual HRESULT OnRestore(Scene* pScene);

	/// Render the grid
	virtual HRESULT Render(Scene* pScene);

	/// Find the intersection of a ray with a polygon on the scene node
	virtual HRESULT Pick(Scene* pScene, RayCast* pRayCast);

	/// Return whether the texture has alpha or not
	bool HasAlpha() const { return m_TextureHasAlpha; }

public:
	/// Does the grid texture have alpha
	bool m_TextureHasAlpha;

protected:
	/// Handle for the grid texture for the axis planes
	shared_ptr<ResHandle> m_Handle;

	/// Grid vertices
	LPDIRECT3DVERTEXBUFFER9 m_pVerts;

	/// Grid indices
	LPDIRECT3DINDEXBUFFER9 m_pIndices;

	/// Number of vertices
	DWORD m_NumVerts;

	/// Number of polygons
	DWORD m_NumPolys;
};
