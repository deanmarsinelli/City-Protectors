/*
	ProtectorController.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <interfaces.h>

class SceneNode;

/**
	This is the main controller for a human player. Multiple inheritance
	is evil... sorry.
*/
class ProtectorController : public IPointerHandler, public IKeyboardHandler
{
public:
	ProtectorController(shared_ptr<SceneNode> object);

	void OnUpdate(float deltaTime);

	// mouse overrides
	virtual bool OnPointerMove(const Point& mousePos, const int radius) override;
	virtual bool OnPointerButtonDown(const Point& mousePos, const int radius, const std::string& buttonName) override;
	virtual bool OnPointerButtonUp(const Point& mousePos, const int radius, const std::string& buttonName) override;

	// keyboard overrides
	virtual bool OnKeyDown(const BYTE c) override;
	virtual bool OnKeyUp(const BYTE c) override;

protected:
	/// Which keys are up and down
	BYTE m_Key[256];

	/// The scene node that owns this controller
	shared_ptr<SceneNode> m_Object;
};
