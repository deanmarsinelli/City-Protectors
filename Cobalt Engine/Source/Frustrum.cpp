/*
	Frustrum.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Frustrum.h"

#include "D3D9Vertex.h"
#include "EngineStd.h"

Frustrum::Frustrum()
{
	// default fov is 90 degrees
	m_Fov = CB_PI / 4.0f;
	
	// default ratio is 1:1 
	m_Aspect = 1.0f;

	// default planes are 1m and 100m away from camera
	m_Near = 1.0f;
	m_Far = 100.0f;
}

void Frustrum::Init(const float fov, const float aspect, const float nearClip, const float farClip)
{
	m_Fov = fov;
	m_Aspect = aspect;
	m_Near = nearClip;
	m_Far = farClip;

	float tanFovOver2 = (float)tan(m_Fov / 2.0f);
	// get coordinates for 2 sides of each clipping plane
	Vec3 nearRight = (m_Near * tanFovOver2) * m_Aspect * g_Right;
	Vec3 farRight = (m_Far * tanFovOver2) * m_Aspect * g_Right;
	Vec3 nearUp = (m_Near * tanFovOver2) * g_Up;
	Vec3 farUp = (m_Far * tanFovOver2) * g_Up;

	// set up the near and far clipping planes. points start in the upper right and go clockwise
	m_NearClip[0] = (m_Near * g_Forward) - nearRight + nearUp;
	m_NearClip[1] = (m_Near * g_Forward) + nearRight + nearUp;
	m_NearClip[2] = (m_Near * g_Forward) + nearRight - nearUp;
	m_NearClip[3] = (m_Near * g_Forward) - nearRight - nearUp;

	m_FarClip[0] = (m_Far * g_Forward) - farRight + farUp;
	m_FarClip[1] = (m_Far * g_Forward) + farRight + farUp;
	m_FarClip[2] = (m_Far * g_Forward) + farRight - farUp;
	m_FarClip[3] = (m_Far * g_Forward) - farRight - farUp;

	// using the points of the clipping planes, construct all 6 planes
	Vec3 origin(0.0f, 0.0f, 0.0f);
	m_Planes[Near].Init(m_NearClip[2], m_NearClip[1], m_NearClip[0]);
	m_Planes[Far].Init(m_FarClip[0], m_FarClip[1], m_FarClip[2]);
	m_Planes[Right].Init(m_FarClip[2], m_FarClip[1], origin);
	m_Planes[Top].Init(m_FarClip[1], m_FarClip[0], origin);
	m_Planes[Left].Init(m_FarClip[0], m_FarClip[3], origin);
	m_Planes[Bottom].Init(m_FarClip[3], m_FarClip[2], origin);
}

void Frustrum::Render()
{
	D3D9Vertex_Colored verts[24];
	for (int i = 0; i < 8; i++)
	{
		verts[i].color = g_White;
	}
	for (int i = 0; i < 8; i++)
	{
		verts[i + 8].color = g_Red;
	}
	for (int i = 0; i < 8; i++)
	{
		verts[i + 8].color = g_Blue;
	}

	// draw the near clip plane
	verts[0].position = m_NearClip[0];	verts[1].position = m_NearClip[1];
	verts[2].position = m_NearClip[1];	verts[3].position = m_NearClip[2];
	verts[4].position = m_NearClip[2];	verts[5].position = m_NearClip[3];
	verts[6].position = m_NearClip[3];	verts[7].position = m_NearClip[0];

	// draw the far clip plane
	verts[8].position = m_FarClip[0];	verts[9].position = m_FarClip[1];
	verts[10].position = m_FarClip[1];	verts[11].position = m_FarClip[2];
	verts[12].position = m_FarClip[2];	verts[13].position = m_FarClip[3];
	verts[14].position = m_FarClip[3];	verts[15].position = m_FarClip[0];

	// draw the edges between the near and far clip plane
	verts[16].position = m_NearClip[0];	verts[17].position = m_FarClip[0];
	verts[18].position = m_NearClip[1];	verts[19].position = m_FarClip[1];
	verts[20].position = m_NearClip[2];	verts[21].position = m_FarClip[2];
	verts[22].position = m_NearClip[3];	verts[23].position = m_FarClip[3];

	// draw
	DWORD oldLightMode;
	DXUTGetD3D9Device()->GetRenderState(D3DRS_LIGHTING, &oldLightMode);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, FALSE);

	DXUTGetD3D9Device()->SetFVF(D3D9Vertex_Colored::FVF);
	DXUTGetD3D9Device()->DrawPrimitiveUP(D3DPT_LINELIST, 12, verts, sizeof(D3D9Vertex_Colored));

	DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, oldLightMode);
}

bool Frustrum::Inside(const Vec3& point) const
{
	// make sure the point is inside each plane
	for (int i = 0; i < NumPlanes; i++)
	{
		if (!m_Planes[i].Inside(point))
			return false;
	}

	return true;
}

bool Frustrum::Inside(const Vec3& point, const float radius) const
{
	for (int i = 0; i < NumPlanes; i++)
	{
		if (!m_Planes[i].Inside(point, radius))
			return false;
	}

	return true;
}

const Frustrum::Plane& Get(Frustrum::Side side)
{
	return m_Planes[side];
}

void Frustrum::SetFOV(float fov)
{
	// reset the fov and reinitialize the view frustrum
	m_Fov = fov;
	Init(m_Fov, m_Aspect, m_Near, m_Far);
}

void Frustrum::SetAspect(float aspect)
{
	// reset the aspect ratio and reinitialize the view frustrum
	m_Aspect = aspect;
	Init(m_Fov, m_Aspect, m_Near, m_Far);
}

void Frustrum::SetNear(float nearClip)
{
	// reset the near clipping plane and reinitialize the view frustrum
	m_Near = nearClip;
	Init(m_Fov, m_Aspect, m_Near, m_Far);
}

void Frustrum::SetFar(float farClip)
{
	// reset the far clipping plane and reinitialize the view frustrum
	m_Far = farClip;
	Init(m_Fov, m_Aspect, m_Near, m_Far);
}
