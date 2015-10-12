/*
	LuaScriptResource.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#include "BaseGameLogic.h"
#include "EngineStd.h"
#include "LuaScriptResource.h"
#include "LuaStateManager.h"

shared_ptr<IResourceLoader> CreateLuaScriptResourceLoader()
{
	return shared_ptr<IResourceLoader>(CB_NEW LuaScriptResourceLoader());
}


bool LuaScriptResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle>)
{
	if (rawSize <= 0)
	{
		return false;
	}

	if (!g_pApp->m_pGame || g_pApp->m_pGame->CanRunLua())
	{
		LuaStateManager::Get()->ExecuteString(rawBuffer);
	}

	return true;
}
