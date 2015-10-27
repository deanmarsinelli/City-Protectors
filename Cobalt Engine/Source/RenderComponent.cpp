/*
	RenderComponent.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include "D3DGrid9.h"
#include "D3DGrid11.h"
#include "D3DShaderMeshNode9.h"
#include "D3DShaderMeshNode11.h"
#include "D3DSkyNode9.h"
#include "D3DSkyNode11.h"
#include "EngineStd.h"
#include "Events.h"
#include "Logger.h"
#include "RenderComponent.h"
#include "SkyNode.h"
#include "StringUtil.h"
#include "Transform.h"

const char* MeshRenderComponent::g_Name = "MeshRenderComponent";
const char* SphereRenderComponent::g_Name = "SphereRenderComponent";
const char* TeapotRenderComponent::g_Name = "TeapotRenderComponent";
const char* GridRenderComponent::g_Name = "GridRenderComponent";
const char* LightRenderComponent::g_Name = "LightRenderComponent";
const char* SkyRenderComponent::g_Name = "SkyRenderComponent";

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


//====================================================
//	GridRenderComponent definitions
//====================================================
GridRenderComponent::GridRenderComponent()
{
	m_TextureResource = "";
	m_Squares = 0;
}

bool GridRenderComponent::DelegateInit(TiXmlElement* pData)
{
	// init the component
	TiXmlElement* pTexture = pData->FirstChildElement("Texture");
	if (pTexture)
	{
		m_TextureResource = pTexture->FirstChild()->Value();
	}

	TiXmlElement* pDivision = pData->FirstChildElement("Division");
	if (pDivision)
	{
		m_Squares = atoi(pDivision->FirstChild()->Value());
	}

	return true;
}

shared_ptr<SceneNode> GridRenderComponent::CreateSceneNode()
{
	Transform* transform = &(m_pOwner->transform);

	WeakBaseRenderComponentPtr weakThis(this);

	switch (WindowsApp::GetRendererImpl())
	{
	case WindowsApp::Renderer_D3D9:
		return shared_ptr<SceneNode>(CB_NEW D3DGrid9(m_pOwner->GetId(), weakThis, &(transform->GetTransform())));

	case WindowsApp::Renderer_D3D11:
		return shared_ptr<SceneNode>(CB_NEW D3DGrid11(m_pOwner->GetId(), weakThis, &(transform->GetTransform())));

	default:
		CB_ERROR("Unknown Renderer Implementation in GridRenderComponent");
	}
	

	return shared_ptr<SceneNode>();
}

void GridRenderComponent::CreateInheritedXmlElements(TiXmlElement* pBaseElement)
{
	TiXmlElement* pTextureNode = CB_NEW TiXmlElement("Texture");
	TiXmlText* pTextureText = CB_NEW TiXmlText(m_TextureResource.c_str());
	pTextureNode->LinkEndChild(pTextureText);
	pBaseElement->LinkEndChild(pTextureNode);

	TiXmlElement* pDivisionNode = CB_NEW TiXmlElement("Division");
	TiXmlText* pDivisionText = CB_NEW TiXmlText(ToStr(m_Squares).c_str());
	pDivisionNode->LinkEndChild(pDivisionText);
	pBaseElement->LinkEndChild(pDivisionNode);
}


//====================================================
//	SphereRenderComponent definitions
//====================================================
SphereRenderComponent::SphereRenderComponent()
{
	m_Segments = 50;
}

bool SphereRenderComponent::DelegateInit(TiXmlElement* pData)
{
	TiXmlElement* pMesh = pData->FirstChildElement("Sphere");
	int segments = 50;
	double radius = 1.0;

	pMesh->Attribute("radius", &radius);
	pMesh->Attribute("segments", &segments);
	m_Radius = (float)radius;
	m_Segments = (unsigned int)segments;

	return true;
}

shared_ptr<SceneNode> SphereRenderComponent::CreateSceneNode()
{
	// get the transform
	// TODO fix transform to return a reference to the transform
	Transform* transform = &(m_pOwner->transform);

	WeakBaseRenderComponentPtr weakThis(this);
	if (WindowsApp::GetRendererImpl() == WindowsApp::Renderer_D3D9)
	{
		// create the sphere Mesh
		ID3DXMesh* pSphereMesh;

		D3DXCreateSphere(DXUTGetD3D9Device(), m_Radius, m_Segments, m_Segments, &pSphereMesh, NULL);

		shared_ptr<SceneNode> sphere(CB_NEW D3DShaderMeshNode9(m_pOwner->GetId(), weakThis, pSphereMesh, "Effects\\GameCode4.fx", RenderPass_Object, &transform->GetTransform()));

		SAFE_RELEASE(pSphereMesh);
		return sphere;
	}
	else if (WindowsApp::GetRendererImpl() == WindowsApp::Renderer_D3D11)
	{
		shared_ptr<SceneNode> sphere(CB_NEW D3DShaderMeshNode11(m_pOwner->GetId(), weakThis, "art\\sphere.sdkmesh", RenderPass_Object, &transform->GetTransform()));
		return sphere;
	}
	else
	{
		CB_ASSERT(0 && "Unknown Renderer Implementation in SphereRenderComponent::VCreateSceneNode");
		return shared_ptr<SceneNode>(NULL);
	}
}

void SphereRenderComponent::CreateInheritedXmlElements(TiXmlElement* pBaseElement)
{
	TiXmlElement* pMesh = CB_NEW TiXmlElement("Sphere");
	pMesh->SetAttribute("radius", ToStr(m_Radius).c_str());
	pMesh->SetAttribute("segments", ToStr(m_Segments).c_str());

	pBaseElement->LinkEndChild(pMesh);
}


//====================================================
//	TeapotRenderComponent definitions
//====================================================
shared_ptr<SceneNode> TeapotRenderComponent::CreateSceneNode()
{
	// get the transform 
	Transform* transform = &(m_pOwner->transform);

	WeakBaseRenderComponentPtr weakThis(this);

	switch (WindowsApp::GetRendererImpl())
	{
	case WindowsApp::Renderer_D3D9:
		return shared_ptr<SceneNode>(CB_NEW D3DTeapotMeshNode9(m_pOwner->GetId(), weakThis, "Effects\\GameCode4.fx", RenderPass_Object, &transform->GetTransform()));

	case WindowsApp::Renderer_D3D11:
	{
		Mat4x4 rot90;
		rot90.BuildRotationY(-CB_PI / 2.0f);
		shared_ptr<SceneNode> parent(CB_NEW SceneNode(m_pOwner->GetId(), weakThis, RenderPass_Object, &transform->GetTransform()));
		shared_ptr<SceneNode> teapot(CB_NEW D3DTeapotMeshNode11(INVALID_GAMEOBJECT_ID, weakThis, RenderPass_Object, &rot90));
		parent->AddChild(teapot);
		return parent;
	}

	default:
		CB_ERROR("Unknown Renderer Implementation in TeapotRenderComponent");
	}
	

	return shared_ptr<SceneNode>();
}

void TeapotRenderComponent::CreateInheritedXmlElements(TiXmlElement* pBaseElement)
{
	// FUTURE
}
