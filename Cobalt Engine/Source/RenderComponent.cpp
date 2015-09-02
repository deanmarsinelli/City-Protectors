/*
	RenderComponent.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DSkyNode9.h"
#include "D3DSkyNode11.h"
#include "EngineStd.h"
#include "Events.h"
#include "Logger.h"
#include "RenderComponent.h"
#include "SkyNode.h"
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


//====================================================
//	MeshRenderComponent definitions
//====================================================
const char* MeshRenderComponent::GetName() const
{
	return g_Name;
}

shared_ptr<SceneNode> MeshRenderComponent::CreateSceneNode()
{
	return shared_ptr<SceneNode>();
}

void MeshRenderComponent::CreateInheritedXmlElements(TiXmlElement* pBaseElement)
{
	CB_ERROR("MeshRenderComponent::CreateInheritedXmlElements() not implemented");
}


//====================================================
//	LightRenderComponent definitions
//====================================================
const char* LightRenderComponent::GetName() const
{
	return g_Name;
}

bool LightRenderComponent::DelegateInit(TiXmlElement* pData)
{
	TiXmlElement* pLight = pData->FirstChildElement("Light");

	TiXmlElement* pAttenuationNode = nullptr;
	pAttenuationNode = pLight->FirstChildElement("Attenuation");
	if (pAttenuationNode)
	{
		double temp;
		pAttenuationNode->Attribute("const", &temp);
		m_Properties.m_Attenuation[0] = (float)temp;

		pAttenuationNode->Attribute("linear", &temp);
		m_Properties.m_Attenuation[1] = (float)temp;

		pAttenuationNode->Attribute("exp", &temp);
		m_Properties.m_Attenuation[2] = (float)temp;
	}

	TiXmlElement* pShapeNode = nullptr;
	pShapeNode = pLight->FirstChildElement("Shape");
	if (pShapeNode)
	{
		double temp;
		pShapeNode->Attribute("range", &temp);
		m_Properties.m_Range = (float)temp;

		pShapeNode->Attribute("falloff", &temp);
		m_Properties.m_Falloff = (float)temp;

		pShapeNode->Attribute("theta", &temp);
		m_Properties.m_Theta = (float)temp;

		pShapeNode->Attribute("phi", &temp);
		m_Properties.m_Phi = (float)temp;
	}

	return true;
}

shared_ptr<SceneNode> LightRenderComponent::CreateSceneNode()
{
	WeakBaseRenderComponentPtr weakThis(this);
	switch (WindowsApp::GetRendererImpl())
	{
	case WindowsApp::Renderer::Renderer_D3D11:
		return shared_ptr<SceneNode>(CB_NEW D3DLightNode11(m_pOwner->GetId(), weakThis, m_Properties, &m_pOwner->transform.GetTransform()));
	
	case WindowsApp::Renderer::Renderer_D3D9:
		return shared_ptr<SceneNode>(CB_NEW D3DLightNode9(m_pOwner->GetId(), weakThis, m_Properties, &m_pOwner->transform.GetTransform()));

	default:
		CB_ASSERT(0 && "Uknown render implementation in LightRenderComponent");
	}

	return shared_ptr<SceneNode>();
}

void LightRenderComponent::CreateInheritedXmlElements(TiXmlElement* pBaseElement)
{
	TiXmlElement* pSceneNode = CB_NEW TiXmlElement("Light");

	// attenuation
	TiXmlElement* pAttenuation = CB_NEW TiXmlElement("Attenuation");
	pAttenuation->SetAttribute("const", ToStr(m_Properties.m_Attenuation[0]).c_str());
	pAttenuation->SetAttribute("linear", ToStr(m_Properties.m_Attenuation[1]).c_str());
	pAttenuation->SetAttribute("exp", ToStr(m_Properties.m_Attenuation[2]).c_str());
	pSceneNode->LinkEndChild(pAttenuation);

	// shape
	TiXmlElement* pShape = CB_NEW TiXmlElement("Shape");
	pAttenuation->SetAttribute("range", ToStr(m_Properties.m_Range).c_str());
	pAttenuation->SetAttribute("falloff", ToStr(m_Properties.m_Falloff).c_str());
	pAttenuation->SetAttribute("theta", ToStr(m_Properties.m_Theta).c_str());
	pAttenuation->SetAttribute("phi", ToStr(m_Properties.m_Phi).c_str());
	pSceneNode->LinkEndChild(pAttenuation);

	pBaseElement->LinkEndChild(pSceneNode);
}


//====================================================
//	SkyRenderComponent definitions
//====================================================
const char* SkyRenderComponent::GetName() const
{
	return g_Name;
}

bool SkyRenderComponent::DelegateInit(TiXmlElement* pData)
{
	TiXmlElement* pTexture = pData->FirstChildElement("Texture");
	if (pTexture)
	{
		m_TextureResource = pTexture->FirstChild()->Value();
	}

	return true;
}

shared_ptr<SceneNode> SkyRenderComponent::CreateSceneNode()
{
	shared_ptr<SkyNode> sky;
	if (WindowsApp::GetRendererImpl() == WindowsApp::Renderer::Renderer_D3D11)
	{
		sky = shared_ptr<SkyNode>(CB_NEW D3DSkyNode11(m_TextureResource.c_str()));
	}
	else if (WindowsApp::GetRendererImpl() == WindowsApp::Renderer::Renderer_D3D9)
	{
		sky = shared_ptr<SkyNode>(CB_NEW D3DSkyNode9(m_TextureResource.c_str()));
	}
	else
	{
		CB_ERROR("Unknown renderer implementation in SkyRenderComponent::CreateSceneNode");
	}

	return sky;
}

void SkyRenderComponent::CreateInheritedXmlElements(TiXmlElement* pBaseElement)
{
	TiXmlElement* pTextureNode = CB_NEW TiXmlElement("Texture");
	TiXmlText* pTextureText = CB_NEW TiXmlText(m_TextureResource.c_str());
	pTextureNode->LinkEndChild(pTextureText);
	
	pBaseElement->LinkEndChild(pTextureNode);
}
