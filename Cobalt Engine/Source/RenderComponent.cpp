/*
	RenderComponent.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "EngineStd.h"
#include "Events.h"
#include "RenderComponent.h"
#include "StringUtil.h"

//====================================================
//	BaseRenderComponent definitions
//====================================================
bool BaseRenderComponent::Init(TiXmlElement* pData)
{
	TiXmlElement* pColorNode = pData->FirstChildElement("Color");
	if (pColorNode)
		m_Color = LoadColor(pColorNode);

	return DelegateInit(pData);
}

void BaseRenderComponent::PostInit()
{
	shared_ptr<SceneNode> pSceneNode(GetSceneNode());
	shared_ptr<Event_NewRenderComponent> pEvent(CB_NEW Event_NewRenderComponent);
	IEventManager::Get()->TriggerEvent(pEvent);
}

void BaseRenderComponent::OnChanged()
{
	shared_ptr<Event_ModifiedRenderComponent> pEvent(CB_NEW Event_ModifiedRenderComponent);
	IEventManager::Get()->TriggerEvent(pEvent);
}

TiXmlElement* BaseRenderComponent::GenerateXml()
{
	TiXmlElement* pBaseElement = CreateBaseElement();

	// generate color
	TiXmlElement* pColor = CB_NEW TiXmlElement("Color");
	pColor->SetAttribute("r", ToStr(m_Color.r).c_str());
	pColor->SetAttribute("g", ToStr(m_Color.g).c_str());
	pColor->SetAttribute("b", ToStr(m_Color.b).c_str());
	pColor->SetAttribute("a", ToStr(m_Color.a).c_str());
	pBaseElement->LinkEndChild(pColor);

	// create XML for inherited classes
	CreateInheritedXmlElements(pBaseElement);

	return pBaseElement;
}

const Color BaseRenderComponent::GetColor() const
{
	return m_Color;
}

bool BaseRenderComponent::DelegateInit(TiXmlElement* pData)
{
	return true;
}

Color BaseRenderComponent::LoadColor(TiXmlElement* pData)
{
	Color color;

	double r = 1.0;
	double g = 1.0;
	double b = 1.0;
	double a = 1.0;

	// get the color from the xml data
	pData->Attribute("r", &r);
	pData->Attribute("g", &g);
	pData->Attribute("b", &b);
	pData->Attribute("a", &a);

	color.r = (float)r;
	color.g = (float)g;
	color.b = (float)b;
	color.a = (float)a;

	return color;
}

TiXmlElement* BaseRenderComponent::CreateBaseElement()
{
	return CB_NEW TiXmlElement(GetName());
}

shared_ptr<SceneNode> BaseRenderComponent::GetSceneNode()
{
	if (!m_pSceneNode)
		m_pSceneNode = CreateSceneNode();

	return m_pSceneNode;
}
