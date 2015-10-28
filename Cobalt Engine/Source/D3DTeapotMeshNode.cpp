/*
	D3DTeapotMeshNode.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DTeapotMeshNode.h"

HRESULT D3DTeapotMeshNode9::OnRestore(Scene* pScene)
{
	HRESULT hr;

	IDirect3DDevice9* pDevice = DXUTGetD3D9Device();

	SAFE_RELEASE(m_pMesh);
	V(D3DXCreateTeapot(pDevice, &m_pMesh, nullptr));

	// rotate the teapot 90 degrees from default so that the spout faces forward
	Mat4x4 rotateY90;
	rotateY90.BuildRotationY(-CB_PI / 2.0f);
	IDirect3DVertexBuffer9* pVB = nullptr;
	m_pMesh->GetVertexBuffer(&pVB);
	Vec3* pVertices = nullptr;
	pVB->Lock(0, 0, (void**)&pVertices, 0);

	for (unsigned int i = 0; i < m_pMesh->GetNumVertices(); ++i)
	{
		*pVertices = rotateY90.Transform(*pVertices);
		++pVertices;
		// The structs depicted in this vertex buffer actually store
		// information for normals in addition to xyz, thereby
		// making the vertices in pVB twice the size of the one described
		// by *pVertices.  So we address that here.
		*pVertices = rotateY90.Transform(*pVertices);	//rotate the normals, too
		++pVertices;
	}
	pVB->Unlock();
	SAFE_RELEASE(pVB);
	//...end rotation

	// Note - the Mesh is needed BEFORE calling the base class VOnRestore.
	V(D3DShaderMeshNode9::OnRestore(pScene));
	return S_OK;
}
