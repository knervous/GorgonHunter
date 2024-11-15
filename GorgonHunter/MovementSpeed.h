#pragma once
#include "Mono.h"
#include "hook.h"
#include "imgui_hook.h"
#include <functional>
#include <string>

static class MovementSpeed {
private:
	inline static float score = 5.0f;
	inline static std::string hookName = std::string("MovementSpeed");
	inline static void* GetCurrentScore;
public:
	MovementSpeed() {};

	static void Enable() {
		AddHook(hookName, [&]() {
			ImGui::SliderFloat("Move Speed", &score, 1.0f, 30.0f);
		});
		GetCurrentScore = Mono::instance().GetMethod("Combatant", "get_MovementSpeed");
		if (GetCurrentScore == 0)
			return;
		CreateHook(GetCurrentScore);
		EnableHook(GetCurrentScore);
	}

	static void Disable() {
		RemoveHook(hookName);
		DisableHook(GetCurrentScore);
	}

	HOOK_DEF(float, GetCurrentScore, (DWORD* __this, DWORD* methodArg)) {
		return score;
	}
};



