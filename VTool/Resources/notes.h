#pragma once


/*


previous project before working on conker, compelte paste of vtool.cpp at that time
#define VLIB_WNDPROC

#include "../Resources/resource.h"

#include <vlib.hpp>
#include <d3d9.h>
#include <d3dx9.h>

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include <thread>

HHOOK g_hook2{};
HWND g_osrs_window_handle{};
std::jthread g_thread{};
std::vector<std::vector<POINT>> g_mouse_paths{};

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};


int DistanceBetweenPoints(const POINT& p1, const POINT& p2) {

	return std::hypot(p1.x - p2.x, p1.y - p2.y);
}

LRESULT CALLBACK MouseHook(int message_type, WPARAM w_param, LPARAM l_param) {

	if (g_osrs_window_handle == GetForegroundWindow()) {

		auto kbdStruct2 = *((MSLLHOOKSTRUCT*)l_param);

		if (ScreenToClient(g_osrs_window_handle, &kbdStruct2.pt)) {

			RECT rect{};
			if (GetClientRect(g_osrs_window_handle, &rect)) {

				if (kbdStruct2.pt.x >= 0 && kbdStruct2.pt.y >= 0 && kbdStruct2.pt.x <= rect.right && kbdStruct2.pt.y <= rect.bottom) {

					static bool record{ false };
					if (w_param == WM_LBUTTONUP) {

						record = !record;
					}

					static POINT old_position{ kbdStruct2.pt };
					static std::vector<POINT> points{};

					if (record) {

						if (DistanceBetweenPoints(old_position, kbdStruct2.pt) > 12) {

							points.push_back(kbdStruct2.pt);

							old_position = kbdStruct2.pt;
						}
					}
					else if(points.empty() == false) {

						g_mouse_paths.push_back(points);
						points.clear();
					}
				}
			}
		}
	}

	return CallNextHookEx(g_hook2, message_type, w_param, l_param);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {

	ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param);

	return -1;
}
bool CreateDeviceD3D(HWND hWnd)
{
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
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}
#include <cstdio>

typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

u8 default_rbm[384] = {
	0x43, 0x41, 0x46, 0x46, 0x32, 0x38, 0x2E, 0x30, 0x31, 0x2E, 0x30, 0x35, 0x2E, 0x30, 0x30, 0x33,
	0x31, 0x00, 0x00, 0x00, 0x50, 0x5D, 0xC9, 0x02, 0x7A, 0x01, 0x00, 0x00, 0xF2, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x02, 0x00, 0x00, 0x74, 0x05, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x78, 0x70, 0x02, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00,
	0x2E, 0x64, 0x61, 0x74, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x02, 0x00, 0x00, 0x00,
	0x04, 0xBF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xB4, 0x59, 0x00, 0x00, 0x2E, 0x67, 0x70, 0x75, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x80, 0xB2, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDD, 0x9A, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* likely an extern to something automatically generated */
const char* gCurrentRevision = "28.01.05.0031";

bool checkString(const u8* a, const u8* b, u32 length)
{
	for (u32 i(0); i < length; ++i, ++a, ++b) {
		/* check characters against each other */
		if (*a != *b) {
			return false;
		}
	}

	return true;
}

bool checkEndian(u8* fileBuffer)
{
	const u8 endian_flag = *(u8*)(fileBuffer + 60);

	/* marker for big endian */
	return (endian_flag & 1);
}

u32 doCaffChecksumByte(const u32 initialVal, const u8 currentByte)
{
	/* the byte buffer is actually signed */
	int val = (initialVal << 4) + static_cast<s8>(currentByte);

	int val_mask = val & 0xF0000000;
	if (val_mask) {
		/* copy the mask over 0xF00000F0 */
		val_mask |= ((u32)val_mask >> 24);
		val ^= val_mask;
	}

	return val;
}

u32 checkCaff(u8* fileBuffer, u32 fileHash)
{
	u32 result = 0;

	/* check file magic */
	if (!checkString(fileBuffer, reinterpret_cast<const u8*>("CAFF"), 4)) {
		result = 1;
	}

	/* check asset revision */
	else if (!checkString(fileBuffer + 4, reinterpret_cast<const u8*>(gCurrentRevision), 13)) {
		result = 2;
	}

	/* validate file hash */
	else
	{
		fileHash = *(u32*)(fileBuffer + 20);

		/* fix file hash if file endian is marked */
		if (checkEndian(fileBuffer)) {
			fileHash = ((fileHash >> 24) & 0x000000FF)
				| ((fileHash << 24) & 0xFF000000)
				| ((fileHash << 8) & 0x00FF0000)
				| ((fileHash >> 8) & 0x0000FF00);
		}

		/* clear hash from buffer */
		*(u32*)(fileBuffer + 20) = 0;

		u32 chksmLocal = 0;
		u8* chkcmBuffer = fileBuffer;

		/* runs over (64 * 6) bytes */
		const u32 run_count = 64 * 6;
		for (u32 run = 0; run < run_count; ++run, ++chkcmBuffer) {

			/* the xbe file runs though 6 bytes at a time */
			chksmLocal = doCaffChecksumByte(chksmLocal, *chkcmBuffer);
		}

		printf("Result   %08X\n", chksmLocal);
		printf("Expected %08X\n", fileHash);

		if (chksmLocal != fileHash) {
			result = 3;
		}
	}

	return result;
}

void getFileInfo(u8* data)
{
	u8 count = *(u8*)(data + 61);

#pragma pack(push,1)
	struct CAFF_SECTION_HEADER
	{
		char name[6];
		char pad[30];
		u32 size;
	};
#pragma pack(pop)

	u8* ptr = (data + 64);
	CAFF_SECTION_HEADER d;

	u32 total_size = 0;

	for (u8 i = 0; i < count; ++i, ptr += 40)
	{
		d = *(CAFF_SECTION_HEADER*)(ptr);
		printf("Found block named %s (size %u)\n", d.name, d.size);
		total_size += d.size;
	}

	u32 data_starts = *(u32*)(data + 56);
	printf("Data offset = %u\n", data_starts);
	printf("Data length = %u\n", total_size);
}


int APIENTRY WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE /*previous_instance*/, _In_ PSTR /*cmd_line*/, _In_ INT /*cmd_show*/) {

	auto process = vlib::VProcess{ { instance, "Vitalflea", "VTool", WS_OVERLAPPEDWINDOW | WS_VISIBLE, WS_EX_LAYERED, { CW_USEDEFAULT, CW_USEDEFAULT, 814, 675  }, IDI_ICON1 } };

	CreateDeviceD3D(process.GetWindowHandle());

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(process.GetWindowHandle());
	ImGui_ImplDX9_Init(g_pd3dDevice);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	g_osrs_window_handle = FindWindow(0, "Old School RuneScape");

	g_thread = std::jthread{ [&](std::stop_token token) {

	g_hook2 = SetWindowsHookEx(WH_MOUSE_LL, MouseHook, instance, 0);

		bool stop{ false };
		while (stop == false) {

			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			MSG msg{};
			while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {

				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT) {

					stop = true;
				}
			}
		}
	} };

	g_thread.detach();
	bool stop{ false };
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

		ImGui::SetNextWindowSize({ 814, 675 });
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::Begin("main window");

		std::ranges::for_each(g_mouse_paths, [&](const std::vector<POINT>& points) {

			std::ranges::for_each(points, [&](const POINT& point) {

				ImGui::GetWindowDrawList()->PathLineTo({ (float)point.x, (float)point.y });
				//ImGui::GetWindowDrawList()->AddRectFilled({ (float)point.x, (float)point.y }, { (float)point.x + 1, (float)point.y + 1 }, IM_COL32(255, 255, 0, 255));
				});

			ImGui::GetWindowDrawList()->PathStroke(IM_COL32(0, 255, 255, 255));
			});

		ImGui::End();

		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0) {

			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}

		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}

	return 0;
}

/*
namespace signatures {
	 Working as of 2/25/2022
constexpr auto g_client_object = { 0x48, 0x8B, 0x0D, 0x02, 0x74 };
constexpr auto g_login_state = { 0xC7, 0x05, 0x1E, 0xCA };
constexpr auto g_npc_list = { 0x48, 0x8D, 0x3D, 0x37, 0x37, 0x37, 0x37, 0xBB };
constexpr auto g_npc_indicies = { 0x4C, 0x8B, 0x35, 0x2D };

namespace functions {

	constexpr auto g_exp_drop_handler = { 0x48, 0x83, 0xEC, 0x28, 0x4C, 0x8B, 0xC1, 0x4C }; // +- 0
	constexpr auto g_do_action = { 0x48, 0x89, 0x7C, 0x24, 0x18, 0x55, 0x41, 0x54 }; // - 10
	constexpr auto g_print_chat = { 0x48, 0x89, 0x74, 0x24, 0x10, 0x57, 0x48, 0x83, 0xEC, 0x40, 0x49 }; // - 5
	constexpr auto g_world_to_screen = { 0x48, 0x89, 0x5C, 0x24, 0x10, 0x48, 0x89, 0x74, 0x24, 0x18, 0x57, 0x48, 0x83, 0xEC, 0x30, 0x41, 0x8B, 0xF9 }; // +- 0
}
}

enum class Skill {

	ATTACK = 0,
	DEFENCE = 1,
	STRENGTH = 2,
	HITPOINTS = 3,
	RANGED = 4,
	PRAYER = 5,
	MAGIC = 6,
	COOKING = 7,
	WOODCUTTING = 8,
	FLETCHING = 9,
	FISHING = 10,
	FIREMAKING = 11,
	CRAFTING = 12,
	SMITHING = 13,
	MINING = 14,
	HERBLORE = 15,
	AGIlITY = 16,
	THIEVING = 17,
	SLAYER = 18,
	FARMING = 19,
	RUNECRAFTING = 20,
	HUNTER = 21,
	CONSTRUCTION = 22
};

enum class GameState : int {

	UNKNOWN = -1,
	LOGIN = 10,
	AUTHENTICATOR = 11,
	LOGGING_IN = 20,
	LOADING = 25,
	LOGGED_IN = 30,
	CONNECTION_LOST = 40,
	WORLD_CHANGE = 45
};

class ClientObject {
public:
	PAD(0x1FA0); //0x00
	GameState m_game_state; //0x1FA0
	PAD(0xB42C); //0x1FA4
	std::uintptr_t m_last_action; //0xD3D0
};
*/


*/