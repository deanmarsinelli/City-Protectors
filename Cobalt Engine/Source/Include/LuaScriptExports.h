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
	void Register();
	void Unregister();
}