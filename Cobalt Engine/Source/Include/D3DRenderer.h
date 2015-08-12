/*
	D3DRenderer.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <DXUT.h>
#include <DXUTgui.h>
#include <SDKmisc.h>

#include "interfaces.h"

/**
	Base class for a D3D based renderer. This inherits from the IRenderer interface
	and is meant to be subclassed by specific versions of D3D renderers.
*/
class D3DRenderer : public IRenderer 
{
public:
	virtual HRESULT OnRestore();
	virtual void Shutdown();

public:
	/// Used to help draw UI elements (buttons, sliders, text boxes, etc)
	static CDXUTDialogResourceManager g_DialogResourceManager;

	/// Used to help draw text on the screen
	static CDXUTTextHelper* g_pTextHelper;
};