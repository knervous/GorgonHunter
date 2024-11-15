#pragma once
#include <string>
#include <MinHook.h>

inline uintptr_t GameAssemblyOffset = (uintptr_t)GetModuleHandle(L"GameAssembly.dll");

#define CreateHook(a) \
	checkStatus(#a, MH_CreateHook((LPVOID*)(GameAssemblyOffset + (uintptr_t)a), &h ## a, reinterpret_cast<LPVOID*>(&original_ ## a)));

#define EnableHook(a) \
	checkStatus(#a, MH_EnableHook((LPVOID*)(GameAssemblyOffset + (uintptr_t)a)));

#define DisableHook(a) \
	checkStatus(#a, MH_DisableHook((LPVOID*)(GameAssemblyOffset + (uintptr_t)a)));

#define HOOK_DEF(a, b, c) typedef a (*t ## b) c; static inline t ## b original_ ##b = NULL; static a h ## b ## c

inline bool checkStatus(std::string name, MH_STATUS status) {
	if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED && status != MH_ERROR_ALREADY_INITIALIZED) {
		printf("Hooked error");
		return false;
	}

	return true;
}