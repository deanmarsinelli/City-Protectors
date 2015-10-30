/*
	TransformComponent.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "Logger.h"
#include "MathUtils.h"
#include "StringUtil.h"
#include "TransformComponent.h"

const char* TransformComponent::g_Name = "TransformComponent";

bool TransformComponent::Init(TiXmlElement* pData)
{
	CB_ASSERT(pData);

	Vec3 yawPitchRoll = m_Transform.GetYawPitchRoll();
	yawPitchRoll.x = RADIANS_TO_DEGREES(yawPitchRoll.x);
	yawPitchRoll.y = RADIANS_TO_DEGREES(yawPitchRoll.y);
	yawPitchRoll.z = RADIANS_TO_DEGREES(yawPitchRoll.z);

	Vec3 position = m_Transform.GetPosition();

	TiXmlElement* pPositionElement = pData->FirstChildElement("Position");
	if (pPositionElement)
	{
		double x = 0;
		double y = 0;
		double z = 0;
		pPositionElement->Attribute("x", &x);
		pPositionElement->Attribute("y", &y);
		pPositionElement->Attribute("z", &z);
		position = Vec3(x, y, z);
	}

	TiXmlElement* pOrientationElement = pData->FirstChildElement("YawPitchRoll");
	if (pOrientationElement)
	{
		double yaw = 0;
		double pitch = 0;
		double roll = 0;
		pOrientationElement->Attribute("x", &yaw);
		pOrientationElement->Attribute("y", &pitch);
		pOrientationElement->Attribute("z", &roll);
		yawPitchRoll = Vec3(yaw, pitch, roll);
	}

	Mat4x4 translation;
	translation.BuildTranslation(position);

	Mat4x4 rotation;
	rotation.BuildYawPitchRoll((float)DEGREES_TO_RADIANS(yawPitchRoll.x), (float)DEGREES_TO_RADIANS(yawPitchRoll.y), (float)DEGREES_TO_RADIANS(yawPitchRoll.z));

	m_Transform = rotation * translation;

	return true;
}

TiXmlElement* TransformComponent::GenerateXml()
{
	TiXmlElement* pBaseElement = CB_NEW TiXmlElement(GetName());

	// initial transform -> position
	TiXmlElement* pPosition = CB_NEW TiXmlElement("Position");
	Vec3 pos(m_Transform.GetPosition());
	pPosition->SetAttribute("x", ToStr(pos.x).c_str());
	pPosition->SetAttribute("y", ToStr(pos.y).c_str());
	pPosition->SetAttribute("z", ToStr(pos.z).c_str());
	pBaseElement->LinkEndChild(pPosition);

	// initial transform -> LookAt
	TiXmlElement* pDirection = CB_NEW TiXmlElement("YawPitchRoll");
	Vec3 orient(m_Transform.GetYawPitchRoll());
	orient.x = RADIANS_TO_DEGREES(orient.x);
	orient.y = RADIANS_TO_DEGREES(orient.y);
	orient.z = RADIANS_TO_DEGREES(orient.z);
	pDirection->SetAttribute("x", ToStr(orient.x).c_str());
	pDirection->SetAttribute("y", ToStr(orient.y).c_str());
	pDirection->SetAttribute("z", ToStr(orient.z).c_str());
	pBaseElement->LinkEndChild(pDirection);

	return pBaseElement;
}
