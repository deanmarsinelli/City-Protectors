/*
	LuaStateManager.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <LuaPlus.h>
#include <string>

#include "interfaces.h"

class Vec3;

/**
	Manages the Lua scripting layer in this engine. This class is a singleton class
	that should use Create() and Destroy() to manage the lifetime of the object. This
	class also encapulates a LuaState which is an execution environment for Lua.
*/
class LuaStateManager : public IScriptManager
{
public:
	/// Create a singleton lua state manager
	static bool Create();

	/// Destroy the singleton lua state manager
	static void Destroy();

	/// Get the singleton pointer to this object
	static LuaStateManager* Get();

	// IScriptManager interface
	/// Initialize the script manager
	virtual bool Init();

	/// Execute a script file
	virtual void ExecuteFile(const char* resource);

	/// Execute a single line of script code
	virtual void ExecuteString(const char* str);

	// Lua Helpers
	/// Get the Lua States global variables
	LuaPlus::LuaObject GetGlobalVars();

	/// Return the Lua State object
	LuaPlus::LuaState* GetLuaState() const;

	/// Create a lua table path from a string
	LuaPlus::LuaObject CreatePath(const char* pathString, bool toIgnoreLastElement = false);
	
	/// Convert a vec3 to a lua table passed in by reference
	void ConvertVec3ToTable(const Vec3& vec, LuaPlus::LuaObject& outLuaTable) const;
	
	/// Convert a lua table to a vec3 passed in by reference
	void ConvertTableToVec3(const LuaPlus::LuaObject& luaTable, Vec3& outVec3) const;

private:
	/// Private constructor
	explicit LuaStateManager();

	/// Private destructor
	virtual ~LuaStateManager();

	/// Set the last error string
	void SetError(int errorNum);

	/// Clear the lua stack
	void ClearStack();
private:
	/// Singleton pointer for this object
	static LuaStateManager* pSingleton;

	/// Represents an execution environment for Lua
	LuaPlus::LuaState* m_pLuaState;

	/// Last error string
	std::string m_lastError;
};
