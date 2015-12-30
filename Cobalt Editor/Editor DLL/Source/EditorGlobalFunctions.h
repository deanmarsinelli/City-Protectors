/*
	EditorGlobalFunctions.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham

	These functions will be exported through the DLL to be
	consumed by the C# editor application.
*/

#pragma once

#define DllExport _declspec(dllexport)

#include "Editor.h"

// Editor frameworke functions
extern "C" DllExport int EditorMain(int* instancePtrAddress, int* hPrevInstancePtrAddress, int* hWndPtrAddress, int nCmdShow, int screenWidth, int screenHeight);
extern "C" DllExport void WndProc(int* hWndPtrAddress, int msg, int wParam, int lParam);
extern "C" DllExport void RenderFrame();
extern "C" DllExport int Shutdown(); 
extern "C" DllExport int IsGameRunning();
extern "C" DllExport void OpenLevel(BSTR lFileName);

// GameObject accessor functions
extern "C" DllExport int GetNumGameObjects();
extern "C" DllExport void GetGameObjectList(int* gameObjectIdArrayPtrAddress, int numGameObjects);
extern "C" DllExport int GetGameObjectXmlSize(GameObjectId gameObjectId);
extern "C" DllExport void GetGameObjectXml(int* gameObjectXmlPtrAddress, GameObjectId gameObjectId);
extern "C" DllExport int PickGameObject(int* hWndPtrAddress);

// Actor Modification functions
extern "C" DllExport int CreateGameObject(BSTR bstrActorResource);
extern "C" DllExport void ModifyGameObject(BSTR bstrActorModificationXML);
extern "C" DllExport void DestroyGameObject(GameObjectId gameObjectId);

// Level script additions
extern "C" DllExport int GetLevelScriptAdditionsXmlSize();
extern "C" DllExport void GetLevelScriptAdditionsXml(int* xmlPtrAddress);
