#pragma once
#include <Windows.h>
#include "pch.h"
#include <cstdio>
#include <iostream>
#include <iomanip>
#include "features.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

void CreateConsole() {
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
}

void main() {
	MH_Initialize();
	EnableAllHooks();

	//CreateConsole();
	printf("Hello 2");
	InitImGui();
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	GetModuleHandleEx
	(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)DllMain,
		&hModule
	);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		auto thrd = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)main, 0, 0, 0);
		Unloader = [hModule, &thrd]() {
			//FreeLibraryAndExitThread(hModule, 0);
			::FreeLibrary(hModule);
			//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibraryAndExitThread, &__ImageBase, 0, NULL);
		};
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

