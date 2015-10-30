/*
	D3DTextureResourceLoader.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DTextureResourceLoader.h"

#include "EngineStd.h"
#include "D3DTextureResourceExtraData.h"
#include "Logger.h"
#include "ResourceHandle.h"

//====================================================
//	D3DTextureResourceLoader methods
//====================================================
bool D3DTextureResourceLoader::UseRawFile()
{
	return false;
}

bool D3DTextureResourceLoader::DiscardRawBufferAfterLoad()
{
	return true;
}

unsigned int D3DTextureResourceLoader::GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
	// return 0 since the resource cache won't manage memory for the texture, the texture
	// will live in VRAM
	return 0;
}

bool D3DTextureResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	// get the renderer type and load the texture into resource extra data
	WindowsApp::Renderer renderer = WindowsApp::GetRendererImpl();
	if (renderer == WindowsApp::Renderer::Renderer_D3D9)
	{
		shared_ptr<D3DTextureResourceExtraData9> extra(CB_NEW D3DTextureResourceExtraData9);

		if (FAILED(D3DXCreateTextureFromFileInMemory(DXUTGetD3D9Device(), rawBuffer, rawSize, &extra->m_pTexture)))
		{
			return false;
		}
		else
		{
			// attach the texture data onto the resource handle
			handle->SetExtra(shared_ptr<D3DTextureResourceExtraData9>(extra));
			return true;
		}
	}
	else if (renderer == WindowsApp::Renderer::Renderer_D3D11)
	{
		shared_ptr<D3DTextureResourceExtraData11> extra(CB_NEW D3DTextureResourceExtraData11);

		// load the texture
		if (FAILED(D3DX11CreateShaderResourceViewFromMemory(DXUTGetD3D11Device(), rawBuffer, rawSize, nullptr, nullptr, &extra->m_pTexture, nullptr)))
		{
			return false;
		}

		// create the sampler state
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		if (FAILED(DXUTGetD3D11Device()->CreateSamplerState(&samplerDesc, &extra->m_pSamplerLinear)))
		{
			return false;
		}
		
		// attach the texture data onto the resource handle
		handle->SetExtra(shared_ptr<D3DTextureResourceExtraData11>(extra));
		return true;
	}

	CB_ASSERT(0 && "Unsupported Renderer in D3DTextureResourceLoader::LoadResource()");
	return false;
}


//====================================================
//	Internal classes for loading specific textures
//====================================================
class DdsResourceLoader : public D3DTextureResourceLoader
{
public:
	virtual std::string GetPattern() { return "*.dds"; }
};

class JpgResourceLoader : public D3DTextureResourceLoader
{
public:
	virtual std::string GetPattern() { return "*.jpg"; }
};


//====================================================
//	Creation functions for the above texture loaders
//====================================================
shared_ptr<IResourceLoader> CreateDDSResourceLoader()
{
	return shared_ptr<IResourceLoader>(CB_NEW DdsResourceLoader);
}

shared_ptr<IResourceLoader> CreateJPGResourceLoader()
{
	return shared_ptr<IResourceLoader>(CB_NEW JpgResourceLoader);
}
