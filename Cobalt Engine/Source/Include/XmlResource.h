/*
	XmlResource.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#pragma once

#include <tinyxml.h>

#include "interfaces.h"
#include "ResourceCache.h"

/**
	Contains extra data for xml resources. These are used to load 
	game settings, object/components, level data, etc.
*/
class XmlResourceExtraData : public IResourceExtraData
{
public:
	/// Get the root element of an xml document
	TiXmlElement* GetRoot() { return m_XmlDocument.RootElement(); }

	/// Parse a raw xml file into a tiny xml document
	void ParseXml(char* pRawBuffer);

	/// Returns a string describing the extra data
	virtual std::string ToStr() { return "XmlResourceExtraData"; }

private:
	/// The stored xml document
	TiXmlDocument m_XmlDocument;
};


/**
	Used to load xml resources into the resource cache.
*/
class XmlResourceLoader : public IResourceLoader
{
public:
	/// Return false since the raw file is not used
	virtual bool UseRawFile() { return false; }

	/// Return true so the raw buffer will be discarded
	virtual bool DiscardRawBufferAfterLoad() { return true; }

	/// Return the loaded resource size
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return rawSize; }

	/// Load a raw xml file into a resource handle
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle>);

	/// Return the pattern for xml files
	virtual std::string GetPattern() { return "*.xml"; }

	/// Convenience method to load and get the root element of an xml resource
	static TiXmlElement* LoadAndReturnRootXmlElement(const char* resourceString);
};
