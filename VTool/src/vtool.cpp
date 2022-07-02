#define VLIB_WNDPROC

#include "../Resources/resource.h"

#include <vlib.hpp>
#include <d3d9.h>
#include <d3dx9.h>

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h> 

#include <thread>
#include <filesystem>
#include <fstream>
#include <iostream>


HWND g_osrs_window_handle{};
std::jthread g_thread{};

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

namespace offsets {

	constexpr auto tediz_bullet_color{ 0x5E7494 };
	constexpr auto shc_bullet_color{ 0x5E7498 };
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) { 
	
	ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param);

	return -1;
}

bool CreateDeviceD3D(HWND hwnd) {

	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

const std::vector<std::string> GetCommandLineArguements(const char* cmd_line) {

	std::vector<std::string> arguements{};

	auto command_line = std::string{ cmd_line };
	if (command_line.size() > 0) {

		while (true) {

			auto pos = command_line.find(" ");
			if (pos != std::string::npos) {

				arguements.emplace_back(command_line.substr(0, pos));
				command_line.erase(0, pos + 1);
			}
			else {

				arguements.emplace_back(command_line);
				break;
			}
		}
	}
	return arguements;
}

std::uintptr_t SigScan(const std::uintptr_t& start, const std::uintptr_t& end, const std::vector<std::uint8_t>& sig_bytes) {

	std::uintptr_t sig_position{ 0 };

	for (std::uintptr_t i = start; i < end; i++) {

		if (sig_position == sig_bytes.size()) {

			return start + i - start - sig_bytes.size();
		}

		*reinterpret_cast<std::uint8_t*>(i) == sig_bytes.at(sig_position) || sig_bytes.at(sig_position) == 0x37 ? sig_position++ : sig_position = 0;
	}

	return 0;
}

std::uintptr_t SigScan(char* startz, int size, const std::vector<std::uint8_t>& sig_bytes) {

	std::uintptr_t sig{ 0 };

	
		auto start = reinterpret_cast<std::uintptr_t>(startz);
		auto end = start + size;

		sig = SigScan(start, end, sig_bytes);
	

#ifdef _DEBUG
	if (sig == 0) {
		std::ranges::for_each(sig_bytes, [&](std::uint8_t byte) { std::cout << std::hex << (int)byte << ' '; });
		system("PAUSE");
	}

	std::cout << '\n';
#endif
	return sig;
}


const char* g_bullet_colors[]{
	"Default"
	"Red",
	"Blue",
	"Purple",
	"Yellow"
};

int APIENTRY WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE /*previous_instance*/, _In_ PSTR cmd_line, _In_ INT /*cmd_show*/) {

	//g_thread = std::jthread{ [&](std::stop_token token) {

	//	while (true) {

	//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	//	}
	//}};
	//g_thread.detach();

	AllocConsole();

	FILE* output{ nullptr };
	freopen_s(&output, "CONOUT$", "w", stdout);
	freopen_s(&output, "CONIN$", "r", stdin);

	std::string path_ = cmd_line;
	path_.erase(std::remove(path_.begin(), path_.end(), '"'), path_.end());
	//auto command_line_args = GetCommandLineArguements(cmd_line);
	//for (auto cmd : command_line_args) {

		std::cout << cmd_line << '\n';
	//}
	//MessageBox(nullptr, command_line_args.at(0).c_str(), "File doesn't exist!", MB_OK);
	
	if (std::filesystem::exists(path_) == false) {

		MessageBox(nullptr, "Please drag and drop your default.xbe onto the vtool.exe", "File doesn't exist!", MB_OK);
		return 1;
	}

	auto xbe_path = std::filesystem::path(path_);

	if (xbe_path.has_extension() == true && xbe_path.extension().string().compare(".xbe") != 0) {

		MessageBox(nullptr, "Please drag and drop your default.xbe onto the vtool.exe", "Not a valid xbe!", MB_OK);
		return 2;
	}

	HANDLE file = CreateFile(xbe_path.string().c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == nullptr) {

		MessageBox(nullptr, "error", "error", MB_OK);
		return 3;
	}

	constexpr auto x = 614;
	constexpr auto y = 375;

	auto process = vlib::VProcess{ { instance, "Vitalflea", "VTool", WS_OVERLAPPEDWINDOW | WS_VISIBLE, WS_EX_LAYERED, { CW_USEDEFAULT, CW_USEDEFAULT, x, y }, IDI_ICON1 } };

	CreateDeviceD3D(process.GetWindowHandle());

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();


	style.WindowPadding = ImVec2(8.00f, 8.00f);
	style.FramePadding = ImVec2(5.00f, 2.00f);
	style.CellPadding = ImVec2(6.00f, 6.00f);
	style.ItemSpacing = ImVec2(6.00f, 6.00f);
	style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
	style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
	style.IndentSpacing = 25;
	style.ScrollbarSize = 15;
	style.GrabMinSize = 10;
	style.WindowBorderSize = 1;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = 1;
	style.TabBorderSize = 1;
	//style.WindowRounding = 7;
	style.ChildRounding = 4;
	style.FrameRounding = 3;
	style.PopupRounding = 4;
	style.ScrollbarRounding = 9;
	style.GrabRounding = 3;
	style.LogSliderDeadzone = 4;
	style.TabRounding = 4;

	ImVec4* colors = style.Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

	ImGui_ImplWin32_Init(process.GetWindowHandle());
	ImGui_ImplDX9_Init(g_pd3dDevice);

	ImColor clear_color = ImColor(0.45f, 0.55f, 0.60f, 1.00f);

	bool xbe_backup{ false };
	bool swap_berri{ false };
	bool swap_eras{ false };
	bool unlock_avatars{ false };
	bool widescreen{ false };
	bool unlock_fps{ false };
	bool purple_bullets{ false };
	bool stop{ false };

	int tediz_bullet_color = 0;
	int shc_bullet_color = 0;

	while (stop == false) {

		MSG msg{};
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {

				stop = true;
			}
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowSize({ x, y });
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::Begin("main window", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

		ImGui::Text("%s", xbe_path.string().c_str());
		ImGui::SameLine();
		if (ImGui::Button("Apply Patches")) {

			int new_offset{ 0 };
			if (purple_bullets) {
				if (SetFilePointer(file, offsets::tediz_bullet_color, 0, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
				{
					DWORD bytes_written{ 0 };
					constexpr BYTE patch[] = { 0x50, 0x55, 0x52, 0x50, 0x4C, 0x45, 0x00, 0x50, 0x55, 0x52, 0x50, 0x4C, 0x45, 0x00, 0x59, 0x45, 0x4C, 0x4C, 0x4F, 0x57, 0x00, 0x50, 0x55, 0x52, 0x50, 0x4C, 0x45 };
					WriteFile(file, patch, sizeof(patch), &bytes_written, NULL);
				}
			}

			CloseHandle(file);
			MessageBox(nullptr, "Patches applied.", "Success", MB_OK);
			return 0;
		}
		ImGui::Separator();

		ImGui::Checkbox("Backup xbe", &xbe_backup);
		ImGui::Checkbox("Swap BerriBarmaid with Berri", &swap_berri);
		ImGui::Checkbox("Swap eras", &swap_berri);
		ImGui::Checkbox("Unlock all avatars", &unlock_avatars);
		ImGui::Checkbox("Purple bullets", &purple_bullets);

		ImGui::Checkbox("Widescreen ratio", &widescreen);
		ImGui::Checkbox("60 FPS limit", &unlock_fps);
		
		ImGui::Separator();
		ImGui::End();

		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		//D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_color, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0) {

			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}

		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}

	return 0;
}