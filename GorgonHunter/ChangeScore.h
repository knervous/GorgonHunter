#pragma once
#include "Mono.h"
#include "hook.h"
#include "imgui_hook.h"
#include <functional>
#include <string>

static class ChangeScore {
private:
	inline static int score = 5.0;
	inline static std::string hookName = std::string("ChangeScore");
	inline static void* PrepMatch3Status;
public:
	ChangeScore() {};

	static void Enable() {
		AddHook(hookName, [&]() {
			ImGui::SliderInt("Score", &score, 1.0, 50000.0);
			});
		PrepMatch3Status = Mono::instance().GetMethod("ClientCommandEncoder", "prepMatch3Status");
		if (PrepMatch3Status == 0)
			return;
		chatlog::ImguiChatLog.AddLog("Added PrepMatch3 hook");
		CreateHook(PrepMatch3Status);
		EnableHook(PrepMatch3Status);
	}

	static void Disable() {
		RemoveHook(hookName);
		DisableHook(PrepMatch3Status);
	}

	HOOK_DEF(Il2CppObject*, PrepMatch3Status, (int gameIID,
		bool finalUpdate,
		int movesRemaining,
		int movesMade,
		int realScore,
		Il2CppArray* pieceCounts,
		Il2CppArray* tiersMade,
		Il2CppString* moveCodes, DWORD* methodArg)) {

		return original_PrepMatch3Status(gameIID, finalUpdate, movesRemaining, movesMade, score, pieceCounts, tiersMade, moveCodes, methodArg);
	}
};



