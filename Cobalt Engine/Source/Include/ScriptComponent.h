/*
	ScriptComponent.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <LuaPlus.h>
#include <string>

#include "Component.h"
#include "Vector.h"

/**
	Interface for different base script components (lua, python, JS, etc)
*/
class IScriptComponent : public Component
{
	// dummy interface for different types of scripts
};


/**
	A lua script component that can be attached to a game object.
*/
class LuaScriptComponent : public IScriptComponent
{
public:
	/// Default constructor
	LuaScriptComponent();
	
	/// Virtual destructor
	virtual ~LuaScriptComponent();

	/// Initialize the script component from xml
	virtual bool Init(TiXmlElement* pData);

	/// If the script has a constructor, it's called here
	virtual void PostInit();

	/// Generate an xml element from this component
	virtual TiXmlElement* GenerateXml();

	/// Register the functions from this script into the global lua object
	static void RegisterScriptFunctions();

	/// Unregister the functions from this script into the global lua object
	static void UnregisterScriptFunctions();

	/// Return the name of this component
	virtual const char* GetName() const { return g_Name; }

private:
	/// Create a lua object and bind it to this object
	void CreateScriptObject();

	/// Return the Id of the game object to lua
	LuaPlus::LuaObject GetObjectId();

	/// Get position of the object in lua
	LuaPlus::LuaObject GetPos();

	/// Set position of the object in lua
	void SetPos(LuaPlus::LuaObject newPos);

	/// Get the look at vector for the object in lua
	LuaPlus::LuaObject GetLookAt() const;

	float GetYOrientationRadians() const;

	void RotateY(float angleRadians);

	// Set position using physics component
	void SetPosition(float x, float y, float z);
	void LookAt(Vec3 target);
	void Stop();

public:
	static const char* g_Name;

private:
	// private data members
	std::string m_ScriptObjectName;
	std::string m_ConstructorName;
	std::string m_DestructorName;

	LuaPlus::LuaObject m_ScriptObject;
	LuaPlus::LuaObject m_ScriptConstructor;
	LuaPlus::LuaObject m_ScriptDestructor;
};