#pragma once
#include "../includes.hpp"

class c_menu : public singleton<c_menu> {
public:
	void draw_skins();
	void draw( bool is_open );
	void menu_setup(ImGuiStyle &style);

	float dpi_scale = 1.f;

	ImFont* futura;
	ImFont* futura_large;
	ImFont* futura_small;

	ImFont* gotham;

	ImFont* ico_menu;
	ImFont* ico_bottom;
	ImFont* tabs;
	float public_alpha;
	IDirect3DDevice9* device;
	float color_buffer[4] = { 1.f, 1.f, 1.f, 1.f };
	int current_profile = -1;
private:
	struct {
		ImVec2 WindowPadding;
		float  WindowRounding;
		ImVec2 WindowMinSize;
		float  ChildRounding;
		float  PopupRounding;
		ImVec2 FramePadding;
		float  FrameRounding;
		ImVec2 ItemSpacing;
		ImVec2 ItemInnerSpacing;
		ImVec2 TouchExtraPadding;
		float  IndentSpacing;
		float  ColumnsMinSpacing;
		float  ScrollbarSize;
		float  ScrollbarRounding;
		float  GrabMinSize;
		float  GrabRounding;
		float  TabRounding;
		float  TabMinWidthForUnselectedCloseButton;
		ImVec2 DisplayWindowPadding;
		ImVec2 DisplaySafeAreaPadding;
		float  MouseCursorScale;
	} styles;

	bool update_dpi = false;
	bool update_scripts = false;
	void dpi_resize(float scale_factor, ImGuiStyle &style);

	int active_tab_index;
	ImGuiStyle style;
	int width = 730, height = 480;
	float child_height;

	float preview_alpha = 1.f;

	int active_tab;

	int rage_section;
	int legit_section;
	int visuals_section;
	int players_section;
	int misc_section;
	int settings_section;

	// we need to use 'int child' to seperate content in 2 childs
	void draw_ragebot(int child);
	

	void draw_legit(int child);
	 
	void draw_visuals(int child);

	

	void draw_players(int child);
	

	void draw_misc(int child);
	

	void draw_settings(int child);

	void draw_lua(int child);
	void draw_radar(int child);
	void draw_player_list(int child);

	std::string preview = crypt_str("None");
};



class menu {
public:
	ImFont* icons;
	LPDIRECT3DTEXTURE9 steam_image();
	LPDIRECT3DTEXTURE9 steam_image_friends(CSteamID SteamId);
	void initialize(IDirect3DDevice9* pDevice);
	void draw_begin();
	void draw();
	void draw_end();
	LPDIRECT3DTEXTURE9 image;
	bool is_menu_initialized();
	bool is_menu_opened();
	void set_menu_opened(bool v);
	void cjec();
	ImColor get_accent_color();
	IDirect3DTexture9* skinImage = nullptr;
	IDirect3DTexture9** friends_images;
	LPDIRECT3DTEXTURE9* custom_image;
	LPDIRECT3DTEXTURE9* load_custom_image();
	std::filesystem::path* lul;
	ImFont* tabs;
	ImFont* f1;
	ImFont* f2;
	ImFont* f3;
	ImFont* weapon_font;
private:
	bool m_bInitialized;
	bool m_bIsOpened;
	int m_nCurrentTab;
	/* user-defined configuration filename in miscellaneous tab */
	std::string	szConfigFile = "";
	/* current selected configuration in miscellaneous tab */
	int	iSelectedConfig = 0;
};

inline menu skeet_menu;



class otmenu
{
public:
	void render();
};
inline otmenu otmenuv3;

