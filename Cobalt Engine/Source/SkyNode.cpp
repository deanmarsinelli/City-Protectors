/*
	SkyNode.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Logger.h"
#include "SkyNode.h"

SkyNode::SkyNode(const char* textureFile) :
	SceneNode(INVALID_GAMEOBJECT_ID, WeakBaseRenderComponentPtr(), 
	RenderPass::RenderPass_Sky, &Mat4x4::Identity)
{
	m_Active = true;
}

HRESULT SkyNode::PreRender(Scene* pScene)
{
	// get the camera position in world space
	Vec3 cameraPos = m_Camera->Get()->ToWorld().GetPosition();
	// get the sky node's world matrix
	Mat4x4 mat = m_Properties.ToWorld();

	// set the matrix's position to the camera position
	mat.SetPosition(cameraPos);
	SetTransform(&mat);

	return SceneNode::PreRender(pScene);
}

bool SkyNode::IsVisible(Scene* pScene) const
{
	return m_Active;
}

std::string SkyNode::GetTextureName(const int side)
{
	std::string name = m_TextureBaseName;
	char* letters[] = { "n", "e", "s", "w", "u" };
	unsigned int index = name.find_first_of("_");
	CB_ASSERT(index >= 0 && index < name.length() - 1);
	if (index >= 0 && index < name.length() - 1)
	{
		name[index + 1] = *letters[side];
	}
	return name;
}
