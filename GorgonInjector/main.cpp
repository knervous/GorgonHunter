/*
* evil_inj.cpp
* classic DLL injection example
* author: @cocomelonc
* https://cocomelonc.github.io/tutorial/2021/09/20/malware-injection-2.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <filesystem>
#include <tray.hpp>
#include <comdef.h>  // you will need this

char evilDLL[] = "C:\\evil.dll";
unsigned int evilLen = sizeof(evilDLL) + 1;


// find process ID by process name
int findMyProc(const char* procname) {

	HANDLE hSnapshot;
	PROCESSENTRY32 pe;
	int pid = 0;
	BOOL hResult;

	// snapshot of all processes in the system
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

	// initializing size: needed for using Process32First
	pe.dwSize = sizeof(PROCESSENTRY32);

	// info about first process encountered in a system snapshot
	hResult = Process32First(hSnapshot, &pe);

	// retrieve information about the processes
	// and exit if unsuccessful
	while (hResult) {
		_bstr_t b(pe.szExeFile);

		if (strcmp(procname, b) == 0) {
			pid = pe.th32ProcessID;
			break;
		}
		hResult = Process32Next(hSnapshot, &pe);
	}

	// closes an open handle (CreateToolhelp32Snapshot)
	CloseHandle(hSnapshot);
	return pid;
}

void doInject() {
	HANDLE ph; // process handle
	HANDLE rt; // remote thread
	LPVOID rb; // remote buffer

	// handle to kernel32 and pass it to GetProcAddress
	HMODULE hKernel32 = GetModuleHandle(L"Kernel32");
	VOID* lb = GetProcAddress(hKernel32, "LoadLibraryA");

	ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(findMyProc("WindowsPlayer.exe")));
	if (ph == 0)
		return;

	wchar_t exePath[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, exePath, MAX_PATH);

	_bstr_t b(exePath);

	std::string path(b);
	path.resize(path.size() - std::string("GorgonInjector.exe").length());
	auto dll = path + "GorgonHunter.dll";
	auto fullPath = dll.c_str();
	auto injectLength = strlen(fullPath) + 1;
	rb = VirtualAllocEx(ph, NULL, injectLength, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(ph, rb, fullPath, injectLength, NULL);

	// our process start new thread
	HANDLE hRemoteThread = CreateRemoteThread(ph, NULL, 0, (LPTHREAD_START_ROUTINE)lb, rb, 0, NULL);
	if (!hRemoteThread || hRemoteThread == INVALID_HANDLE_VALUE) {
		throw "Failed to load dll into target process";
	}
	WaitForSingleObject(hRemoteThread, INFINITE);
	CloseHandle(ph);
}

int main(int argc, char* argv[]) {
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	Tray::Tray tray("My Tray", "icon.ico");
	tray.addEntry(Tray::Button("Exit", [&] {
		tray.exit();
	}));

	tray.addEntry(Tray::Button("Inject", [&] {
		doInject();
	}));

	tray.run();

	return 0;
}
