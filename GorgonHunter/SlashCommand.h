#pragma once
#include "Mono.h"
#include "hook.h"
#include <string>

static class SlashCommand {
private:
	inline static std::string hookName = std::string("SlashCommand");
	inline static void* ProcessSlashCommand;
public:
	SlashCommand() {};

	static void Enable() {
		ProcessSlashCommand = Mono::instance().GetMethod("SlashCommandProcessor", "ProcessSlashCommand");
		if (ProcessSlashCommand == 0)
			return;
		CreateHook(ProcessSlashCommand);
		EnableHook(ProcessSlashCommand);
	}

	static void Disable() {
		DisableHook(ProcessSlashCommand);
	}

	HOOK_DEF(bool, ProcessSlashCommand, (DWORD* __this, DWORD* message, DWORD* methodArg)) {
		printf("Slash command\n");
		auto cast = (Il2CppObject*)message;
		auto cmd = ConvertString(message);
		printf(cmd.c_str());
		return original_ProcessSlashCommand(__this, message, methodArg);
	}
};



