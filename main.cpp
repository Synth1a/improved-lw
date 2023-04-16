// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <ShlObj.h>
#include <ShlObj_core.h>
#include "includes.hpp"
#include "utils\ctx.hpp"
#include "utils\recv.h"
#include "utils\imports.h"
//#include "utils\anti_debug.h"
#include "nSkinz\SkinChanger.h"
#include "minhook/minhook.h"
#
inline unsigned int get_virtual(void* _class, unsigned int index) { return static_cast<unsigned int>((*static_cast<int**>(_class))[index]); }


//#include "utils\sha-256.h"
//#include "utils\protect.h"

//using namespace jwt::params;

enum error_type
{
	ERROR_NONE,
	ERROR_DEBUG,
	ERROR_OPEN_KEY,
	ERROR_QUERY_DATA,
	ERROR_CONNECT,
	ERROR_1,
	ERROR_2,
	ERROR_3,
	ERROR_4,
	ERROR_5,
	ERROR_6,
	ERROR_7,
	ERROR_8,
	ERROR_9,
	ERROR_CHECK_HASH
};

PVOID base_address = nullptr;
//Anti_debugger anti_debugger;
volatile error_type error = ERROR_NONE;

LONG CALLBACK ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
__forceinline void crash(bool debug = false);
__forceinline void setup_render();
__forceinline void setup_netvars();
__forceinline void setup_skins();
__forceinline void setup_hooks();

DWORD WINAPI main(PVOID base)
{

	// grab some modules.
	auto m_kernel32_dll = PE::GetModule(HASH("kernel32.dll"));
	auto m_user32_dll = PE::GetModule(HASH("user32.dll"));
	auto m_shell32_dll = PE::GetModule(HASH("shell32.dll"));
	auto m_shlwapi_dll = PE::GetModule(HASH("shlwapi.dll"));
	auto m_client_dll = PE::GetModule(HASH("client.dll"));
	auto m_engine_dll = PE::GetModule(HASH("engine.dll"));
	auto m_vstdlib_dll = PE::GetModule(HASH("vstdlib.dll"));
	auto m_tier0_dll = PE::GetModule(HASH("tier0.dll"));
	auto m_shaderapidx9_dll = PE::GetModule(HASH("shaderapidx9.dll"));

	// import winapi functions.
	g_winapi.WideCharToMultiByte = PE::GetExport(m_kernel32_dll, HASH("WideCharToMultiByte")).as< WinAPI::WideCharToMultiByte_t >();
	g_winapi.MultiByteToWideChar = PE::GetExport(m_kernel32_dll, HASH("MultiByteToWideChar")).as< WinAPI::MultiByteToWideChar_t >();
	g_winapi.GetTickCount = PE::GetExport(m_kernel32_dll, HASH("GetTickCount")).as< WinAPI::GetTickCount_t >();
	g_winapi.VirtualProtect = PE::GetExport(m_kernel32_dll, HASH("VirtualProtect")).as< WinAPI::VirtualProtect_t >();
	g_winapi.VirtualQuery = PE::GetExport(m_kernel32_dll, HASH("VirtualQuery")).as< WinAPI::VirtualQuery_t >();
	g_winapi.CreateDirectoryA = PE::GetExport(m_kernel32_dll, HASH("CreateDirectoryA")).as< WinAPI::CreateDirectoryA_t >();
	g_winapi.SetWindowLongA = PE::GetExport(m_user32_dll, HASH("SetWindowLongA")).as< WinAPI::SetWindowLongA_t >();
	g_winapi.CallWindowProcA = PE::GetExport(m_user32_dll, HASH("CallWindowProcA")).as< WinAPI::CallWindowProcA_t >();
	g_winapi.SHGetFolderPathA = PE::GetExport(m_shell32_dll, HASH("SHGetFolderPathA")).as< WinAPI::SHGetFolderPathA_t >();
	g_winapi.PathAppendA = PE::GetExport(m_shlwapi_dll, HASH("PathAppendA")).as< WinAPI::PathAppendA_t >();



	g_ctx.signatures =
	{
		crypt_str("A1 ? ? ? ? 50 8B 08 FF 51 0C"),
		crypt_str("B9 ?? ?? ?? ?? A1 ?? ?? ?? ?? FF 10 A1 ?? ?? ?? ?? B9"),
		crypt_str("0F 11 05 ?? ?? ?? ?? 83 C8 01"),
		crypt_str("8B 0D ?? ?? ?? ?? 8B 46 08 68"),
		crypt_str("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10"),
		crypt_str("8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7"),
		crypt_str("A1 ? ? ? ? 8B 0D ? ? ? ? 6A 00 68 ? ? ? ? C6"),
		crypt_str("80 3D ? ? ? ? ? 53 56 57 0F 85"),
		crypt_str("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C"),
		crypt_str("80 3D ? ? ? ? ? 74 06 B8"),
		crypt_str("55 8B EC 83 E4 F0 B8 D8"),
		crypt_str("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C"),
		crypt_str("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6"),
		crypt_str("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"),
		crypt_str("56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 21"),
		crypt_str("55 8B EC 83 E4 F8 83 EC 5C 53 8B D9 56 57 83"),
		crypt_str("55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9"),
		crypt_str("57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"),
		crypt_str("55 8B EC 81 EC ? ? ? ? 53 8B D9 89 5D F8 80"),
		crypt_str("53 0F B7 1D ? ? ? ? 56"),
		crypt_str("8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6")
	};
	shonax_void();
	g_ctx.indexes =
	{
		5,
		33,
		339,
		218,
		219,
		34,
		157,
		75,
		460,
		482,
		452,
		483,
		284,
		223,
		246,
		27,
		17,
		123
	};

	while (!GetModuleHandle(crypt_str("serverbrowser.dll")))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	base_address = base;
	IdManager->GetClassIDALL();
	
	static TCHAR path[MAX_PATH];
	std::string folder;
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path);
	folder = std::string(path) + crypt_str("\\legendware\\");
	CreateDirectory(folder.c_str(), 0);

	//shonax


	/*some steam interfaces*/
	typedef uint32_t SteamPipeHandle;
	typedef uint32_t SteamUserHandle;
	SteamUserHandle hSteamUser = ((SteamUserHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamUser"))();
	SteamPipeHandle hSteamPipe = ((SteamPipeHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamPipe"))();
	SteamClient = ((ISteamClient * (__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamClient"))();
	SteamGameCoordinator = (ISteamGameCoordinator*)SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, "SteamGameCoordinator001");
	SteamUser = (ISteamUser*)SteamClient->GetISteamUser(hSteamUser, hSteamPipe, "SteamUser019");
	SteamFriends = SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
	static auto SteamInventory = SteamClient->GetISteamInventory(hSteamUser, hSteamPipe, "STEAMINVENTORY_INTERFACE_V002");
	auto _ = SteamFriends->GetLargeFriendAvatar(SteamUser->GetSteamID());
	SteamUtils = SteamClient->GetISteamUtils(hSteamPipe, "SteamUtils009");




	g_csgo.studioHdr = pattern::find(m_client_dll, ("8B 86 ? ? ? ? 89 44 24 10 85 C0")).add(2).to< size_t >();
	g_csgo.LockStudioHdr = pattern::find(m_client_dll, ("55 8B EC 51 53 8B D9 56 57 8D B3"));
	g_csgo.AnimOverlay = pattern::find(m_client_dll, ("8B 80 ? ? ? ? 8D 34 C8")).add(0x2).to< size_t >();
	g_csgo.SetAbsOrigin = pattern::find(m_client_dll, ("55 8B EC 83 E4 F8 51 53 56 57 8B F1"));
	g_csgo.SetAbsAngles = pattern::find(m_client_dll, ("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));
	g_csgo.AngleMatrix = pattern::find(m_client_dll, ("E8 ? ? ? ? 8B 07 89 46 0C")).rel32(0x1).as< AngleMatrix_t >();
	g_csgo.PlayerAnimState = pattern::find(m_client_dll, ("8B 8E ? ? ? ? 85 C9 74 3E")).add(2).to< size_t >();





	setup_sounds();

	
	setup_skins();

	
	setup_netvars();


	setup_render();


	cfg_manager->setup();

	c_lua::get().initialize();


	key_binds::get().initialize_key_binds();

	setup_hooks();
	Netvars::Netvars();
	
	std::cout << crypt_str("Done!");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	return EXIT_SUCCESS;

}


std::filesystem::path GetWorkingPath()
{
	std::filesystem::path fsWorkingPath;

	// get path to user documents
	if (PWSTR pszPathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0UL, nullptr, &pszPathToDocuments)))
	{
		fsWorkingPath.assign(pszPathToDocuments);
		fsWorkingPath.append((".base"));
		CoTaskMemFree(pszPathToDocuments);
	}

	return fsWorkingPath;
}


DWORD WINAPI nn(LPVOID lpThreadParameter)
{
	static int generate_number = 0;
	
	while (true)
	{
		for (auto& p : std::filesystem::directory_iterator(GetWorkingPath() / ("custom image\\")))
			generate_number++;
		g_ctx.generate_last_number = generate_number;
		if (!g_ctx.generate_last_number)
			g_ctx.custom_reload_picture = false;
		generate_number = 0;
	}
}

DWORD WINAPI custom(LPVOID lpThreadParameter)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
		if (!g_ctx.generate_last_number)
			g_ctx.custom_counter = 0;
		if (g_ctx.custom_counter != g_ctx.generate_last_number - 1)
			g_ctx.custom_counter++;
		else
			g_ctx.custom_counter = 0;
	}
}




BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		auto current_process = GetCurrentProcess();
		auto priority_class = GetPriorityClass(current_process);

		if (priority_class != HIGH_PRIORITY_CLASS && priority_class != REALTIME_PRIORITY_CLASS)
			SetPriorityClass(current_process, HIGH_PRIORITY_CLASS);


		//if (MessageBox(nullptr, "Minhooks?", "Choose", MB_OKCANCEL) == 1)
		//	g_ctx.test_hooks = true;

		g_ctx.test_hooks = true;

		auto path = GetWorkingPath();
		std::filesystem::create_directories(path);
		std::filesystem::create_directories(path / ("custom image\\"));
		//g_ctx.custom_saves = std::filesystem::directory_iterator(g_ctx.path / ("custom image\\"));
		CreateThread(nullptr, 0, main, hModule, 0, nullptr); //-V718 //-V513
		//CreateThread(nullptr, 0, nn, hModule, 0, nullptr); //-V718 //-V513
		//CreateThread(nullptr, 0, custom, hModule, 0, nullptr); //-V718 //-V513

	}

	return TRUE;
}



__forceinline void setup_render()
{
	static auto create_font = [](const char* name, int size, int weight, DWORD flags) -> vgui::HFont
	{
		g_ctx.last_font_name = name;

		auto font = m_surface()->FontCreate();
		m_surface()->SetFontGlyphSet(font, name, size, weight, 0, 0, flags);

		return font;
	};

	fonts[LOGS] = create_font(crypt_str("Lucida Console"), 10, FW_MEDIUM, FONTFLAG_DROPSHADOW);
	fonts[ESP] = create_font(crypt_str("Smallest Pixel-7"), 11, FW_MEDIUM, FONTFLAG_OUTLINE);
	fonts[NAME] = create_font(crypt_str("Verdana"), 12, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[SUBTABWEAPONS] = create_font(crypt_str("undefeated"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[KNIFES] = create_font(crypt_str("icomoon"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[GRENADES] = create_font(crypt_str("undefeated"), 20, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[INDICATORFONT] = create_font(crypt_str("Verdana"), 25, FW_HEAVY, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[DAMAGE_MARKER] = create_font(crypt_str("CrashNumberingGothic"), 15, FW_HEAVY, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);

	g_ctx.last_font_name.clear();
}

__forceinline void setup_netvars()
{
	netvars::get().tables.clear();
	auto client = m_client()->GetAllClasses();

	if (!client)
		return;

	while (client)
	{
		auto recvTable = client->m_pRecvTable;

		if (recvTable)
			netvars::get().tables.emplace(std::string(client->m_pNetworkName), recvTable);

		client = client->m_pNext;
	}
}

__forceinline void setup_skins()
{
	auto items = std::ifstream(crypt_str("csgo/scripts/items/items_game_cdn.txt"));
	auto gameItems = std::string(std::istreambuf_iterator <char> { items }, std::istreambuf_iterator <char> { });

	if (!items.is_open())
		return;

	items.close();
	memory.initialize();

	for (auto i = 0; i <= memory.itemSchema()->paintKits.lastElement; i++)
	{
		auto paintKit = memory.itemSchema()->paintKits.memory[i].value;

		if (paintKit->id == 9001)
			continue;

		auto itemName = m_localize()->FindSafe(paintKit->itemName.buffer + 1);
		auto itemNameLength = WideCharToMultiByte(CP_UTF8, 0, itemName, -1, nullptr, 0, nullptr, nullptr);

		if (std::string name(itemNameLength, 0); WideCharToMultiByte(CP_UTF8, 0, itemName, -1, &name[0], itemNameLength, nullptr, nullptr))
		{
			if (paintKit->id < 10000)
			{
				if (auto pos = gameItems.find('_' + std::string{ paintKit->name.buffer } + '='); pos != std::string::npos && gameItems.substr(pos + paintKit->name.length).find('_' + std::string{ paintKit->name.buffer } + '=') == std::string::npos)
				{
					if (auto weaponName = gameItems.rfind(crypt_str("weapon_"), pos); weaponName != std::string::npos)
					{
						name.back() = ' ';
						name += '(' + gameItems.substr(weaponName + 7, pos - weaponName - 7) + ')';
					}
				}
				SkinChanger::skinKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
			else
			{
				std::string_view gloveName{ paintKit->name.buffer };
				name.back() = ' ';
				name += '(' + std::string{ gloveName.substr(0, gloveName.find('_')) } + ')';
				SkinChanger::gloveKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
		}
	}

	std::sort(SkinChanger::skinKits.begin(), SkinChanger::skinKits.end());
	std::sort(SkinChanger::gloveKits.begin(), SkinChanger::gloveKits.end());
}


__declspec(naked) void shonax_create_move(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push ebp
		mov  ebp, esp
		push ebx;
		push esp
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call hooks::hl_create_move
			pop  ebx
			pop  ebp
			retn 0Ch
	}
}
int __stdcall hGetUnverifiedFileHashes(void* _this, void* someclass, int nMaxFiles)
{
	return 0;
}
void __fastcall hooked_do_procedural_foot_plant(void* ecx,
	void* edx, void* boneToWorld, void* pLeftFootChain, void* pRightFootChain, void* pos)
{
	return;
}
typedef void(__thiscall* do_procedural_foot_plant_fn)(void*, void*, void*, void*, void*);
do_procedural_foot_plant_fn do_procedural_foot_plant_original;
vmthook* what_ever_want_die;
using GetBool_t = bool(__thiscall*)(void*);

bool NetShowFragmentsGetBool() {
	if (!m_engine()->IsInGame())
		return what_ever_want_die->get_func_address< GetBool_t >(13);

	static auto read_sub_channel_data_ret = pattern::find(PE::GetModule(HASH("engine.dll")), "85 C0 74 12 53 FF 75 0C 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 0C").as< uintptr_t*>();
	static auto check_receiving_list_ret = pattern::find(PE::GetModule(HASH("engine.dll")), "8B 1D ? ? ? ? 85 C0 74 16 FF B6").as< uintptr_t*>();

	static uint32_t last_fragment = 0;

	if (_ReturnAddress() == reinterpret_cast<void*>(read_sub_channel_data_ret) && last_fragment > 0)
	{
		const auto data = &reinterpret_cast<uint32_t*>(m_clientstate()->m_net_channel)[0x56];
		const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

		if (bytes_fragments == last_fragment)
		{
			auto& buffer = reinterpret_cast<uint32_t*>(data)[0x42];
			buffer = 0;
		}
	}

	if (_ReturnAddress() == check_receiving_list_ret)
	{
		const auto data = &reinterpret_cast<uint32_t*>(m_clientstate()->m_net_channel)[0x56];
		const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

		last_fragment = bytes_fragments;
	}

	return what_ever_want_die->get_func_address< GetBool_t >(13);
}



class CMatchSessionOnlineHost {
public:
};

class CMatchFramework {
public:
	enum indices : size_t {
		GETMATCHSESSION = 13,
	};
};

CMatchFramework* m_match_framework;
using GetMatchSession_t = CMatchSessionOnlineHost * (__thiscall*)(void*);

#include "../cheats/misc/logs.h"
VMT m_match_framework_hook;
CMatchSessionOnlineHost* GetMatchSession() {
	Stack       stack;
	Address     kv;
	std::string map;

	// ClientJob_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve::BYieldingRunJobFromMsg
	static Address BYieldingRunJobFromMsg{ pattern::find(PE::GetModule(HASH("client.dll")), ("8B D8 85 DB 0F 84 ? ? ? ? 8B 13 8B CB 68 ? ? ? ? 68")) };

	// NOTE;
	// string xref "game/mmqueue" above that string is a virtual call which is g_pMatchFramework->GetMatchSession( )
	// string xref "Matchmaking reservation confirmed: %llx/%s\n"

	if (stack.ReturnAddress() == BYieldingRunJobFromMsg) {
		// note - nitro:
		// deref ebp = old stack -> old stack - 0xA8 -> keyvalue ptr -> deref keyvalue ptr + 0x24 = mapname

		kv = stack.next().local(0xA8).get();

		if (kv && kv.at(0x24)) {
			map = { kv.at(0x24).as< char* >() };
			std::stringstream shonax;
			shonax << "map :" << map;
			eventlogs::get().add(shonax.str().c_str(), false); //-V106
		}
	}

	// call og.
	return m_match_framework_hook.GetOldMethod< GetMatchSession_t >(CMatchFramework::GETMATCHSESSION)(m_match_framework);
}


template< typename o = void*, typename i = void* >
o force_cast_p(i in) {
	union { i in; o out; }
	u = { in };
	return u.out;
};


VMT m_client_hook;
using LevelInitPreEntity_t = void(__thiscall*)(void*, const char*);
using LevelInitPostEntity_t = void(__thiscall*)(void*);


void __fastcall LevelInitEntity(void* ecx, void* edx, const char* pMapName)
{
	static auto original_fn = hooks::client_hook->get_func_address<LevelInitPreEntity_t>(5);
	std::stringstream shonax;
	shonax << "map " << pMapName;
	eventlogs::get().add(shonax.str().c_str(), false); //-V106
	original_fn(m_client(), pMapName);
}







class CHLClient
{

};
__forceinline void setup_hooks()
{
	static auto getforeignfallbackfontname = (DWORD)(util::FindSignature(crypt_str("vguimatsurface.dll"), g_ctx.signatures.at(9).c_str()));
	hooks::original_getforeignfallbackfontname = (DWORD)DetourFunction((PBYTE)getforeignfallbackfontname, (PBYTE)hooks::hooked_getforeignfallbackfontname); //-V206

	static auto setupbones = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(10).c_str()));
	hooks::original_setupbones = (DWORD)DetourFunction((PBYTE)setupbones, (PBYTE)hooks::hooked_setupbones); //-V206

	static auto doextrabonesprocessing = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(11).c_str()));
	hooks::original_doextrabonesprocessing = (DWORD)DetourFunction((PBYTE)doextrabonesprocessing, (PBYTE)hooks::hooked_doextrabonesprocessing); //-V206

	static auto standardblendingrules = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(12).c_str()));
	hooks::original_standardblendingrules = (DWORD)DetourFunction((PBYTE)standardblendingrules, (PBYTE)hooks::hooked_standardblendingrules); //-V206


	static DWORD* do_procedural_foot_plant = nullptr;
	if (!do_procedural_foot_plant) do_procedural_foot_plant = reinterpret_cast<DWORD*>(util::FindSignature("client.dll", "55 8B EC 83 E4 F0 83 EC 78 56 8B F1 57 8B 56 60"));
	if (do_procedural_foot_plant) do_procedural_foot_plant_original =	reinterpret_cast<do_procedural_foot_plant_fn>(DetourFunction(reinterpret_cast<PBYTE>(do_procedural_foot_plant), reinterpret_cast<PBYTE>(hooked_do_procedural_foot_plant)));


	static auto physicssimulate = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(14).c_str()));
	hooks::original_physicssimulate = (DWORD)DetourFunction((PBYTE)physicssimulate, (PBYTE)hooks::hooked_physicssimulate);

	static auto modifyeyeposition = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(15).c_str()));
	hooks::original_modifyeyeposition = (DWORD)DetourFunction((PBYTE)modifyeyeposition, (PBYTE)hooks::hooked_modifyeyeposition);

	static auto calcviewmodelbob = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(16).c_str()));
	hooks::original_calcviewmodelbob = (DWORD)DetourFunction((PBYTE)calcviewmodelbob, (PBYTE)hooks::hooked_calcviewmodelbob);

	static auto shouldskipanimframe = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(17).c_str()));
	DetourFunction((PBYTE)shouldskipanimframe, (PBYTE)hooks::hooked_shouldskipanimframe);

	static auto checkfilecrcswithserver = (DWORD)(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(18).c_str()));
	DetourFunction((PBYTE)checkfilecrcswithserver, (PBYTE)hooks::hooked_checkfilecrcswithserver);

	static auto processinterpolatedlist = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(19).c_str()));
	hooks::original_processinterpolatedlist = (DWORD)DetourFunction((byte*)processinterpolatedlist, (byte*)hooks::processinterpolatedlist); //-V206


	static auto net_showfragments = m_cvar()->FindVar(("net_showfragments"));
	what_ever_want_die = new vmthook(reinterpret_cast<DWORD**>(net_showfragments));
	what_ever_want_die->hook_function(reinterpret_cast<uintptr_t>(NetShowFragmentsGetBool), 13); //-V107 //-V221


	Interfaces interfaces{};
	m_match_framework = interfaces.get< CMatchFramework* >(HASH("MATCHFRAMEWORK_"));
	static auto fm_client = interfaces.get< CHLClient* >(HASH("VClient"));
	if (m_match_framework)
	{
		m_match_framework_hook.init(m_match_framework);
		m_match_framework_hook.add(CMatchFramework::GETMATCHSESSION, force_cast_p(&GetMatchSession));


	}





	MH_Initialize();


	//for wmt
	hooks::client_hook = new vmthook(reinterpret_cast<DWORD**>(m_client()));
	hooks::clientstate_hook = new vmthook(reinterpret_cast<DWORD**>((CClientState*)(uint32_t(m_clientstate()) + 0x8)));
	hooks::panel_hook = new vmthook(reinterpret_cast<DWORD**>(m_panel()));
	hooks::clientmode_hook = new vmthook(reinterpret_cast<DWORD**>(m_clientmode()));
	hooks::inputinternal_hook = new vmthook(reinterpret_cast<DWORD**>(m_inputinternal())); 
	hooks::engine_hook = new vmthook(reinterpret_cast<DWORD**>(m_engine()));
	hooks::renderview_hook = new vmthook(reinterpret_cast<DWORD**>(m_renderview()));
	hooks::materialsys_hook = new vmthook(reinterpret_cast<DWORD**>(m_materialsystem()));
	hooks::modelrender_hook = new vmthook(reinterpret_cast<DWORD**>(m_modelrender()));
	hooks::surface_hook = new vmthook(reinterpret_cast<DWORD**>(m_surface()));
	hooks::bspquery_hook = new vmthook(reinterpret_cast<DWORD**>(m_engine()->GetBSPTreeQuery()));
	hooks::prediction_hook = new vmthook(reinterpret_cast<DWORD**>(m_prediction())); 
	hooks::trace_hook = new vmthook(reinterpret_cast<DWORD**>(m_trace()));
	hooks::filesystem_hook = new vmthook(reinterpret_cast<DWORD**>(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(20).c_str()) + 0x2));
	hooks::directx_hook = new vmthook(reinterpret_cast<DWORD**>(m_device()));
	//for wmt

	//test
	const auto create_move_target = reinterpret_cast<void*>(get_virtual(m_clientmode(), 24));
	const auto postscreeneffects_target = reinterpret_cast<void*>(get_virtual(m_clientmode(), 44));
	const auto overrideview_target = reinterpret_cast<void*>(get_virtual(m_clientmode(), 18));
	const auto drawfog_target = reinterpret_cast<void*>(get_virtual(m_clientmode(), 17));
	const auto fsn_target = reinterpret_cast<void*>(get_virtual(m_client(), 37));
	//test


	
	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_writeusercmddeltatobuffer), 24); 
	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(LevelInitEntity), 5);

	hooks::clientstate_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_packetstart), 5); 
	hooks::clientstate_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_packetend), 6); 
	
	hooks::panel_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_painttraverse), 41); 

	
	if (!g_ctx.test_hooks) //chill bro look at BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) function where g_ctx.test_hooks = true
	{
		hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_fsn), 37);
		hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_createmove), 24); 
		hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_postscreeneffects), 44); 
		hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_overrideview), 18); 
		hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_drawfog), 17);

	}
	else
	{
		MH_CreateHook(fsn_target, &hooks::hooked_fsn, reinterpret_cast<void**>(&hooks::oFrameStageNotify_t));
		MH_CreateHook(create_move_target, &hooks::hooked_createmove, reinterpret_cast<void**>(&hooks::oCreateMove_t));
		MH_CreateHook(postscreeneffects_target, &hooks::hooked_postscreeneffects, reinterpret_cast<void**>(&hooks::oDoPostScreenEffects_t));
		MH_CreateHook(overrideview_target, &hooks::hooked_overrideview, reinterpret_cast<void**>(&hooks::oOverrideView_t));
		MH_CreateHook(drawfog_target, &hooks::hooked_drawfog, nullptr);
	}


	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_isconnected), 27); //-V107 //-V221
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getscreenaspectratio), 101); //-V107 //-V221
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_ishltv), 93); //-V107 //-V221

	
	hooks::renderview_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_sceneend), 9); //-V107 //-V221

	
	hooks::materialsys_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_beginframe), 42); //-V107 //-V221
	hooks::materialsys_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getmaterial), 84); //-V107 //-V221

	
	hooks::modelrender_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_dme), 21); //-V107 //-V221
	

	hooks::surface_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_lockcursor), 67); //-V107 //-V221

	
	hooks::bspquery_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_listleavesinbox), 6); //-V107 //-V221

	
	hooks::prediction_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_runcommand), 19); //-V107 //-V221

	
	hooks::trace_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_clip_ray_collideable), 4); //-V107 //-V221
	hooks::trace_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_trace_ray), 5); //-V107 //-V221

	
	hooks::filesystem_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_loosefileallowed), 128); //-V107 //-V221
	hooks::filesystem_hook->hook_function(reinterpret_cast<uintptr_t>(hGetUnverifiedFileHashes), 101); //-V107 //-V221


	
	
	while (!(INIT::Window = FindWindowA(crypt_str("Valve001"), nullptr)))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	INIT::OldWindow = (WNDPROC)g_winapi.SetWindowLongA(INIT::Window, GWL_WNDPROC, (LONG_PTR)hooks::Hooked_WndProc);

	
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::Hooked_EndScene_Reset), 16); //-V107 //-V221
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_present), 17); //-V107 //-V221
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::Hooked_EndScene), 42); //-V107 //-V221





	MH_EnableHook(MH_ALL_HOOKS);

	


	hooks::hooked_events.RegisterSelf();
}