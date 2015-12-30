/*
 * Native Methods.cs
 * 
 * Provides access to the C++ editor methods through
 * the cobalt editor DLL
 */

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Cobalt_Editor
{
    static class NativeMethods
    {
        const string editorDllName = "Cobalt Editor DLL.dll";

        // Editor framework functions
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int EditorMain(IntPtr instancePtrAddress, IntPtr hPrevInstancePtrAddress,
            IntPtr hWndPtrAddress, int nCmdShow, int screenWidth, int screenHeight);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void WndProc(int* hWndPtrAddress, int msg, int wParam, int lParam);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void RenderFrame();
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int Shutdown(); 
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int IsGameRunning();
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void OpenLevel([MarshalAs(UnmanagedType.BStr)] string lFileName);


        // Accessors
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int GetNumGameObjects();
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void GetGameObjectList(IntPtr gameObjectIdArrayPtrAddress, int numGameObjects);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int GetGameObjectXmlSize(uint gameObjectId);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void GetGameObjectXml(IntPtr gameObjectXmlPtrAddress, uint gameObjectId);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int PickGameObject(IntPtr hWndPtrAddress);


        // Modification functions
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int CreateGameObject([MarshalAs(UnmanagedType.BStr)] string lGameObjectResource);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void ModifyGameObject([MarshalAs(UnmanagedType.BStr)] string lGameObjectModificationXML);
        [DllImport(editorDllName, CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void DestroyGameObject(uint gameObjectId);
    }
}
