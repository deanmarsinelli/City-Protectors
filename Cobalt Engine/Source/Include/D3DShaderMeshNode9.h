/*
	D3DShaderMeshNode9.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <d3dx9.h>

#include "D3DSceneNode.h"
#include "EngineStd.h"
#include "interfaces.h"

/**
	Add a ID3DXMesh Mesh object to the scene graph.
*/
class D3DMeshNode9 : public D3DSceneNode9
{
public:
	/// Constructor taking a mesh object 
	D3DMeshNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
		ID3DXMesh* mesh, RenderPass renderPass, const Mat4x4* t);

	/// Constructor taking a file path for a mesh object
	D3DMeshNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
		std::wstring meshFileName, RenderPass renderPass, const Mat4x4* t);

	/// Destructor releases the mesh
	virtual ~D3DMeshNode9() { CB_COM_RELEASE(m_pMesh); }

	/// Render the mesh
	HRESULT Render(Scene* pScene);

	/// Loads the mesh and ensures the mesh has normals. Also optimizes the 
	/// mesh by organizing the internal triangle list for less cache misses.
	virtual HRESULT OnRestore(Scene* pScene);

	/// Find the intersection of a ray with a polygon on a scene node
	virtual HRESULT Pick(Scene* pScene, RayCast* pRayCast);

	/// Calculate a bounding sphere for the mesh
	float CalcBoundingSphere();

protected:
	/// The Direct3D mesh object
	ID3DXMesh* m_pMesh;

	/// File name of the mesh to load
	std::wstring m_FileName;
};


/*
	Add a ID3DXMesh object to the scene graph and render it with a shader.
*/
class D3DShaderMeshNode9 : public D3DMeshNode9
{
public:
	/// Constructor taking a mesh object 
	D3DShaderMeshNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
		ID3DXMesh* mesh, std::string fxFileName, RenderPass renderPass, const Mat4x4* t);

	/// Constructor taking a file path
	D3DShaderMeshNode9(const GameObjectId objectId, WeakBaseRenderComponentPtr renderComponent,
		std::wstring meshFileName, std::string fxFileName, RenderPass renderPass, const Mat4x4* t);

	/// Destructor releases the D3D effect
	virtual ~D3DShaderMeshNode9() { CB_COM_RELEASE(m_pEffect); }
	
	virtual HRESULT OnRestore(Scene *pScene);
	
	virtual HRESULT OnLostDevice(Scene *pScene);
	
	/// Render the mesh node with a shader
	HRESULT Render(Scene *pScene);

protected:
	/// Pointer to the ID3DX effect
	ID3DXEffect* m_pEffect;

	/// File name of the mesh to load
	std::string m_FXFileName;
};
