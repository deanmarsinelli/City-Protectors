/*
	interfaces.h

	This file contains declaration data and
	type definitions for commonly used interface
	classes in the engine.
*/

#pragma once

#include <list>
#include <memory>

#include "EngineStd.h"
#include "Geometry.h"
#include "types.h"

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

/// Less than compare functor to sort shared_ptr's. If T is a custom class, the less than operator should be overloaded.
template<class T>
struct SortBy_SharedPtr_Content
{
	bool operator()(const shared_ptr<T> &lhs, const shared_ptr<T> &rhs) const
	{
		return *lhs < *rhs;
	}
};

//====================================================
//	UI and Logic Interfaces
//====================================================
typedef unsigned int GameViewId;

enum GameViewType
{
	GameView_Human,
	GameView_Remote,
	GameView_AI,
	GameView_Recorder,
	GameView_Other
};

class IGameView
{
public:
	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc). Also used for initialization
	virtual HRESULT OnRestore() = 0;

	/// Render the Game View
	virtual void OnRender(double time, float deltaTime) = 0;
	virtual HRESULT OnLostDevice() = 0;
	virtual GameViewType GetType() = 0;
	virtual GameViewId GetId() const = 0;
	virtual void OnAttach(GameViewId viewId, GameObjectId objectId) = 0;
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) = 0;
	virtual void OnUpdate(float deltaTime) = 0;
	virtual ~IGameView() { };
};

class IScreenElement
{
public:
	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc). Also used for initialization
	virtual HRESULT OnRestore() = 0;

	/// Render the screen element
	virtual HRESULT OnRender(double time, float deltaTime) = 0;
	virtual HRESULT OnLostDevice() = 0;
	virtual void OnUpdate(float deltaTime) = 0;
	virtual int GetZOrder() const = 0;
	virtual bool IsVisible() const = 0;
	virtual void SetVisible(bool visible) = 0;
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) = 0;
	virtual ~IScreenElement() { };

	/// Less than overloaded operator that compares Z order
	virtual bool operator <(const IScreenElement &other) { return GetZOrder() < other.GetZOrder(); }
};

typedef std::list<shared_ptr<IGameView>> GameViewList;
typedef std::list<shared_ptr<IScreenElement>> ScreenElementList;

//====================================================
//	Rendering Interfaces
//====================================================
class LightNode;
typedef std::list<shared_ptr<LightNode>> Lights;

/**
	Interface for representing the state of the renderer.
*/
class IRenderState
{
public:
	virtual std::string ToStr() = 0;
};

/**
	Generic Interface for implementing a renderer.
*/
class IRenderer
{
public:
	virtual void SetBackgroundColor(BYTE r, BYTE g, BYTE b, BYTE a) = 0;
	virtual HRESULT OnRestore() = 0;
	virtual void Shutdown() = 0;
	virtual bool PreRender() = 0;
	virtual bool PostRender() = 0;
	virtual void CalcLighting(Lights* lights, int maxLights) = 0;
	virtual void SetWorldTransform(const Mat4x4* m) = 0;
	virtual void SetViewTransform(const Mat4x4* m) = 0;
	virtual void SetProjectionTransform(const Mat4x4* m) = 0;
	virtual shared_ptr<IRenderState> PrepareAlphaPass() = 0;
	virtual shared_ptr<IRenderState> PrepareSkyBoxPass() = 0;
	virtual void DrawLine(const Vec3& from, const Vec3& to, const Color& color) = 0;
};

//====================================================
//	Resource Interfaces
//====================================================
class Resource;
class ResHandle;

/**
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

	/// Return whether to release the raw buffer of a resource after it has been loaded
	virtual bool DiscardRawBufferAfterLoad() = 0;

	/// Return whether the file buffer ends in a null terminator after the raw data
	virtual bool AddNullZero() { return false; }

	/// Return the size of the loaded resource
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) = 0;

	/// Load a resource into a resource handle
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle) = 0;
};
 

//====================================================
//	Input Interfaces   TODO documentation
//====================================================
class IKeyboardHandler
{
public:
	virtual bool OnKeyDown(const BYTE keycode) = 0;
	virtual bool OnKeyUp(const BYTE keycode) = 0;
};

class IPointerHandler
{
public:
	virtual bool OnPointerMove(const Point& mousePos, const int radius) = 0;
	virtual bool OnPointerButtonDown(const Point& mousePos, const int radius, const std::string& buttonName) = 0;
	virtual bool OnPointerButtonUp(const Point& mousePos, const int radius, const std::string& buttonName) = 0;
};

class IGamepadHandler
{
	virtual bool OnButtonDown(const std::string& buttonName, const int pressure) = 0;
	virtual bool OnButtonUp(const std::string& buttonName) = 0;
	virtual bool OnTrigger(const std::string& triggerName, const float pressure) = 0;
	virtual bool OnThumbstick(const std::string& stickName, const float x, const float y) = 0;
	virtual bool OnDirectionalPad(const std::string& direction) = 0;
};

class IJoystickHandler
{
	virtual bool OnButtonDown(const std::string& buttonName, const int pressure) = 0;
	virtual bool OnButtonUp(const std::string& buttonName) = 0;
	virtual bool OnJoystick(const float x, const float y) = 0;
};