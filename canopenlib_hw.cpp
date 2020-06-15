#include "windows.h"
#include "stdio.h"
#include "canopenlib_hw.h"

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <conio.h>
#include <shlwapi.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#ifdef _MANAGED
#pragma managed(pop)
#endif

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortLibraryInit(void)
{
	return CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortOpen(int port, canPortHandle* handle)
{
	return CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortClose(canPortHandle handle)
{
	return CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortBitrateSet(canPortHandle handle, int bitrate)
{
	return CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortEcho(canPortHandle handle, bool enabled)
{
	return CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortGoBusOn(canPortHandle handle)
{
	return CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortGoBusOff(canPortHandle handle)
{
	return CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortWrite(canPortHandle handle,
	long id,
	void* msg,
	unsigned int dlc,
	unsigned int flags)
{
	return CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus    __stdcall canPortRead(canPortHandle handle,
	long* id,
	void* msg,
	unsigned int* dlc,
	unsigned int* flags)
{
	return CANOPEN_ERROR;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

CANOPENLIB_HW_API   canOpenStatus  __stdcall canPortGetSerialNumber(canPortHandle handle,
	char* buffer, int bufferLen)
{
	return CANOPEN_ERROR;
}