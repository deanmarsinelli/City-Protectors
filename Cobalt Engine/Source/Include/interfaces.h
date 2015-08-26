/*
	interfaces.h

	This file contains declaration data and
	type definitions for commonly used interface
	classes in the engine.
*/

#pragma once

#include <concurrent_queue.h>
#include <d3dx9.h>
#include <FastDelegate.h>
#include <list>
#include <memory>
#include <tinyxml.h>
#include <Windows.h>

using std::shared_ptr;
using std::weak_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;

// forward declarations
class GameObject;
class Component;

typedef unsigned int GameObjectId;
typedef unsigned int ComponentId;

const GameObjectId INVALID_GAMEOBJECT_ID = 0;
const ComponentId INVALID_COMPONENT_ID = 0;

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
struct AppMsg;
typedef unsigned int GameViewId;

/// Types of views available
enum GameViewType
{
	GameView_Human,
	GameView_Remote,
	GameView_AI,
	GameView_Recorder,
	GameView_Other
};

/**
	Interface for implementing the view layer in a game. This layer is 
	separate from the game logic layer, but will represent it on screen.
*/
class IGameView
{
public:
	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc). Also used for initialization
	virtual HRESULT OnRestore() = 0;

	/// Render the Game View
	virtual void OnRender(float time, float deltaTime) = 0;

	/// Method to handle the device being lost
	virtual HRESULT OnLostDevice() = 0;

	/// Return the type of game view
	virtual GameViewType GetType() = 0;

	/// Return the id of the game view
	virtual GameViewId GetId() const = 0;

	/// Attach a game view to a game object
	virtual void OnAttach(GameViewId viewId, GameObjectId objectId) = 0;

	/// Msg callback from the application layer to the view
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) = 0;

	/// Update method for the game view
	virtual void OnUpdate(float deltaTime) = 0;

	/// Default destructor
	virtual ~IGameView() { }
};

/**
	Interface for any element that can be represented in the view such as 
	a UI element or a 3D scene representation of the game world.
*/
class IScreenElement
{
public:
	/// Recreates anything that might have been lost while the game is running (because of system sleep, etc). Also used for initialization
	virtual HRESULT OnRestore() = 0;

	/// Render the screen element
	virtual HRESULT OnRender(float time, float deltaTime) = 0;

	/// Method to handle the device being lost
	virtual HRESULT OnLostDevice() = 0;

	/// Return the Z-order value of the element
	virtual int GetZOrder() const = 0;

	/// Set the Z-order value of the element
	virtual void SetZOrder(const int zOrder) = 0;

	/// Return whether or not the element is visible
	virtual bool IsVisible() const = 0;

	/// Set an element visible or invisible
	virtual void SetVisible(bool visible) = 0;

	/// Msg callback from the view to the screen element
	virtual LRESULT CALLBACK OnMsgProc(AppMsg msg) = 0;

	/// Update method for a screen element
	virtual void OnUpdate(float deltaTime) = 0;

	/// Default destructor
	virtual ~IScreenElement() { }

	/// Less than overloaded operator that compares Z order
	virtual bool operator <(const IScreenElement &other) { return GetZOrder() < other.GetZOrder(); }
};

typedef std::list<shared_ptr<IGameView>> GameViewList;
typedef std::list<shared_ptr<IScreenElement>> ScreenElementList;

//====================================================
//	Rendering Interfaces
//====================================================
typedef D3DXCOLOR Color;
class Mat4x4;
class Vec3;
class LightNode;

/// Describes which pass a scene node is rendered in
enum RenderPass
{
	RenderPass_0,
	RenderPass_Static = RenderPass_0,
	RenderPass_Object,
	RenderPass_Sky,
	RenderPass_NotRendered, // things that are not rendered, like editor triggers
	RenderPass_Last // not used - a counter for for-loops
};

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

class Scene;
class SceneNodeProperties;
class RayCast;
/**
	The interface for all nodes in a scene graph. These nodes are the basis of the 3d world.
*/
class ISceneNode
{
public:
	/// Virtual destructor
	virtual ~ISceneNode() { }

	/// Return the properties of the scene node
	virtual const SceneNodeProperties* Get() const = 0;

	/// Calculate the inverse transform matrix (world space to object space)
	virtual void SetTransform(const Mat4x4* toWorld, const Mat4x4* fromWorld = nullptr) = 0;

	/// Update method called once per frame
	virtual HRESULT OnUpdate(Scene* pScene, float deltaTime) = 0;

	/// Restore when the device is lost
	virtual HRESULT OnRestore(Scene* pScene) = 0;

	/// Set up the node for rendering
	virtual HRESULT PreRender(Scene* pScene) = 0;

	/// Return whether or not the node is visible
	virtual bool IsVisible(Scene* pScene) const = 0;

	/// Render an object
	virtual HRESULT Render(Scene* pScene) = 0;

	/// Render all children objects (used for animation)
	virtual HRESULT RenderChildren(Scene* pScene) = 0;

	/// Perform any post render actions
	virtual HRESULT PostRender(Scene* pScene) = 0;

	/// Add a child node
	virtual bool AddChild(shared_ptr<ISceneNode> child) = 0;

	/// Remove a child node
	virtual bool RemoveChild(GameObjectId id) = 0;

	/// Called when the device is lost
	virtual HRESULT OnLostDevice(Scene* pScene) = 0;

	/// Find the intersection of a ray with a polygon on a scene node
	virtual HRESULT Pick(Scene* pScene, RayCast* pRayCast) = 0;
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
	
	/// Return true if using the games development directories
	virtual bool IsUsingDevelopmentDirectories() const = 0;

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
	virtual std::string ToStr() = 0;
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
class Point;

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


//====================================================
//	Event Interfaces
//====================================================
typedef unsigned long EventType;
class IEvent;
typedef shared_ptr<IEvent> IEventPtr;

/**
	Interface for every event object.
*/
class IEvent
{
public:
	/// Virtual destructor
	virtual ~IEvent() {}
	
	/// Return the type of event
	virtual const EventType& GetEventType() const = 0;

	/// Return the time that the event occured
	virtual float GetTimeStamp() const = 0;

	/// Serialze the event to an output stream
	virtual void Serialize(std::ostream& out) const = 0;

	/// Deserialize an event from an input stream
	virtual void Deserialize(std::istream& in) = 0;

	/// Copy the event and return a pointer to it
	virtual IEventPtr Copy() const = 0;

	/// Return the name of the event
	virtual const char* GetName() const = 0;
};

// create a typedef for an event listener function aka delegate
typedef fastdelegate::FastDelegate1<IEventPtr> EventListenerDelegate;
typedef Concurrency::concurrent_queue<IEventPtr> ThreadSafeEventQueue;

/**
	Interface for an event manager. This object will maintain a list of registered events 
	and their listeners. A listener may listen for many events and an event may have 
	many individual listeners.
*/
class IEventManager
{
public:
	/// Constructor to set the event manager global or not
	explicit IEventManager(const char* pName, bool setAsGlobal);

	/// Virtual destructor
	virtual ~IEventManager();

	/// Registers a delegate function that will get called when the event type is triggered -- returns true if successful
	virtual bool AddListener(const EventListenerDelegate& eventDelegate, const EventType& type) = 0;

	/// Remove a delegate/event type pairing -- returns false if the pairing is not found
	virtual bool RemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type) = 0;

	/// Fire this event now and immediately call all delegate functions listening for this event
	virtual bool TriggerEvent(const IEventPtr& pEvent) const = 0;

	/// Queue the event and fire it on the next update if there is enough time
	virtual bool QueueEvent(const IEventPtr& pEvent) = 0;

	/// [thread safe] Queue the event and fire it on the next update if there is enough time
	virtual bool ThreadSafeQueueEvent(const IEventPtr& pEvent) = 0;

	/// Find the next instance of the event type and remove it from the processing queue -- if allOfType is true all events of this type will be removed
	virtual bool AbortEvent(const EventType& type, bool allOfType = false) = 0;

	/// Process events from the queue and optionally limit the processing time
	virtual bool Update(unsigned long maxMillis = kINFINITE) = 0;

	/// Return the main global event manager
	static IEventManager* Get();

public:
	enum Constants 
	{
		kINFINITE = 0xffffffff
	};
};


//====================================================
//	Scripting Interfaces
//====================================================
/**
	Interface for a script manager that is agnostic to which language is used.
*/
class IScriptManager
{
public:
	/// Virtual destructor
	virtual ~IScriptManager() { }

	/// Initialize the script manager
	virtual bool Init() = 0;

	/// Execute a script file
	virtual void ExecuteFile(const char* resource) = 0;

	/// Execute a single line of script code
	virtual void ExecuteString(const char* str) = 0;
};


//====================================================
//	Audio Interfaces
//====================================================
/**
	Interface for a buffer that contains audio data. This class
	is agnostic to what kind of audio data it stores and the 
	audio platform that plays it.
*/
class IAudioBuffer
{
public:
	/// Virtual destructor
	virtual ~IAudioBuffer() { }

	/// Return an implementation specific handle to the allocated sound
	virtual void* Get() = 0;

	/// Return a pointer to the resource handle of this audio sound
	virtual shared_ptr<ResHandle> GetResource() = 0;

	/// Restore an audio buffer that has been lost
	virtual bool OnRestore() = 0;

	/// Play an audio sound, volume should be 0 - 100
	virtual bool Play(int volume, bool looping) = 0;

	/// Pause a sound that is currently playing
	virtual bool Pause() = 0;

	/// Stop a sound that is playing
	virtual bool Stop() = 0;

	/// Resume a paused sound
	virtual bool Resume() = 0;

	/// Pause or unpause a sound based on the current state of the sound
	virtual bool TogglePause() = 0;

	/// Return true if the sound is currently playing
	virtual bool IsPlaying() = 0;

	/// Return true if the sound is currently set to loop
	virtual bool IsLooping() const = 0;

	/// Set the volume of the sound
	virtual void SetVolume(int volume) = 0;

	/// Instantly set the sound to a new position
	virtual void SetPosition(unsigned long newPosition) = 0;

	/// Return the volume of the sound
	virtual int GetVolume() const = 0;

	/// Return a value between 0.0 and 1.0 that represents how much of a sound has played
	virtual float GetProgress() = 0;
};

/**
	Interface for an audio system that manages a list of sounds.
*/
class IAudio
{
public:
	/// Initialize the sound system
	virtual bool Initialize(HWND hWnd) = 0;

	/// Shutdown the sound system
	virtual void Shutdown() = 0;

	/// Return true if the sound system is active
	virtual bool Active() = 0;

	/// Initialize an audio buffer
	virtual IAudioBuffer* InitAudioBuffer(shared_ptr<ResHandle> soundResource) = 0;

	/// Release an audio buffer
	virtual void ReleaseAudioBuffer(IAudioBuffer* audioBuffer) = 0;

	/// Stop all currently playing sounds
	virtual void StopAllSounds() = 0;

	/// Pause all currently playing sounds
	virtual void PauseAllSounds() = 0;

	/// Resume all paused sounds
	virtual void ResumeAllSounds() = 0;
};


//====================================================
//	Physics Interfaces
//====================================================
class IGamePhysics
{
public:
	/// Apply a directional force to a game object
	virtual void ApplyForce(const Vec3& dir, float newtons, GameObjectId id) = 0;
	
	/// Apply torque to a game object
	virtual void ApplyTorque(const Vec3& dir, float newtons, GameObjectId id) = 0;
};


//====================================================
//	Logic Interfaces
//====================================================
class IGameLogic
{
public:
	/// Return a weak pointer to a game object
	virtual WeakGameObjectPtr GetGameObject(const GameObjectId id) = 0;
	
	/// Create a game object
	virtual StrongGameObjectPtr CreateGameObject(const std::string& objectResource, TiXmlElement* overrides, 
		const Mat4x4* initialTransform = nullptr, const GameObjectId serversObjectId = INVALID_GAMEOBJECT_ID) = 0;
	
	/// Destroy a game object
	virtual void DestroyGameObject(const GameObjectId id) = 0;
	
	/// Load a game from a resource
	virtual bool LoadGame(const char* levelResource) = 0;
	
	
	virtual void SetProxy() = 0;
	
	/// Update the game logic -- called once per frame
	virtual void OnUpdate(float time, float deltaTime) = 0;
	
	/// Change the state of the game logic
	virtual void ChangeState(enum BaseGameState newState) = 0;

	/// Move a game object
	virtual void MoveGameObject(const GameObjectId id, const Mat4x4& mat) = 0;
	
	/// Return a pointer to the game physics
	virtual shared_ptr<IGamePhysics> GetGamePhysics() = 0;
};
