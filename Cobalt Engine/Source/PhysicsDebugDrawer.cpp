/*
	PhysicsDebugDrawer.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "EngineStd.h"
#include "HumanView.h"
#include "Logger.h"
#include "PhysicsDebugDrawer.h"

void BulletDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	const btVector3 startPoint = PointOnB;
	const btVector3 endPoint = PointOnB + normalOnB * distance;

	drawLine(startPoint, endPoint, color);
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString)
{
	CB_WARNING(warningString);
}

void BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	// TODO -- no implementation yet
}

void BulletDebugDrawer::setDebugMode(int debugMode)
{
	m_DebugModes = (DebugDrawModes)debugMode;
}

int	BulletDebugDrawer::getDebugMode() const
{
	return m_DebugModes;
}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	shared_ptr<Scene> pScene = g_pApp->GetHumanView()->m_pScene;
	shared_ptr<IRenderer> pRenderer = pScene->GetRenderer();

	Vec3 vec3From;
	vec3From.x = from.x();
	vec3From.y = from.y();
	vec3From.z = from.z();

	Vec3 vec3To;
	vec3To.x = to.x();
	vec3To.y = to.y();
	vec3To.z = to.z();

	Color lineColor = D3DCOLOR_XRGB((BYTE)(255 * color.x()), (BYTE)(255 * color.y()), (BYTE)(255 * color.z()));
	pRenderer->DrawLine(vec3From, vec3To, lineColor);
}
