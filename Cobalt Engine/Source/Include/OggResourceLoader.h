/*
	OggResourceLoader.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <memory>
#include <string>

#include "interfaces.h"
#include "ResourceHandle.h"

/**
	Resource loader class for an ogg sound file resource.
*/
class OggResourceLoader : public IResourceLoader
{
public:
	/// Returns a pattern *.ogg
	virtual std::string GetPattern();

	/// Return false since the file is not used raw
	virtual bool UseRawFile();

	/// Return true to release the raw ogg buffer once the sound is loaded
	virtual bool DiscardRawBufferAfterLoad();

	/// Return the size of the loaded ogg resource
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize);

	/// Load a wave file into a resource handle
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);

protected:
	/// Parse the ogg file and load it into the resource -handle
	bool ParseOgg(char* oggStream, size_t length, shared_ptr<ResHandle> handle);
};
