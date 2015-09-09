/*
	Raycast.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <algorithm>
#include <DXUT.h>

#include "CameraNode.h"
#include "D3D9Vertex.h"
#include "EngineStd.h"
#include "Raycast.h"
#include "Scene.h"

RayCast::RayCast(Point point, DWORD maxIntersections)
{
	m_MaxIntersections = maxIntersections;
	m_IntersectionArray.reserve(m_MaxIntersections);
	m_UseD3DXIntersect = true;
	m_AllHits = true;
	m_NumIntersections = 0;
	m_Point = point;
}

HRESULT RayCast::Pick(Scene* pScene, GameObjectId objectId, ID3DXMesh* pMesh)
{
	if (!m_AllHits && m_NumIntersections > 0)
		return S_OK;

	HRESULT hr;

	IDirect3DDevice9* pDevice = DXUTGetD3D9Device();

	// get the world view and projection matrices
	const Mat4x4 matWorld = pScene->GetTopMatrix();
	const Mat4x4 matView = pScene->GetCamera()->GetView();
	const Mat4x4 proj = pScene->GetCamera()->GetProjection();

	// compute the vector of the pick ray in screen space
	D3DXVECTOR3 v;
	v.x = (((2.0f * m_Point.x) / g_pApp->GetScreenSize().x) - 1) / proj._11;
	v.y = -(((2.0f * m_Point.y) / g_pApp->GetScreenSize().y) - 1) / proj._22;
	v.z = 1.0f;

	// get the inverse worldview matrix (which will be view to object space)
	D3DXMATRIX mWorldView = matWorld * matView;
	D3DXMATRIX m;
	D3DXMatrixInverse(&m, NULL, &mWorldView);

	// transform the screen space pick ray into 3d space
	m_PickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	m_PickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	m_PickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	m_PickRayOrigin.x = m._41;
	m_PickRayOrigin.y = m._42;
	m_PickRayOrigin.z = m._43;

	ID3DXMesh* pTempMesh;
	V(pMesh->CloneMeshFVF(pMesh->GetOptions(), D3D9Vertex_UnlitTextured::FVF, DXUTGetD3D9Device(), &pTempMesh));

	LPDIRECT3DVERTEXBUFFER9 pVB;
	LPDIRECT3DINDEXBUFFER9 pIB;
	pTempMesh->GetVertexBuffer(&pVB);
	pTempMesh->GetIndexBuffer(&pIB);

	WORD* pIndices;
	D3D9Vertex_UnlitTextured* pVertices;

	pIB->Lock(0, 0, (void**)&pIndices, 0);
	pVB->Lock(0, 0, (void**)&pVertices, 0);

	DWORD intersections = 0;

	if (!m_AllHits)
	{
		// get only first intersection
		BOOL hit;
		DWORD face;
		FLOAT bary1, bary2, dist;

		D3DXIntersect(pTempMesh, &m_PickRayOrigin, &m_PickRayDir, &hit, &face, &bary1, &bary2, &dist, nullptr, nullptr);
		if (hit)
		{
			m_NumIntersections = 1;
			m_IntersectionArray.resize(1);
			InitIntersection(m_IntersectionArray[0], face, dist, bary1, bary2, objectId, pIndices, pVertices, matWorld);
		}
		else
		{
			m_NumIntersections = 0;
		}
	}
	else
	{
		// get all intersections
		BOOL hit;
		LPD3DXBUFFER pBuffer = nullptr;
		D3DXINTERSECTINFO* pIntersectionInfoArray;
		
		if (FAILED(hr = D3DXIntersect(pTempMesh, &m_PickRayOrigin, &m_PickRayDir, &hit, nullptr, nullptr, nullptr, nullptr, &pBuffer, &intersections)))
		{
			CB_COM_RELEASE(pTempMesh);
			CB_COM_RELEASE(pVB);
			CB_COM_RELEASE(pIB);

			return hr;
		}
		if (intersections > 0)
		{
			m_IntersectionArray.resize(m_NumIntersections + intersections);

			pIntersectionInfoArray = (D3DXINTERSECTINFO*)pBuffer->GetBufferPointer();

			if (m_NumIntersections > m_MaxIntersections)
			{
				m_NumIntersections = m_MaxIntersections;
			}

			for (DWORD i = 0; i < intersections; i++)
			{
				Intersection* pIntersection;
				pIntersection = &m_IntersectionArray[i + m_NumIntersections];
				InitIntersection(*pIntersection, pIntersectionInfoArray[i].FaceIndex, pIntersectionInfoArray[i].Dist,
					pIntersectionInfoArray[i].U, pIntersectionInfoArray[i].V, objectId, pIndices, pVertices, matWorld);
			}
		}

		CB_COM_RELEASE(pBuffer);
	}

	m_NumIntersections += intersections;
	pVB->Unlock();
	pIB->Unlock();

	CB_COM_RELEASE(pTempMesh);
	CB_COM_RELEASE(pVB);
	CB_COM_RELEASE(pIB);

	return S_OK;
}

HRESULT RayCast::Pick(Scene* pScene, GameObjectId objectId, CDXUTSDKMesh* pMesh)
{
	if (!m_AllHits && m_NumIntersections > 0)
		return S_OK;

	IDirect3DDevice9* pDevice = DXUTGetD3D9Device();

	// get the world view and projection matrices
	const Mat4x4 matWorld = pScene->GetTopMatrix();
	const Mat4x4 matView = pScene->GetCamera()->GetView();
	const Mat4x4 proj = pScene->GetCamera()->GetProjection();

	// compute the vector of the pick ray in screen space
	D3DXVECTOR3 v;
	v.x = (((2.0f * m_Point.x) / g_pApp->GetScreenSize().x) - 1) / proj._11;
	v.y = -(((2.0f * m_Point.y) / g_pApp->GetScreenSize().y) - 1) / proj._22;
	v.z = 1.0f;

	// get the inverse worldview matrix (which will be view to object space)
	D3DXMATRIX mWorldView = matWorld * matView;
	D3DXMATRIX m;
	D3DXMatrixInverse(&m, NULL, &mWorldView);

	// transform the screen space pick ray into 3d space
	m_PickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	m_PickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	m_PickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	m_PickRayOrigin.x = m._41;
	m_PickRayOrigin.y = m._42;
	m_PickRayOrigin.z = m._43;

	return E_FAIL;
}

HRESULT RayCast::Pick(Scene* pScene, GameObjectId objectId, LPDIRECT3DVERTEXBUFFER9 pVB, LPDIRECT3DINDEXBUFFER9 pIB, DWORD numPolys)
{
	if (!m_AllHits && m_NumIntersections > 0)
		return S_OK;

	WORD* pIndices;
	D3D9Vertex_ColoredTextured* pVertices;

	pIB->Lock(0, 0, (void**)&pIndices, 0);
	pVB->Lock(0, 0, (void**)&pVertices, 0);

	IDirect3DDevice9* pDevice = DXUTGetD3D9Device();

	// get the world view and projection matrices
	const Mat4x4 matWorld = pScene->GetTopMatrix();
	const Mat4x4 matView = pScene->GetCamera()->GetView();
	const Mat4x4 proj = pScene->GetCamera()->GetProjection();

	// compute the vector of the pick ray in screen space
	D3DXVECTOR3 v;
	v.x = (((2.0f * m_Point.x) / g_pApp->GetScreenSize().x) - 1) / proj._11;
	v.y = -(((2.0f * m_Point.y) / g_pApp->GetScreenSize().y) - 1) / proj._22;
	v.z = 1.0f;

	// get the inverse worldview matrix (which will be view to object space)
	D3DXMATRIX mWorldView = matWorld * matView;
	D3DXMATRIX m;
	D3DXMatrixInverse(&m, NULL, &mWorldView);

	// transform the screen space pick ray into 3d space
	m_PickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	m_PickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	m_PickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	m_PickRayOrigin.x = m._41;
	m_PickRayOrigin.y = m._42;
	m_PickRayOrigin.z = m._43;

	FLOAT bary1, bary2, dist;

	for (DWORD i = 0; i < numPolys; i++)
	{
		Vec3 v0 = pVertices[pIndices[3 * i + 0]].position;
		Vec3 v1 = pVertices[pIndices[3 * i + 1]].position;
		Vec3 v2 = pVertices[pIndices[3 * i + 2]].position;

		// check if the pick ray passes through this triangle
		if (IntersectTriangle(m_PickRayOrigin, m_PickRayDir, v0, v1, v2, &dist, &bary1, &bary2))
		{
			if (m_AllHits || m_NumIntersections == 0 || dist < m_IntersectionArray[0].m_Dist)
			{
				if (!m_AllHits)
					m_NumIntersections = 0;

				m_NumIntersections++;

				m_IntersectionArray.resize(m_NumIntersections);

				Intersection* pIntersection = &m_IntersectionArray[m_NumIntersections - 1];
				InitIntersection(*pIntersection, i, dist, bary1, bary2, objectId, pIndices, pVertices, matWorld);

				if (m_NumIntersections == m_MaxIntersections)
					break;
			}
		}
	}

	pVB->Unlock();
	pIB->Unlock();

	return S_OK;
}


void RayCast::Sort()
{
	std::sort(m_IntersectionArray.begin(), m_IntersectionArray.end());
}
