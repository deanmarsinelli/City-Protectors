/*
	Console.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <string>

#include "EngineStd.h"

/**
	The console can be used by players to input script commands directly into the engine.
*/
class Console
{
public:
	/// Default constructor
	Console();

	/// Add a new line of output text to display in the console
	void AddDisplayText(const std::string& text);

	/// Overwrite the text to display in the console
	void SetDisplayText(const std::string& text);

	/// Activate or deactivate the console
	void SetActive(const bool isActive);

	/// Return whether the console is currently activated
	bool IsActive() const;

	/// Handle input from the keyboard 
	void HandleKeyboardInput(const unsigned int keyVal, const unsigned int oemKeyVal, const bool keyDown);

	/// Update the console
	void Update(const float deltaTime);

	/// Render the console
	void Render();

private:
	/// Is the console currently active or not
	bool m_IsActive;

	/// Where results are shown
	RECT m_ConsoleOutputRect;

	/// Where input is entered
	RECT m_ConsoleInputRect;

	/// Color of the output text
	Color m_OutputColor;

	/// Color of the input text
	Color m_InputColor;

	/// The current output string
	std::string m_CurrentOutputString;

	/// The current input string
	std::string m_CurrentInputString;

	/// The height of the console input window
	int m_ConsoleInputSize;

	/// Counter to toggle cursor blink
	int m_CursorBlinkTimer;

	/// Whether or not the cursor is displayed
	bool m_CursorOn;

	/// Is the shit button currently pressed
	bool m_ShiftDown;

	/// Is the caps lock button currently pressed
	bool m_CapsLockDown;

	/// If true, there is a string to execute in the next update cycle
	bool m_ExecutesStringOnUpdate;
};