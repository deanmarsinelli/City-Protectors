#pragma once

#include <D3D11.h>
#include <string>

#include "interfaces.h"

class Scene;

class LineDraw_Hlsl_Shader
{
public:
	HRESULT OnRestore(Scene* pScene);
	HRESULT SetupRender(Scene* pScene);
	HRESULT SetDiffuse(const std::string& textureName, const Color& color);
	HRESULT SetTexture(const ID3D11ShaderResourceView** pDiffuseRV, const ID3D11SamplerState** pSamplers);
};