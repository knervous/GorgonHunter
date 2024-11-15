#pragma once
#include <Windows.h>
#include "pch.h"
#include "imgui_hook.h"
#include "features.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace chatlog {
	ImguiWindowLog ImguiChatLog = ImguiWindowLog();
}

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
std::unordered_map<std::string, std::function<void()>> imgui_hooks;
std::unordered_map<std::string, std::function<void()>> imgui_tab_hooks;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return true;
	}
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}


bool didInit = false;
bool didShutdown = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (didShutdown) {
		return oPresent(pSwapChain, SyncInterval, Flags);
	}
	if (!didInit)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(pDevice, pContext);
			didInit = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("GorgonHunter");

	if (ImGui::BeginTabBar("GorgonHunter")) {
		if (ImGui::BeginTabItem("Active Config")) {
			for (auto& it : imgui_hooks) {
				it.second();
			}
			if (ImGui::Button("Unload")) {
				didShutdown = true;
				DisableAllHooks();
				ImGui_ImplWin32_Shutdown();
				ImGui_ImplDX11_Shutdown();
				MH_Uninitialize();
				SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
				kiero::unbind(8);
				kiero::shutdown();
				Unloader();
				return true;
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Chat Log")) {
			chatlog::ImguiChatLog.Draw();
			ImGui::EndTabItem();
		}

		for (auto& it : imgui_tab_hooks) {
			if (ImGui::BeginTabItem(it.first.c_str())) {
				it.second();
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

void AddHook(std::string name, std::function<void()> callback) {
	imgui_hooks[name] = callback;
}

void RemoveHook(std::string name) {
	imgui_hooks.erase(name);
}

void AddTabHook(std::string name, std::function<void()> callback) {
	imgui_tab_hooks[name] = callback;
}

void RemoveTabHook(std::string name) {
	imgui_tab_hooks.erase(name);
}



void InitImGui() {
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);

			init_hook = true;
		}
	} while (!init_hook);
}