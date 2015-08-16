/*
	LuaScriptProcess.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "LuaScriptProcess.h"

#include <string>

#include "Logger.h"
#include "LuaStateManager.h"

const char* SCRIPT_PROCESS_NAME = "LuaScriptProcess";

LuaScriptProcess::LuaScriptProcess()
{
	// zero out all the data
	LuaPlus::LuaState* pLuaState = LuaStateManager::Get()->GetLuaState();

	m_Frequency = 0.0f;
	m_Time = 0.0f;
	m_ScriptInitFunction.AssignNil(pLuaState);
	m_ScriptUpdateFunction.AssignNil(pLuaState);
	m_ScriptSuccessFunction.AssignNil(pLuaState);
	m_ScriptFailFunction.AssignNil(pLuaState);
	m_ScriptAbortFunction.AssignNil(pLuaState);
}

void LuaScriptProcess::RegisterScriptClass()
{
	// create a meta table for this script process class and make it global
	LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().CreateTable(SCRIPT_PROCESS_NAME);
	// set the table as its own index and base object (parent class)
	metaTableObj.SetObject("__index", metaTableObj); 
	metaTableObj.SetObject("base", metaTableObj);
	metaTableObj.SetBoolean("cpp", true);
	// register functions for a lua script process
	RegisterScriptClassFunctions(metaTableObj);
	metaTableObj.RegisterDirect("Create", &LuaScriptProcess::CreateFromScript);	
}

void LuaScriptProcess::OnInit()
{
	// call base class init
	Process::OnInit();

	// if there is a script init function, call that too
	if (!m_ScriptInitFunction.IsNil())
	{
		LuaPlus::LuaFunction<void> func(m_ScriptInitFunction);
		func(m_Self);
	}

	// if there is no update function, fail immediately.
	if (!m_ScriptUpdateFunction.IsFunction())
		Fail();
}

void LuaScriptProcess::OnUpdate(const float deltaTime)
{
	m_Time += deltaTime;
	// once m_Time is greater than frequency, call the scripts update function
	if (m_Time >= m_Frequency)
	{
		LuaPlus::LuaFunction<void> func(m_ScriptUpdateFunction);
		func(m_Self, m_Time);
		m_Time = 0;
	}
}

void LuaScriptProcess::OnSuccess()
{
	// if there is a lua success function, call it
	if (!m_ScriptSuccessFunction.IsNil())
	{
		LuaPlus::LuaFunction<void> func(m_ScriptSuccessFunction);
		func(m_Self);
	}
}

void LuaScriptProcess::OnFail()
{
	// if there is a lua fail function, call it
	if (!m_ScriptFailFunction.IsNil())
	{
		LuaPlus::LuaFunction<void> func(m_ScriptFailFunction);
		func(m_Self);
	}
}

void LuaScriptProcess::OnAbort()
{
	// if there is a lua abort function, call it
	if (!m_ScriptAbortFunction.IsNil())
	{
		LuaPlus::LuaFunction<void> func(m_ScriptAbortFunction);
		func(m_Self);
	}
}

void LuaScriptProcess::RegisterScriptClassFunctions(LuaPlus::LuaObject& metaTableObj)
{
	// register LuaScriptProcess function call backs with lua
	metaTableObj.RegisterObjectDirect("Succeed",	 (Process*)0, &Process::Succeed);
	metaTableObj.RegisterObjectDirect("Fail",		 (Process*)0, &Process::Fail);
	metaTableObj.RegisterObjectDirect("Pause",		 (Process*)0, &Process::Pause);
	metaTableObj.RegisterObjectDirect("UnPause",	 (Process*)0, &Process::UnPause);
	metaTableObj.RegisterObjectDirect("IsAlive",	 (Process*)0, &LuaScriptProcess::ScriptIsAlive);
	metaTableObj.RegisterObjectDirect("IsDead",		 (Process*)0, &LuaScriptProcess::ScriptIsDead);
	metaTableObj.RegisterObjectDirect("IsPaused",	 (Process*)0, &LuaScriptProcess::ScriptIsPaused);
	metaTableObj.RegisterObjectDirect("AttachChild", (Process*)0, &LuaScriptProcess::ScriptAttachChild);
}

LuaPlus::LuaObject LuaScriptProcess::CreateFromScript(LuaPlus::LuaObject /*self*/, LuaPlus::LuaObject constructionData, LuaPlus::LuaObject originalSubClass)
{
	CB_LOG("Script", std::string("Creating instance of ") + SCRIPT_PROCESS_NAME);
	LuaScriptProcess* pObj = CB_NEW LuaScriptProcess;

	pObj->m_Self.AssignNewTable(LuaStateManager::Get()->GetLuaState());
	if (pObj->BuildCppDataFromScript(originalSubClass, constructionData))
	{
		LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().Lookup(SCRIPT_PROCESS_NAME);
		CB_ASSERT(!metaTableObj.IsNil());

		pObj->m_Self.SetLightUserData("__object", pObj);
		pObj->m_Self.SetMetaTable(metaTableObj);	
	}
	else
	{
		pObj->m_Self.AssignNil(LuaStateManager::Get()->GetLuaState());
		CB_SAFE_DELETE(pObj);
		LuaPlus::LuaObject nil;
		nil.AssignNil(LuaStateManager::Get()->GetLuaState());
		return nil;
	}

	return pObj->m_Self;
}

bool LuaScriptProcess::BuildCppDataFromScript(LuaPlus::LuaObject scriptClass, LuaPlus::LuaObject constructionData)
{
	// this method sets up the data and callbacks of a script process from a lua script
	if (scriptClass.IsTable())
	{
		// init function
		LuaPlus::LuaObject temp = scriptClass.GetByName("OnInit");
		if (temp.IsFunction())
		{
			m_ScriptInitFunction = temp;
		}
		
		// update function
		temp = scriptClass.GetByName("OnUpdate");
		if (temp.IsFunction())
		{
			m_ScriptUpdateFunction = temp;
		}
		else
		{
			CB_ERROR("No OnUpdate() function found in the script process. Type = " + std::string(temp.TypeName())); 
		}

		// on success function
		temp = scriptClass.GetByName("OnSuccess");
		if (temp.IsFunction())
		{
			m_ScriptSuccessFunction = temp;
		}

		// on fail function
		temp = scriptClass.GetByName("OnFail");
		if (temp.IsFunction())
		{
			m_ScriptFailFunction = temp;
		}

		// on abort function
		temp = scriptClass.GetByName("OnAbort");
		if (temp.IsFunction())
		{
			m_ScriptAbortFunction = temp;
		}
	}
	else
	{
		CB_ERROR("Script class is not a table in LuaScriptProcess::BuildCppDataFromScript()");
		return false;
	}

	if (constructionData.IsTable())
	{
		// iterate the data table looking for the frequency member variable
		for (LuaPlus::LuaTableIterator constructionDataIt(constructionData); constructionDataIt; constructionDataIt.Next())
		{
			const char* key = constructionDataIt.GetKey().GetString();
			LuaPlus::LuaObject val = constructionDataIt.GetValue();

			if (strcmp(key, "frequency") == 0 && val.IsNumber())
			{
				m_Frequency = val.GetFloat();
			}
			else
			{
				m_Self.SetObject(key, val);
			}
		}
	}

	return true;
}

void LuaScriptProcess::ScriptAttachChild(LuaPlus::LuaObject child)
{
	// this attaches the child to this process as a C++ process child
	if (child.IsTable())
	{
		LuaPlus::LuaObject obj = child.GetByName("__object");
		if (!obj.IsNil())
		{
			shared_ptr<Process> pProcess(static_cast<Process*>(obj.GetLightUserData()));
			CB_ASSERT(pProcess);
			AttachChild(pProcess);
		}
		else
		{
			CB_ERROR("Cannot attach child to LuaScriptProcess with no valid __object");
		}
	}
	else
	{
		CB_ERROR("Invalid object passted to LuaScriptProcess::ScriptAttachChild(). Type = " + std::string(child.TypeName()));
	}
}
