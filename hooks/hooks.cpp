// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.hpp"

#include <tchar.h>
#include <iostream>
#include <d3d9.h>
#include <dinput.h>

#include "..\cheats\misc\logs.h"
#include "..\cheats\misc\misc.h"
#include "..\cheats\visuals\other_esp.h"
#include "..\cheats\visuals\radar.h"
#include "../ImGui/imgui_freetype.h"
#include "../cheats/shonax_font.hpp"
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment(lib, "freetype.lib")

#include <shlobj.h>
#include <shlwapi.h>
#include <thread>
#include "..\cheats\menu.h"
#include "../Bytesa.h"
#include "../cheats/tabs.h"
#include "../cheats/bindsfont.h"
#include "../cheats/WeaponFonts.h"
auto _visible = true;
static auto d3d_init = false;

namespace INIT
{
	HMODULE Dll;
	HWND Window;
	WNDPROC OldWindow;
}

//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//extern IMGUI_IMPL_API LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace hooks
{
	int rage_weapon = 0;
	int legit_weapon = 0;
	bool menu_open = false;
	bool input_shouldListen = false;

	ButtonCode_t* input_receivedKeyval;
	
	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//static auto is_down = true;
		//static auto is_clicked = false;
		//
		//if (GetAsyncKeyState(VK_INSERT))
		//{
		//	is_clicked = false;
		//	is_down = true;
		//}
		//else if (!GetAsyncKeyState(VK_INSERT) && is_down)
		//{
		//	is_clicked = true;
		//	is_down = false;
		//}
		//else
		//{
		//	is_clicked = false;
		//	is_down = false;
		//}
		//
		//if (is_clicked)
		//{
		//	menu_open = !menu_open;
		//
		//	if (menu_open && g_ctx.available())
		//	{
		//		if (g_ctx.globals.current_weapon != -1)
		//		{
		//			if (g_cfg.ragebot.enable)
		//				rage_weapon = g_ctx.globals.current_weapon;
		//			else if (g_cfg.legitbot.enabled)
		//				legit_weapon = g_ctx.globals.current_weapon;
		//		}
		//	}
		//}
		//
		//auto pressed_buttons = false;
		//auto pressed_menu_key = uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_MOUSEWHEEL;
		//
		//if (g_ctx.local()->is_alive() && !pressed_menu_key && !g_ctx.globals.focused_on_input)
		//	pressed_buttons = true;
		//
		//if (!pressed_buttons && d3d_init && menu_open && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)/* && !input_shouldListen*/)
		//	return true;
		//
		//if (menu_open && (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE)/* && !input_shouldListen*/)
		//	return false;
		//
		//return CallWindowProc(INIT::OldWindow, hWnd, uMsg, wParam, lParam);


		if (uMsg == WM_KEYDOWN)
			if (wParam == VK_INSERT)
			{
				menu_open = !menu_open;
				
				if (menu_open && g_ctx.available())
				{
					if (g_ctx.globals.current_weapon != -1)
					{
						if (g_cfg.ragebot.enable)
							rage_weapon = g_ctx.globals.current_weapon;
						else if (g_cfg.legitbot.enabled)
							legit_weapon = g_ctx.globals.current_weapon;
					}
				}
			}

		//c_keyhandler::get()->wnd_handler(uMsg, wParam);
		if (menu_open)
		{
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

			if (wParam != 'W' && wParam != 'A' && wParam != 'S' && wParam != 'D' && wParam != VK_SHIFT && wParam != VK_CONTROL && wParam != VK_TAB && wParam != VK_SPACE || ui::GetIO().WantTextInput)
				return true;
		}
		return CallWindowProcA(INIT::OldWindow, hWnd, uMsg, wParam, lParam);

	}

	long __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		static auto original_fn = directx_hook->get_func_address <EndSceneFn> (42);
		return original_fn(pDevice);
	}

	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		ui::CreateContext();

		ImGui_ImplWin32_Init(INIT::Window);
		ImGui_ImplDX9_Init(pDevice);

		auto& io = ui::GetIO();
		auto& style = ui::GetStyle();

		style.WindowMinSize = ImVec2(10, 10);

		ImFontConfig m_config;
		m_config.OversampleH = m_config.OversampleV = 3;
		m_config.PixelSnapH = false;

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF,
			0x0400, 0x044F,
			0
		};

		char windows_directory[MAX_PATH];
		GetWindowsDirectory(windows_directory, MAX_PATH);

		auto verdana_directory = (std::string)windows_directory + "\\Fonts\\Verdana.ttf";

		// Menu fonts
		c_menu::get().futura_small = io.Fonts->AddFontFromMemoryTTF((void*)futuramediumc, sizeof(futuramediumc), 15.f, &m_config, ranges);
		c_menu::get().futura = io.Fonts->AddFontFromMemoryTTF((void*)futuramediumc, sizeof(futuramediumc), 17.f, &m_config, ranges);
		c_menu::get().futura_large = io.Fonts->AddFontFromMemoryTTF((void*)futuramediumc, sizeof(futuramediumc), 25.f, &m_config, ranges);
		c_menu::get().gotham = io.Fonts->AddFontFromMemoryTTF((void*)GothamPro, sizeof(GothamPro), 13.f, &m_config, ranges);

		// Icon fonts
		c_menu::get().ico_menu = io.Fonts->AddFontFromMemoryTTF((void*)icomenu, sizeof(icomenu), 20.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());
		c_menu::get().ico_bottom = io.Fonts->AddFontFromMemoryTTF((void*)iconbot, sizeof(iconbot), 20.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());
		c_menu::get().tabs = io.Fonts->AddFontFromMemoryTTF(Evolve, sizeof(Evolve), 32.0f, nullptr, ranges);

		skeet_menu.weapon_font = io.Fonts->AddFontFromMemoryTTF((void*)WeaponzFont, sizeof(WeaponzFont), 17.f, &m_config, ranges);
		skeet_menu.tabs = io.Fonts->AddFontFromMemoryTTF(skeet, 15400, 50.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());
		skeet_menu.f1 = io.Fonts->AddFontFromMemoryTTF(new_shonax_font, 67240, 15.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());
		skeet_menu.f2 = io.Fonts->AddFontFromMemoryTTF(new_shonax_font, 67240, 11.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());
		skeet_menu.f3 = io.Fonts->AddFontFromMemoryTTF(new_shonax_font, 67240, 8.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());

		skeet_menu.icons = io.Fonts->AddFontFromMemoryTTF(WeaponzFont, sizeof(WeaponzFont), 15.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());


		ImGui_ImplDX9_CreateDeviceObjects();
		d3d_init = true;
	}

	long __stdcall hooked_present(IDirect3DDevice9* device, RECT* src_rect, RECT* dest_rect, HWND dest_wnd_override, RGNDATA* dirty_region)
	{
		static auto original = directx_hook->get_func_address <PresentFn> (17);
		g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

		if (!d3d_init)
			GUI_Init(device);

		IDirect3DVertexDeclaration9* vertex_dec;
		device->GetVertexDeclaration(&vertex_dec);

		IDirect3DVertexShader9* vertex_shader;
		device->GetVertexShader(&vertex_shader);

		c_menu::get().device = device;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ui::NewFrame();

		g_cfg.player.enable = true;//why not ?

		if (g_ctx.menu_type == 1)
			c_menu::get().draw(menu_open);
		else
		{
			if (g_ctx.menu_type == 0)
			{
				skeet_menu.set_menu_opened(menu_open); //p shit
				skeet_menu.draw();
			}
			if (menu_open && g_ctx.menu_type == 2)
				otmenuv3.render();
		}




		otheresp::get().spread_crosshair(device);

		if (g_ctx.globals.should_update_radar)
			Radar::get().OnMapLoad(m_engine()->GetLevelNameShort(), device);
		else
			Radar::get().Render();

		ui::EndFrame();
		ui::Render();

		ImGui_ImplDX9_RenderDrawData(ui::GetDrawData());

		device->SetVertexShader(vertex_shader);
		device->SetVertexDeclaration(vertex_dec);

		return original(device, src_rect, dest_rect, dest_wnd_override, dirty_region);
	}

	long __stdcall Hooked_EndScene_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto ofunc = directx_hook->get_func_address<EndSceneResetFn>(16);

		if (!d3d_init)
			return ofunc(pDevice, pPresentationParameters);

		ImGui_ImplDX9_InvalidateDeviceObjects();

		auto hr = ofunc(pDevice, pPresentationParameters);

		if (SUCCEEDED(hr))
			ImGui_ImplDX9_CreateDeviceObjects();

		return hr;
	}

	DWORD original_getforeignfallbackfontname;
	DWORD original_setupbones;
	DWORD original_doextrabonesprocessing;
	DWORD original_standardblendingrules;
	DWORD original_updateclientsideanimation;
	DWORD original_physicssimulate;
	DWORD original_modifyeyeposition;
	DWORD original_calcviewmodelbob;
	DWORD original_processinterpolatedlist;

	vmthook* directx_hook;
	vmthook* client_hook;
	vmthook* clientstate_hook;
	vmthook* engine_hook;
	vmthook* clientmode_hook;
	vmthook* inputinternal_hook;
	vmthook* renderview_hook;
	vmthook* panel_hook;
	vmthook* modelcache_hook;
	vmthook* materialsys_hook;
	vmthook* modelrender_hook;
	vmthook* surface_hook;
	vmthook* bspquery_hook;
	vmthook* prediction_hook;
	vmthook* trace_hook;
	vmthook* filesystem_hook;

	C_HookedEvents hooked_events;
}

void __fastcall hooks::hooked_setkeycodestate(void* thisptr, void* edx, ButtonCode_t code, bool bDown)
{
	static auto original_fn = inputinternal_hook->get_func_address <SetKeyCodeState_t> (91);

	if (input_shouldListen && bDown)
	{
		input_shouldListen = false;

		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return original_fn(thisptr, code, bDown);
}

void __fastcall hooks::hooked_setmousecodestate(void* thisptr, void* edx, ButtonCode_t code, MouseCodeState_t state)
{
	static auto original_fn = inputinternal_hook->get_func_address <SetMouseCodeState_t> (92);

	if (input_shouldListen && state == BUTTON_PRESSED)
	{
		input_shouldListen = false;

		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return original_fn(thisptr, code, state);
}


IMaterial* zblockCreateMaterial(bool lit, const std::string& material_data)
{
	static auto created = 0;
	std::string type = lit ? crypt_str("VertexLitGeneric") : crypt_str("UnlitGeneric");

	auto matname = crypt_str("SHONAX-PROJECT_") + std::to_string(created);
	++created;

	auto keyValues = new KeyValues(matname.c_str());
	static auto key_values_address = util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 51 33 C0 C7 45"));

	using KeyValuesFn = void(__thiscall*)(void*, const char*);
	reinterpret_cast <KeyValuesFn> (key_values_address)(keyValues, type.c_str());

	static auto load_from_buffer_address = util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"));
	using LoadFromBufferFn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*);

	reinterpret_cast <LoadFromBufferFn> (load_from_buffer_address)(keyValues, matname.c_str(), material_data.c_str(), nullptr, nullptr, nullptr);

	auto material = m_materialsystem()->CreateMaterial(matname.c_str(), keyValues);
	material->IncrementReferenceCount();

	return material;
}



void hit_cham::draw_hit_matrix1()
{
	if (!g_ctx.available())
		m_Hitmatrix.clear();

	if (m_Hitmatrix.empty())
		return;

	if (!m_modelrender())
		return;

	auto ctx = m_materialsystem()->GetRenderContext();

	if (!ctx)
		return;

	auto it = m_Hitmatrix.begin();
	while (it != m_Hitmatrix.end()) {
		if (!it->state.m_pModelToWorld || !it->state.m_pRenderable || !it->state.m_pStudioHdr || !it->state.m_pStudioHWData ||
			!it->info.pRenderable || !it->info.pModelToWorld || !it->info.pModel) {
			++it;
			continue;
		}

		auto alpha = 1.0f;
		auto delta = m_globals()->m_realtime - it->time;
		if (delta > 0.0f) {
			alpha -= delta;
			if (delta > 1.0f) {
				it = m_Hitmatrix.erase(it);
				continue;
			}
		}

		static IMaterial* materials[] =
		{
			zblockCreateMaterial(true, crypt_str(R"#("VertexLitGeneric"
			{
				"$basetexture"				"vgui/white"
				"$ignorez"					"0"
				"$envmap"					" "
				"$nofog"					"1"
				"$model"					"1"
				"$nocull"					"0"
				"$selfillum"				"1"
				"$halflambert"				"1"
				"$znearer"					"0"
				"$flat"						"0"
				"$wireframe"				"0"
			}
		)#")),
		zblockCreateMaterial(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/white" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
		zblockCreateMaterial(false, crypt_str(R"#("UnlitGeneric"
			{
				"$basetexture"				"vgui/white"
				"$ignorez"					"0"
				"$envmap"					" "
				"$nofog"					"1"
				"$model"					"1"
				"$nocull"					"0"
				"$selfillum"				"1"
				"$halflambert"				"1"
				"$znearer"					"0"
				"$flat"						"1"
				"$wireframe"				"0"
			}
		)#")),
		m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/dogtags/dogtags_outline"), nullptr), //-V807
		m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/trophy_majors/crystal_clear"), nullptr),
		m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/cologne_prediction/cologne_prediction_glass"), nullptr),
		m_materialsystem()->FindMaterial(crypt_str("dev/glow_armsrace.vmt"), nullptr),
		m_materialsystem()->FindMaterial(crypt_str("models/inventory_items/wildfire_gold/wildfire_gold_detail"), nullptr),
		zblockCreateMaterial(true, crypt_str(R"#("VertexLitGeneric" 
			{ 
				"$additive"					"1" 
				"$envmap"					"models/effects/cube_white" 
				"$envmaptint"				"[1 1 1]" 
				"$envmapfresnel"			"1" 
				"$envmapfresnelminmaxexp" 	"[0 1 2]" 
				"$alpha" 					"0.8" 
			}
		)#")),
		zblockCreateMaterial(true, crypt_str(R"#("VertexLitGeneric"
		    {
		        "$basetexture"				"dev/zone_warning"
		        "$additive"					"1"
		        "$envmap"					"editor/cube_vertigo"
		        "$envmaptint"				"[0 0.5 0.55]"
		        "$envmapfresnel"			"1"
		        "$envmapfresnelminmaxexp"   "[0.00005 0.6 6]"
		        "$alpha"					"1"
   
		        Proxies
		        {
		            TextureScroll
		            {
		                "texturescrollvar"			"$baseTextureTransform"
		                "texturescrollrate"			"0.25"
		                "texturescrollangle"		"270"
		            }
		            Sine
		            {
		                "sineperiod"				"2"
		                "sinemin"					"0.1"
		                "resultVar"					"$envmapfresnelminmaxexp[1]"
		            }
		        }
		    }
		)#"))
		};
		auto material = materials[g_cfg.player.type[ENEMY].chams_type];



		auto alpha_c = (float)g_cfg.player.lag_hitbox_color.a() / 255.0f;
		float normal_color[3] =
		{
			g_cfg.player.lag_hitbox_color[0] / 255.0f,
			g_cfg.player.lag_hitbox_color[1] / 255.0f,
			g_cfg.player.lag_hitbox_color[2] / 255.0f
		};

		m_renderview()->SetBlend(alpha_c * alpha);
		util::color_modulate(normal_color, material);

		material->IncrementReferenceCount();
		material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

		m_modelrender()->ForcedMaterialOverride(material);
		m_modelrender()->DrawModelExecute(ctx, it->state, it->info, it->pBoneToWorld);
		m_modelrender()->ForcedMaterialOverride(nullptr);

		++it;
	}
}

void hit_cham::add_matrix1(player_t* player, matrix3x4_t* bones)
{
	auto& hit = m_Hitmatrix.emplace_back();

	std::memcpy(hit.pBoneToWorld, bones, player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));
	hit.time = m_globals()->m_realtime + 2.5f;

	static int m_nSkin = util::find_in_datamap(player->GetPredDescMap(), crypt_str("m_nSkin"));
	static int m_nBody = util::find_in_datamap(player->GetPredDescMap(), crypt_str("m_nBody"));

	hit.info.origin = player->GetAbsOrigin();
	hit.info.angles = player->GetAbsAngles();

	auto renderable = player->GetClientRenderable();

	if (!renderable)
		return;

	auto model = player->GetModel();

	if (!model)
		return;

	auto hdr = *(studiohdr_t**)(player->m_pStudioHdr());

	if (!hdr)
		return;

	hit.state.m_pStudioHdr = hdr;
	hit.state.m_pStudioHWData = m_modelcache()->GetHardwareData(model->studio);
	hit.state.m_pRenderable = renderable;
	hit.state.m_drawFlags = 0;

	hit.info.pRenderable = renderable;
	hit.info.pModel = model;
	hit.info.pLightingOffset = nullptr;
	hit.info.pLightingOrigin = nullptr;
	hit.info.hitboxset = player->m_nHitboxSet();
	hit.info.skin = (int)(uintptr_t(player) + m_nSkin);
	hit.info.body = (int)(uintptr_t(player) + m_nBody);
	hit.info.entity_index = player->EntIndex();
	hit.info.instance = call_virtual<ModelInstanceHandle_t(__thiscall*)(void*) >(renderable, 30u)(renderable);
	hit.info.flags = 0x1;

	hit.info.pModelToWorld = &hit.model_to_world;
	hit.state.m_pModelToWorld = &hit.model_to_world;

	hit.model_to_world.AngleMatrix(hit.info.angles, hit.info.origin);
}