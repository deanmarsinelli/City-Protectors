/*
	DefaultResourceLoader.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#pragma once

#include <string>

#include "interfaces.h"

/**
	Used to load resources that need no additional processing on load. It will use the
	raw data that has already been loaded into memory from the IResourceFile and is now
	stored in the ResHandle.
*/
class DefaultResourceLoader : public IResourceLoader
{
public:
	/// Return * as the default pattern
	virtual std::string GetPattern() { return "*"; }

	/// Return true, use the raw file without additional processing
	virtual bool UseRawFile() { return true; }

	/// Return the raw size
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return rawSize; }

	/// Return true, the resource needs no additional logic to load
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle) { return true; }
};