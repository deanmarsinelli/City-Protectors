/*
	LuaStateManager.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "LuaStateManager.h"

#include "Logger.h"
#include "StringUtil.h"

LuaStateManager* LuaStateManager::pSingleton = nullptr;

//====================================================
//	Lifetime Management methods
//====================================================
bool LuaStateManager::Create()
{
	if (pSingleton)
	{
		CB_ERROR("Overwriting LuaStateManager singleton");
		CB_SAFE_DELETE(pSingleton);
	}

	// create a singleton and initialize it
	pSingleton = CB_NEW LuaStateManager;
	if (pSingleton)
	{
		return pSingleton->Init();
	}

	return false;
}

void LuaStateManager::Destroy()
{
	CB_ASSERT(pSingleton);
	CB_SAFE_DELETE(pSingleton);
}

LuaStateManager::LuaStateManager()
{
	m_pLuaState = nullptr;
}

LuaStateManager::~LuaStateManager()
{
	if (m_pLuaState)
	{
		LuaPlus::LuaState::Destroy(m_pLuaState);
		m_pLuaState = nullptr;
	}
}

//====================================================
//	IScript Manager interface definitions
//====================================================
bool LuaStateManager::Init()
{
	// create a lua state
	m_pLuaState = LuaPlus::LuaState::Create(true);
	if (m_pLuaState == nullptr)
	{
		return false;
	}

	// register functions so they can be used in lua
	m_pLuaState->GetGlobals().RegisterDirect("ExecuteFile", *this, &LuaStateManager::ExecuteFile);
	m_pLuaState->GetGlobals().RegisterDirect("ExecuteString", *this, &LuaStateManager::ExecuteString);

	return true;
}

void LuaStateManager::ExecuteFile(const char* resource)
{
	// pass the file path of the script to the lua state
	int result = m_pLuaState->DoFile(resource);
	if (result != 0)
	{
		SetError(result);
	}
}

void LuaStateManager::ExecuteString(const char* str)
{
	int result = 0;

	// pass most strings directly into lua interpreter
	if (strlen(str) <= 1 || str[0] != '=')
	{
		result = m_pLuaState->DoString(str);
	}
	else
	{
		// if the string starts with '=' wrap it in a print function
		std::string buffer("print(");
		buffer += str + 1;
		buffer += ")";
		result = m_pLuaState->DoString(buffer.c_str());
	}

	if (result != 0)
	{
		SetError(result);
	}
}

//====================================================
//	Lua method definitions
//====================================================
LuaPlus::LuaObject LuaStateManager::GetGlobalVars()
{
	CB_ASSERT(m_pLuaState);
	return m_pLuaState->GetGlobals();
}

LuaPlus::LuaState* LuaStateManager::GetLuaState() const
{
	return m_pLuaState;
}

LuaPlus::LuaObject LuaStateManager::CreatePath(const char* pathString, bool toIgnoreLastElement)
{
	// this will create a table path in lua from a string
	// e.g. if you pass in A.B.C it will create a table called A with a single element
	// named B which has a single element named C

	StringVec splitPath;
	// split the string using '.' as a delimiter
	Split(pathString, splitPath, '.');
	if (toIgnoreLastElement)
		splitPath.pop_back();

	LuaPlus::LuaObject context = GetGlobalVars();
	for (auto it = splitPath.begin(); it != splitPath.end(); ++it)
	{
		// make sure the context is value
		if (context.IsNil())
		{
			CB_ERROR("Something broke in CreatePath(); bailing out (element == " + (*it) + ")");
			return context;
		}

		// grab whatever this element is
		const std::string& element = *it;
		LuaPlus::LuaObject curr = context.GetByName(element.c_str());

		if (!curr.IsTable())
		{
			// if the element is not a table, delete it
			if (!curr.IsNil())
			{
				CB_WARNING("Overwriting element '" + element + "' in table");
				context.SetNil(element.c_str());
			}

			context.CreateTable(element.c_str());
		}

		context = context.GetByName(element.c_str());
	}

	// path created
	return context;
}

void LuaStateManager::ConvertVec3ToTable(const Vec3& vec, LuaPlus::LuaObject& outLuaTable) const
{
	outLuaTable.AssignNewTable(GetLuaState());
	outLuaTable.SetNumber("x", vec.x);
	outLuaTable.SetNumber("y", vec.y);
	outLuaTable.SetNumber("z", vec.z);
}

void LuaStateManager::ConvertTableToVec3(const LuaPlus::LuaObject& luaTable, Vec3& outVec3) const
{
	// get x, y, and z values
	LuaPlus::LuaObject temp;

	temp = luaTable.Get("x");
	if (temp.IsNumber())
	{
		outVec3.x = temp.GetFloat();
	}
	else
	{
		CB_ERROR("luaTable.x is not a number");
	}

	temp = luaTable.Get("y");
	if (temp.IsNumber())
	{
		outVec3.y = temp.GetFloat();
	}
	else
	{
		CB_ERROR("luaTable.y is not a number");
	}

	temp = luaTable.Get("z");
	if (temp.IsNumber())
	{
		outVec3.z = temp.GetFloat();
	}
	else
	{
		CB_ERROR("luaTable.z is not a number");
	}
}

void LuaStateManager::SetError(int errorNum)
{
	// LuaPlus can throw exceptions on errors, which will be crashes
	LuaPlus::LuaStackObject obj(m_pLuaState, -1);
	const char* errStr = obj.GetString();
	if (errStr)
	{
		m_lastError = errStr;
		ClearStack();
	}
	else
	{
		m_lastError = "Unknown Lua parse error";
	}

	CB_ERROR(m_lastError);
}

void LuaStateManager::ClearStack()
{
	m_pLuaState->SetTop(0);
}
