/*
	D3DTextureResourceLoader.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <memory>

#include "interfaces.h"

class ResHandle;

/*
	Base class implementation of a D3D texture resource loader. This class should be
	subclassed by implemenetations for specific texture types.
*/
class D3DTextureResourceLoader : public IResourceLoader
{
public:
	/// Return false because textures require extra processing
	virtual bool UseRawFile();

	/// Return true to discard the raw buffer once the texture is loaded
	virtual bool DiscardRawBufferAfterLoad();

	/// Return 0 since the resource cache won't manage memory for a texture
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize);

	/// Load the resource into a resource handle
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
};
