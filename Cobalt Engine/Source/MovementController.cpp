/*
	MovementController.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "MathUtils.h"
#include "MovementController.h"
#include "SceneNode.h"
#include "Vector.h"

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

MovementController::MovementController(shared_ptr<SceneNode> object, float initialYaw, float initialPitch, bool rotateWhenLButtonDown) :
m_Object(object)
{
	// get the transform matrices for this objects scene node
	m_Object->Get()->Transform(&m_MatToWorld, &m_MatFromWorld);

	m_TargetYaw = RADIANS_TO_DEGREES(-initialYaw);
	m_Yaw =	RADIANS_TO_DEGREES(-initialYaw);
	m_TargetPitch = RADIANS_TO_DEGREES(initialPitch);
	m_Pitch = RADIANS_TO_DEGREES(initialPitch);
	
	// max speed set to 30 meters per second
	m_MaxSpeed = 30.0f; 
	m_CurrentSpeed = 0.0f;

	// set the object's position
	Vec3 pos = m_MatToWorld.GetPosition();
	m_MatPosition.BuildTranslation(pos);

	// set the mouse cursor position
	POINT cursor;
	GetCursorPos(&cursor);
	m_LastMousePos = cursor;

	ZeroMemory(m_Key, sizeof(m_Key));

	m_MouseLButtonDown = false;
	m_RotateWhenLButtonDown = rotateWhenLButtonDown;
}

void MovementController::SetObject(shared_ptr<SceneNode> object)
{
	m_Object = object;
}

void MovementController::OnUpdate(float deltaTime)
{
	bool translating = false;
	Vec4 atWorld(0.0f, 0.0f, 0.0f, 0.0f);
	Vec4 rightWorld(0.0f, 0.0f, 0.0f, 0.0f);
	Vec4 upWorld(0.0f, 0.0f, 0.0f, 0.0f);

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

	// Handle rotation with mouse position
	{
		// this is the secret formula says Mike McShaffry
		m_Yaw += (m_TargetYaw - m_Yaw) * (0.35f);
		m_TargetPitch = MAX(-90, MIN(90, m_TargetPitch));
		m_Pitch += (m_TargetPitch - m_Pitch) * (0.35f);

		// calculate new rotation matrix from the camera
		Mat4x4 matRot;
		matRot.BuildYawPitchRoll(DEGREES_TO_RADIANS(-m_Yaw), DEGREES_TO_RADIANS(m_Pitch), 0.0f);

		// create new object to world space matrix and new world space to object space matrix
		m_MatToWorld = matRot * m_MatPosition;
		m_MatFromWorld = m_MatToWorld.Inverse();
		m_Object->SetTransform(&m_MatToWorld, &m_MatFromWorld);
	}

	if (translating)
	{
		// get the looking direction unit vector
		Vec3 direction = atWorld + rightWorld + upWorld;
		direction.Normalize();

		// increase speed
		m_CurrentSpeed += (m_MaxSpeed * deltaTime);
		if (m_CurrentSpeed > m_MaxSpeed)
		{
			m_CurrentSpeed = m_MaxSpeed;
		}

		direction *= m_CurrentSpeed;

		// update transform
		Vec3 pos = m_MatPosition.GetPosition() + direction;
		m_MatPosition.SetPosition(pos);
		m_MatToWorld.SetPosition(pos);
		m_MatFromWorld = m_MatToWorld.Inverse();

		m_Object->SetTransform(&m_MatToWorld, &m_MatFromWorld);
	}
	else
	{
		m_CurrentSpeed = 0.0f;
	}
}

bool MovementController::OnKeyDown(const BYTE keycode)
{
	m_Key[keycode] = true;
	return true;
}

bool MovementController::OnKeyUp(const BYTE keycode)
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

const Mat4x4* MovementController::GetToWorld()
{
	return &m_MatToWorld;
}

const Mat4x4* MovementController::GetFromWorld()
{
	return &m_MatFromWorld;
}