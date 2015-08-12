/*
	MovementController.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "MovementController.h"

#include "SceneNode.h"

MovementController::MovementController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown) :
m_Object(object)
{
	// TODO
}

void MovementController::SetObject(shared_ptr<SceneNode> object)
{
	m_Object = object;
}

void MovementController::OnUpdate(float deltaTime)
{
	bool translating = false;
	Vec4 atWorld(0, 0, 0, 0);
	Vec4 rightWorld(0, 0, 0, 0);
	Vec4 upWorld(0, 0, 0, 0);

	if (m_Key['W'] || m_Key['S'])
	{
		// default "look at" vector is positive Z-axis
		Vec4 at = g_Forward4;
		if (m_Key['S'])
			at *= -1;

		// get the "look at" vector in world space
		atWorld = m_MatToWorld.Transform(at);
		translating = true;
	}

	if (m_Key['A'] || m_Key['D'])
	{
		// default "right" is the positive X-axis
		Vec4 right = g_Right4;
		if (m_Key['A'])
			right *= -1;

		// get the "right" vector in world space
		rightWorld = m_MatToWorld.Transform(right);
		translating = true;
	}

	if (m_Key[' '] || m_Key['C'] || m_Key['X'])
	{
		// default "up" is the positive Y-axis
		Vec4 up = g_Up4;
		if (!m_Key[' '])
			up *= -1;

		// up is always up, no need to transform
		upWorld = up;
		translating = true;
	}

	// TODO
}

bool MovementController::OnKeyDown(BYTE keycode)
{
	m_Key[keycode] = true;
	return true;
}

bool MovementController::OnKeyUp(BYTE keycode)
{
	m_Key[keycode] = false;
	return true;
}

bool MovementController::OnPointerMove(const Point& mousePos, const int radius)
{
	if (m_RotateWhenLButtonDown)
	{
		// rotate the view when the left mouse is down
		if (m_LastMousePos != mousePos && m_MouseLButtonDown)
		{
			m_TargetYaw = m_TargetYaw + (m_LastMousePos.x - mousePos.x);
			m_TargetPitch = m_TargetPitch + (mousePos.y - m_LastMousePos.y);
			m_LastMousePos = mousePos;
		}
	}
	else if (m_LastMousePos != mousePos)
	{
		// rotate the view when the mouse buttons are up
		m_TargetYaw = m_TargetYaw + (m_LastMousePos.x - mousePos.x);
		m_TargetPitch = m_TargetPitch + (mousePos.y - m_LastMousePos.y);
		m_LastMousePos = mousePos;
	}

	return true;
}

bool MovementController::OnPointerButtonDown(const Point& mousePos, const int radius, const std::string& buttonName)
{
	if (buttonName == "PointerLeft")
	{
		m_MouseLButtonDown = true;

		m_LastMousePos = mousePos;
		return true;
	}

	return false;
}

bool MovementController::OnPointerButtonUp(const Point& mousePos, const int radius, const std::string& buttonName)
{
	if (buttonName == "PointerLeft")
	{
		m_MouseLButtonDown = false;
		return true;
	}

	return false;
}

inline const Mat4x4* MovementController::GetToWorld()
{
	return &m_MatToWorld;
}

inline const Mat4x4* MovementController::GetFromWorld()
{
	return &m_MatFromWorld;
}