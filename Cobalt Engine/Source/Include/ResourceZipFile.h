/*
	ResourceZipFile.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham.
*/

#pragma once

#include <string>

#include "interfaces.h"
#include "ZipFile.h"

class Resource;

/**
	Implements the IResourceFile interface and manages a single ZipFile object
	which may contain many individual resources.
*/
class ResourceZipFile : public IResourceFile
{
public:
	/// Constructor taking a file name as a param
	ResourceZipFile(const std::wstring& resFileName);

	/// Virtual Destructor
	virtual ~ResourceZipFile();

	/// Open the file and return success or failure
	virtual bool Open();

	/// Return the size of the resource based on the name of the resource
	virtual int GetRawResourceSize(const Resource& r);

	/// Read the resource into a buffer and return how many bytes were read
	virtual int GetRawResource(const Resource& r, char* buffer);

	/// Return the number of resources in a resource file
	virtual int GetNumResources() const;

	/// Return the name of the nth resource
	virtual std::string GetResourceName(int n) const;

	/// Return true if using the games development directories
	virtual bool IsUsingDevelopmentDirectories() const { return false; }

private:
	/// Pointer to the ZipFile object this class manages
	ZipFile *m_pZipFile;

	/// Name of the resource file on disk
	std::wstring m_resFileName;
};


/**
	This class fakes a zip file from a regular directory and is used in the editor.
	This class is for developmental purposes only.
*/
class DevelopmentResourceZipFile : public ResourceZipFile
{
public:
	enum Mode
	{
		/// Development mode checks the original asset directory for changes
		Development,
		/// Editor mode only checks the original asset directory, the zip file is left unopened
		Editor
	};

	/// Constructor given a file name and mode
	DevelopmentResourceZipFile(const std::wstring resourceFileName, const Mode mode);

	/// Open the file and return success or failure
	virtual bool Open();

	/// Return the size of the resource based on the name of the resource
	virtual int GetRawResourceSize(const Resource& r);

	/// Read the resource into a buffer and return how many bytes were read
	virtual int GetRawResource(const Resource& r, char* buffer);

	/// Return the number of resources in a resource file
	virtual int GetNumResources() const;

	/// Return the name of the nth resource
	virtual std::string GetResourceName(int n) const;

	/// Return true if using the games development directories
	virtual bool IsUsingDevelopmentDirectories() const { return true; }

	int Find(const std::string& path);

protected:
	void ReadAssetsDirectory(std::wstring fileSpec);

public:
	// TODO DOCUMENTAION
	Mode m_Mode;
	
	std::wstring m_AssetsDir;

	std::vector<WIN32_FIND_DATA> m_AssetFileInfo;

	ZipContentsMap m_DirectoryContentsMap;
};
