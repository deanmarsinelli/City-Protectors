/*
	interfaces.h

	This file contains declaration data and
	type definitions for commonly used interface
	classes in the engine.
*/

#pragma once

#include <memory>

using std::shared_ptr;
using std::weak_ptr;

// forward declarations
class GameObject;
class Component;

typedef unsigned int GameObjectId;
typedef unsigned int ComponentId;

typedef shared_ptr<GameObject> StrongGameObjectPtr;
typedef weak_ptr<GameObject> WeakGameObjectPtr;
typedef shared_ptr<Component> StrongComponentPtr;
typedef weak_ptr<Component> WeakComponentPtr;


class Resource;
class ResHandle;

/*
	Interface for a resource file. This is the base class for a single resource file
	that contains several resources inside of it. Subclasses must implement all the pure
	virtual methods in this class.
*/
class IResourceFile
{
public:
	/// Open the file and return success or failure
	virtual bool Open() = 0;

	/// Return the size of the resource based on the name of the resource
	virtual int GetRawResourceSize(const Resource& r) = 0;

	/// Read the resource into a buffer and return how many bytes were read
	virtual int GetRawResource(const Resource& r, char* buffer) = 0;
	
	/// Return the number of resources in a resource file
	virtual int GetNumResources() const = 0;

	/// Return the name of the nth resource
	virtual std::string GetResourceName(int n) const = 0;
	
	/// Virtual Destructor
	virtual ~IResourceFile() { }
};

/**
	Used to attach extra data to a resource such as length or file format.
*/
class IResourceExtraData
{
public:
	/// Returns a string describing the extra data
	virtual std::string ToString() = 0;
};

/**
	Defines the behavior that individual resource loaders must implement. Loaders are used
	for resources that need additional processing on load.
*/
class IResourceLoader
{
public:
	/// Returns a pattern that the resource cache uses to distinguish which loader is used with which files
	virtual std::string GetPattern() = 0;

	/// Return true if the resource loader can use the stored data with no extra processing
	virtual bool UseRawFile() = 0;

	/// Return the size of the loaded resource
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) = 0;

	/// Load a resource into a resource handle
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle) = 0;
};