/*
	MovementController.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "interfaces.h"
#include "Matrix.h"
#include "types.h"

class SceneNode;

class MovementController : public IPointerHandler, public IKeyboardHandler
{
public:
	MovementController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown);

	void SetObject(shared_ptr<SceneNode> object);

	void OnUpdate(float deltaTime);

	// interface methods
	bool OnKeyDown(const BYTE keycode);
	bool OnKeyUp(const BYTE keycode);

	bool OnPointerMove(const Point& mousePos, const int radius);
	bool OnPointerButtonDown(const Point& mousePos, const int radius, const std::string& buttonName);
	bool OnPointerButtonUp(const Point& mousePos, const int radius, const std::string& buttonName);

	const Mat4x4* GetToWorld();
	const Mat4x4* GetFromWorld();

protected:
	/// Transform matrix from world space to object space
	Mat4x4 m_MatFromWorld;

	/// Transform matrix to world space from object space
	Mat4x4 m_MatToWorld;

	/// Matrix holding position in world space
	Mat4x4 m_MatPosition;

	/// Last position of the mouse
	Point m_LastMousePos;

	/// Which keys are up or down
	bool m_Key[256];

	float m_TargetYaw;
	float m_TargetPitch;
	float m_Yaw;
	float m_Pitch;
	float m_PitchOnDown;
	float m_YawOnDown;
	float m_MaxSpeed;
	float m_CurrentSpeed;

	bool m_MouseLButtonDown;
	bool m_RotateWhenLButtonDown;

	shared_ptr<SceneNode> m_Object;
};
