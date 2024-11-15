#pragma once
#include "pch.h"
#include "SendCommand.h"
#include "Mono.h"
#include "hook.h"
#include "imgui_hook.h"
#include "kiero/imgui_stdlib.h"
#include <string>
#include <algorithm> 
#include <iomanip>
#include <iostream>
#include <sstream>


void SendCommand::Enable() {
	ProcessSendCommand = Mono::instance().GetMethod("GorgonClient", "SendCmd", 1, "", "GorgonCore");
	if (ProcessSendCommand == 0)
		return;
	CreateHook(ProcessSendCommand);
	EnableHook(ProcessSendCommand);
	AddTabHook("Packet Sender", []() {
		ImGui::InputText("Packet", &packetText);
		if (ImGui::Button("Send")) {
			SendPacket(packetText);
		}
	});
}

void SendCommand::SendPacket(const std::string& hexPacket) {
	std::istringstream hex_chars_stream(hexPacket);
	std::vector<unsigned char> bytes;
	unsigned int c;
	while (hex_chars_stream >> std::hex >> c)
	{
		bytes.push_back(c);
	}
	auto clientInstance = Mono::instance().runtime_invoke(
		Mono::instance().GetIl2CppMethod("GorgonClient", "get_Instance", -1, "", "GorgonCore"), NULL, NULL, NULL);

	auto newCmd = Mono::instance().runtime_invoke(
		Mono::instance().GetIl2CppMethod("GorgonClient", "NewCmdBuffer", -1, "", "GorgonCore"), (DWORD*)clientInstance, NULL, NULL);
	auto stream = Mono::instance().GetField(newCmd, "stream");

	auto byteArrayMethod = Mono::instance().GetIl2CppMethod(stream->klass, "WriteByte");

	for (auto& b : bytes) {
		void* write_args[1];
		uint8_t by = (uint8_t)b;
		write_args[0] = &by;
		Mono::instance().runtime_invoke(byteArrayMethod, (DWORD*)stream, write_args, NULL);
	}


	auto sendCmd = Mono::instance().GetIl2CppMethod("GorgonClient", "SendCmd", 1, "", "GorgonCore");
	void* args[1];
	args[0] = newCmd;

	auto thread = Mono::instance().mono_thread_attach(Mono::instance().mono_get_root_domain());
	Mono::instance().runtime_invoke(sendCmd, (DWORD*)clientInstance, args, NULL);
	Mono::instance().mono_thread_detach(thread);
}

void SendCommand::Disable() {
	DisableHook(ProcessSendCommand);
	RemoveTabHook("Packet Sender");
}

void SendCommand::NetworkLog(Il2CppObject* cmd, unsigned short opcode, Il2CppArray* bytesArray, unsigned char* bytes) {
	auto mono_method = Mono::instance().GetIl2CppMethod("GorgonClient/Cmd", "DebugDump", -1, "", "GorgonCore");
	auto res = Mono::instance().runtime_invoke(mono_method, (DWORD*)cmd, NULL, NULL);
	auto msg = ConvertString((DWORD*)res);
	std::stringstream hexStream;
	for (int i = 0; i < ((Il2CppArray*)bytesArray)->max_length; i++) {
		hexStream << std::setw(2) << std::setfill('0') << std::hex << (unsigned)bytes[i] << " ";
	}
	std::string hexString = hexStream.str();
	transform(hexString.begin(), hexString.end(), hexString.begin(), ::toupper);
	chatlog::ImguiChatLog.AddLog(Network::GetNetworkCommand((Network::Cmd)opcode), hexString.c_str());
	chatlog::ImguiChatLog.AddLog(msg.c_str());
}