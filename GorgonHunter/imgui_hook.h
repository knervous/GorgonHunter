#pragma once
#include "SendCommand.h"

#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <functional>
#include <string>
#include <deque>

#include "kiero/includes.h"

void AddHook(std::string name, std::function<void()> callback);
void RemoveHook(std::string name);

void AddTabHook(std::string name, std::function<void()> callback);
void RemoveTabHook(std::string name);

namespace chatlog {
	using MutexType = std::shared_timed_mutex;
	using ReadLock = std::shared_lock<MutexType>;
	using WriteLock = std::unique_lock<MutexType>;
	struct ChatEntry {
		const char* message;
		const char* copyData;
	};
	struct ImguiWindowLog
	{
		mutable MutexType _mutex;

		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		bool                AutoScroll;  // Keep scrolling if already at the bottom.
		std::deque<ChatEntry> chatEntries;

		ImguiWindowLog()
		{
			AutoScroll = true;
			Clear();
			chatEntries = std::deque<ChatEntry>();
		}

		void Clear()
		{
			chatEntries.clear();
		}

		void AddLog(const char* fmt, const char* copyData = "")
		{
			//WriteLock lhs_lk(_mutex, std::defer_lock);
			//ReadLock  rhs_lk(_mutex, std::defer_lock);
			//std::lock(lhs_lk, rhs_lk);

			ChatEntry chatEntry(_strdup(fmt), _strdup(copyData));
			chatEntries.push_back(chatEntry);
		}

		void Draw()
		{
			//WriteLock lhs_lk(_mutex, std::defer_lock);
			//ReadLock  rhs_lk(_mutex, std::defer_lock);
			//std::lock(lhs_lk, rhs_lk);

			// Options menu
			if (ImGui::BeginPopup("Options"))
			{
				ImGui::Checkbox("Auto-scroll", &AutoScroll);
				ImGui::EndPopup();
			}

			// Main window
			if (ImGui::Button("Options"))
				ImGui::OpenPopup("Options");
			ImGui::SameLine();
			bool clear = ImGui::Button("Clear");
			ImGui::SameLine();
			bool copy = ImGui::Button("Copy");
			ImGui::SameLine();
			Filter.Draw("Filter", -100.0f);

			ImGui::Separator();

			if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
			{
				if (clear)
					Clear();
				if (copy)
					ImGui::LogToClipboard();

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
				int counter = 0;
				for (const auto& entry : chatEntries) {
					if (strlen(entry.copyData) > 0) {
						std::string entryMessage(entry.message);
						entryMessage += "##" + std::to_string(counter);
						if (ImGui::Button(entry.message)) {
							const size_t len = strlen(entry.copyData) + 1;
							HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
							memcpy(GlobalLock(hMem), entry.copyData, len);
							GlobalUnlock(hMem);
							OpenClipboard(0);
							EmptyClipboard();
							SetClipboardData(CF_TEXT, hMem);
							CloseClipboard();
						}

						ImGui::SameLine();

						std::string replay("Replay##");
						replay += std::to_string(counter);

						if (ImGui::Button(replay.c_str())) {
							SendCommand::SendPacket(entry.copyData);
						}

					}
					else {
						ImGui::Text(entry.message);
					}
					counter++;
				}

				ImGui::PopStyleVar();

				// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
				// Using a scrollbar or mouse-wheel will take away from the bottom edge.
				if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
					ImGui::SetScrollHereY(1.0f);
			}
			ImGui::EndChild();
		}

	};
	extern ImguiWindowLog ImguiChatLog;
}


void InitImGui();