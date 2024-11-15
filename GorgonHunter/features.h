#pragma once
#include "MovementSpeed.h"
#include "SlashCommand.h"
#include "DisableAntiCheat.h"
#include "SendCommand.h"
#include "ChangeScore.h"

extern std::function<void()> Unloader;

inline void DisableAllHooks() {
	MovementSpeed::Disable();
	SlashCommand::Disable();
	DisableAntiCheat::Disable();
	SendCommand::Disable();
	ChangeScore::Disable();
}

inline void EnableAllHooks() {
	MovementSpeed::Enable();
	SlashCommand::Enable();
	DisableAntiCheat::Enable();
	SendCommand::Enable();
	ChangeScore::Enable();
}