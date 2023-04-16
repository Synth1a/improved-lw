#include "menu.h"
#include <ShlObj_core.h>
#include "../constchars.h"
#include "../cheats/misc/logs.h"


void draw_multicomboOT(std::string name, std::vector<int>& variable, const char* labels[], int count, std::string& preview)
{
	static auto howmuchsel = [](std::vector<int> e) -> int {
		int s = 0;
		for (int i = 0; i < e.size(); i++)
			if (e[i])
				s++;


		return s;
	};
	if (ui::BeginComboOT(name.c_str(), (howmuchsel(variable) > 0 ? std::to_string(howmuchsel(variable)) + " selected" : "None").c_str(), ImGuiComboFlags_HeightLarge, count)) // draw start
	{
		ui::BeginGroup();
			for (auto i = 0; i < count; i++)
				ui::SelectableOT(labels[i], (bool*)&variable[i], ImGuiSelectableFlags_DontClosePopups);
		ui::EndGroup();
		ui::EndComboOT();
	}

	preview = crypt_str("None"); // reset preview to use later

}
std::vector <std::string> zzfiles;
std::vector <std::string> zzscripts;
std::string zzediting_script;
IDirect3DTexture9* zzall_skins[36];
static float zpfix = 226;
auto zzselected_script = 0;
auto zzloaded_editing_script = false;

std::string zzget_wep(int id, int custom_index = -1, bool knife = true)
{
	if (custom_index > -1)
	{
		if (knife)
		{
			switch (custom_index)
			{
			case 0: return crypt_str("weapon_knife");
			case 1: return crypt_str("weapon_bayonet");
			case 2: return crypt_str("weapon_knife_css");
			case 3: return crypt_str("weapon_knife_skeleton");
			case 4: return crypt_str("weapon_knife_outdoor");
			case 5: return crypt_str("weapon_knife_cord");
			case 6: return crypt_str("weapon_knife_canis");
			case 7: return crypt_str("weapon_knife_flip");
			case 8: return crypt_str("weapon_knife_gut");
			case 9: return crypt_str("weapon_knife_karambit");
			case 10: return crypt_str("weapon_knife_m9_bayonet");
			case 11: return crypt_str("weapon_knife_tactical");
			case 12: return crypt_str("weapon_knife_falchion");
			case 13: return crypt_str("weapon_knife_survival_bowie");
			case 14: return crypt_str("weapon_knife_butterfly");
			case 15: return crypt_str("weapon_knife_push");
			case 16: return crypt_str("weapon_knife_ursus");
			case 17: return crypt_str("weapon_knife_gypsy_jackknife");
			case 18: return crypt_str("weapon_knife_stiletto");
			case 19: return crypt_str("weapon_knife_widowmaker");
			}
		}
		else
		{
			switch (custom_index)
			{
			case 0: return crypt_str("ct_gloves"); //-V1037
			case 1: return crypt_str("studded_bloodhound_gloves");
			case 2: return crypt_str("t_gloves");
			case 3: return crypt_str("ct_gloves");
			case 4: return crypt_str("sporty_gloves");
			case 5: return crypt_str("slick_gloves");
			case 6: return crypt_str("leather_handwraps");
			case 7: return crypt_str("motorcycle_gloves");
			case 8: return crypt_str("specialist_gloves");
			case 9: return crypt_str("studded_hydra_gloves");
			}
		}
	}
	else
	{
		switch (id)
		{
		case 0: return crypt_str("knife");
		case 1: return crypt_str("gloves");
		case 2: return crypt_str("weapon_ak47");
		case 3: return crypt_str("weapon_aug");
		case 4: return crypt_str("weapon_awp");
		case 5: return crypt_str("weapon_cz75a");
		case 6: return crypt_str("weapon_deagle");
		case 7: return crypt_str("weapon_elite");
		case 8: return crypt_str("weapon_famas");
		case 9: return crypt_str("weapon_fiveseven");
		case 10: return crypt_str("weapon_g3sg1");
		case 11: return crypt_str("weapon_galilar");
		case 12: return crypt_str("weapon_glock");
		case 13: return crypt_str("weapon_m249");
		case 14: return crypt_str("weapon_m4a1_silencer");
		case 15: return crypt_str("weapon_m4a1");
		case 16: return crypt_str("weapon_mac10");
		case 17: return crypt_str("weapon_mag7");
		case 18: return crypt_str("weapon_mp5sd");
		case 19: return crypt_str("weapon_mp7");
		case 20: return crypt_str("weapon_mp9");
		case 21: return crypt_str("weapon_negev");
		case 22: return crypt_str("weapon_nova");
		case 23: return crypt_str("weapon_hkp2000");
		case 24: return crypt_str("weapon_p250");
		case 25: return crypt_str("weapon_p90");
		case 26: return crypt_str("weapon_bizon");
		case 27: return crypt_str("weapon_revolver");
		case 28: return crypt_str("weapon_sawedoff");
		case 29: return crypt_str("weapon_scar20");
		case 30: return crypt_str("weapon_ssg08");
		case 31: return crypt_str("weapon_sg556");
		case 32: return crypt_str("weapon_tec9");
		case 33: return crypt_str("weapon_ump45");
		case 34: return crypt_str("weapon_usp_silencer");
		case 35: return crypt_str("weapon_xm1014");
		default: return crypt_str("unknown");
		}
	}
}

IDirect3DTexture9* zzget_skin_preview(const char* weapon_name, const std::string& skin_name, IDirect3DDevice9* device)
{
	IDirect3DTexture9* skin_image = nullptr;
	std::string vpk_path;

	if (strcmp(weapon_name, crypt_str("unknown")) && strcmp(weapon_name, crypt_str("knife")) && strcmp(weapon_name, crypt_str("gloves"))) //-V526
	{
		if (skin_name.empty() || skin_name == crypt_str("default"))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/") + std::string(weapon_name) + crypt_str(".png");
		else
			vpk_path = crypt_str("resource/flash/econ/default_generated/") + std::string(weapon_name) + crypt_str("_") + std::string(skin_name) + crypt_str("_light_large.png");
	}
	else
	{
		if (!strcmp(weapon_name, crypt_str("knife")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_knife.png");
		else if (!strcmp(weapon_name, crypt_str("gloves")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else if (!strcmp(weapon_name, crypt_str("unknown")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_snowball.png");

	}
	const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));
	if (handle)
	{
		int file_len = m_basefilesys()->Size(handle);
		char* image = new char[file_len]; //-V121

		m_basefilesys()->Read(image, file_len, handle);
		m_basefilesys()->Close(handle);

		D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
		delete[] image;
	}

	if (!skin_image)
	{
		std::string vpk_path;

		if (strstr(weapon_name, crypt_str("bloodhound")) != NULL || strstr(weapon_name, crypt_str("hydra")) != NULL)
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/") + std::string(weapon_name) + crypt_str(".png");

		const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));

		if (handle)
		{
			int file_len = m_basefilesys()->Size(handle);
			char* image = new char[file_len]; //-V121

			m_basefilesys()->Read(image, file_len, handle);
			m_basefilesys()->Close(handle);

			D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
			delete[] image;
		}
	}

	return skin_image;
}



std::string zzget_config_dir()
{
	std::string folder;
	static TCHAR path[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPath(NULL, 0x001a, NULL, NULL, path)))
		folder = std::string(path) + crypt_str("\\SHONAX-PROJECT\\Configs\\");

	CreateDirectory(folder.c_str(), NULL);
	return folder;
}

void zzload_config()
{
	if (cfg_manager->files.empty())
		return;

	cfg_manager->load(cfg_manager->files.at(g_cfg.selected_config), false);
	c_lua::get().unload_all_scripts();

	for (auto& script : g_cfg.scripts.scripts)
		c_lua::get().load_script(c_lua::get().get_script_id(script));

	zzscripts = c_lua::get().scripts;

	if (zzselected_script >= zzscripts.size())
		zzselected_script = zzscripts.size() - 1; //-V103

	for (auto& current : zzscripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}

	for (auto i = 0; i < g_cfg.skins.skinChanger.size(); ++i)
		zzall_skins[i] = nullptr;

	g_cfg.scripts.scripts.clear();

	cfg_manager->load(cfg_manager->files.at(g_cfg.selected_config), true);
	cfg_manager->config_files();

	eventlogs::get().add(crypt_str("Loaded ") + zzfiles.at(g_cfg.selected_config) + crypt_str(" config"), false);
}

void zzsave_config()
{
	if (cfg_manager->files.empty())
		return;

	g_cfg.scripts.scripts.clear();

	for (auto i = 0; i < c_lua::get().scripts.size(); ++i)
	{
		auto script = c_lua::get().scripts.at(i);

		if (c_lua::get().loaded.at(i))
			g_cfg.scripts.scripts.emplace_back(script);
	}

	cfg_manager->save(cfg_manager->files.at(g_cfg.selected_config));
	cfg_manager->config_files();

	eventlogs::get().add(crypt_str("Saved ") + zzfiles.at(g_cfg.selected_config) + crypt_str(" config"), false);
}

void zzremove_config()
{
	if (cfg_manager->files.empty())
		return;

	eventlogs::get().add(crypt_str("Removed ") + zzfiles.at(g_cfg.selected_config) + crypt_str(" config"), false);

	cfg_manager->remove(cfg_manager->files.at(g_cfg.selected_config));
	cfg_manager->config_files();

	zzfiles = cfg_manager->files;

	if (g_cfg.selected_config >= zzfiles.size())
		g_cfg.selected_config = zzfiles.size() - 1; //-V103
}

void zzadd_config()
{
	auto empty = true;

	for (auto current : g_cfg.new_config_name)
	{
		if (current != ' ')
		{
			empty = false;
			break;
		}
	}

	if (empty)
		g_cfg.new_config_name = crypt_str("config");

	eventlogs::get().add(crypt_str("Added ") + g_cfg.new_config_name + crypt_str(" config"), false);

	if (g_cfg.new_config_name.find(crypt_str(".one")) == std::string::npos)
		g_cfg.new_config_name += crypt_str(".one");

	cfg_manager->save(g_cfg.new_config_name);
	cfg_manager->config_files();

	g_cfg.selected_config = cfg_manager->files.size() - 1; //-V103
	zzfiles = cfg_manager->files;
}













void otmenu::render()
{
	std::string preview = crypt_str("None");
	static bool window_set = false;
	static int windowsize_x = 575;
	static int windowsize_y = 465;
	static int oldwindowsize_x = 500;
	static int oldwindowsize_y = 400;
	static int speed = 5;
	static bool finish = false;
	static bool sub_tabs = false;

	if (windowsize_x != oldwindowsize_x)
	{
		if (windowsize_x > oldwindowsize_x)
		{
			windowsize_x -= speed;
		}
		if (windowsize_x < oldwindowsize_x)
		{
			windowsize_x += speed;
		}
	}
	if (windowsize_y != oldwindowsize_y)
	{
		if (windowsize_y > oldwindowsize_y)
		{
			windowsize_y -= speed;
		}
		if (windowsize_y < oldwindowsize_y)
		{
			windowsize_y += speed;
		}
	}
	if (windowsize_x == oldwindowsize_x && windowsize_y == oldwindowsize_y)
	{
		finish = true;
	}
	else
	{
		finish = false;
	}


	ImGuiStyle* style = &ui::GetStyle();
	style->Colors[ImGuiCol_ScrollbarBg] = ImColor(117, 187, 253, 255);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(32, 143, 253, 255);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(89, 171, 253, 255);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(89, 171, 253, 255);


	style->ScrollbarSize = 2;
	style->ScrollbarRounding = 2;
	ui::PushFont(skeet_menu.f2);
	ui::SetNextWindowSizeConstraints(ImVec2(windowsize_x, windowsize_y), ImVec2(windowsize_x, windowsize_y));
	ui::BeginOT("Onetap", 0, ImGuiWindowFlags_NoCollapse || ImGuiWindowFlags_NoScrollbar || ImGuiWindowFlags_NoTitleBar || ImGuiWindowFlags_NoResize);
	{
		auto wp = ui::GetWindowPos();
		auto ws = ui::GetWindowSize();
		static int tab = 0;
		static int sub_rage = 0;
		static int sub_legit = 0;
		static int sub_visuals = 0;
		static int sub_misc = 0;
		static int sub_skins = 0;

		ui::TabButtonOT("Legit", &tab, 0, 5);
		ui::TabButtonOT("Rage", &tab, 1, 5);
		ui::TabButtonOT("Visuals", &tab, 2, 5);
		ui::TabButtonOT("Skins", &tab, 3, 5);
		ui::TabButtonOT("Misc", &tab, 4, 5);


		auto defsize = ImVec2(370, 350);
		auto def_pos = ImVec2(wp.x + 160, wp.y + 80);

		static int alphas = 0;

		if (finish == true)
		{
			alphas += 5;
		}
		else
		{
			alphas = 0;
		}
		ui::GetStyle().secondalpha = alphas;
		if (tab == 0)
		{
			oldwindowsize_x = 575;
			oldwindowsize_y = 620;

			if (finish == true)
			{
				ui::GetWindowDrawList()->AddRectFilled(ImVec2(wp.x + 15, wp.y + 80), ImVec2(wp.x + ws.x - 15, wp.y + 130), ImColor(32, 32, 38, alphas), 5);
				ui::GetWindowDrawList()->AddRect(ImVec2(wp.x + 15, wp.y + 80), ImVec2(wp.x + ws.x - 15, wp.y + 130), ImColor(46, 48, 56, alphas), 5);

				ui::TabButton_littOT("General", "q", &sub_legit, 0, 7, skeet_menu.icons);
				ui::TabButton_littOT("R8", "J", &sub_legit, 1, 7, skeet_menu.icons);
				ui::TabButton_littOT("Pistol", "C", &sub_legit, 2, 7, skeet_menu.icons);
				ui::TabButton_littOT("Rifle", "W", &sub_legit, 3, 7, skeet_menu.icons);
				ui::TabButton_littOT("Smg", "N", &sub_legit, 4, 7, skeet_menu.icons);

				ui::TabButton_littOT("Snipers", "Z", &sub_legit, 5, 7, skeet_menu.icons);
				ui::TabButton_littOT("Heavy", "d", &sub_legit, 6, 7, skeet_menu.icons);

				static float slid;
				if (sub_legit)
					hooks::legit_weapon = sub_legit - 1;
					
				

			
					ui::SetNextWindowPos(ImVec2(wp.x + 15, wp.y + 145));
					ui::BeginChildOT("General", ImVec2(260, 410));
					{

						ui::CheckboxOT(crypt_str("Enable"), &g_cfg.legitbot.enabled); ui::Spacing();


						if (g_cfg.legitbot.enabled)
							g_cfg.ragebot.enable = false;

						ui::CheckboxOT(crypt_str("Friendly fire"), &g_cfg.legitbot.friendly_fire); ui::Spacing();
						ui::CheckboxOT(crypt_str("Automatic pistols"), &g_cfg.legitbot.autopistol); ui::Spacing();

						ui::CheckboxOT(crypt_str("Automatic scope"), &g_cfg.legitbot.autoscope); ui::Spacing();

						if (g_cfg.legitbot.autoscope)
						{
							ui::CheckboxOT(crypt_str("Unscope after shot"), &g_cfg.legitbot.unscope); ui::Spacing();
						}

						ui::CheckboxOT(crypt_str("Snipers in zoom only"), &g_cfg.legitbot.sniper_in_zoom_only); ui::Spacing();

						ui::CheckboxOT(crypt_str("Enable in air"), &g_cfg.legitbot.do_if_local_in_air); ui::Spacing();
						ui::CheckboxOT(crypt_str("Enable if flashed"), &g_cfg.legitbot.do_if_local_flashed); ui::Spacing();
						ui::CheckboxOT(crypt_str("Enable in smoke"), &g_cfg.legitbot.do_if_enemy_in_smoke); ui::Spacing();
						ui::Text("Key");
						ui::KeybindSkeet("#sd", &g_cfg.legitbot.key.key, &g_cfg.legitbot.key.mode);
						ui::Text("Key autofire");
						ui::KeybindSkeet("#q1asgf", &g_cfg.legitbot.autofire_key.key, &g_cfg.legitbot.autofire_key.mode);
						ui::SliderIntOT(crypt_str("Automatic fire delay"), &g_cfg.legitbot.autofire_delay, 0, 12, (!g_cfg.legitbot.autofire_delay ? crypt_str("None") : (g_cfg.legitbot.autofire_delay == 1 ? crypt_str("%.0f") : crypt_str("%.0f"))));


					}
					ui::EndChildOT();

					
					ui::SetNextWindowPos(ImVec2(wp.x + 300, wp.y + 145));
					ui::BeginChildOT("Weapon Config", ImVec2(260, 410));
					{

						if (sub_legit)
						{
							std::vector<const char*> hitbox_legit = { crypt_str("Nearest"), crypt_str("Head"), crypt_str("Body") };
							ui::ComboOT(crypt_str("Hitbox"), &g_cfg.legitbot.weapon[hooks::legit_weapon].priority, hitbox_legit);

							ui::CheckboxOT(crypt_str("Automatic stop"), &g_cfg.legitbot.weapon[hooks::legit_weapon].auto_stop);

							ui::ComboOT(crypt_str("Field of view type"), &g_cfg.legitbot.weapon[hooks::legit_weapon].fov_type, LegitFov);//"%.0f"
							ui::SliderFloatOT(crypt_str("Field of view amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].fov, 0.f, 30.f, crypt_str("%.0f"));



							ui::SliderFloatOT(crypt_str("Silent field of view"), &g_cfg.legitbot.weapon[hooks::legit_weapon].silent_fov, 0.f, 30.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].silent_fov ? crypt_str("None") : crypt_str("%.0f"))); //-V550



							ui::ComboOT(crypt_str("Smooth type"), &g_cfg.legitbot.weapon[hooks::legit_weapon].smooth_type, LegitSmooth);
							ui::SliderFloatOT(crypt_str("Smooth amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].smooth, 1.f, 12.f, crypt_str("%.0f"));



							ui::ComboOT(crypt_str("RCS type"), &g_cfg.legitbot.weapon[hooks::legit_weapon].rcs_type, RCSType);
							ui::SliderFloatOT(crypt_str("RCS amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].rcs, 0.f, 100.f, crypt_str("%.0f%%"), 1.f);

							if (g_cfg.legitbot.weapon[hooks::legit_weapon].rcs > 0)
							{
								ui::SliderFloatOT(crypt_str("RCS custom field of view"), &g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_fov, 0.f, 30.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_fov ? crypt_str("None") : crypt_str("%.0f"))); //-V550
								ui::SliderFloatOT(crypt_str("RCS Custom smooth"), &g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_smooth, 0.f, 12.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_smooth ? crypt_str("None") : crypt_str("%.0f"))); //-V550
							}



							ui::SliderIntOT(crypt_str("Automatic wall damage"), &g_cfg.legitbot.weapon[hooks::legit_weapon].awall_dmg, 0, 100, (!g_cfg.legitbot.weapon[hooks::legit_weapon].awall_dmg ? crypt_str("None") : crypt_str("%.0f")));
							ui::SliderIntOT(crypt_str("Automatic fire hitchance"), &g_cfg.legitbot.weapon[hooks::legit_weapon].autofire_hitchance, 0, 100, (!g_cfg.legitbot.weapon[hooks::legit_weapon].autofire_hitchance ? crypt_str("None") : crypt_str("%.0f")));
							ui::SliderFloatOT(crypt_str("Target switch delay"), &g_cfg.legitbot.weapon[hooks::legit_weapon].target_switch_delay, 0.f, 1.f, "%.0f");
						}
						

					}
					ui::EndChildOT();



					
				



			}
		}
		if (tab == 1)
		{
			oldwindowsize_x = 575;
			oldwindowsize_y = 820;
			if (finish == true)
			{
				ui::GetWindowDrawList()->AddRectFilled(ImVec2(wp.x + 15, wp.y + 80), ImVec2(wp.x + ws.x - 15, wp.y + 130), ImColor(32, 32, 38, alphas), 5);
				ui::GetWindowDrawList()->AddRect(ImVec2(wp.x + 15, wp.y + 80), ImVec2(wp.x + ws.x - 15, wp.y + 130), ImColor(46, 48, 56, alphas), 5);

				ui::TabButton_littOT("General", "q", &sub_rage, 0, 9, skeet_menu.icons);
				ui::TabButton_littOT("R8", "J", &sub_rage, 1, 9, skeet_menu.icons);
				ui::TabButton_littOT("Pistol", "C", &sub_rage, 2, 9, skeet_menu.icons);
				ui::TabButton_littOT("SMG", "N", &sub_rage, 3, 9, skeet_menu.icons);
				ui::TabButton_littOT("Rifle", "W", &sub_rage, 4, 9, skeet_menu.icons);
				ui::TabButton_littOT("Autosniper", "X", &sub_rage, 5, 9, skeet_menu.icons);
				ui::TabButton_littOT("Scout", "a", &sub_rage, 6, 9, skeet_menu.icons);

				ui::TabButton_littOT("Awp", "Z", &sub_rage, 7, 9, skeet_menu.icons);
				ui::TabButton_littOT("Heavy", "f", &sub_rage, 8, 9, skeet_menu.icons);

				hooks::rage_weapon = sub_rage - 1;

				ui::SetNextWindowPos(ImVec2(wp.x + 15, wp.y + 145));
				ui::BeginChildOT("General", ImVec2(260, 290));
				{
					ui::CheckboxOT(crypt_str("Enable"), &g_cfg.ragebot.enable); ui::Spacing();

					if (g_cfg.ragebot.enable)
						g_cfg.legitbot.enabled = false;

					ui::SliderIntOT(crypt_str("View"), &g_cfg.ragebot.field_of_view, 1, 180, crypt_str("%.0f"));
					ui::CheckboxOT(crypt_str("Silent aim"), &g_cfg.ragebot.silent_aim); ui::Spacing();
					ui::CheckboxOT(crypt_str("Automatic wall"), &g_cfg.ragebot.autowall); ui::Spacing();
					ui::CheckboxOT(crypt_str("Aimbot with zeus"), &g_cfg.ragebot.zeus_bot); ui::Spacing();
					ui::CheckboxOT(crypt_str("Aimbot with knife"), &g_cfg.ragebot.knife_bot); ui::Spacing();
					ui::CheckboxOT(crypt_str("Automatic fire"), &g_cfg.ragebot.autoshoot); ui::Spacing();
					ui::CheckboxOT(crypt_str("Automatic scope"), &g_cfg.ragebot.autoscope); ui::Spacing();
					ui::CheckboxOT(crypt_str("Pitch desync correction"), &g_cfg.ragebot.pitch_antiaim_correction); ui::Spacing();

					ui::CheckboxOT(crypt_str("Double tap"), &g_cfg.ragebot.double_tap); ui::Spacing();

					if (g_cfg.ragebot.double_tap)
					{
						ui::Text("Double tap key");
						ui::KeybindSkeet("##asdg111111asf", &g_cfg.ragebot.double_tap_key.key, &g_cfg.ragebot.double_tap_key.mode);
						ui::CheckboxOT(crypt_str("Slow teleport"), &g_cfg.ragebot.slow_teleport); ui::Spacing();
					}

					ui::CheckboxOT(crypt_str("Hide shots"), &g_cfg.antiaim.hide_shots); ui::Spacing();

					if (g_cfg.antiaim.hide_shots)
					{
						ui::Text("Hide shots key");
						ui::KeybindSkeet("##asdg111111asf1ads", &g_cfg.antiaim.hide_shots_key.key, &g_cfg.antiaim.hide_shots_key.mode);

					}


					ui::Text("Force safe points");
					ui::KeybindSkeet("##aq1wt", &g_cfg.ragebot.safe_point_key.key, &g_cfg.ragebot.safe_point_key.mode);

					ui::Text("Force body aim");
					ui::KeybindSkeet("##ff", &g_cfg.ragebot.body_aim_key.key, &g_cfg.ragebot.body_aim_key.mode);

				}
				ui::EndChildOT();


				ui::SetNextWindowPos(ImVec2(wp.x + 300, wp.y + 145));
				ui::BeginChildOT("Weapon Config", ImVec2(260, 290));
				{

					if (sub_rage)
					{
						ui::ComboOT(crypt_str("Target selection"), &g_cfg.ragebot.weapon[hooks::rage_weapon].selection_type, selection);

						draw_multicomboOT(crypt_str("Hitboxes"), g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes, hitboxes, ARRAYSIZE(hitboxes), preview);

						ui::CheckboxOT(crypt_str("Static point scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].static_point_scale); ui::Spacing();

						if (g_cfg.ragebot.weapon[hooks::rage_weapon].static_point_scale)
						{
							ui::SliderFloatOT(crypt_str("Head scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].head_scale, 0.0f, 1.0f, g_cfg.ragebot.weapon[hooks::rage_weapon].head_scale ? crypt_str("%.0f") : crypt_str("None"));
							ui::SliderFloatOT(crypt_str("Body scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].body_scale, 0.0f, 1.0f, g_cfg.ragebot.weapon[hooks::rage_weapon].body_scale ? crypt_str("%.0f") : crypt_str("None"));
						}

						ui::CheckboxOT(crypt_str("Enable max misses"), &g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses); ui::Spacing();

						if (g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses)
							ui::SliderIntOT(crypt_str("Max misses"), &g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses_amount, 0, 6, "%.0f");

						ui::CheckboxOT(crypt_str("Prefer safe points"), &g_cfg.ragebot.weapon[hooks::rage_weapon].prefer_safe_points); ui::Spacing();
						ui::CheckboxOT(crypt_str("Prefer body aim"), &g_cfg.ragebot.weapon[hooks::rage_weapon].prefer_body_aim); ui::Spacing();





						ui::CheckboxOT(crypt_str("Automatic stop"), &g_cfg.ragebot.weapon[hooks::rage_weapon].autostop); ui::Spacing();

						if (g_cfg.ragebot.weapon[hooks::rage_weapon].autostop)
							draw_multicomboOT(crypt_str("Modifiers"), g_cfg.ragebot.weapon[hooks::rage_weapon].autostop_modifiers, autostop_modifiers, ARRAYSIZE(autostop_modifiers), preview);

						ui::CheckboxOT(crypt_str("Hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance); ui::Spacing();

						if (g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance)
							ui::SliderIntOT(crypt_str("Hitchance amount"), &g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance_amount, 1, 100, "%.0f");

						if (g_cfg.ragebot.double_tap && hooks::rage_weapon <= 4)
						{
							ui::CheckboxOT(crypt_str("Double tap hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance); ui::Spacing();

							if (g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance)
								ui::SliderIntOT(crypt_str("Double tap hitchance amount"), &g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance_amount, 1, 100, "%.0f");
						}

						ui::SliderIntOT(crypt_str("Minimum visible damage"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_visible_damage, 1, 120, "%.0f");

						if (g_cfg.ragebot.autowall)
							ui::SliderIntOT(crypt_str("Minimum wall damage"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_damage, 1, 120, "%.0f");


						ui::Text("Damage override");
						ui::KeybindSkeet("##sasgd", &g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key, &g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.mode);


						if (g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key > KEY_NONE&& g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key < KEY_MAX)
							ui::SliderIntOT(crypt_str("Minimum override damage"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_override_damage, 1, 120, "%.0f");


					}


				}
				ui::EndChildOT();




				ui::SetNextWindowPos(ImVec2(wp.x + 15, wp.y + 145 + 300));

				ui::BeginChildOT("Anti-aim", ImVec2(260, 290));
				{
					
					static auto type = 0;

					ui::CheckboxOT(crypt_str("Enable"), &g_cfg.antiaim.enable); ui::Spacing();
					ui::ComboOT(crypt_str("Anti-aim type"), &g_cfg.antiaim.antiaim_type, antiaim_type);

					if (g_cfg.antiaim.antiaim_type)
					{

						ui::ComboOT(crypt_str("Desync"), &g_cfg.antiaim.desync, desync);

						if (g_cfg.antiaim.desync)
						{

							ui::ComboOT(crypt_str("LBY type"), &g_cfg.antiaim.legit_lby_type, lby_type);

							if (g_cfg.antiaim.desync == 1)
							{
								ui::Text("Invert desync key");
								ui::KeybindSkeet("##asdg111111asf1adsas12qw", &g_cfg.antiaim.flip_desync.key, &g_cfg.antiaim.flip_desync.mode);

							}
						}
					}
					else
					{
						ui::ComboOT(crypt_str("Movement type"), &type, movement_type);

						ui::ComboOT(crypt_str("Pitch"), &g_cfg.antiaim.type[type].pitch, pitch);

						ui::ComboOT(crypt_str("Yaw"), &g_cfg.antiaim.type[type].yaw, yaw);

						ui::ComboOT(crypt_str("Base angle"), &g_cfg.antiaim.type[type].base_angle, baseangle);

						if (g_cfg.antiaim.type[type].yaw)
						{
							ui::SliderIntOT(g_cfg.antiaim.type[type].yaw == 1 ? crypt_str("Jitter range") : crypt_str("Spin range"), &g_cfg.antiaim.type[type].range, 1, 180, "%0.f");

							if (g_cfg.antiaim.type[type].yaw == 2)
								ui::SliderIntOT(crypt_str("Spin speed"), &g_cfg.antiaim.type[type].speed, 1, 15, "%0.f");


						}

						ui::ComboOT(crypt_str("Desync"), &g_cfg.antiaim.type[type].desync, desync);

						if (g_cfg.antiaim.type[type].desync)
						{
							if (type == ANTIAIM_STAND)
							{

								ui::ComboOT(crypt_str("LBY type"), &g_cfg.antiaim.lby_type, lby_type);
							}

							if (type != ANTIAIM_STAND || !g_cfg.antiaim.lby_type)
							{
								ui::SliderIntOT(crypt_str("Desync range"), &g_cfg.antiaim.type[type].desync_range, 1, 60, "%0.f");
								ui::SliderIntOT(crypt_str("Inverted desync range"), &g_cfg.antiaim.type[type].inverted_desync_range, 1, 60, "%0.f");
								ui::SliderIntOT(crypt_str("Body lean"), &g_cfg.antiaim.type[type].body_lean, 0, 100, "%0.f");
								ui::SliderIntOT(crypt_str("Inverted body lean"), &g_cfg.antiaim.type[type].inverted_body_lean, 0, 100, "%0.f");
							}

							if (g_cfg.antiaim.type[type].desync == 1)
							{
								static bool asdg;
								ui::Text("Invert desync");
								ui::KeybindSkeet("##eryasdgaeg", &g_cfg.antiaim.flip_desync.key, &g_cfg.antiaim.flip_desync.mode);

							}
						}



						ui::Text("Manual back");
						ui::KeybindSkeet("asfeqrygqwerty", &g_cfg.antiaim.manual_back.key, &g_cfg.antiaim.manual_back.mode);
						ui::Text("Manual left");
						ui::KeybindSkeet("asfeqrygqwertasy", &g_cfg.antiaim.manual_left.key, &g_cfg.antiaim.manual_left.mode);
						ui::Text("Manual right");
						ui::KeybindSkeet("asfeqrygqwertas1y", &g_cfg.antiaim.manual_right.key, &g_cfg.antiaim.manual_right.mode);




						if (g_cfg.antiaim.manual_back.key > KEY_NONE&& g_cfg.antiaim.manual_back.key < KEY_MAX || g_cfg.antiaim.manual_left.key > KEY_NONE&& g_cfg.antiaim.manual_left.key < KEY_MAX || g_cfg.antiaim.manual_right.key > KEY_NONE&& g_cfg.antiaim.manual_right.key < KEY_MAX)
						{
							ui::CheckboxOT(crypt_str("Manuals indicator"), &g_cfg.antiaim.flip_indicator); ui::Spacing();

							static float col[4];
							ui::ColorEdit4Fix("##invc", &g_cfg.antiaim.flip_indicator_color, ImGuiWindowFlags_NoTitleBar);
						}
					}
				}
				ui::EndChildOT();

				ui::SetNextWindowPos(ImVec2(wp.x + 300, wp.y + 145 + 300));


				ui::BeginChildOT("Fakelags", ImVec2(260, 250));
				{
					ui::CheckboxOT(crypt_str("Fake lag"), &g_cfg.antiaim.fakelag); ui::Spacing();
					if (g_cfg.antiaim.fakelag)
					{
						ui::ComboOT(crypt_str("Fake lag type"), &g_cfg.antiaim.fakelag_type, fakelags);
						ui::SliderIntOT(crypt_str("Limit"), &g_cfg.antiaim.fakelag_amount, 1, 16, "%0.f");

						draw_multicomboOT(crypt_str("Fake lag triggers"), g_cfg.antiaim.fakelag_enablers, lagstrigger, ARRAYSIZE(lagstrigger), preview);

						auto enabled_fakelag_triggers = false;

						for (auto i = 0; i < ARRAYSIZE(lagstrigger); i++)
							if (g_cfg.antiaim.fakelag_enablers[i])
								enabled_fakelag_triggers = true;

						if (enabled_fakelag_triggers)
							ui::SliderIntOT(crypt_str("Triggers limit"), &g_cfg.antiaim.triggers_fakelag_amount, 1, 16, "%0.f");
					}

					ui::CheckboxOT(crypt_str("No duck cooldown"), &g_cfg.misc.noduck); ui::Spacing();

					if (g_cfg.misc.noduck)
					{
						ui::Text("Fake duck");
						ui::KeybindSkeet("a1eryasdgaeg", &g_cfg.misc.fakeduck_key.key, &g_cfg.misc.fakeduck_key.mode);
					}

					ui::Text("Slow walk");
					ui::KeybindSkeet("a1eryasdgaeg1", &g_cfg.misc.slowwalk_key.key, &g_cfg.misc.slowwalk_key.mode);
					ui::Text("Auto peek");
					ui::KeybindSkeet("a1eryasdgaeg2", &g_cfg.misc.automatic_peek.key, &g_cfg.misc.automatic_peek.mode);
					ui::Text("Edge jump");
					ui::KeybindSkeet("Edgesadgjump", &g_cfg.misc.edge_jump.key, &g_cfg.misc.edge_jump.mode);

				}
				ui::EndChildOT();


			}
		}







		if (tab == 3)
		{
			oldwindowsize_x = 575;
			oldwindowsize_y = 550;
			
			if (finish == true)
			{
				ui::GetWindowDrawList()->AddRectFilled(ImVec2(wp.x + 15, wp.y + 80), ImVec2(wp.x + ws.x - 15, wp.y + 130), ImColor(32, 32, 38, alphas), 5);
				ui::GetWindowDrawList()->AddRect(ImVec2(wp.x + 15, wp.y + 80), ImVec2(wp.x + ws.x - 15, wp.y + 130), ImColor(46, 48, 56, alphas), 5);

				static auto adgtsadgfsad = 0;
				ui::TabButton_littOT("Skinchanger", "", &adgtsadgfsad, 0, 1, skeet_menu.icons);
				//ui::TabButton_littOT("Settings", "", &misc_sub, 1, 3, skeet_menu.icons);
				//ui::TabButton_littOT("Lua", "", &misc_sub, 2, 3, skeet_menu.icons); //dont know what letter use xd

				static ImVec2 verycode;
				static bool drugs = false;

				// some animation logic(switch)
				static bool active_animation = false;
				static bool preview_reverse = false;
				static float switch_alpha = 1.f;
				static int next_id = -1;
				if (active_animation)
				{
					if (preview_reverse)
					{
						if (switch_alpha == 1.f) //-V550
						{
							preview_reverse = false;
							active_animation = false;
						}

						switch_alpha = math::clamp(switch_alpha + (4.f * ui::GetIO().DeltaTime), 0.01f, 1.f);
					}
					else
					{
						if (switch_alpha == 0.01f) //-V550
						{
							preview_reverse = true;
						}

						switch_alpha = math::clamp(switch_alpha - (4.f * ui::GetIO().DeltaTime), 0.01f, 1.f);
					}
				}
				else
					switch_alpha = math::clamp(switch_alpha + (4.f * ui::GetIO().DeltaTime), 0.0f, 1.f);


				verycode = ImVec2(550, 330);

				ui::SetNextWindowPos(ImVec2(wp.x + 15, wp.y + 145));
				ui::BeginChildOT("Skins", verycode);
				{
					{
						// we need to count our items in 1 line
						auto same_line_counter = 0;

						// if we didnt choose any weapon
						if (c_menu::get().current_profile == -1)
						{
							for (auto i = 0; i < g_cfg.skins.skinChanger.size(); i++)
							{
								// do we need update our preview for some reasons?
								if (!zzall_skins[i])
								{
									g_cfg.skins.skinChanger.at(i).update();
									zzall_skins[i] = zzget_skin_preview(zzget_wep(i, (i == 0 || i == 1) ? g_cfg.skins.skinChanger.at(i).definition_override_vector_index : -1, i == 0).c_str(), g_cfg.skins.skinChanger.at(i).skin_name, c_menu::get().device); //-V810
								}

								// we licked on weapon
								if (ui::ImageButton(zzall_skins[i], ImVec2(107, 76)))
								{
									next_id = i;
									active_animation = true;
								}

								// if our animation step is half from all - switch profile
								if (active_animation && preview_reverse)
								{
									ui::SetScrollY(0);
									c_menu::get().current_profile = next_id;
								}

								if (same_line_counter < 4) { // continue push same-line
									ui::SameLine();
									same_line_counter++;
								}
								else { // we have maximum elements in 1 line
									same_line_counter = 0;
								}
							}
						}
						else
						{
							// update skin preview bool
							static bool need_update[36];

							// we pressed crypt_str("Save & Close") button
							static bool leave;

							// update if we have nullptr texture or if we push force update
							if (!zzall_skins[c_menu::get().current_profile] || need_update[c_menu::get().current_profile])
							{
								zzall_skins[c_menu::get().current_profile] = zzget_skin_preview(zzget_wep(c_menu::get().current_profile, (c_menu::get().current_profile == 0 || c_menu::get().current_profile == 1) ? g_cfg.skins.skinChanger.at(c_menu::get().current_profile).definition_override_vector_index : -1, c_menu::get().current_profile == 0).c_str(), g_cfg.skins.skinChanger.at(c_menu::get().current_profile).skin_name, c_menu::get().device); //-V810
								need_update[c_menu::get().current_profile] = false;
							}

							// get settings for selected weapon
							auto& selected_entry = g_cfg.skins.skinChanger[c_menu::get().current_profile];
							selected_entry.itemIdIndex = c_menu::get().current_profile;

							//ui::BeginGroup();
							//ui::PushItemWidth(260 * dpi_scale);

							// search input later
							static char search_skins[64] = "\0";
							static auto item_index = selected_entry.paint_kit_vector_index;

							if (!c_menu::get().current_profile)
							{

								//ui::SetCursorPosX(ui::GetCursorPosX() - 125 - 111);

								if (ui::SingleSelectOT("Knife model", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
									{
										*out_text = game_data::knife_names[idx].name;
										return true;
									}, IM_ARRAYSIZE(game_data::knife_names)))
									need_update[c_menu::get().current_profile] = true; // push force update
							}
							else if (c_menu::get().current_profile == 1)
							{

								//ui::SetCursorPosX(ui::GetCursorPosX() - 125 - 111);
								if (ui::SingleSelectOT(crypt_str("Glove model"), &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
									{
										*out_text = game_data::glove_names[idx].name;
										return true;
									}, IM_ARRAYSIZE(game_data::glove_names)))
								{
									item_index = 0; // set new generated paintkits element to 0;
									need_update[c_menu::get().current_profile] = true; // push force update
								}
							}
							else
								selected_entry.definition_override_vector_index = 0;

							if (c_menu::get().current_profile != 1)
							{
								ui::Text(crypt_str("Search"));
								ui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

								if (ui::InputText(crypt_str("##search"), search_skins, sizeof(search_skins)))
									item_index = -1;

								ui::PopStyleVar();
							}

							auto main_kits = c_menu::get().current_profile == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits;
							auto display_index = 0;

							SkinChanger::displayKits = main_kits;

							// we dont need custom gloves
							if (c_menu::get().current_profile == 1)
							{
								for (auto i = 0; i < main_kits.size(); i++)
								{
									auto main_name = main_kits.at(i).name;

									for (auto i = 0; i < main_name.size(); i++)
										if (iswalpha((main_name.at(i))))
											main_name.at(i) = towlower(main_name.at(i));

									char search_name[64];

									if (!strcmp(game_data::glove_names[selected_entry.definition_override_vector_index].name, crypt_str("Hydra")))
										strcpy_s(search_name, sizeof(search_name), crypt_str("Bloodhound"));
									else
										strcpy_s(search_name, sizeof(search_name), game_data::glove_names[selected_entry.definition_override_vector_index].name);

									for (auto i = 0; i < sizeof(search_name); i++)
										if (iswalpha(search_name[i]))
											search_name[i] = towlower(search_name[i]);

									if (main_name.find(search_name) != std::string::npos)
									{
										SkinChanger::displayKits.at(display_index) = main_kits.at(i);
										display_index++;
									}
								}

								SkinChanger::displayKits.erase(SkinChanger::displayKits.begin() + display_index, SkinChanger::displayKits.end());
							}
							else
							{
								if (strcmp(search_skins, crypt_str(""))) //-V526
								{
									for (auto i = 0; i < main_kits.size(); i++)
									{
										auto main_name = main_kits.at(i).name;

										for (auto i = 0; i < main_name.size(); i++)
											if (iswalpha(main_name.at(i)))
												main_name.at(i) = towlower(main_name.at(i));

										char search_name[64];
										strcpy_s(search_name, sizeof(search_name), search_skins);

										for (auto i = 0; i < sizeof(search_name); i++)
											if (iswalpha(search_name[i]))
												search_name[i] = towlower(search_name[i]);

										if (main_name.find(search_name) != std::string::npos)
										{
											SkinChanger::displayKits.at(display_index) = main_kits.at(i);
											display_index++;
										}
									}

									SkinChanger::displayKits.erase(SkinChanger::displayKits.begin() + display_index, SkinChanger::displayKits.end());
								}
								else
									item_index = selected_entry.paint_kit_vector_index;
							}

							ui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
							if (!SkinChanger::displayKits.empty())
							{
								if (ui::ListBoxSkeet(crypt_str("##PAINTKITS"), &item_index, [](void* data, int idx, const char** out_text) //-V107
									{
										while (SkinChanger::displayKits.at(idx).name.find(crypt_str("¸")) != std::string::npos) //-V807
											SkinChanger::displayKits.at(idx).name.replace(SkinChanger::displayKits.at(idx).name.find(crypt_str("¸")), 2, crypt_str("å"));

										*out_text = SkinChanger::displayKits.at(idx).name.c_str();
										return true;
									}, nullptr, SkinChanger::displayKits.size(), SkinChanger::displayKits.size() > 9 ? 9 : SkinChanger::displayKits.size()) || !zzall_skins[c_menu::get().current_profile])
								{
									SkinChanger::scheduleHudUpdate();
									need_update[c_menu::get().current_profile] = true;

									auto i = 0;

									while (i < main_kits.size())
									{
										if (main_kits.at(i).id == SkinChanger::displayKits.at(item_index).id)
										{
											selected_entry.paint_kit_vector_index = i;
											break;
										}

										i++;
									}

								}
							}
							ui::PopStyleVar();
							ui::Text("Seed");
							if (ui::InputInt(crypt_str("##Seed"), &selected_entry.seed, 1, 100))
								SkinChanger::scheduleHudUpdate();
							ui::Text("StatTrak");
							if (ui::InputInt(crypt_str("##StatTrak"), &selected_entry.stat_trak, 1, 15))
								SkinChanger::scheduleHudUpdate();
							//ui::SetCursorPosX(ui::GetCursorPosX() - 125 - 111);
							if (ui::SliderFloatOT(crypt_str("Wear"), &selected_entry.wear, 0.0f, 1.0f, "0.2f"))
								drugs = true;
							else if (drugs)
							{
								SkinChanger::scheduleHudUpdate();
								drugs = false;
							}


							//ui::SetCursorPosX(ui::GetCursorPosX() - 125 - 111);
							if (ui::SingleSelectOT(crypt_str("Quality"), &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
								{
									*out_text = game_data::quality_names[idx].name;
									return true;
								}, IM_ARRAYSIZE(game_data::quality_names)))
								SkinChanger::scheduleHudUpdate();

								if (c_menu::get().current_profile != 1)
								{
									if (!g_cfg.skins.custom_name_tag[c_menu::get().current_profile].empty())
										strcpy_s(selected_entry.custom_name, sizeof(selected_entry.custom_name), g_cfg.skins.custom_name_tag[c_menu::get().current_profile].c_str());

									ui::Text(crypt_str("Name Tag"));
									ui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

									if (ui::InputText(crypt_str("##nametag"), selected_entry.custom_name, sizeof(selected_entry.custom_name)))
									{
										g_cfg.skins.custom_name_tag[c_menu::get().current_profile] = selected_entry.custom_name;
										SkinChanger::scheduleHudUpdate();
									}

									ui::PopStyleVar();
								}

								//ui::PopItemWidth();

								//ui::EndGroup();



								//ui::BeginGroup();
								//if (ui::ImageButton(all_skins[current_profile], ImVec2(190 * dpi_scale, 155 * dpi_scale)))
								//{
								//	// maybe i will do smth later where, who knows :/
								//}

								if (ui::ButtonSkeet(crypt_str("Close"), ImVec2(198, 26)))
								{
									// start animation
									active_animation = true;
									next_id = -1;
									leave = true;
								}
								//ui::EndGroup();

								// update element
								selected_entry.update();

								// we need to reset profile in the end to prevent render images with massive's index == -1
								if (leave && (preview_reverse || !active_animation))
								{
									ui::SetScrollY(0);
									c_menu::get().current_profile = next_id;
									leave = false;
								}

						}
					}
				}
				ui::EndChildOT();

			


			}


		}







		if (tab == 2)
		{
			oldwindowsize_x = 575;
			oldwindowsize_y = 620;
			static int visual_sub = 0;
			



			if (finish == true)
			{
				ui::GetWindowDrawList()->AddRectFilled(ImVec2(wp.x + 15, wp.y + 80), ImVec2(wp.x + ws.x - 15, wp.y + 130), ImColor(32, 32, 38, alphas), 5);
				ui::GetWindowDrawList()->AddRect(ImVec2(wp.x + 15, wp.y + 80), ImVec2(wp.x + ws.x - 15, wp.y + 130), ImColor(46, 48, 56, alphas), 5);

				ui::TabButton_littOT("Enemy", "", &visual_sub, 0, 7, skeet_menu.icons);
				ui::TabButton_littOT("Team", "", &visual_sub, 1, 7, skeet_menu.icons);
				ui::TabButton_littOT("Local", "", &visual_sub, 2, 7, skeet_menu.icons);
				ui::TabButton_littOT("Effects", "", &visual_sub, 3, 7, skeet_menu.icons);
				ui::TabButton_littOT("Other", "", &visual_sub, 4, 7, skeet_menu.icons);



				static float slid;
				if (sub_legit)
					hooks::legit_weapon = sub_legit - 1;

				static std::string valuable_shit1, valuable_shit2;
				static int player = 0;
				if (visual_sub < 3)
				{
					player = visual_sub;
					valuable_shit1 = "Visuals";
					valuable_shit2 = "Colored models";
				}
				else if (visual_sub == 3)
				{
					valuable_shit1 = "Some";
					valuable_shit2 = "No";
				}
				else if (visual_sub == 4)
				{
					valuable_shit1 = "Effects";
					valuable_shit2 = "Other";
				}

				ui::SetNextWindowPos(ImVec2(wp.x + 15, wp.y + 145));
				ui::BeginChildOT(valuable_shit1.c_str(), ImVec2(260, 410));
				{
					if (visual_sub < 3)
					{
						ui::CheckboxOT(crypt_str("Bounding box"), &g_cfg.player.type[player].box);
						ui::ColorEdit4Fix(crypt_str("##boxcolor"), &g_cfg.player.type[player].box_color, ImGuiWindowFlags_NoTitleBar);
						ui::CheckboxOT(crypt_str("Name"), &g_cfg.player.type[player].name);
						ui::ColorEdit4Fix(crypt_str("##namecolor"), &g_cfg.player.type[player].name_color, ImGuiWindowFlags_NoTitleBar);
						ui::CheckboxOT(crypt_str("Health bar"), &g_cfg.player.type[player].health);
						ui::CheckboxOT(crypt_str("Health color"), &g_cfg.player.type[player].custom_health_color);
						ui::ColorEdit4Fix(crypt_str("##healthcolor"), &g_cfg.player.type[player].health_color, ImGuiWindowFlags_NoTitleBar);



						draw_multicomboOT(crypt_str("Flags"), g_cfg.player.type[player].flags, flags, ARRAYSIZE(flags), preview);
						draw_multicomboOT(crypt_str("Weapon"), g_cfg.player.type[player].weapon, weaponplayer, ARRAYSIZE(weaponplayer), preview);


						if (g_cfg.player.type[player].weapon[WEAPON_ICON] || g_cfg.player.type[player].weapon[WEAPON_TEXT])
						{
							ui::Text(crypt_str("Color"));

							ui::ColorEdit4Fix(crypt_str("##weapcolor"), &g_cfg.player.type[player].weapon_color, ImGuiWindowFlags_NoTitleBar);
						}

						ui::CheckboxOT(crypt_str("Skeleton"), &g_cfg.player.type[player].skeleton);

						ui::ColorEdit4Fix(crypt_str("##skeletoncolor"), &g_cfg.player.type[player].skeleton_color, ImGuiWindowFlags_NoTitleBar);

						ui::CheckboxOT(crypt_str("Ammo bar"), &g_cfg.player.type[player].ammo);

						ui::ColorEdit4Fix(crypt_str("##ammocolor"), &g_cfg.player.type[player].ammobar_color, ImGuiWindowFlags_NoTitleBar);

						ui::CheckboxOT(crypt_str("Footsteps"), &g_cfg.player.type[player].footsteps);

						ui::ColorEdit4Fix(crypt_str("##footstepscolor"), &g_cfg.player.type[player].footsteps_color, ImGuiWindowFlags_NoTitleBar);

						if (g_cfg.player.type[player].footsteps)
						{
							ui::SliderIntOT(crypt_str("Thickness"), &g_cfg.player.type[player].thickness, 1, 10, "%.0f");
							ui::SliderIntOT(crypt_str("Radius"), &g_cfg.player.type[player].radius, 50, 500, "%.0f");
						}

						if (player == ENEMY || player == TEAM)
						{
							ui::CheckboxOT(crypt_str("Snap lines"), &g_cfg.player.type[player].snap_lines);

							ui::ColorEdit4Fix(crypt_str("##snapcolor"), &g_cfg.player.type[player].snap_lines_color, ImGuiWindowFlags_NoTitleBar);

							if (player == ENEMY)
							{
								if (g_cfg.ragebot.enable)
								{
									ui::CheckboxOT(crypt_str("Aimbot points"), &g_cfg.player.show_multi_points);

									ui::ColorEdit4Fix(crypt_str("##showmultipointscolor"), &g_cfg.player.show_multi_points_color, ImGuiWindowFlags_NoTitleBar);
								}

								ui::CheckboxOT(crypt_str("Aimbot hitboxes"), &g_cfg.player.lag_hitbox);

								ui::ColorEdit4Fix(crypt_str("##lagcompcolor"), &g_cfg.player.lag_hitbox_color, ImGuiWindowFlags_NoTitleBar);
							}
						}
						else
						{
							ui::ComboOT(crypt_str("Player model T"), &g_cfg.player.player_model_t, player_model_t);

							ui::ComboOT(crypt_str("Player model CT"), &g_cfg.player.player_model_ct, player_model_ct);
						}





						if (player == ENEMY)
						{
							ui::CheckboxOT(crypt_str("OOF arrows"), &g_cfg.player.arrows);

							ui::ColorEdit4Fix(crypt_str("##arrowscolor"), &g_cfg.player.arrows_color, ImGuiWindowFlags_NoTitleBar);

							if (g_cfg.player.arrows)
							{
								ui::SliderIntOT(crypt_str("Arrows distance"), &g_cfg.player.distance, 1, 100, "%.0f");
								ui::SliderIntOT(crypt_str("Arrows size"), &g_cfg.player.size, 1, 100, "%.0f");
							}
						}
					}
					else if (visual_sub == 3)
					{
						ui::CheckboxOT(crypt_str("Rare animations"), &g_cfg.skins.rare_animations); ui::Spacing();
						ui::SliderIntOT(crypt_str("Viewmodel field of view"), &g_cfg.esp.viewmodel_fov, 0, 89, "%.0f");
						ui::SliderIntOT(crypt_str("Viewmodel X"), &g_cfg.esp.viewmodel_x, -50, 50, "%.0f");
						ui::SliderIntOT(crypt_str("Viewmodel Y"), &g_cfg.esp.viewmodel_y, -50, 50, "%.0f");
						ui::SliderIntOT(crypt_str("Viewmodel Z"), &g_cfg.esp.viewmodel_z, -50, 50, "%.0f");
						ui::SliderIntOT(crypt_str("Viewmodel roll"), &g_cfg.esp.viewmodel_roll, -180, 180, "%.0f");




						ui::CheckboxOT(crypt_str("Rain"), &g_cfg.esp.rain); ui::Spacing();
						ui::CheckboxOT(crypt_str("Full bright"), &g_cfg.esp.bright); ui::Spacing();

						ui::ComboOT(crypt_str("Skybox"), &g_cfg.esp.skybox, skybox);

						ui::Text(crypt_str("Color"));

						ui::ColorEdit4Fix(crypt_str("##skyboxcolor"), &g_cfg.esp.skybox_color, ImGuiWindowFlags_NoTitleBar);

						if (g_cfg.esp.skybox == 21)
						{
							static char sky_custom[64] = "\0";

							if (!g_cfg.esp.custom_skybox.empty())
								strcpy_s(sky_custom, sizeof(sky_custom), g_cfg.esp.custom_skybox.c_str());

							ui::Text(crypt_str("Name"));
							ui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

							if (ui::InputText(crypt_str("##customsky"), sky_custom, sizeof(sky_custom)))
								g_cfg.esp.custom_skybox = sky_custom;

							ui::PopStyleVar();
						}

					}
					else if (visual_sub == 4)
					{
						draw_multicomboOT(crypt_str("Indicators"), g_cfg.esp.indicators, indicators, ARRAYSIZE(indicators), preview);


						draw_multicomboOT(crypt_str("Removals"), g_cfg.esp.removals, removals, ARRAYSIZE(removals), preview);

						if (g_cfg.esp.removals[REMOVALS_ZOOM])
						{
							ui::CheckboxOT(crypt_str("Fix zoom sensivity"), &g_cfg.esp.fix_zoom_sensivity); ui::Spacing();
						}

						ui::CheckboxOT(crypt_str("Grenade prediction"), &g_cfg.esp.grenade_prediction);

						ui::ColorEdit4Fixed("##box_color", &g_cfg.esp.grenade_prediction_color, ImGuiWindowFlags_NoTitleBar);

						if (g_cfg.esp.grenade_prediction)
						{
							ui::CheckboxSkeet(crypt_str("On click"), &g_cfg.esp.on_click);
							ui::ColorEdit4Fixed(crypt_str("##tracergrenpredcolor"), &g_cfg.esp.grenade_prediction_tracer_color, ImGuiWindowFlags_NoTitleBar);
						}

						ui::CheckboxOT(crypt_str("Grenade projectiles"), &g_cfg.esp.projectiles); ui::Spacing();

						if (g_cfg.esp.projectiles)
							draw_multicomboOT(crypt_str("Grenade ESP"), g_cfg.esp.grenade_esp, proj_combo, ARRAYSIZE(proj_combo), preview);

						if (g_cfg.esp.grenade_esp[GRENADE_ICON] || g_cfg.esp.grenade_esp[GRENADE_TEXT])
						{
							ui::Text(crypt_str("Color"));

							ui::ColorEdit4Fixed(crypt_str("##projectcolor"), &g_cfg.esp.projectiles_color, ImGuiWindowFlags_NoTitleBar);
						}

						if (g_cfg.esp.grenade_esp[GRENADE_BOX])
						{
							ui::Text(crypt_str("Box color"));

							ui::ColorEdit4Fixed(crypt_str("##grenade_box_color"), &g_cfg.esp.grenade_box_color, ImGuiWindowFlags_NoTitleBar);
						}

						if (g_cfg.esp.grenade_esp[GRENADE_GLOW])
						{
							ui::Text(crypt_str("Glow color"));

							ui::ColorEdit4Fixed(crypt_str("##grenade_glow_color"), &g_cfg.esp.grenade_glow_color, ImGuiWindowFlags_NoTitleBar);
						}

						ui::CheckboxOT(crypt_str("Fire timer"), &g_cfg.esp.molotov_timer);

						ui::ColorEdit4Fixed(crypt_str("##molotovcolor"), &g_cfg.esp.molotov_timer_color, ImGuiWindowFlags_NoTitleBar);

						ui::CheckboxOT(crypt_str("Smoke timer"), &g_cfg.esp.smoke_timer);

						ui::ColorEdit4Fixed(crypt_str("##smokecolor"), &g_cfg.esp.smoke_timer_color, ImGuiWindowFlags_NoTitleBar);

					}

				}
				ui::EndChildOT();


				ui::SetNextWindowPos(ImVec2(wp.x + 300, wp.y + 145));
				ui::BeginChildOT(valuable_shit2.c_str(), ImVec2(260, 410));
				{

					if (visual_sub < 3)
					{
						if (player == LOCAL)
							ui::ComboOT(crypt_str("Type"), &g_cfg.player.local_chams_type, local_chams_type);

						if (player != LOCAL || !g_cfg.player.local_chams_type)
							draw_multicomboOT(crypt_str("Chams"), g_cfg.player.type[player].chams, g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] ? chamsvisact : chamsvis, g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] ? ARRAYSIZE(chamsvisact) : ARRAYSIZE(chamsvis), preview);

						if (g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] || player == LOCAL && g_cfg.player.local_chams_type) //-V648
						{
							if (player == LOCAL && g_cfg.player.local_chams_type)
							{
								ui::CheckboxOT(crypt_str("Enable desync chams"), &g_cfg.player.fake_chams_enable); ui::Spacing();
								ui::CheckboxOT(crypt_str("Visualize lag"), &g_cfg.player.visualize_lag); ui::Spacing();
								ui::CheckboxOT(crypt_str("Layered"), &g_cfg.player.layered); ui::Spacing();

								ui::ComboOT(crypt_str("Player chams material"), &g_cfg.player.fake_chams_type, chamstype);

								ui::Text(crypt_str("Color "));

								ui::ColorEdit4Fix(crypt_str("##fakechamscolor"), &g_cfg.player.fake_chams_color, ImGuiWindowFlags_NoTitleBar);

								if (g_cfg.player.fake_chams_type != 6)
								{
									ui::CheckboxOT(crypt_str("Double material "), &g_cfg.player.fake_double_material);

									ui::ColorEdit4Fix(crypt_str("##doublematerialcolor"), &g_cfg.player.fake_double_material_color, ImGuiWindowFlags_NoTitleBar);
								}

								ui::CheckboxOT(crypt_str("Animated material"), &g_cfg.player.fake_animated_material);

								ui::ColorEdit4Fix(crypt_str("##animcolormat"), &g_cfg.player.fake_animated_material_color, ImGuiWindowFlags_NoTitleBar);
							}
							else
							{
								ui::ComboOT(crypt_str("Player chams material"), &g_cfg.player.type[player].chams_type, chamstype);

								if (g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE])
								{
									ui::Text(crypt_str("Visible"));

									ui::ColorEdit4Fix(crypt_str("##chamsvisible"), &g_cfg.player.type[player].chams_color, ImGuiWindowFlags_NoTitleBar);
								}

								if (g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] && g_cfg.player.type[player].chams[PLAYER_CHAMS_INVISIBLE])
								{
									ui::Text(crypt_str("Invisible"));

									ui::ColorEdit4Fix(crypt_str("##chamsinvisible"), &g_cfg.player.type[player].xqz_color, ImGuiWindowFlags_NoTitleBar);
								}

								if (g_cfg.player.type[player].chams_type != 6)
								{
									ui::CheckboxOT(crypt_str("Double material "), &g_cfg.player.type[player].double_material);

									ui::ColorEdit4Fix(crypt_str("##doublematerialcolor"), &g_cfg.player.type[player].double_material_color, ImGuiWindowFlags_NoTitleBar);
								}

								ui::CheckboxOT(crypt_str("Animated material"), &g_cfg.player.type[player].animated_material);

								ui::ColorEdit4Fix(crypt_str("##animcolormat"), &g_cfg.player.type[player].animated_material_color, ImGuiWindowFlags_NoTitleBar);

								if (player == ENEMY)
								{
									ui::CheckboxOT(crypt_str("Backtrack chams"), &g_cfg.player.backtrack_chams); ui::Spacing();

									if (g_cfg.player.backtrack_chams)
									{
										ui::ComboOT(crypt_str("Backtrack chams material"), &g_cfg.player.backtrack_chams_material, chamstype);

										ui::Text(crypt_str("Color"));

										ui::ColorEdit4Fix(crypt_str("##backtrackcolor"), &g_cfg.player.backtrack_chams_color, ImGuiWindowFlags_NoTitleBar);
									}
								}
							}
						}

						if (player == ENEMY || player == TEAM)
						{
							ui::CheckboxOT(crypt_str("Ragdoll chams"), &g_cfg.player.type[player].ragdoll_chams); ui::Spacing();

							if (g_cfg.player.type[player].ragdoll_chams)
							{
								ui::ComboOT(crypt_str("Ragdoll chams material"), &g_cfg.player.type[player].ragdoll_chams_material, chamstype);

								ui::Text(crypt_str("Color"));

								ui::ColorEdit4Fix(crypt_str("##ragdollcolor"), &g_cfg.player.type[player].ragdoll_chams_color, ImGuiWindowFlags_NoTitleBar);
							}
						}
						else if (!g_cfg.player.local_chams_type)
						{
							ui::CheckboxOT(crypt_str("Transparency in scope"), &g_cfg.player.transparency_in_scope); ui::Spacing();

							if (g_cfg.player.transparency_in_scope)
								ui::SliderFloatOT(crypt_str("Alpha"), &g_cfg.player.transparency_in_scope_amount, 0.0f, 1.0f, "%.0f");
						}



						ui::CheckboxOT(crypt_str("Glow"), &g_cfg.player.type[player].glow); ui::Spacing();

						if (g_cfg.player.type[player].glow)
						{
							ui::ComboOT(crypt_str("Glow type"), &g_cfg.player.type[player].glow_type, glowtype);
							ui::Text(crypt_str("Color"));

							ui::ColorEdit4Fix(crypt_str("##glowcolor"), &g_cfg.player.type[player].glow_color, ImGuiWindowFlags_NoTitleBar);
						}




						ui::CheckboxOT(crypt_str("Arms chams"), &g_cfg.esp.arms_chams);

						ui::ColorEdit4Fix(crypt_str("##armscolor"), &g_cfg.esp.arms_chams_color, ImGuiWindowFlags_NoTitleBar);


						ui::ComboOT(crypt_str("Arms chams material"), &g_cfg.esp.arms_chams_type, chamstype);

						if (g_cfg.esp.arms_chams_type != 6)
						{
							ui::CheckboxOT(crypt_str("Arms double material "), &g_cfg.esp.arms_double_material);

							ui::ColorEdit4Fix(crypt_str("##armsdoublematerial"), &g_cfg.esp.arms_double_material_color, ImGuiWindowFlags_NoTitleBar);
						}

						ui::CheckboxOT(crypt_str("Arms animated material "), &g_cfg.esp.arms_animated_material);

						ui::ColorEdit4Fix(crypt_str("##armsanimatedmaterial"), &g_cfg.esp.arms_animated_material_color, ImGuiWindowFlags_NoTitleBar);



						ui::CheckboxOT(crypt_str("Weapon chams"), &g_cfg.esp.weapon_chams);

						ui::ColorEdit4Fix(crypt_str("##weaponchamscolors"), &g_cfg.esp.weapon_chams_color, ImGuiWindowFlags_NoTitleBar);

						ui::ComboOT(crypt_str("Weapon chams material"), &g_cfg.esp.weapon_chams_type, chamstype);

						if (g_cfg.esp.weapon_chams_type != 6)
						{
							ui::CheckboxOT(crypt_str("Double material "), &g_cfg.esp.weapon_double_material);

							ui::ColorEdit4Fix(crypt_str("##weapondoublematerial"), &g_cfg.esp.weapon_double_material_color, ImGuiWindowFlags_NoTitleBar);
						}

						ui::CheckboxOT(crypt_str("Animated material "), &g_cfg.esp.weapon_animated_material);

						ui::ColorEdit4Fix(crypt_str("##weaponanimatedmaterial"), &g_cfg.esp.weapon_animated_material_color, ImGuiWindowFlags_NoTitleBar);



					}
					else if (visual_sub == 3)
					{

						ui::CheckboxOT(crypt_str("Color modulation"), &g_cfg.esp.nightmode); ui::Spacing();
						if (g_cfg.esp.nightmode)
						{
							ui::Text(crypt_str("World color"));

							ui::ColorEdit4Fix(crypt_str("##worldcolor"), &g_cfg.esp.world_color, ImGuiWindowFlags_NoTitleBar);

							ui::Text(crypt_str("Props color"));

							ui::ColorEdit4Fix(crypt_str("##propscolor"), &g_cfg.esp.props_color, ImGuiWindowFlags_NoTitleBar);
						}

						ui::CheckboxOT(crypt_str("World modulation"), &g_cfg.esp.world_modulation); ui::Spacing();

						if (g_cfg.esp.world_modulation)
						{
							ui::SliderFloatOT(crypt_str("Bloom"), &g_cfg.esp.bloom, 0.0f, 750.0f, "%.0f");
							ui::SliderFloatOT(crypt_str("Exposure"), &g_cfg.esp.exposure, 0.0f, 2000.0f, "%.0f");
							ui::SliderFloatOT(crypt_str("Ambient"), &g_cfg.esp.ambient, 0.0f, 1500.0f, "%.0f");
						}

						ui::CheckboxOT(crypt_str("Fog modulation"), &g_cfg.esp.fog);

						if (g_cfg.esp.fog)
						{
							ui::SliderIntOT(crypt_str("Distance"), &g_cfg.esp.fog_distance, 0, 2500, "%.0f");
							ui::SliderIntOT(crypt_str("Density"), &g_cfg.esp.fog_density, 0, 100, "%.0f");

							ui::Text(crypt_str("Color"));

							ui::ColorEdit4Fix(crypt_str("##fogcolor"), &g_cfg.esp.fog_color, ImGuiWindowFlags_NoTitleBar);
						}



					}
					else if (visual_sub == 4)
					{
						ui::CheckboxOT(crypt_str("Bomb indicator"), &g_cfg.esp.bomb_timer); ui::Spacing();
						draw_multicomboOT(crypt_str("Weapon ESP"), g_cfg.esp.weapon, weaponesp, ARRAYSIZE(weaponesp), preview);

						if (g_cfg.esp.weapon[WEAPON_ICON] || g_cfg.esp.weapon[WEAPON_TEXT] || g_cfg.esp.weapon[WEAPON_DISTANCE])
						{
							ui::Text(crypt_str("Color"));

							ui::ColorEdit4Fixed(crypt_str("##weaponcolor"), &g_cfg.esp.weapon_color, ImGuiWindowFlags_NoTitleBar);
						}

						if (g_cfg.esp.weapon[WEAPON_BOX])
						{
							ui::Text(crypt_str("Box color"));

							ui::ColorEdit4Fixed(crypt_str("##weaponboxcolor"), &g_cfg.esp.box_color, ImGuiWindowFlags_NoTitleBar);
						}

						if (g_cfg.esp.weapon[WEAPON_GLOW])
						{
							ui::Text(crypt_str("Glow color"));

							ui::ColorEdit4Fixed(crypt_str("##weaponglowcolor"), &g_cfg.esp.weapon_glow_color, ImGuiWindowFlags_NoTitleBar);
						}

						if (g_cfg.esp.weapon[WEAPON_AMMO])
						{
							ui::Text(crypt_str("Ammo bar color"));

							ui::ColorEdit4Fixed(crypt_str("##weaponammocolor"), &g_cfg.esp.weapon_ammo_color, ImGuiWindowFlags_NoTitleBar);
						}

						ui::CheckboxOT(crypt_str("Client bullet impacts"), &g_cfg.esp.client_bullet_impacts);

						ui::ColorEdit4Fixed(crypt_str("##clientbulletimpacts"), &g_cfg.esp.client_bullet_impacts_color, ImGuiWindowFlags_NoTitleBar);

						ui::CheckboxOT(crypt_str("Server bullet impacts"), &g_cfg.esp.server_bullet_impacts);

						ui::ColorEdit4Fixed(crypt_str("##serverbulletimpacts"), &g_cfg.esp.server_bullet_impacts_color, ImGuiWindowFlags_NoTitleBar);

						ui::CheckboxOT(crypt_str("Local bullet tracers"), &g_cfg.esp.bullet_tracer);

						ui::ColorEdit4Fixed(crypt_str("##bulltracecolor"), &g_cfg.esp.bullet_tracer_color, ImGuiWindowFlags_NoTitleBar);

						ui::CheckboxOT(crypt_str("Enemy bullet tracers"), &g_cfg.esp.enemy_bullet_tracer);


						ui::ColorEdit4Fixed(crypt_str("##enemybulltracecolor"), &g_cfg.esp.enemy_bullet_tracer_color, ImGuiWindowFlags_NoTitleBar);
						draw_multicomboOT(crypt_str("Hit marker"), g_cfg.esp.hitmarker, hitmarkers, ARRAYSIZE(hitmarkers), preview);
						ui::CheckboxOT(crypt_str("Damage marker"), &g_cfg.esp.damage_marker); ui::Spacing();
						ui::CheckboxOT(crypt_str("Kill effect"), &g_cfg.esp.kill_effect); ui::Spacing();

						if (g_cfg.esp.kill_effect)
							ui::SliderFloatOT(crypt_str("Duration"), &g_cfg.esp.kill_effect_duration, 0.01f, 3.0f, "%.0f");
						static bool a1qsagd;
						ui::Text("Thirdperson");
						ui::KeybindSkeet("#q1qq1asgf", &g_cfg.misc.thirdperson_toggle.key, &g_cfg.misc.thirdperson_toggle.mode);

						ui::CheckboxOT(crypt_str("Thirdperson when spectating"), &g_cfg.misc.thirdperson_when_spectating); ui::Spacing();

						if (g_cfg.misc.thirdperson_toggle.key > KEY_NONE&& g_cfg.misc.thirdperson_toggle.key < KEY_MAX)
							ui::SliderIntOT(crypt_str("Thirdperson distance"), &g_cfg.misc.thirdperson_distance, 100, 300, "%.0f");

						ui::SliderIntOT(crypt_str("Field of view"), &g_cfg.esp.fov, 0, 89, "%.0f");
						ui::CheckboxOT(crypt_str("Taser range"), &g_cfg.esp.taser_range); ui::Spacing();
						ui::CheckboxOT(crypt_str("Show spread"), &g_cfg.esp.show_spread); ui::Spacing();

						ui::ColorEdit4Fixed(crypt_str("##spredcolor"), &g_cfg.esp.show_spread_color, ImGuiWindowFlags_NoTitleBar);
						ui::CheckboxOT(crypt_str("Penetration crosshair"), &g_cfg.esp.penetration_reticle);


					}





				}
				ui::EndChildOT();








			}



		}





		if (tab == 4)
		{
			oldwindowsize_x = 575;
			oldwindowsize_y = 550;
			static int misc_sub = 0;
			if (finish == true)
			{
				ui::GetWindowDrawList()->AddRectFilled(ImVec2(wp.x + 15, wp.y + 80), ImVec2(wp.x + ws.x - 15, wp.y + 130), ImColor(32, 32, 38, alphas), 5);
				ui::GetWindowDrawList()->AddRect(ImVec2(wp.x + 15, wp.y + 80), ImVec2(wp.x + ws.x - 15, wp.y + 130), ImColor(46, 48, 56, alphas), 5);


				ui::TabButton_littOT("General", "q", &misc_sub, 0, 3, skeet_menu.icons);
				ui::TabButton_littOT("Settings", "", &misc_sub, 1, 3, skeet_menu.icons);
				ui::TabButton_littOT("Lua", "", &misc_sub, 2, 3, skeet_menu.icons); //dont know what letter use xd

				static ImVec2 verycode;

				if (misc_sub == 2)
				{
					verycode = ImVec2(260, 280);
				}
				else
					verycode = ImVec2(550, 330);
				
				ui::SetNextWindowPos(ImVec2(wp.x + 15, wp.y + 145));
				ui::BeginChildOT("Misc", verycode);
				{



					if (misc_sub == 1)
					{
						static auto should_update = true;

						if (should_update)
						{
							should_update = false;

							cfg_manager->config_files();
							zzfiles = cfg_manager->files;

						}

						if (ui::ButtonSkeet(crypt_str("Open configs folder"), ImVec2(zpfix + verycode.x / 2, 0)))
						{
							std::string folder;

							auto get_dir = [&folder]() -> void
							{
								static TCHAR path[MAX_PATH];

								if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
									folder = std::string(path) + crypt_str("\\SHONAX-PROJECT\\Configs\\");

								CreateDirectory(folder.c_str(), NULL);
							};

							get_dir();
							ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
						}

						ui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
						ui::ListBoxConfigArraySkeet(crypt_str("##CONFIGS"), &g_cfg.selected_config, zzfiles, 7);
						ui::PopStyleVar();

						if (ui::ButtonSkeet(crypt_str("Refresh configs"), ImVec2(zpfix + verycode.x / 2, 0)))
						{
							cfg_manager->config_files();
							zzfiles = cfg_manager->files;
						}

						static char config_name[64] = "\0";

						ui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
						ui::Text("Cfg name");
						ui::InputText(crypt_str("##configname"), config_name, sizeof(config_name));
						ui::PopStyleVar();

						if (ui::ButtonSkeet(crypt_str("Create config"), ImVec2(zpfix + verycode.x / 2, 0)))
						{
							g_cfg.new_config_name = config_name;
							zzadd_config();
						}

						static auto next_save = false;
						static auto prenext_save = false;
						static auto clicked_sure = false;
						static auto save_time = m_globals()->m_realtime;
						static auto save_alpha = 1.0f;

						save_alpha = math::clamp(save_alpha + (4.f * ui::GetIO().DeltaTime * (!prenext_save ? 1.f : -1.f)), 0.01f, 1.f);
						//ui::PushStyleVar(ImGuiStyleVar_Alpha, save_alpha * public_alpha * (1.f - preview_alpha));

						if (!next_save)
						{
							clicked_sure = false;

							if (prenext_save && save_alpha <= 0.01f)
								next_save = true;

							if (ui::ButtonSkeet(crypt_str("Save config"), ImVec2(zpfix + verycode.x / 2, 0)))
							{
								save_time = m_globals()->m_realtime;
								prenext_save = true;
							}
						}
						else
						{
							if (prenext_save && save_alpha <= 0.01f)
							{
								prenext_save = false;
								next_save = !clicked_sure;
							}

							if (ui::ButtonSkeet(crypt_str("Are you sure?"), ImVec2(zpfix + verycode.x / 2, 0)))
							{
								zzsave_config();
								prenext_save = true;
								clicked_sure = true;
							}

							if (!clicked_sure && m_globals()->m_realtime > save_time + 1.5f)
							{
								prenext_save = true;
								clicked_sure = true;
							}
						}

						//ui::PopStyleVar();

						if (ui::ButtonSkeet(crypt_str("Load config"), ImVec2(zpfix + verycode.x / 2, 0)))
							zzload_config();

						static auto next_delete = false;
						static auto prenext_delete = false;
						static auto clicked_sure_del = false;
						static auto delete_time = m_globals()->m_realtime;
						static auto delete_alpha = 1.0f;

						delete_alpha = math::clamp(delete_alpha + (4.f * ui::GetIO().DeltaTime * (!prenext_delete ? 1.f : -1.f)), 0.01f, 1.f);
						//ui::PushStyleVar(ImGuiStyleVar_Alpha, delete_alpha * public_alpha * (1.f - preview_alpha));

						if (!next_delete)
						{
							clicked_sure_del = false;

							if (prenext_delete && delete_alpha <= 0.01f)
								next_delete = true;

							if (ui::ButtonSkeet(crypt_str("Remove config"), ImVec2(zpfix + verycode.x / 2, 0)))
							{
								delete_time = m_globals()->m_realtime;
								prenext_delete = true;
							}
						}
						else
						{
							if (prenext_delete && delete_alpha <= 0.01f)
							{
								prenext_delete = false;
								next_delete = !clicked_sure_del;
							}

							if (ui::ButtonSkeet(crypt_str("Are you sure?"), ImVec2(zpfix + verycode.x / 2, 0)))
							{
								zzremove_config();
								prenext_delete = true;
								clicked_sure_del = true;
							}

							if (!clicked_sure_del && m_globals()->m_realtime > delete_time + 1.5f)
							{
								prenext_delete = true;
								clicked_sure_del = true;
							}
						}
					}

					if (misc_sub == 2)
					{
						static auto should_update = true;

						if (should_update)
						{
							should_update = false;
							zzscripts = c_lua::get().scripts;

							for (auto& current : zzscripts)
							{
								if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
									current.erase(current.size() - 5, 5);
								else if (current.size() >= 4)
									current.erase(current.size() - 4, 4);
							}
						}

						if (ui::ButtonSkeet(crypt_str("Open scripts folder"), ImVec2(zpfix, 0)))
						{
							std::string folder;

							auto get_dir = [&folder]() -> void
							{
								static TCHAR path[MAX_PATH];

								if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
									folder = std::string(path) + crypt_str("\\SHONAX-PROJECT\\Scripts\\");

								CreateDirectory(folder.c_str(), NULL);
							};

							get_dir();
							ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
						}

						ui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

						if (zzscripts.empty())
							ui::ListBoxConfigArraySkeet(crypt_str("##LUAS"), &zzselected_script, zzscripts, 7);
						else
						{
							auto backup_scripts = zzscripts;

							for (auto& script : zzscripts)
							{
								auto script_id = c_lua::get().get_script_id(script + crypt_str(".lua"));

								if (script_id == -1)
									continue;

								if (c_lua::get().loaded.at(script_id))
									zzscripts.at(script_id) += crypt_str(" [loaded]");
							}

							ui::ListBoxConfigArraySkeet(crypt_str("##LUAS"), &zzselected_script, zzscripts, 7);
							zzscripts = std::move(backup_scripts);
						}

						ui::PopStyleVar();

						if (ui::ButtonSkeet(crypt_str("Refresh scripts"), ImVec2(zpfix, 0)))
						{
							c_lua::get().refresh_scripts();
							zzscripts = c_lua::get().scripts;

							if (zzselected_script >= zzscripts.size())
								zzselected_script = zzscripts.size() - 1; //-V103

							for (auto& current : zzscripts)
							{
								if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
									current.erase(current.size() - 5, 5);
								else if (current.size() >= 4)
									current.erase(current.size() - 4, 4);
							}
						}

						//if (ui::Button(crypt_str("Edit script"), ImVec2(pfix, 0)))
						//{
						//	zloaded_editing_script = false;
						//	zediting_script = zscripts.at(zselected_script);
						//}

						if (ui::ButtonSkeet(crypt_str("Load script"), ImVec2(zpfix, 0)))
						{
							c_lua::get().load_script(zzselected_script);
							c_lua::get().refresh_scripts();

							zzscripts = c_lua::get().scripts;

							if (zzselected_script >= zzscripts.size())
								zzselected_script = zzscripts.size() - 1; //-V103

							for (auto& current : zzscripts)
							{
								if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
									current.erase(current.size() - 5, 5);
								else if (current.size() >= 4)
									current.erase(current.size() - 4, 4);
							}

							eventlogs::get().add(crypt_str("Loaded ") + zzscripts.at(zzselected_script) + crypt_str(" script"), false); //-V106
						}

						if (ui::ButtonSkeet(crypt_str("Unload script"), ImVec2(zpfix, 0)))
						{
							c_lua::get().unload_script(zzselected_script);
							c_lua::get().refresh_scripts();

							zzscripts = c_lua::get().scripts;

							if (zzselected_script >= zzscripts.size())
								zzselected_script = zzscripts.size() - 1; //-V103

							for (auto& current : zzscripts)
							{
								if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
									current.erase(current.size() - 5, 5);
								else if (current.size() >= 4)
									current.erase(current.size() - 4, 4);
							}

							eventlogs::get().add(crypt_str("Unloaded ") + zzscripts.at(zzselected_script) + crypt_str(" script"), false); //-V106
						}

						if (ui::ButtonSkeet(crypt_str("Reload all scripts"), ImVec2(zpfix, 0)))
						{
							c_lua::get().reload_all_scripts();
							c_lua::get().refresh_scripts();

							zzscripts = c_lua::get().scripts;

							if (zzselected_script >= zzscripts.size())
								zzselected_script = zzscripts.size() - 1; //-V103

							for (auto& current : zzscripts)
							{
								if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
									current.erase(current.size() - 5, 5);
								else if (current.size() >= 4)
									current.erase(current.size() - 4, 4);
							}
						}

						if (ui::ButtonSkeet(crypt_str("Unload all scripts"), ImVec2(zpfix, 0)))
						{
							c_lua::get().unload_all_scripts();
							c_lua::get().refresh_scripts();

							zzscripts = c_lua::get().scripts;

							if (zzselected_script >= zzscripts.size())
								zzselected_script = zzscripts.size() - 1; //-V103

							for (auto& current : zzscripts)
							{
								if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
									current.erase(current.size() - 5, 5);
								else if (current.size() >= 4)
									current.erase(current.size() - 4, 4);
							}
						}
					}


					if (misc_sub == 0)
					{

						ui::Columns(2);
						ui::NewLine();
						ui::CheckboxOT(crypt_str("Anti-untrusted"), &g_cfg.misc.anti_untrusted); ui::Spacing();
						ui::CheckboxOT(crypt_str("Rank reveal"), &g_cfg.misc.rank_reveal); ui::Spacing();
						ui::CheckboxOT(crypt_str("Unlock inventory access"), &g_cfg.misc.inventory_access); ui::Spacing();
						ui::CheckboxOT(crypt_str("Gravity ragdolls"), &g_cfg.misc.ragdolls); ui::Spacing();
						ui::CheckboxOT(crypt_str("Preserve killfeed"), &g_cfg.esp.preserve_killfeed); ui::Spacing();
						ui::CheckboxOT(crypt_str("Aspect ratio"), &g_cfg.misc.aspect_ratio); ui::Spacing();


						if (g_cfg.misc.aspect_ratio)
							ui::SliderFloatSkeet(crypt_str("Amount"), &g_cfg.misc.aspect_ratio_amount, 1.0f, 2.0f, "%.0f");


						ui::CheckboxOT(crypt_str("Watermark"), &g_cfg.menu.watermark); ui::Spacing();
						ui::CheckboxOT(crypt_str("Spectators list"), &g_cfg.misc.spectators_list); ui::Spacing();
						ui::ComboOT(crypt_str("Hitsound"), &g_cfg.esp.hitsound, sounds);
						ui::CheckboxOT(crypt_str("Killsound"), &g_cfg.esp.killsound); ui::Spacing();
						draw_multicomboOT(crypt_str("Logs"), g_cfg.misc.events_to_log, events, ARRAYSIZE(events), preview);

						draw_multicomboOT(crypt_str("Logs output"), g_cfg.misc.log_output, events_output, ARRAYSIZE(events_output), preview);

						if (g_cfg.misc.events_to_log[EVENTLOG_HIT] || g_cfg.misc.events_to_log[EVENTLOG_ITEM_PURCHASES] || g_cfg.misc.events_to_log[EVENTLOG_BOMB])
						{
							ui::Text(crypt_str("Color"));

							ui::ColorEdit4Fix(crypt_str("##logcolor"), &g_cfg.misc.log_color, ImGuiWindowFlags_NoTitleBar);
						}

						ui::NextColumn();
						ui::NewLine();

						ui::CheckboxOT(crypt_str("Show CS:GO logs"), &g_cfg.misc.show_default_log); ui::Spacing();

						ui::CheckboxOT(crypt_str("Automatic jump"), &g_cfg.misc.bunnyhop); ui::Spacing();
						ui::ComboOT(crypt_str("Automatic strafes"), &g_cfg.misc.airstrafe, strafes);
						ui::CheckboxOT(crypt_str("Crouch in air"), &g_cfg.misc.crouch_in_air); ui::Spacing();
						ui::CheckboxOT(crypt_str("Fast stop"), &g_cfg.misc.fast_stop); ui::Spacing();
						ui::CheckboxOT(crypt_str("Slide walk"), &g_cfg.misc.slidewalk); ui::Spacing();

						ui::CheckboxOT(crypt_str("Anti-screenshot"), &g_cfg.misc.anti_screenshot); ui::Spacing();
						ui::CheckboxOT(crypt_str("Clantag"), &g_cfg.misc.clantag_spammer); ui::Spacing();
						ui::CheckboxOT(crypt_str("Chat spam"), &g_cfg.misc.chat); ui::Spacing();
						ui::CheckboxOT(crypt_str("Enable buybot"), &g_cfg.misc.buybot_enable); ui::Spacing();

						if (g_cfg.misc.buybot_enable)
						{
							ui::ComboOT(crypt_str("Snipers"), &g_cfg.misc.buybot1, mainwep);

							ui::ComboOT(crypt_str("Pistols"), &g_cfg.misc.buybot2, secwep);

							draw_multicomboOT(crypt_str("Other"), g_cfg.misc.buybot3, grenades, ARRAYSIZE(grenades), preview);
						}


						ui::ComboOT(crypt_str("Menu"), &g_ctx.menu_type, { "Skeet", "Evolve", "Onetap v3" });

						ui::EndColumns();
					}

				

				}
				ui::EndChildOT();

				if (misc_sub == 2)
				{

					ui::SetNextWindowPos(ImVec2(wp.x + 300, wp.y + 145));
					ui::BeginChildOT("Lua elements", ImVec2(260, 310));
					{

						ui::CheckboxOT(crypt_str("Developer mode"), &g_cfg.scripts.developer_mode); ui::Spacing();
						ui::CheckboxOT(crypt_str("Allow HTTP requests"), &g_cfg.scripts.allow_http); ui::Spacing();
						ui::CheckboxOT(crypt_str("Allow files read or write"), &g_cfg.scripts.allow_file); ui::Spacing();


						auto previous_check_box = false;

						for (auto& current : c_lua::get().scripts)
						{
							auto& items = c_lua::get().items.at(c_lua::get().get_script_id(current));

							for (auto& item : items)
							{
								std::string item_name;

								auto first_point = false;
								auto item_str = false;

								for (auto& c : item.first)
								{
									if (c == '.')
									{
										if (first_point)
										{
											item_str = true;
											continue;
										}
										else
											first_point = true;
									}

									if (item_str)
										item_name.push_back(c);
								}

								switch (item.second.type)
								{
								case NEXT_LINE:
									previous_check_box = false;
									break;
								case CHECK_BOX:
									previous_check_box = true;
									ui::CheckboxOT(item_name.c_str(), &item.second.check_box_value); ui::Spacing();
									break;
								case COMBO_BOX:
									previous_check_box = false;
									ui::SingleSelectOT(item_name.c_str(), &item.second.combo_box_value, [](void* data, int idx, const char** out_text)
										{
											auto labels = (std::vector <std::string>*)data;
											*out_text = labels->at(idx).c_str(); //-V106
											return true;
										}, item.second.combo_box_labels.size());
									break;
								case SLIDER_INT:
									previous_check_box = false;
									ui::SliderIntOT(item_name.c_str(), &item.second.slider_int_value, item.second.slider_int_min, item.second.slider_int_max);
									break;
								case SLIDER_FLOAT:
									previous_check_box = false;
									ui::SliderFloatOT(item_name.c_str(), &item.second.slider_float_value, item.second.slider_float_min, item.second.slider_float_max, "%0.f");
									break;
								case COLOR_PICKER:
									if (previous_check_box)
										previous_check_box = false;
									else
										ui::Text((item_name + ' ').c_str());


									ui::ColorEdit4Fix((crypt_str("##") + item_name).c_str(), &item.second.color_picker_value, ImGuiWindowFlags_NoTitleBar);
									break;
								}
							}
						}
					}
					ui::EndChildOT();
				}



				

			}


		}





	}
	ui::PopFont();
	ui::EndOT();
	
}