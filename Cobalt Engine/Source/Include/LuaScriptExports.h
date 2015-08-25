/*
	LuaScriptExports.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

// All global script exports will exist in an internal class for organization.
// This allows for a single place to export functions from.
namespace LuaScriptExports
{
	/// Register C++ functions to be used in lua script. Called in WindowsApp::Init()
	void Register();

	/// Unregister all C++ functions from lua script
	void Unregister();
}
