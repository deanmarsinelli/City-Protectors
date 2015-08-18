/*
	WaveResourceLoader.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <memory>

#include "interfaces.h"
#include "ResourceHandle.h"

/**
	Resource loader class for a wave file resource.
*/
class WaveResourceLoader : public IResourceLoader
{
public:
	/// Returns a pattern *.wav
	virtual std::string GetPattern();

	/// Return false since the file is not used raw
	virtual bool UseRawFile();

	/// Return true to release the raw wave buffer once the sound is loaded
	virtual bool DiscardRawBufferAfterLoad();

	/// Return the size of the loaded wave resource
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize);

	/// Load a wave file into a resource handle
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);

protected:
	bool ParseWave(char* wavStream, size_t length, shared_ptr<ResHandle> handle);
};