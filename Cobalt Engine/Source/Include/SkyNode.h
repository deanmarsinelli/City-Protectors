/*
	SkyNode.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "CameraNode.h"
#include "SceneNode.h"

/**
	This is the base class for a sky box scene node. Specific sky
	node implementations should derive from this class.
*/
class SkyNode : public SceneNode
{
public:
	/// Constructor taking in a texture file
	SkyNode(const char* textureFile);

	/// Default destructor
	virtual ~SkyNode() { }

	/// Executes before the render stage
	HRESULT PreRender(Scene* pScene);
	
	/// Is the sky node visible?
	bool IsVisible(Scene* pScene) const;

protected:
	/// Return the texture name of a side
	std::string GetTextureName(const int side);

protected:
	/// Number of vertices in the sky node
	DWORD m_NumVerts;

	/// Number of sides in the sky node
	DWORD m_Sides;

	/// Base name of the texture
	const char* m_TextureBaseName;

	/// Pointer to the camera the sky node is attached to
	shared_ptr<CameraNode> m_Camera;
	
	/// Is the sky node active
	bool m_Active;
};