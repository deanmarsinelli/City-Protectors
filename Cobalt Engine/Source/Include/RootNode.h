/*
	RootNode.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include "SceneNode.h"

/**
	This is the root node of the scene graph. All other nodes should be
	children of a root node.
*/
class RootNode : public SceneNode
{
public:
	/// Default constructor
	RootNode();

	/// Add a child node
	virtual bool AddChild(shared_ptr<ISceneNode> child);

	/// Remove a child node
	virtual bool RemoveChild(GameObjectId id);

	/// Return whether or not the node is visible
	virtual bool IsVisible(Scene* pScene) const;

	/// Render all child nodes
	virtual HRESULT RenderChildren(Scene* pScene);
};