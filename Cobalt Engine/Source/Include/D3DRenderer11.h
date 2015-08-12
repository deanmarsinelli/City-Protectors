/*
	D3DRenderer.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "D3DRenderer.h"

class D3DLineDrawer11;

/**
	D3D11 renderer implementation.
*/
class D3DRenderer11 : public D3DRenderer
{
public:
	D3DRenderer11();

	virtual HRESULT OnRestore();

protected:
	float m_BackgroundColor[4];
	D3DLineDrawer11* m_pLineDrawer;
};