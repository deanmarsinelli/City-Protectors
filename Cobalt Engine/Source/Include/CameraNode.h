/*
	CameraNode.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "Frustrum.h"
#include "Matrix.h"
#include "SceneNode.h"
#include "Vector.h"

class Vec4;

/**
	Represents a camera node that exists in a 3d scene and is used
	for rendering the other visible nodes in the scene.
*/
class CameraNode : public SceneNode
{
public:
	/// Set up the camera node
	CameraNode(const Mat4x4* mat, const Frustrum& frustrum);

	/// Render the projection lines if this is a debug camera
	virtual HRESULT Render(Scene* pScene);

	/// Restore the camera after the device is lost
	virtual HRESULT OnRestore(Scene* pScene);

	/// Return whether the camera is active or not
	virtual bool IsVisible(Scene* pScene) const;

	/// Return the camera's view frustrum
	const Frustrum& GetFrustrum();

	/// Set the camera to target a node in the scene
	void SetTarget(shared_ptr<SceneNode> pTarget);

	/// Clear the camera target
	void ClearTarget();

	/// Return a pointer to the camera's target
	shared_ptr<SceneNode> GetTarget();

	/// Return a WorldViewProjection matrix for this scene. 
	/// This transforms vertices from object space to screen space
	Mat4x4 GetWorldViewProjection(Scene* pScene);

	HRESULT SetViewTransform(Scene* pScene);

	/// Return the projection matrix
	const Mat4x4& GetProjection();

	/// Return the view matrix
	const Mat4x4& GetView();

	/// Set the camera's offset from its target
	void SetCameraOffset(const Vec4& cameraOffset);

protected:
	/// View frustrum for the camera
	Frustrum m_Frustrum;

	/// View matrix converts points from world space to camera space
	Mat4x4 m_View;

	/// Projection matrix converts points from camera space to screen space
	Mat4x4 m_Projection;

	/// Is the camera node active?
	bool m_IsActive;

	/// Is this a debug camera?
	bool m_DebugCamera;

	/// SceneNode target for the camera (if one exists)
	shared_ptr<SceneNode> m_pTarget;

	/// Direction offset of the camera relative to the target
	Vec4 m_CamOffsetVector;
};