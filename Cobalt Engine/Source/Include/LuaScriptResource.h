/*
	LuaScriptResource.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#pragma once

#include "interfaces.h"

/**
	Used to load lua scripts into the resource cache.
*/
class LuaScriptResourceLoader : public IResourceLoader
{
public:
	/// Return false since the raw file is not used
	virtual bool UseRawFile() { return false; }

	/// Return true so the raw buffer will be discarded
	virtual bool DiscardRawBufferAfterLoad() { return true; }

	/// Add a null zero to the resource
	virtual bool AddNullZero() { return true; }

	/// Return the loaded resource size
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return rawSize; }

	/// Load a raw xml file into a resource handle
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle>);

	/// Return the pattern for xml files
	virtual std::string GetPattern() { return "*.lua"; }
};
