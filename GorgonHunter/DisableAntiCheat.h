#pragma once
#include "Mono.h"
#include "hook.h"
#include <string>

static class DisableAntiCheat {
private:
	inline static std::string hookName = std::string("DisableAntiCheat");
	inline static void* AntiCheatUpdate;
	inline static void* CrashReporter;
public:
	DisableAntiCheat() {};

	static void Enable() {
		AntiCheatUpdate = Mono::instance().GetMethod("CheatReporter", "Update");
		if (AntiCheatUpdate == 0)
			return;
		CreateHook(AntiCheatUpdate);
		EnableHook(AntiCheatUpdate);
		chatlog::ImguiChatLog.AddLog("Added Cheat Report Hook");

		CrashReporter = Mono::instance().GetMethod("CrashReporting", "Log", -1, "UnityEngine.CrashLog", "UnityEngine.CrashLog");
		if (CrashReporter == 0)
			return;
		chatlog::ImguiChatLog.AddLog("Added Crash Handler Hook");
		CreateHook(CrashReporter);
		EnableHook(CrashReporter);


	}

	static void Disable() {
		DisableHook(AntiCheatUpdate);
		DisableHook(CrashReporter);
	}

	HOOK_DEF(void, AntiCheatUpdate, (DWORD* __this, DWORD* methodArg)) {}
	HOOK_DEF(void, CrashReporter, (DWORD* __this, Il2CppString* message, Il2CppString* stackTrace, DWORD* type, DWORD* methodArg)) {
		std::string c_msg = il2cppi_to_string(message);
		std::string c_stackTrace = il2cppi_to_string(stackTrace);
		std::string crashMessage("Crash handler intercepted.\n");
		crashMessage += c_msg + "\n";
		crashMessage += c_stackTrace + "\n";
		chatlog::ImguiChatLog.AddLog(crashMessage.c_str());
	}
};



