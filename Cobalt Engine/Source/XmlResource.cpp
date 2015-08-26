/*
	XmlResource.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#include "EngineStd.h"
#include "ResourceHandle.h"
#include "XmlResource.h"

void XmlResourceExtraData::ParseXml(char* pRawBuffer)
{
	m_XmlDocument.Parse(pRawBuffer);
}


bool XmlResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	if (rawSize <= 0)
		return false;

	// create an extra data, and parse the xml into an xml object
	shared_ptr<XmlResourceExtraData> pExtraData = shared_ptr<XmlResourceExtraData>(CB_NEW XmlResourceExtraData);
	pExtraData->ParseXml(rawBuffer);

	handle->SetExtra(shared_ptr<XmlResourceExtraData>(pExtraData));

	return true;
}

TiXmlElement* XmlResourceLoader::LoadAndReturnRootXmlElement(const char* resourceString)
{
	Resource resource(resourceString);
	shared_ptr<ResHandle> pResHandle = g_pApp->m_ResCache->GetHandle(&resource);
	shared_ptr<XmlResourceExtraData> pExtraData = static_pointer_cast<XmlResourceExtraData>(pResHandle->GetExtra());
	return pExtraData->GetRoot();
}


shared_ptr<IResourceLoader> CreateXmlResourceLoader()
{
	return shared_ptr<IResourceLoader>(CB_NEW XmlResourceLoader());
}