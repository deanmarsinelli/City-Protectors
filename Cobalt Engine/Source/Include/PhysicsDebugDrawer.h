/*
	PhysicsDebugDrawer.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <btBulletDynamicsCommon.h>

/**
	Used to draw debug physics information from the bullet sdk. This uses
	Direct3d lines to represent physics colliders, normals, etc.
*/
class BulletDebugDrawer : public btIDebugDraw
{
public:
	// btIDebugDraw interface
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
	virtual void reportErrorWarning(const char* warningString) override;
	virtual void draw3dText(const btVector3& location, const char* textString) override;
	virtual void setDebugMode(int debugMode) override;
	virtual int	 getDebugMode() const override;
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

	void SetOptions(int options);

	// TODO add turning this on through the console

private:
	DebugDrawModes m_DebugModes;
};