// dllmain.cpp : Implementation of DllMain.

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "DotNetHooker_i.h"
#include "dllmain.h"

CDotNetHookerModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved);
}
