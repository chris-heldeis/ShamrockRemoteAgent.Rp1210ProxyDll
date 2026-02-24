#pragma once
#include "Platform.h"

extern "C" {
	__declspec(dllexport) short __stdcall RP1210_ClientConnect(HWND, short, char*, long, long, short);
	__declspec(dllexport) short __stdcall RP1210_ClientDisconnect(short);
	__declspec(dllexport) short __stdcall RP1210_SendMessage(short, char*, short, short, short);
	__declspec(dllexport) short __stdcall RP1210_ReadMessage(short, char*, short, short);
	__declspec(dllexport) short __stdcall RP1210_SendCommand(short, short, char*, short);
	__declspec(dllexport) void __stdcall RP1210_ReadVersion(char*, char*, char*, char*);
	__declspec(dllexport) short __stdcall RP1210_GetErrorMsg(short, char*);
	__declspec(dllexport) short __stdcall RP1210_GetHardwareStatus(short, char*, short, short);
	__declspec(dllexport) short __stdcall RP1210_GetLastErrorMsg(short, int*, char*, short);
	__declspec(dllexport) short __stdcall RP1210_ReadDetailedVersion(short, char*, char*, char*);
	__declspec(dllexport) short __stdcall RP1210_Ioctl(short, long, void*, void*);
}