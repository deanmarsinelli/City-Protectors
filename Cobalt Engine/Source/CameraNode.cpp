/*
	CameraNode.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <DXUT.h>

#include "CameraNoda.h"
#include "Scene.h"

CameraNode::CameraNode(const Mat4x4* mat, const Frustrum& frustrum) :
	SceneNode(INVALID_GAMEOBJECT_ID, WeakBaseRenderComponentPtr(), RenderPass::RenderPass_0, mat),
	m_Frustrum(frustrum),
	m_IsActive(true),
	m_DebugCamera(false),
	m_pTarget(shared_ptr<SceneNode>()),
	m_CamOffsetVector(0.0f, 1.0f, -10.0f, 0.0f)
{}

HRESULT CameraNode::Render(Scene* pScene)
{
	if (m_DebugCamera)
	{
		pScene->PopMatrix();
		m_Frustrum.Render();
		pScene->PushAndSetMatrix(m_Properties.ToWorld());
	}

	return S_OK;
}

HRESULT CameraNode::OnRestore(Scene* pScene)
{
	m_Frustrum.SetAspect(DXUTGetWindowWidth() / (FLOAT)DXUTGetWindowHeight());
	D3DXMatrixPerspectiveFovLH(&m_Projection, m_Frustrum.m_Fov, m_Frustrum.m_Aspect, m_Frustrum.m_Near, m_Frustrum.m_Far);
	pScene->GetRenderer()->SetProjectionTransform(&m_Projection);
	return S_OK;
}

bool CameraNode::IsVisible(Scene* pScene) const
{
	return m_IsActive;
}

const Frustrum& CameraNode::GetFrustrum()
{
	return m_Frustrum;
}

void CameraNode::SetTarget(shared_ptr<SceneNode> pTarget)
{
	m_pTarget = pTarget;
}

void CameraNode::ClearTarget()
{
	m_pTarget = shared_ptr<SceneNode>();
}

shared_ptr<SceneNode> CameraNode::GetTarget()
{
	return m_pTarget;
}

Mat4x4 CameraNode::GetWorldViewProjection(Scene *pScene)
{
	// combine the world view and projection matrices into one transformation
	Mat4x4 world = pScene->GetTopMatrix();
	Mat4x4 view = Get()->FromWorld();
	Mat4x4 worldView = world * view;
	return worldView * m_Projection;
}

HRESULT CameraNode::SetViewTransform(Scene* pScene)
{
	// if there is a target, make sure the camera is attached
	// right behind the target
	if (m_pTarget)
	{
		Mat4x4 mat = m_pTarget->Get()->ToWorld();
		Vec4 at = m_CamOffsetVector;
		Vec4 atWorld = mat.Transform(at);
		Vec3 pos = mat.GetPosition() + Vec3(atWorld);
		mat.SetPosition(pos);
		SetTransform(&mat);
	}

	m_View = Get()->FromWorld();
	pScene->GetRenderer()->SetViewTransform(&m_View);

	return S_OK;
}

const Mat4x4& CameraNode::GetProjection()
{
	return m_Projection;
}

const Mat4x4& CameraNode::GetView()
{
	return m_View;
}

void CameraNode::SetCameraOffset(const Vec4& cameraOffset)
{
	m_CamOffsetVector = cameraOffset;
}
