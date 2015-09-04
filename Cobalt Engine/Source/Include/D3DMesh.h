/*
	D3DMesh.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <DXUT.h>
#include <SDKmesh.h>

#include "interfaces.h"

/**
	Extra detail to include with a D3D SDK mesh resource.
*/
class D3DSdkMeshResourceExtraData11 : public IResourceExtraData
{
	friend class SdkMeshResourceLoader;

public:
	/// Default constructor
	D3DSdkMeshResourceExtraData11() { }

	/// Virtual Destructor
	~D3DSdkMeshResourceExtraData11() { }

	/// Returns a string describing the extra data
	virtual std::string ToStr() { return "D3DSdkMeshResourceExtraData11"; }

	/// The actual SDK mesh
	CDXUTSDKMesh m_Mesh11;
};

/**
	Resource loader for a D3D SDK mesh
*/
class SdkMeshResourceLoader : public IResourceLoader
{
public:
	/// Return the pattern for the mesh loader
	virtual std::string GetPattern() { return "*.sdkmesh"; }

	/// Return false because the mesh needs extra processing
	virtual bool UseRawFile() { return false;  }

	/// Return false because we keep the raw mesh data
	virtual bool DiscardRawBufferAfterLoad() { return false; }

	/// Return the size of the loaded mesh
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return rawSize; }

	/// Load a resource into a resource handle
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
};
