/*
	LuaScriptProcess.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <LuaPlus.h>

#include "Process.h"

/**
	A Lua script process is a process that knows about the scripting system
	and allows Lua scripts to update over multiple frames.
*/
class LuaScriptProcess : public Process
{
public:
	/// Register the script class within lua to get access to these process methods -- call this in application initialization
	static void RegisterScriptClass();

protected:
	// Process interface
	/// Initialize the process
	virtual void OnInit();

	/// Update method must be overriden in derived class and is called every frame
	virtual void OnUpdate(const float deltaTime);

	/// Called if a process ends with sucess
	virtual void OnSuccess();

	/// Called if a process ends with failure
	virtual void OnFail();

	/// Called if a process ends with abort
	virtual void OnAbort();

private:
	/// Private constructor -- don't allow construction outside of the class
	explicit LuaScriptProcess();

	// Private helper methods
	/// Register the process class functions to be used by lua script
	static void RegisterScriptClassFunctions(LuaPlus::LuaObject& metaTableObj);
	
	/// Create a C++ process object from script and bind it to lua
	static LuaPlus::LuaObject CreateFromScript(LuaPlus::LuaObject self, LuaPlus::LuaObject constructionData, LuaPlus::LuaObject originalSubClass);
	
	/// Populate a C++ process object data from script
	virtual bool BuildCppDataFromScript(LuaPlus::LuaObject scriptClass, LuaPlus::LuaObject constructionData);

	// These methods do the same thing as the process ones, but Lua can't register const functions
	// so these are just proxy methods
	bool ScriptIsAlive() { return IsAlive(); }
	bool ScriptIsDead() { return IsDead(); }
	bool ScriptIsPaused() { return IsPaused(); }

	/// Attach a child to this script process
	void ScriptAttachChild(LuaPlus::LuaObject child);

private:
	/// Frequency at which the lua script update function should be called
	float m_Frequency;

	/// Keeps track of elapsed time
	float m_Time;

	/// Lua callback function for init
	LuaPlus::LuaObject m_ScriptInitFunction;

	/// Lua callback function for update
	LuaPlus::LuaObject m_ScriptUpdateFunction;

	/// Lua callback function for success
	LuaPlus::LuaObject m_ScriptSuccessFunction;

	/// Lua callback function for fail
	LuaPlus::LuaObject m_ScriptFailFunction;

	/// Lua callback function for abort
	LuaPlus::LuaObject m_ScriptAbortFunction;

	/// Lua self reference
	LuaPlus::LuaObject m_Self;
};