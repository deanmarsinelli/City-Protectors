/*
	D3DMesh.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DMesh.h"
#include "EngineStd.h"
#include "Logger.h"
#include "ResourceHandle.h"
#include "WindowsApp.h"

shared_ptr<IResourceLoader> CreateSdkMeshResourceLoader()
{
	return shared_ptr<IResourceLoader>(CB_NEW SdkMeshResourceLoader());
}

bool SdkMeshResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	WindowsApp::Renderer renderer = WindowsApp::GetRendererImpl();
	if (renderer == WindowsApp::Renderer::Renderer_D3D11)
	{
		shared_ptr<D3DSdkMeshResourceExtraData11> extra = shared_ptr<D3DSdkMeshResourceExtraData11>(CB_NEW D3DSdkMeshResourceExtraData11());
		
		// load the mesh
		if (SUCCEEDED(extra->m_Mesh11.Create(DXUTGetD3D11Device(), (BYTE*)rawBuffer, (UINT)rawSize, true)))
		{
			handle->SetExtra(shared_ptr<D3DSdkMeshResourceExtraData11>(extra));
		}

		return true;
	
	}
	else if (renderer == WindowsApp::Renderer::Renderer_D3D9)
	{
		CB_ASSERT(0 && "Not Supported in D3D9");
	}

	CB_ASSERT(0 && "Unsupported renderer in SdkMeshResourceLoader::LoadResource");
	return false;
}
