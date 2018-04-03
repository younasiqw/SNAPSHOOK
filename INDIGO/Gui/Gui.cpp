#include "Gui.h"
//#include "E:/SNAPSHOOKXYZ Website/RESOLVE/INDIGO/Engine/Hook.h"
#include "../Engine/Hook.h"

using namespace Client;

//[enc_string_enable /]
//[junk_enable /]

bool bIsGuiInitalize = false;
bool bIsGuiVisible = false;
WNDPROC WndProc_o = nullptr;

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

LRESULT WINAPI GUI_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

CGui::CGui() {}

CGui::~CGui()
{
	ImGui_ImplDX9_Shutdown();
}

void CGui::GUI_Init(IDirect3DDevice9 * pDevice)
{
	HWND hWindow = FindWindowA("Valve001", 0);

	ImGui_ImplDX9_Init(hWindow, pDevice);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	io.IniFilename = GuiFile.c_str();

	ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf",
		15.f, 0, io.Fonts->GetGlyphRangesCyrillic());

	//ImFont* font = io.Fonts->AddFontFromMemoryCompressedTTF( 
	//	Avalon_compressed_data , Avalon_compressed_size , 12.f );

	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowRounding = 0.5f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildWindowRounding = 0.0f;
	style.FramePadding = ImVec2(4, 2);
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 0.0f;
	//style.GrabMinSize = 0.3f;
	style.GrabMinSize = 2.0f;
	style.GrabRounding = 0.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.AntiAliasedShapes = true;
	style.CurveTessellationTol = 1.25f;
	DefaultSheme();

	ImGui_ImplDX9_CreateDeviceObjects();

	WndProc_o = (WNDPROC)SetWindowLongA(hWindow, GWL_WNDPROC, (LONG)(LONG_PTR)GUI_WndProc);

	bIsGuiInitalize = true;
}

void CGui::GUI_Begin_Render()
{
	ImGui_ImplDX9_NewFrame();
}

void CGui::GUI_End_Render()
{
	ImGui::Render();
}

LRESULT WINAPI GUI_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static bool is_down = false;
	static bool is_clicked = false;
	static bool check_closed = false;

	if (GUI_KEY_DOWN(VK_INSERT))
	{
		is_clicked = false;
		is_down = true;
	}
	else if (!GUI_KEY_DOWN(VK_INSERT) && is_down)
	{
		is_clicked = true;
		is_down = false;
	}
	else
	{
		is_clicked = false;
		is_down = false;
	}

	if (!bIsGuiVisible && !is_clicked && check_closed)
	{
		string msg = "cl_mouseenable " + to_string(!bIsGuiVisible);
		Interfaces::Engine()->ClientCmd_Unrestricted2(msg.c_str());
		check_closed = false;
	}

	if (is_clicked)
	{
		bIsGuiVisible = !bIsGuiVisible;

		string msg = "cl_mouseenable " + to_string(!bIsGuiVisible);
		Interfaces::Engine()->ClientCmd_Unrestricted2(msg.c_str());

		if (!check_closed)
			check_closed = true;
	}

	if (bIsGuiVisible && ImGui_ImplDX9_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProcA(WndProc_o, hwnd, uMsg, wParam, lParam);
}

void CGui::GUI_Draw_Elements()
{
	if (!bIsGuiInitalize || Interfaces::Engine()->IsTakingScreenshot() || !Interfaces::Engine()->IsActiveApp())
		return;

	g_pGui->GUI_Begin_Render();

	ImGui::GetIO().MouseDrawCursor = bIsGuiVisible;

	bool bOpenTimer = (bIsGuiVisible || (bC4Timer && iC4Timer));

	/*if (g_pEsp && Settings::Esp::esp_BombTimer && bOpenTimer)
	{
	if (GetKeyState(VK_DELETE))
	{
	ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

	ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

	ImGui::SetNextWindowSize(ImVec2(125.f, 30.f));

	if (ImGui::Begin("Bomb Timer", &bOpenTimer,
	ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
	{

	ImGui::Text(C4_TIMER_STRING, g_pEsp->fC4Timer);
	ImGui::End();

	}

	ImGui::GetStyle().WindowMinSize = OldMinSize;
	}
	} // 500 x 320 Medal Changer Size*/


	

	if (Settings::Misc::misc_MedalChanger && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);
		ImGui::SetNextWindowSize(ImVec2(450.f, 300.f));

		if (ImGui::Begin("Medals and Profile Changer", &Settings::Misc::misc_MedalChanger,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{

			ImGui::Columns(2, nullptr, false);
			ImGui::Checkbox("Enabled Medal Changer", &Settings::MedalChanger::enabled);
			static int medal_id = 0;
			ImGui::InputInt("Medal id", &medal_id);
			if (ImGui::Button("Add") && medal_id != 0) {
				Settings::MedalChanger::medals.insert(Settings::MedalChanger::medals.end(), medal_id);
				medal_id = 0;
			}
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
			ImGui::ListBoxHeader("Medal list");
			for (int m = 0; m < Settings::MedalChanger::medals.size(); m++) {
				if (ImGui::Selectable(std::to_string(Settings::MedalChanger::medals[m]).c_str())) {
					if (Settings::MedalChanger::equipped_medal == Settings::MedalChanger::medals[m]) {
						Settings::MedalChanger::equipped_medal = 0;
						Settings::MedalChanger::equipped_medal_override = false;
					}
					Settings::MedalChanger::medals.erase(Settings::MedalChanger::medals.begin() + m);
				}
			}
			ImGui::ListBoxFooter();
			ImGui::PopStyleColor();

			if (ImGui::Button("Apply##Medals")) {
				SendClientHello();
			}

			ImGui::NextColumn();
			ImGui::Checkbox("Enabled Profile Changer", &Settings::ProfileChanger::enabled);
			// I think this is how you do it so should be fairly easy?
			static const char* ranks[] = {
				"Default",
				"Silver 1",
				"Silver 2",
				"Silver 3",
				"Silver 4",
				"Silver 5",
				"Silver 6",
				"Gold Nova 1",
				"Gold Nova 2",
				"Gold Nova 3",
				"Gold Nova 4",
				"Master Guardian 1",
				"Master Guardian 2",
				"Master Guardian Elite",
				"Distinguished Master Guardian",
				"Legendary Eagle",
				"Legendary Eagle Master",
				"Supreme Master First Class",
				"Global Elite"
			};
			ImGui::Combo("Rank", &Settings::ProfileChanger::rank_id, ranks, IM_ARRAYSIZE(ranks));
			ImGui::SliderInt("Level", &Settings::ProfileChanger::level, 0, 40);
			ImGui::SliderInt("XP", &Settings::ProfileChanger::xp, 0, 5000);
			ImGui::InputInt("Wins", &Settings::ProfileChanger::wins);
			ImGui::InputInt("Friendly", &Settings::ProfileChanger::cmd_friendly);
			ImGui::InputInt("Teacher", &Settings::ProfileChanger::cmd_teaching);
			ImGui::InputInt("Leader", &Settings::ProfileChanger::cmd_leader);
			if (ImGui::Button("Apply##Profile")) {
				SendMMHello();
			}
			ImGui::Columns(1, nullptr, false);


			ImGui::End();
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;
	}

	if (Settings::Misc::misc_InvChanger && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);
		ImGui::SetNextWindowSize(ImVec2(450.f, 300.f));

		if (ImGui::Begin("Inventory Changer", &Settings::Misc::misc_InvChanger,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{

			ImGui::Columns(2, nullptr, false);
			ImGui::Checkbox("Enable Inventory Changer", &Settings::InventoryChanger::enabled);
			static int itemDefinitionIndex = 0;
			static int paintKit = 0;
			static int rarity = 0;
			static int seed = 0;
			static float wear = 0.f;

			static const char* weapons[] = {
				"None",
				"1 - Desert Eagle",
				"2 - Dual Berettas",
				"3 - Five SeveN",
				"4 - Glock",
				"",
				"",
				"7 - AK47",
				"8 - AUG",
				"9 - AWP",
				"10 - FAMAS",
				"11 - G3SG1",
				"",
				"13 - Galil AR",
				"14 - M249",
				"",
				"16 - M4A4",
				"17 - MAC10",
				"",
				"19 - P90",
				"",
				"",
				"",
				"",
				"24 - UMP45",
				"25 - XM1014",
				"26 - PPBizon",
				"27 - MAG7",
				"28 - Negev",
				"29 - SawedOff",
				"30 - Tec9",
				" ",
				"32 - P2000",
				"33 - MP7",
				"34 - MP9",
				"35 - Nova",
				"36 - P250",
				"38 - SCAR20",
				"39 - SG553",
				"40 - SCOUT",
			};
			ImGui::Combo("Weapon", &itemDefinitionIndex, weapons, IM_ARRAYSIZE(weapons));
			//			ImGui::InputInt("Weapon ID", &itemDefinitionIndex);

			ImGui::InputInt("Paint Kit", &paintKit);
			static const char* rare[] =
			{
				"None",
				"1 - Consumer grade (white)",
				"2 - Industrial grade (light blue)",
				"3 - Mil-Spec (darker blue)",
				"4 - Restricted (purple)",
				"5 - Classified (pinkish purple)",
				"6 - Covert (red)",
				"7 - Exceedingly Rare (gold)",
			};
			ImGui::Combo("Rarity", &rarity, rare, IM_ARRAYSIZE(rare));
			//ImGui::InputInt("Rarity", &rarity);
			ImGui::InputInt("Seed", &seed);
			ImGui::SliderFloat("Wear", &wear, FLT_MIN, 1.f, "%.10f", 5);
			if (ImGui::Button("Add")) {
				Settings::InventoryChanger::weapons.insert(Settings::InventoryChanger::weapons.end(), { itemDefinitionIndex , paintKit , rarity , seed, wear });
			} ImGui::SameLine();
			if (ImGui::Button("Apply##Skin")) {
				SendClientHello();
			}
			ImGui::NextColumn();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
			ImGui::ListBoxHeader("Skins");
			int weapon_index = 0;
			for (auto weapon : Settings::InventoryChanger::weapons) {
				if (ImGui::Selectable(std::string(std::to_string(weapon.itemDefinitionIndex) + " " + std::to_string(weapon.paintKit)).c_str())) {
					Settings::InventoryChanger::weapons.erase(Settings::InventoryChanger::weapons.begin() + weapon_index);
				}
				weapon_index++;
			}
			ImGui::ListBoxFooter();
			ImGui::PopStyleColor();
			ImGui::Columns(1, nullptr, false);
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;
	}

	if (Settings::Misc::misc_FilterList && bIsGuiVisible)
	{
		ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

		ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);
		ImGui::SetNextWindowSize(ImVec2(525.f, 445.f));

		if (ImGui::Begin("SNAPSHOOK$ SEARCH ENGINE", &Settings::Misc::misc_FilterList,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			ImGui::BeginGroup(); //begins the group
			ImGui::PushItemWidth(150.f); //item width

			ImGui::BeginChild("second child", ImVec2(555, 205), true); //begins the child and the size of the child.
			{
				ImGui::Text("Medal List");
				static ImGuiTextFilter filter;
				filter.Draw();
				const char* lines2[] = {
					"874 - 5 Year Veteran Coin",
					"-----Tournament Trophys, etc..-----",
					"875 - Champion at DreamHack 2013",
					"876 - Finalist at DreamHack 2013",
					"877 - Semifinalist at DreamHack 2013",
					"878 - Quarterfinalist at DreamHack 2013",
					"879 - Champion at EMS One Katowice 2014",
					"880 - Finalist at EMS One Katowice 2014",
					"881 - Semifinalist at EMS One Katowice 2014",
					"882 - Quarterfinalist at EMS One Katowice 2014",
					"883 - Champion at ESL One Cologne 2014",
					"884 - Finalist at ESL One Cologne 2014",
					"885 - Semifinalist at ESL One Cologne 2014",
					"886 - Quarterfinalist at ESL One Cologne 2014",
					"887 - Bronze Cologne 2014 Pick'Em Trophy",
					"888 - Silver Cologne 2014 Pick'Em Trophy",
					"889 - Gold Cologne 2014 Pick'Em Trophy",
					"890 - Champion at DreamHack Winter 2014",
					"891 - Finalist at DreamHack Winter 2014",
					"892 - Semifinalist at DreamHack Winter 2014",
					"893 - Quarterfinalist at DreamHack Winter 2014",
					"894 - Bronze DreamHack 2014 Pick'Em Trophy",
					"895 - Silver DreamHack 2014 Pick'Em Trophy",
					"896 - Gold DreamHack 2014 Pick'Em Trophy",
					"897 - Champion at ESL One Katowice 2015",
					"898 - Finalist at ESL One Katowice 2015",
					"899 - Semifinalist at ESL One Katowice 2015",
					"900 - Quarterfinalist at ESL One Katowice 2015",
					"901 - Bronze Katowice 2015 Pick'Em Trophy",
					"902 - Silver Katowice 2015 Pick'Em Trophy",
					"903 - Gold Katowice 2015 Pick'Em Trophy",
					"904 - Champion at ESL One Cologne 2015",
					"905 - Finalist at ESL One Cologne 2015",
					"906 - Semifinalist at ESL One Cologne 2015",
					"907 - Quarterfinalist at ESL One Cologne 2015",
					"908 - Bronze Cologne 2015 Pick'Em Trophy",
					"909 - Silver Cologne 2015 Pick'Em Trophy",
					"910 - Gold Cologne 2015 Pick'Em Trophy",
					"911 - Bronze Cluj - Napoca 2015 Pick'Em Trophy",
					"912 - Silver Cluj - Napoca 2015 Pick'Em Trophy",
					"913 - Gold Cluj - Napoca 2015 Pick'Em Trophy",
					"914 - Bronze Cluj - Napoca 2015 Fantasy Trophy",
					"915 - Silver Cluj - Napoca 2015 Fantasy Trophy",
					"916 - Gold Cluj - Napoca 2015 Fantasy Trophy",
					"917 - Champion at DreamHack Cluj - Napoca 2015",
					"918 - Finalist at DreamHack Cluj - Napoca 2015",
					"919 - Semifinalist at DreamHack Cluj - Napoca 2015",
					"920 - Quarterfinalist at DreamHack Cluj - Napoca 2015",
					"921 - Bronze Columbus 2016 Pick'Em Trophy",
					"922 - Silver Columbus 2016 Pick'Em Trophy",
					"923 - Gold Columbus 2016 Pick'Em Trophy",
					"924 - Bronze Columbus 2016 Fantasy Trophy",
					"925 - Silver Columbus 2016 Fantasy Trophy",
					"926 - Gold Columbus 2016 Fantasy Trophy",
					"927 - Champion at MLG Columbus 2016",
					"928 - Finalist at MLG Columbus 2016",
					"929 - Semifinalist at MLG Columbus 2016",
					"930 - Quarterfinalist at MLG Columbus 2016",
					"931 - Champion at ESL One Cologne 2016",
					"932 - Finalist at ESL One Cologne 2016",
					"923 - Semifinalist at ESL One Cologne 2016",
					"934 - Quarterfinalist at ESL One Cologne 2016",
					"935 - Bronze Cologne 2016 Pick'Em Trophy",
					"936 - Silver Cologne 2016 Pick'Em Trophy",
					"937 - Gold Cologne 2016 Pick'Em Trophy",
					"938 - Bronze Cologne 2016 Fantasy Trophy",
					"939 - Silver Cologne 2016 Fantasy Trophy",
					"940 - Gold Cologne 2016 Fantasy Trophy",
					"941 - Champion at ELEAGUE Atlanta 2017",
					"942 - Finalist at ELEAGUE Atlanta 2017",
					"943 - Semifinalist at ELEAGUE Atlanta 2017",
					"944 - Quarterfinalist at ELEAGUE Atlanta 2017",
					"945 - Bronze Atlanta 2017 Pick'Em Trophy",
					"946 - Silver Atlanta 2017 Pick'Em Trophy",
					"947 - Gold Atlanta 2017 Pick'Em Trophy",
					"948 - Champion at PGL Krakow 2017",
					"949 - Finalist at PGL Krakow 2017",
					"950 - Semifinalist at PGL Krakow 2017",
					"951 - Quarterfinalist at PGL Krakow 2017",
					"952 - Bronze Krakow 2017 Pick'Em Trophy ",
					"953 - Silver Krakow 2017 Pick'Em Trophy",
					"954 - Gold Krakow 2017 Pick'Em Trophy",
					"955 - Champion at ELEAGUE Boston 2018",
					"956 - Finalist at ELEAGUE Boston 2018",
					"957 - Semifinalist at ELEAGUE Boston 2018",
					"958 - Quarterfinalist at ELEAGUE Boston 2018",
					"959 - Bronze Boston 2018 Pick'Em Trophy",
					"960 - Silver Boston 2018 Pick'Em Trophy",
					"961 - Gold Boston 2018 Pick'Em Trophy",
					"-----Operation Payback-----",
					"1001 - Operation Payback Challenge Coin",
					"1002 - Silver Operation Payback Coin",
					"1003 - Gold Operation Payback Coin",
					"-----Map coins-----",
					"1004 - Museum Map Coin" ,
					"1005 - Downtown Map Coin" ,
					"1006 - Thunder Map Coin" ,
					"1007 - Fevela Map Coin" ,
					"1008 - Motel Map Coin" ,
					"1009 - Seaside Map Coin" ,
					"1010 - Library Map Coin" ,
					"-----Operation Bravo-----",
					"1013 - Operation Bravo Challenge Coin" ,
					"1014 - Silver Operation Bravo Coin" ,
					"1015 - Gold Operation Bravo Coin" ,
					"-----Some Coins-----",
					"1016 - Agency Map Coin" ,
					"1017 - Ali Map Coin" ,
					"1018 - Cache Map Coin" ,
					"1019 - Chinatown Map Coin" ,
					"1020 - Gwalior Map Coin" ,
					"1021 - Ruins Map Coin" ,
					"1022 - Siege Map Coin" ,
					"-----Operation Phoenix-----",
					"1024 - Operation Phoenix Challenge Coin" ,
					"1025 - Silver Operation Phoenix Coin" ,
					"1026 - Gold Operation Phoenix Coin" ,
					"-----Operation Breakout-----",
					"1028 - Operation Breakout Challenge Coin" ,
					"1029 - Silver Operation Breakout Coin" ,
					"1030 - Gold Operation Breakout Coin" ,
					"-----Map Coins-----",
					"1031 - Castle Map Coin" ,
					"1032 - Black Gold Map Coin" ,
					"1033 - Rush Map Coin" ,
					"1034 - Mist Map Coin" ,
					"1035 - Insertion Map Coin" ,
					"1036 - Overgrown Map Coin" ,
					"1037 - Marquis Map Coin" ,
					"1038 - Workout Map Coin" ,
					"1039 - Backalley Map Coin" ,
					"1040 - Season Map Coin" ,
					"1041 - Bazaar Map Coin" ,
					"1042 - Facade Map Coin" ,
					"1043 - Log Map Coin" ,
					"1044 - Rails Map Coin" ,
					"1045 - Resort Map Coin" ,
					"1046 - Zoo Map Coin" ,
					"1047 - Santorini Map Coin",
					"1048 - Coast Map Coin",
					"1049 - Mikla Map Coin",
					"1050 - Royal Map Coin",
					"1051 - Empire Map Coin",
					"1052 - Tulip Map Coin",
					"1053 - Cruise Map Coin",
					"-----Operation Vanguard-----",
					"1316 - Operation Vanguard Challenge Coin" ,
					"1317 - Silver Operation Vanguard Coin" ,
					"1318 - Gold Operation Vanguard Coin" ,
					"-----Operation Bloodhound-----",
					"1327 - Operation Bloodhound Challenge Coin" ,
					"1328 - Silver Operation Bloodhound Coin" ,
					"1329 - Gold Operation Bloodhound Coin" ,
					"-----2015 Service Medals-----",
					"1331 - Blue 2015 Service Medal" ,
					"1332 - Purple 2015 Service Medal" ,
					"-----Operation Wildfire-----",
					"1336 - Bronze Operation Wildfire Challenge Coin",
					"1337 - Silver Operation Wildfire Coin",
					"1338 - Gold Operation Wildfire Coin",
					"-----2016 Service Medals-----",
					"1339 - Grise 2016 Service Medal" ,
					"1340 - LBlue 2016 Service Medal" ,
					"1341 - Blue 2016 Service Medal" ,
					"1342 - Purple 2016 Service Medal",
					"1343 - Rose 2016 Service Medal",
					"1344 - Red 2016 Service Medal",
					"-----2017 Service Medals-----",
					"1357 - Grise 2017 Service Medal",
					"1358 - LBlue 2017 Service Medal",
					"1359 - Blue 2017 Service Medal,"
					"1360 - Purple 2017 Service Medal",
					"1361 - Rose 2017 Service Medal",
					"1362 - Red 2017 Service Medal",
					"1363 - Black 2017 Service Medal",
					"-----2018 Service Medals-----",
					"1367 - Grise 2018 Service Medal",
					"1368 - Green 2018 Service Medal",
					"1369 - Blue 2018 Service Medal",
					"1370 - Purple 2018 Service Medal",
					"1371 - Rose 2018 Service Medal",
					"1372 - Red 2018 Service Medal",
					"-----Operation Hydra-----",
					"4353 - Bronze Operation Hydra Coin",
					"4354 - Silver Operation Hydra Coin",
					"4355 - Gold Operation Hydra Coin",
					"4356 - Diamond Operation Hydra Coin",
					"-----Map pins-----",
					"6001 - Dust II Pin",
					"6002 - Guardian Elite Pin",
					"6003 - Mirage Pin",
					"6004 - Inferno Pin",
					"6005 - Italy Pin",
					"6006 - Victory Pin",
					"6007 - Militia Pin",
					"6008 - Nuke Pin",
					"6009 - Train Pin",
					"6010 - Guardian Pin",
					"6011 - Tactics Pin",
					"6012 - Guardian 2 Pin",
					"6013 - Bravo Pin",
					"6014 - Baggage Pin",
					"6015 - Phoenix Pin",
					"6016 - Office Pin",
					"6017 - Cobblestone Pin",
					"6018 - Overpass Pin",
					"6019 - Bloodhound Pin",
					"6020 - Cache Pin",
					"6021 - Valeria Phoenix Pin",
					"6022 - Guardian 3 Pin",
					"6024 - Canals Pin",
					"6025 - Welcome to the Clutch Pin",
					"6026 - Death Sentence Pin",
					"6027 - Inferno 2 Pin",
					"6028 - Wildfire Pin",
					"6029 - Easy Peasy Pin",
					"6030 - Aces High pin",
					"6031 - Hydra Pin",
					"6032 - Howl Pin",
					"6033 - Brigadier General Pin",
					"-----Music Kits-----",
				};
				for (int i = 0; i < IM_ARRAYSIZE(lines2); i++)
					if (filter.PassFilter(lines2[i]))
						ImGui::Text("%s", lines2[i]);
			}

			ImGui::EndChild();

			ImGui::PopItemWidth();
			ImGui::BeginChild("##childstick", ImVec2(555, 205), true); // ur gay
			{
				ImGui::Text("Sticker Changer List");
				static ImGuiTextFilter filter;
				filter.Draw();
				const char* lines[] = {
					"1 - Shooter",
					"2 - Shooter (Foil)",
					"3 - Shooter Close",
					"4 - Shooter Close (Foil)",
					"5 - Blue Snowflake",
					"6 - Blue Snowflake (Foil)",
					"7 - Polar Bears",
					"8 - Polar Bears (Foil)",
					"9 - Mountain",
					"10 - Mountain (Foil)",
					"11 - Frosty the Hitman",
					"12 - Frosty the Hitman (Foil)",
					"13 - Lucky 13",
					"14 - Aces High",
					"15 - Aces High (Holo)",
					"16 - I Conquered",
					"17 - Seek & Destroy",
					"18 - Black Dog",
					"19 - Fearsome",
					"20 - Fearsome (Holo)",
					"21 - Cerberus",
					"22 - Easy Peasy",
					"23 - Luck Skill",
					"24 - Vigilance",
					"25 - Vigilance (Holo)",
					"26 - Lucky 13 (Foil)",
					"27 - Luck Skill (Foil)",
					"31 - Bish (Holo)",
					"32 - Bash (Holo)",
					"33 - Bosh (Holo)",
					"34 - Banana",
					"35 - Bomb Code",
					"36 - Chicken Lover",
					"37 - Crown (Foil)",
					"38 - Good Game",
					"39 - Good Luck",
					"40 - Have Fun",
					"41 - Let's Roll-oll",
					"42 - Let's Roll-oll (Holo)",
					"43 - Metal",
					"44 - Nice Shot",
					"46 - Stupid Banana (Foil)",
					"47 - Welcome to the Clutch",
					"48 - 3DMAX | Katowice 2014",
					"49 - 3DMAX (Holo) | Katowice 2014",
					"50 - compLexity Gaming | Katowice 2014",
					"51 - compLexity Gaming (Holo) | Katowice 2014",
					"52 - Team Dignitas | Katowice 2014",
					"53 - Team Dignitas (Holo) | Katowice 2014",
					"55 - Fnatic | Katowice 2014",
					"56 - Fnatic (Holo) | Katowice 2014",
					"57 - HellRaisers | Katowice 2014",
					"58 - HellRaisers (Holo) | Katowice 2014",
					"59 - iBUYPOWER | Katowice 2014",
					"60 - iBUYPOWER (Holo) | Katowice 2014",
					"61 - Team LDLC.com | Katowice 2014",
					"62 - Team LDLC.com (Holo) | Katowice 2014",
					"63 - LGB eSports | Katowice 2014",
					"64 - LGB eSports (Holo) | Katowice 2014",
					"65 - mousesports | Katowice 2014",
					"66 - mousesports (Holo) | Katowice 2014",
					"67 - Clan-Mystik | Katowice 2014",
					"68 - Clan-Mystik (Holo) | Katowice 2014",
					"69 - Natus Vincere | Katowice 2014",
					"70 - Natus Vincere (Holo) | Katowice 2014",
					"71 - Ninjas in Pyjamas | Katowice 2014",
					"72 - Ninjas in Pyjamas (Holo) | Katowice 2014",
					"73 - Reason Gaming | Katowice 2014",
					"73 - Reason Gaming | Katowice 2014",
					"74 - Reason Gaming (Holo) | Katowice 2014",
					"75 - Titan | Katowice 2014",
					"76 - Titan (Holo) | Katowice 2014",
					"77 - Virtus.Pro | Katowice 2014",
					"78 - Virtus.Pro (Holo) | Katowice 2014",
					"79 - Vox Eminor | Katowice 2014",
					"80 - Vox Eminor (Holo) | Katowice 2014",
					"81 - ESL Wolf (Foil) | Katowice 2014",
					"82 - ESL Skull (Foil) | Katowice 2014",
					"83 - 3DMAX (Foil) | Katowice 2014",
					"84 - compLexity Gaming (Foil) | Katowice 2014",
					"85 - Team Dignitas (Foil) | Katowice 2014",
					"86 - Fnatic (Foil) | Katowice 2014",
					"87 - HellRaisers (Foil) | Katowice 2014",
					"88 - iBUYPOWER (Foil) | Katowice 2014",
					"89 - Team LDLC.com (Foil) | Katowice 2014",
					"90 - LGB eSports (Foil) | Katowice 2014",
					"91 - mousesports (Foil) | Katowice 2014",
					"92 - Clan-Mystik (Foil) | Katowice 2014",
					"93 - Natus Vincere (Foil) | Katowice 2014",
					"94 - Ninjas in Pyjamas (Foil) | Katowice 2014",
					"95 - Reason Gaming (Foil) | Katowice 2014",
					"96 - Titan (Foil) | Katowice 2014",
					"100 - Gold ESL Skull (Foil) | Katowice 2014",
					"101 - Backstab",
					"102 - King on the Field",
					"103 - Howling Dawn",
					"104 - Bomb Doge",
					"105 - Burn Them All",
					"106 - Harp of War (Holo)",
					"107 - Flammable (Foil)",
					"108 - Headhunter (Foil)",
					"109 - Llama Cannon",
					"110 - New Sheriff (Foil)",
					"111 - My Other Awp",
					"112 - Shave Master",
					"113 - Rising Skull",
					"114 - Sneaky Beaky Like",
					"115 - Swag (Foil)",
					"116 - Teamwork (Holo)",
					"117 - To B or not to B",
					"118 - Winged Defuser",
					"119 - Pocket BBQ",
					"120 - Death Comes",
					"121 - Rekt (Holo)",
					"122 - Cloud9 | Cologne 2014",
					"123 - Cloud9 (Holo) | Cologne 2014",
					"124 - Fnatic | Cologne 2014",
					"125 - Fnatic (Holo) | Cologne 2014",
					"126 - HellRaisers | Cologne 2014",
					"127 - HellRaisers (Holo) | Cologne 2014",
					"128 - Ninjas in Pyjamas | Cologne 2014",
					"129 - Ninjas in Pyjamas (Holo) | Cologne 2014",
					"130 - Team Dignitas | Cologne 2014",
					"131 - Team Dignitas (Holo) | Cologne 2014",
					"132 - Team LDLC.com | Cologne 2014",
					"133 - Team LDLC.com (Holo) | Cologne 2014",
					"134 - Virtus.Pro | Cologne 2014",
					"135 - Virtus.Pro (Holo) | Cologne 2014",
					"136 - Copenhagen Wolves | Cologne 2014",
					"137 - Copenhagen Wolves (Holo) | Cologne 2014",
					"138 - dAT team | Cologne 2014",
					"139 - dAT team (Holo) | Cologne 2014",
					"140 - Epsilon eSports | Cologne 2014",
					"141 - Epsilon eSports (Holo) | Cologne 2014",
					"142 - iBUYPOWER | Cologne 2014",
					"143 - iBUYPOWER (Holo) | Cologne 2014",
					"144 - London Conspiracy | Cologne 2014",
					"145 - London Conspiracy (Holo) | Cologne 2014",
					"146 - Natus Vincere | Cologne 2014",
					"147 - Natus Vincere (Holo) | Cologne 2014",
					"148 - Titan | Cologne 2014",
					"149 - Titan (Holo) | Cologne 2014",
					"150 - Vox Eminor | Cologne 2014",
					"151 - Vox Eminor (Holo) | Cologne 2014",
					"152 - MTS GameGod Wolf | Cologne 2014",
					"153 - MTS GameGod Wolf (Holo) | Cologne 2014",
					"154 - ESL One Cologne 2014 (Blue)",
					"155 - ESL One Cologne 2014 (Red)",
					"156 - Cloud9 (Foil) | Cologne 2014",
					"157 - Fnatic (Foil) | Cologne 2014",
					"158 - HellRaisers (Foil) | Cologne 2014",
					"159 - Ninjas in Pyjamas (Foil) | Cologne 2014",
					"160 - Team Dignitas (Foil) | Cologne 2014",
					"161 - Team LDLC.com (Foil) | Cologne 2014",
					"162 - Virtus.Pro (Foil) | Cologne 2014",
					"163 - Copenhagen Wolves (Foil) | Cologne 2014",
					"164 - dAT team (Foil) | Cologne 2014",
					"165 - Epsilon eSports (Foil) | Cologne 2014",
					"166 - iBUYPOWER (Foil) | Cologne 2014",
					"167 - London Conspiracy (Foil) | Cologne 2014",
					"168 - Natus Vincere (Foil) | Cologne 2014",
					"169 - Titan (Foil) | Cologne 2014",
					"170 - Vox Eminor (Foil) | Cologne 2014",
					"171 - MTS GameGod Wolf (Foil) | Cologne 2014",
					"172 - ESL One Cologne 2014 (Gold)",
					"173 - Bossy Burger",
					"174 - Cat Call",
					"175 - Chicken Strike",
					"176 - CT in Banana",
					"177 - Don't Worry, I'm Pro",
					"178 - Fight like a Girl",
					"179 - Flashbang",
					"180 - Kawaii Killer CT",
					"181 - Nelu the Bear",
					"182 - One Shot One Kill",
					"183 - Shooting Star Return",
					"184 - T On Cat",
					"185 - War Penguin",
					"186 - Windy Walking Club",
					"187 - Blitzkrieg",
					"188 - Pigeon Master",
					"189 - Terrorized",
					"190 - Till Death Do Us Part",
					"191 - Stay Frosty",
					"192 - Doomed",
					"193 - Queen Of Pain",
					"194 - Trick Or Threat",
					"195 - Trick Or Treat",
					"196 - Witch",
					"197 - Zombie Lover",
					"198 - Fnatic | DreamHack 2014",
					"199 - Fnatic (Holo) | DreamHack 2014",
					"200 - Fnatic (Foil) | DreamHack 2014",
					"201 - Cloud9 | DreamHack 2014",
					"202 - Cloud9 (Holo) | DreamHack 2014",
					"203 - Cloud9 (Foil) | DreamHack 2014",
					"207 - Virtus.Pro | DreamHack 2014",
					"208 - Virtus.Pro (Holo) | DreamHack 2014",
					"209 - Virtus.Pro (Foil) | DreamHack 2014",
					"210 - Ninjas in Pyjamas | DreamHack 2014",
					"211 - Ninjas in Pyjamas (Holo) | DreamHack 2014",
					"212 - Ninjas in Pyjamas (Foil) | DreamHack 2014",
					"213 - Natus Vincere | DreamHack 2014",
					"214 - Natus Vincere (Holo) | DreamHack 2014",
					"215 - Natus Vincere (Foil) | DreamHack 2014",
					"219 - Team Dignitas | DreamHack 2014",
					"220 - Team Dignitas (Holo) | DreamHack 2014",
					"221 - Team Dignitas (Foil) | DreamHack 2014",
					"222 - Bravado Gaming | DreamHack 2014",
					"223 - ESC Gaming | DreamHack 2014",
					"224 - HellRaisers | DreamHack 2014",
					"225 - iBUYPOWER | DreamHack 2014",
					"226 - PENTA Sports | DreamHack 2014",
					"227 - Planetkey Dynamics | DreamHack 2014",
					"228 - Team LDLC.com | DreamHack 2014",
					"229 - myXMG | DreamHack 2014",
					"230 - DreamHack Winter 2014",
					"231 - DreamHack Winter 2014 (Foil)",
					"232 - 3DMAX | DreamHack 2014",
					"233 - Copenhagen Wolves | DreamHack 2014",
					"234 - dAT team | DreamHack 2014",
					"235 - London Conspiracy | DreamHack 2014",
					"236 - mousesports | DreamHack 2014",
					"237 - 3DMAX (Gold) | DreamHack 2014",
					"238 - Bravado Gaming (Gold) | DreamHack 2014",
					"239 - Cloud9 (Gold) | DreamHack 2014",
					"240 - Copenhagen Wolves (Gold) | DreamHack 2014",
					"241 - dAT team (Gold) | DreamHack 2014",
					"242 - Team Dignitas (Gold) | DreamHack 2014",
					"243 - ESC Gaming (Gold) | DreamHack 2014",
					"244 - Fnatic (Gold) | DreamHack 2014",
					"245 - HellRaisers (Gold) | DreamHack 2014",
					"246 - iBUYPOWER 9Gold) | DreamHack 2014",
					"247 - London Conspiracy (Gold) | DreamHack 2014",
					"248 - mousesports (Gold) | DreamHack 2014",
					"249 - myXMG (Gold) | DreamHack 2014",
					"250 - Natus Vincere (Gold) | DreamHack 2014",
					"251 - Ninjas in Pyjamas (Gold) | DreamHack 2014",
					"252 - PENTA Sports (Gold) | DreamHack 2014",
					"253 - Planetkey Dynamics (Gold) | DreamHack 2014",
					"254 - Team LDLC.com (Gold) | DreamHack 2014",
					"255 - Virtus.Pro (Gold) | DreamHack 2014",
					"256 - Flipsid3 Tactics | DreamHack 2014",
					"257 - Flipsid3 Tactics (Gold) | DreamHack 2014",
					"258 - Blood Boiler",
					"259 - Dinked",
					"260 - Drug War Veteran",
					"261 - Ho Ho Ho",
					"262 - Massive Pear",
					"263 - My Little Friend",
					"264 - Pandamonium",
					"265 - Piece Of Cake",
					"266 - SAS Chicken",
					"267 - Thug Life",
					"268 - T-Rekt",
					"269 - Warowl",
					"270 - Work For Ammo",
					"271 - Phoenix (Foil)",
					"272 - Bomb Squad (Foil)",
					"273 - Flickshot",
					"274 - Headshot Guarantee",
					"275 - Eco Rush",
					"276 - Just Trolling",
					"278 - Firestarter (Holo)",
					"279 - Lucky Cat (Foil)",
					"280 - Robo",
					"281 - Witchcraft",
					"282 - Wanna Fight",
					"283 - Hostage Rescue",
					"284 - Hamster Hawk",
					"285 - Headless Chicken",
					"286 - 3DMAX | Katowice 2015",
					"287 - 3DMAX (Holo) | Katowice 2015",
					"288 - 3DMAX (Foil) | Katowice 2015",
					"289 - 3DMAX (Gold) | Katowice 2015",
					"290 - Cloud9 G2A | Katowice 2015",
					"291 - Cloud9 G2A (Holo) | Katowice 2015",
					"292 - Cloud9 G2A (Foil) | Katowice 2015",
					"293 - Cloud9 G2A (Gold) | Katowice 2015",
					"294 - Counter Logic Gaming | Katowice 2015",
					"295 - Counter Logic Gaming (Holo) | Katowice 2015",
					"296 - Counter Logic Gaming (Foil) | Katowice 2015",
					"297 - Counter Logic Gaming (Gold) | Katowice 2015",
					"300 - ESL One (Foil) | Katowice 2015",
					"301 - ESL One (Gold) | Katowice 2015",
					"302 - Flipsid3 Tactics | Katowice 2015",
					"303 - Flipsid3 Tactics (Holo) | Katowice 2015",
					"304 - Flipsid3 Tactics (Foil) | Katowice 2015",
					"305 - Flipsid3 Tactics (Gold) | Katowice 2015",
					"306 - Fnatic | Katowice 2015",
					"307 - Fnatic (Holo) | Katowice 2015",
					"308 - Fnatic (Foil) | Katowice 2015",
					"309 - Fnatic (Gold) | Katowice 2015",
					"310 - HellRaisers | Katowice 2015",
					"311 - HellRaisers (Holo) | Katowice 2015",
					"312 - HellRaisers (Foil) | Katowice 2015",
					"313 - HellRaisers (Gold) | Katowice 2015",
					"314 - Keyd Stars | Katowice 2015",
					"315 - Keyd Stars (Holo) | Katowice 2015",
					"316 - Keyd Stars (Foil) | Katowice 2015",
					"317 - Keyd Stars (Gold) | Katowice 2015",
					"318 - LGB eSports | Katowice 2015",
					"319 - LGB eSports (Holo) | Katowice 2015",
					"320 - LGB eSports (Foil) | Katowice 2015",
					"321 - LGB eSports (Gold) | Katowice 2015",
					"322 - Natus Vincere | Katowice 2015",
					"323 - Natus Vincere (Holo) | Katowice 2015",
					"324 - Natus Vincere (Foil) | Katowice 2015",
					"325 - Natus Vincere (Gold) | Katowice 2015",
					"326 - Ninjas in Pyjamas  | Katowice 2015",
					"327 - Ninjas in Pyjamas (Holo) | Katowice 2015",
					"328 - Ninjas in Pyjamas (Foil) | Katowice 2015",
					"329 - Ninjas in Pyjamas (Gold) | Katowice 2015",
					"330 - PENTA Sports  | Katowice 2015",
					"331 - PENTA Sports (Holo) | Katowice 2015",
					"332 - PENTA Sports (Foil) | Katowice 2015",
					"333 - PENTA Sports (Gold) | Katowice 2015",
					"334 - Team EnVyUs | Katowice 2015",
					"335 - Team EnVyUs (Holo) | Katowice 2015",
					"336 - Team EnVyUs (Foil) | Katowice 2015",
					"337 - Team EnVyUs (Gold) | Katowice 2015",
					"338 - TSM Kinguin | Katowice 2015",
					"339 - TSM Kinguin (Holo) | Katowice 2015",
					"340 - TSM Kinguin (Foil) | Katowice 2015",
					"341 - TSM Kinguin (Gold) | Katowice 2015",
					"342 - Titan | Katowice 2015",
					"343 - Titan (Holo) | Katowice 2015",
					"344 - Titan (Foil) | Katowice 2015",
					"345 - Titan (Gold) | Katowice 2015",
					"346 - Virtus.pro | Katowice 2015",
					"347 - Virtus.pro (Holo) | Katowice 2015",
					"348 - Virtus.pro (Foil) | Katowice 2015",
					"349 - Virtus.pro (Gold) | Katowice 2015",
					"350 - Vox Eminor  | Katowice 2015",
					"351 - Vox Eminor (Holo) | Katowice 2015",
					"352 - Vox Eminor (Foil) | Katowice 2015",
					"353 - Vox Eminor (Gold) | Katowice 2015",
					"354 - ESL One | Katowice 2015",
					"355 - Chabo",
					"356 - Bombsquad",
					"357 - Bombsquad (Foil)",
					"358 - The Guru",
					"359 - Silent Ninja",
					"360 - Silent Ninja (Foil)",
					"361 - The Samurai",
					"362 - Skull Lil Boney",
					"363 - Skulltorgeist",
					"364 - Skull Troop",
					"365 - Salute!",
					"366 - The Spartan",
					"367 - Unicorn",
					"368 - Unicorn (Holo)",
					"369 - The Zombie",
					"370 - Awp Country",
					"371 - Chi Bomb",
					"372 - Doru The Fox",
					"373 - Knife Club",
					"374 - CS On The Mind",
					"375 - Ninja Defuse",
					"376 - Pros Don't Fake",
					"377 - Kawaii Killer Terrorist",
					"378 - Baaa-ckstabber!",
					"379 - Delicious Tears",
					"380 - pronax | Cologne 2015",
					"381 - pronax (Foil) | Cologne 2015",
					"382 - pronax (Gold) | Cologne 2015",
					"383 - flusha | Cologne 2015",
					"384 - flusha (Foil) | Cologne 2015",
					"385 - flusha (Gold) | Cologne 2015",
					"386 - JW | Cologne 2015",
					"387 - JW (Foil) | Cologne 2015",
					"388 - JW (Gold) | Cologne 2015",
					"389 - KRIMZ | Cologne 2015",
					"390 - KRIMZ (Foil) | Cologne 2015",
					"391 - KRIMZ (Gold) | Cologne 2015",
					"392 - olofmeister | Cologne 2015",
					"393 - olofmeister (Foil) | Cologne 2015",
					"394 - olofmeister (Gold) | Cologne 2015",
					"395 - FalleN | Cologne 2015",
					"396 - FalleN (Foil) | Cologne 2015",
					"397 - FalleN (Gold) | Cologne 2015",
					"398 - steel | Cologne 2015",
					"399 - steel (Foil) | Cologne 2015",
					"400 - steel (Gold) | Cologne 2015",
					"401 - fer | Cologne 2015",
					"402 - fer (Foil) | Cologne 2015",
					"403 - fer (Gold) | Cologne 2015",
					"404 - boltz | Cologne 2015",
					"405 - boltz (Foil) | Cologne 2015",
					"406 - boltz (Gold) | Cologne 2015",
					"407 - coldzera | Cologne 2015",
					"408 - coldzera (Foil) | Cologne 2015",
					"409 - coldzera (Gold) | Cologne 2015",
					"410 - GuardiaN | Cologne 2015",
					"411 - GuardiaN (Foil) | Cologne 2015",
					"412 - GuardiaN (Gold) | Cologne 2015",
					"413 - Zeus | Cologne 2015",
					"414 - Zeus (Foil) | Cologne 2015",
					"415 - Zeus (Gold) | Cologne 2015",
					"416 - seized | Cologne 2015",
					"417 - seized (Foil) | Cologne 2015",
					"418 - seized (Gold) | Cologne 2015",
					"419 - Edward | Cologne 2015",
					"420 - Edward (Foil) | Cologne 2015",
					"421 - Edward (Gold) | Cologne 2015",
					"422 - flamie | Cologne 2015",
					"423 - flamie (Foil) | Cologne 2015",
					"424 - flamie (Gold) | Cologne 2015",
					"425 - Xizt | Cologne 2015",
					"426 - Xizt (Foil) | Cologne 2015",
					"427 - Xizt (Gold) | Cologne 2015",
					"428 - f0rest | Cologne 2015",
					"429 - f0rest (Foil) | Cologne 2015",
					"430 - f0rest (Gold) | Cologne 2015",
					"431 - GeT_RiGhT | Cologne 2015",
					"432 - GeT_RiGhT (Foil) | Cologne 2015",
					"433 - GeT_RiGhT (Gold) | Cologne 2015",
					"434 - friberg | Cologne 2015",
					"435 - friberg (Foil) | Cologne 2015",
					"436 - friberg (Gold) | Cologne 2015",
					"437 - allu | Cologne 2015",
					"438 - allu (Foil) | Cologne 2015",
					"439 - allu (Gold) | Cologne 2015",
					"440 - kennyS | Cologne 2015",
					"441 - kennyS (Foil) | Cologne 2015",
					"442 - kennyS (Gold) | Cologne 2015",
					"443 - kioShiMa | Cologne 2015",
					"444 - kioShiMa (Foil) | Cologne 2015",
					"445 - kioShiMa (Gold) | Cologne 2015",
					"446 - Happy | Cologne 2015",
					"447 - Happy (Foil) | Cologne 2015",
					"448 - Happy (Gold) | Cologne 2015",
					"449 - apEX | Cologne 2015",
					"450 - apEX (Foil) | Cologne 2015",
					"451 - apEX (Gold) | Cologne 2015",
					"452 - NBK- | Cologne 2015",
					"453 - NBK- (Foil) | Cologne 2015",
					"454 - NBK- (Gold) | Cologne 2015",
					"455 - karrigan | Cologne 2015",
					"456 - karrigan (Foil) | Cologne 2015",
					"457 - karrigan (Gold) | Cologne 2015",
					"458 - device | Cologne 2015",
					"459 - device (Foil) | Cologne 2015",
					"460 - device (Gold) | Cologne 2015",
					"461 - dupreeh | Cologne 2015",
					"462 - dupreeh (Foil) | Cologne 2015",
					"463 - dupreeh (Gold) | Cologne 2015",
					"464 - Xyp9x | Cologne 2015",
					"465 - Xyp9x (Foil) | Cologne 2015",
					"466 - Xyp9x (Gold) | Cologne 2015",
					"467 - cajunb | Cologne 2015",
					"468 - cajunb (Foil) | Cologne 2015",
					"469 - cajunb (Gold) | Cologne 2015",
					"470 - NEO | Cologne 2015",
					"471 - NEO (Foil) | Cologne 2015",
					"472 - NEO (Gold) | Cologne 2015",
					"473 - pashaBiceps | Cologne 2015",
					"474 - pashaBiceps (Foil) | Cologne 2015",
					"475 - pashaBiceps (Gold) | Cologne 2015",
					"476 - TaZ | Cologne 2015",
					"477 - TaZ (Foil) | Cologne 2015",
					"478 - TaZ (Gold) | Cologne 2015",
					"479 - Snax | Cologne 2015",
					"480 - Snax (Foil) | Cologne 2015",
					"481 - Snax (Gold) | Cologne 2015",
					"482 - byali | Cologne 2015",
					"483 - byali (Foil) | Cologne 2015",
					"484 - byali (Gold) | Cologne 2015",
					"485 - chrisJ | Cologne 2015",
					"486 - chrisJ (Foil) | Cologne 2015",
					"487 - chrisJ (Gold) | Cologne 2015",
					"488 - gob b | Cologne 2015",
					"489 - gob b (Foil) | Cologne 2015",
					"490 - gob b (Gold) | Cologne 2015",
					"491 - denis | Cologne 2015",
					"492 - denis (Foil) | Cologne 2015",
					"493 - denis (Gold) | Cologne 2015",
					"494 - nex | Cologne 2015",
					"495 - nex (Foil) | Cologne 2015",
					"496 - nex (Gold) | Cologne 2015",
					"497 - Spiidi | Cologne 2015",
					"498 - Spiidi (Foil) | Cologne 2015",
					"499 - Spiidi (Gold) | Cologne 2015",
					"500 - AZR | Cologne 2015",
					"501 - AZR (Foil) | Cologne 2015",
					"502 - AZR (Gold) | Cologne 2015",
					"503 - Havoc | Cologne 2015",
					"504 - Havoc (Foil) | Cologne 2015",
					"505 - Havoc (Gold) | Cologne 2015",
					"506 - jks | Cologne 2015",
					"507 - jks (Foil) | Cologne 2015",
					"508 - jks (Gold) | Cologne 2015",
					"509 - SPUNJ | Cologne 2015",
					"510 - SPUNJ (Foil) | Cologne 2015",
					"511 - SPUNJ (Gold) | Cologne 2015",
					"512 - yam | Cologne 2015",
					"513 - yam (Foil) | Cologne 2015",
					"514 - yam (Gold) | Cologne 2015",
					"515 - USTILO | Cologne 2015",
					"516 - USTILO (Foil) | Cologne 2015",
					"517 - USTILO (Gold) | Cologne 2015",
					"518 - Rickeh | Cologne 2015",
					"519 - Rickeh (Foil) | Cologne 2015",
					"520 - Rickeh (Gold) | Cologne 2015",
					"521 - emagine | Cologne 2015",
					"522 - emagine (Foil) | Cologne 2015",
					"523 - emagine (Gold) | Cologne 2015",
					"524 - SnypeR | Cologne 2015",
					"525 - SnypeR (Foil) | Cologne 2015",
					"526 - SnypeR (Gold) | Cologne 2015",
					"527 - James | Cologne 2015",
					"528 - James (Foil) | Cologne 2015",
					"529 - James (Gold) | Cologne 2015",
					"530 - markeloff | Cologne 2015",
					"531 - markeloff (Foil) | Cologne 2015",
					"532 - markeloff (Gold) | Cologne 2015",
					"533 - B1ad3 | Cologne 2015",
					"534 - B1ad3 (Foil) | Cologne 2015",
					"535 - B1ad3 (Gold) | Cologne 2015",
					"536 - bondik | Cologne 2015",
					"537 - bondik (Foil) | Cologne 2015",
					"538 - bondik (Gold) | Cologne 2015",
					"539 - WorldEdit | Cologne 2015",
					"540 - WorldEdit (Foil) | Cologne 2015",
					"541 - WorldEdit (Gold) | Cologne 2015",
					"542 - DavCost | Cologne 2015",
					"543 - DavCost (Foil) | Cologne 2015",
					"544 - DavCost (Gold) | Cologne 2015",
					"545 - dennis | Cologne 2015",
					"546 - dennis (Foil) | Cologne 2015",
					"547 - dennis (Gold) | Cologne 2015",
					"548 - ScreaM | Cologne 2015",
					"549 - ScreaM (Foil) | Cologne 2015",
					"550 - ScreaM (Gold) | Cologne 2015",
					"551 - rain | Cologne 2015",
					"552 - rain (Foil) | Cologne 2015",
					"553 - rain (Gold) | Cologne 2015",
					"554 - Maikelele | Cologne 2015",
					"555 - Maikelele (Foil) | Cologne 2015",
					"556 - Maikelele (Gold) | Cologne 2015",
					"557 - fox | Cologne 2015",
					"558 - fox (Foil) | Cologne 2015",
					"559 - fox (Gold) | Cologne 2015",
					"560 - rallen | Cologne 2015",
					"561 - rallen (Foil) | Cologne 2015",
					"562 - rallen (Gold) | Cologne 2015",
					"563 - Hyper | Cologne 2015",
					"564 - Hyper (Foil) | Cologne 2015",
					"565 - Hyper (Gold) | Cologne 2015",
					"566 - peet | Cologne 2015",
					"567 - peet (Foil) | Cologne 2015",
					"568 - peet (Gold) | Cologne 2015",
					"569 - Furlan | Cologne 2015",
					"570 - Furlan (Foil) | Cologne 2015",
					"571 - Furlan (Gold) | Cologne 2015",
					"572 - GruBy | Cologne 2015",
					"573 - GruBy (Foil) | Cologne 2015",
					"574 - GruBy (Gold) | Cologne 2015",
					"575 - Maniac | Cologne 2015",
					"576 - Maniac (Foil) | Cologne 2015",
					"577 - Maniac (Gold) | Cologne 2015",
					"578 - Ex6TenZ | Cologne 2015",
					"579 - Ex6TenZ (Foil) | Cologne 2015",
					"580 - Ex6TenZ (Gold) | Cologne 2015",
					"581 - shox | Cologne 2015",
					"582 - shox (Foil) | Cologne 2015",
					"583 - shox (Gold) | Cologne 2015",
					"584 - SmithZz | Cologne 2015",
					"585 - SmithZz (Foil) | Cologne 2015",
					"586 - SmithZz (Gold) | Cologne 2015",
					"587 - RpK | Cologne 2015",
					"588 - RpK (Foil) | Cologne 2015",
					"589 - RpK (Gold) | Cologne 2015",
					"590 - hazed | Cologne 2015",
					"591 - hazed (Foil) | Cologne 2015",
					"592 - hazed (Gold) | Cologne 2015",
					"593 - FNS | Cologne 2015",
					"594 - FNS (Foil) | Cologne 2015",
					"595 - FNS (Gold) | Cologne 2015",
					"596 - jdm64 | Cologne 2015",
					"597 - jdm64 (Foil) | Cologne 2015",
					"598 - jdm64 (Gold) | Cologne 2015",
					"599 - reltuC | Cologne 2015",
					"600 - reltuC (Foil) | Cologne 2015",
					"601 - reltuC (Gold) | Cologne 2015",
					"602 - tarik | Cologne 2015",
					"603 - tarik (Foil) | Cologne 2015",
					"604 - tarik (Gold) | Cologne 2015",
					"605 - n0thing | Cologne 2015",
					"606 - n0thing (Foil) | Cologne 2015",
					"607 - n0thing (Gold) | Cologne 2015",
					"608 - seang@res | Cologne 2015",
					"609 - seang@res (Foil) | Cologne 2015",
					"610 - seang@res (Gold) | Cologne 2015",
					"611 - shroud | Cologne 2015",
					"612 - shroud (Foil) | Cologne 2015",
					"613 - shroud (Gold) | Cologne 2015",
					"614 - freakazoid | Cologne 2015",
					"615 - freakazoid (Foil) | Cologne 2015",
					"616 - freakazoid (Gold) | Cologne 2015",
					"617 - Skadoodle | Cologne 2015",
					"618 - Skadoodle (Foil) | Cologne 2015",
					"619 - Skadoodle (Gold) | Cologne 2015",
					"620 - Fnatic | Cologne 2015",
					"621 - Fnatic (Foil) | Cologne 2015",
					"622 - Fnatic (Gold) | Cologne 2015",
					"623 - Virtus.Pro | Cologne 2015",
					"624 - Virtus.Pro (Foil) | Cologne 2015",
					"625 - Virtus.Pro (Gold) | Cologne 2015",
					"626 - mousesports | Cologne 2015",
					"627 - mousesports (Foil) | Cologne 2015",
					"628 - mousesports (Gold) | Cologne 2015",
					"629 - Natus Vincere | Cologne 2015",
					"630 - Natus Vincere (Foil) | Cologne 2015",
					"631 - Natus Vincere (Gold) | Cologne 2015",
					"632 - Renegades | Cologne 2015",
					"633 - Renegades (Foil) | Cologne 2015",
					"634 - Renegades (Gold) | Cologne 2015",
					"635 - Team Kinguin | Cologne 2015",
					"636 - Team Kinguin (Foil) | Cologne 2015",
					"637 - Team Kinguin (Gold) | Cologne 2015",
					"638 - Team eBettle | Cologne 2015",
					"639 - Team eBettle (Foil) | Cologne 2015",
					"640 - Team eBettle (Gold) | Cologne 2015",
					"641 - Cloud9 G2A | Cologne 2015",
					"642 - Cloud9 G2A (Foil) | Cologne 2015",
					"643 - Cloud9 G2A (Gold) | Cologne 2015",
					"644 - Ninjas in Pyjamas | Cologne 2015",
					"645 - Ninjas in Pyjamas (Foil) | Cologne 2015",
					"646 - Ninjas in Pyjamas (Gold) | Cologne 2015",
					"647 - Team EnVyUs | Cologne 2015",
					"648 - Team EnVyUs (Foil) | Cologne 2015",
					"649 - Team EnVyUs (Gold) | Cologne 2015",
					"650 - Luminosity Gaming | Cologne 2015",
					"651 - Luminosity Gaming (Foil) | Cologne 2015",
					"652 - Luminosity Gaming (Gold) | Cologne 2015",
					"653 - Team SoloMid | Cologne 2015",
					"654 - Team SoloMid (Foil) | Cologne 2015",
					"655 - Team SoloMid (Gold) | Cologne 2015",
					"656 - Team Immunity | Cologne 2015",
					"657 - Team Immunity (Foil) | Cologne 2015",
					"658 - Team Immunity (Gold) | Cologne 2015",
					"659 - Flipsid3 Tactics | Cologne 2015",
					"660 - Flipsid3 Tactics (Foil) | Cologne 2015",
					"661 - Flipsid3 Tactics (Gold) | Cologne 2015",
					"662 - Titan | Cologne 2015",
					"663 - Titan (Foil) | Cologne 2015",
					"664 - Titan (Gold) | Cologne 2015",
					"665 - Counter Logic Gaming | Cologne 2015",
					"666 - Counter Logic Gaming (Foil) | Cologne 2015",
					"667 - Counter Logic Gaming (Gold) | Cologne 2015",
					"668 - ESL | Cologne 2015",
					"669 - ESL (Foil) | Cologne 2015",
					"670 - ESL (Gold) | Cologne 2015",
					"671 - reltuC | Cluj-Napoca 2015",
					"672 - reltuC (Foil) | Cluj-Napoca 2015",
					"673 - reltuC (Gold) | Cluj-Napoca 2015",
					"674 - FNS | Cluj-Napoca 2015",
					"675 - FNS (Foil) | Cluj-Napoca 2015",
					"676 - FNS (Gold) | Cluj-Napoca 2015",
					"677 - hazed | Cluj-Napoca 2015",
					"678 - hazed (Foil) | Cluj-Napoca 2015",
					"679 - hazed (Gold) | Cluj-Napoca 2015",
					"680 - jdm64 | Cluj-Napoca 2015",
					"681 - jdm64 (Foil) | Cluj-Napoca 2015",
					"682 - jdm64 (Gold) | Cluj-Napoca 2015",
					"683 - tarik | Cluj-Napoca 2015",
					"684 - tarik (Foil) | Cluj-Napoca 2015",
					"685 - tarik (Gold) | Cluj-Napoca 2015",
					"686 - freakazoid | Cluj-Napoca 2015",
					"687 - freakazoid (Foil) | Cluj-Napoca 2015",
					"688 - freakazoid (Gold) | Cluj-Napoca 2015",
					"689 - seang@res | Cluj-Napoca 2015",
					"690 - seang@res (Foil) | Cluj-Napoca 2015",
					"691 - seang@res (Gold) | Cluj-Napoca 2015",
					"692 - shroud | Cluj-Napoca 2015",
					"693 - shroud (Foil) | Cluj-Napoca 2015",
					"694 - shroud (Gold) | Cluj-Napoca 2015",
					"695 - Skadoodle | Cluj-Napoca 2015",
					"696 - Skadoodle (Foil) | Cluj-Napoca 2015",
					"697 - Skadoodle (Gold) | Cluj-Napoca 2015",
					"698 - n0thing | Cluj-Napoca 2015",
					"699 - n0thing (Foil) | Cluj-Napoca 2015",
					"700 - n0thing (Gold) | Cluj-Napoca 2015",
					"701 - apEX | Cluj-Napoca 2015",
					"702 - apEX (Foil) | Cluj-Napoca 2015",
					"703 - apEX (Gold) | Cluj-Napoca 2015",
					"704 - Happy | Cluj-Napoca 2015",
					"705 - Happy (Foil) | Cluj-Napoca 2015",
					"706 - Happy (Gold) | Cluj-Napoca 2015",
					"707 - kioShiMa | Cluj-Napoca 2015",
					"708 - kioShiMa (Foil) | Cluj-Napoca 2015",
					"709 - kioShiMa (Gold) | Cluj-Napoca 2015",
					"710 - kennyS | Cluj-Napoca 2015",
					"711 - kennyS (Foil) | Cluj-Napoca 2015",
					"712 - kennyS (Gold) | Cluj-Napoca 2015",
					"713 - NBK- | Cluj-Napoca 2015",
					"714 - NBK- (Foil) | Cluj-Napoca 2015",
					"715 - NBK- (Gold) | Cluj-Napoca 2015",
					"716 - B1ad3 | Cluj-Napoca 2015",
					"717 - B1ad3 (Foil) | Cluj-Napoca 2015",
					"718 - B1ad3 (Gold) | Cluj-Napoca 2015",
					"719 - bondik | Cluj-Napoca 2015",
					"720 - bondik (Foil) | Cluj-Napoca 2015",
					"721 - bondik (Gold) | Cluj-Napoca 2015",
					"722 - DavCost | Cluj-Napoca 2015",
					"723 - DavCost (Foil) | Cluj-Napoca 2015",
					"724 - DavCost (Gold) | Cluj-Napoca 2015",
					"725 - markeloff | Cluj-Napoca 2015",
					"726 - markeloff (Foil) | Cluj-Napoca 2015",
					"727 - markeloff (Gold) | Cluj-Napoca 2015",
					"728 - WorldEdit | Cluj-Napoca 2015",
					"729 - WorldEdit (Foil) | Cluj-Napoca 2015",
					"730 - WorldEdit (Gold) | Cluj-Napoca 2015",
					"731 - flusha | Cluj-Napoca 2015",
					"732 - flusha (Foil) | Cluj-Napoca 2015",
					"733 - flusha (Gold) | Cluj-Napoca 2015",
					"734 - JW | Cluj-Napoca 2015",
					"735 - JW (Foil) | Cluj-Napoca 2015",
					"736 - JW (Gold) | Cluj-Napoca 2015",
					"737 - KRIMZ | Cluj-Napoca 2015",
					"738 - KRIMZ (Foil) | Cluj-Napoca 2015",
					"739 - KRIMZ (Gold) | Cluj-Napoca 2015",
					"740 - olofmeister | Cluj-Napoca 2015",
					"741 - olofmeister (Foil) | Cluj-Napoca 2015",
					"742 - olofmeister (Gold) | Cluj-Napoca 2015",
					"743 - pronax | Cluj-Napoca 2015",
					"744 - pronax (Foil) | Cluj-Napoca 2015",
					"745 - pronax (Gold) | Cluj-Napoca 2015",
					"746 - dennis | Cluj-Napoca 2015",
					"747 - dennis (Foil) | Cluj-Napoca 2015",
					"748 - dennis (Gold) | Cluj-Napoca 2015",
					"749 - fox | Cluj-Napoca 2015",
					"750 - fox (Foil) | Cluj-Napoca 2015",
					"751 - fox (Gold) | Cluj-Napoca 2015",
					"752 - Maikelele | Cluj-Napoca 2015",
					"753 - Maikelele (Foil) | Cluj-Napoca 2015",
					"754 - Maikelele (Gold) | Cluj-Napoca 2015",
					"755 - rain | Cluj-Napoca 2015",
					"756 - rain (Foil) | Cluj-Napoca 2015",
					"757 - rain (Gold) | Cluj-Napoca 2015",
					"758 - jkaem | Cluj-Napoca 2015",
					"759 - jkaem (Foil) | Cluj-Napoca 2015",
					"760 - jkaem (Gold) | Cluj-Napoca 2015",
					"761 - boltz | Cluj-Napoca 2015",
					"762 - boltz (Foil) | Cluj-Napoca 2015",
					"763 - boltz (Gold) | Cluj-Napoca 2015",
					"764 - coldzera | Cluj-Napoca 2015",
					"765 - coldzera (Foil) | Cluj-Napoca 2015",
					"766 - coldzera (Gold) | Cluj-Napoca 2015",
					"767 - FalleN | Cluj-Napoca 2015",
					"768 - FalleN (Foil) | Cluj-Napoca 2015",
					"769 - FalleN (Gold) | Cluj-Napoca 2015",
					"770 - fer | Cluj-Napoca 2015",
					"771 - fer (Foil) | Cluj-Napoca 2015",
					"772 - fer (Gold) | Cluj-Napoca 2015",
					"773 - steel | Cluj-Napoca 2015",
					"774 - steel (Foil) | Cluj-Napoca 2015",
					"775 - steel (Gold) | Cluj-Napoca 2015",
					"776 - chrisJ | Cluj-Napoca 2015",
					"777 - chrisJ (Foil) | Cluj-Napoca 2015",
					"778 - chrisJ (Gold) | Cluj-Napoca 2015",
					"779 - denis | Cluj-Napoca 2015",
					"780 - denis (Foil) | Cluj-Napoca 2015",
					"781 - denis (Gold) | Cluj-Napoca 2015",
					"782 - gob b | Cluj-Napoca 2015",
					"783 - gob b (Foil) | Cluj-Napoca 2015",
					"784 - gob b (Gold) | Cluj-Napoca 2015",
					"785 - nex | Cluj-Napoca 2015",
					"786 - nex (Foil) | Cluj-Napoca 2015",
					"787 - nex (Gold) | Cluj-Napoca 2015",
					"788 - NiKo | Cluj-Napoca 2015",
					"789 - NiKo (Foil) | Cluj-Napoca 2015",
					"790 - NiKo (Gold) | Cluj-Napoca 2015",
					"791 - Edward | Cluj-Napoca 2015",
					"792 - Edward (Foil) | Cluj-Napoca 2015",
					"793 - Edward (Gold) | Cluj-Napoca 2015",
					"794 - flamie | Cluj-Napoca 2015",
					"795 - flamie (Foil) | Cluj-Napoca 2015",
					"796 - flamie (Gold) | Cluj-Napoca 2015",
					"797 - GuardiaN | Cluj-Napoca 2015",
					"798 - GuardiaN (Foil) | Cluj-Napoca 2015",
					"799 - GuardiaN (Gold) | Cluj-Napoca 2015",
					"800 - seized | Cluj-Napoca 2015",
					"801 - seized (Foil) | Cluj-Napoca 2015",
					"802 - seized (Gold) | Cluj-Napoca 2015",
					"803 - Zeus | Cluj-Napoca 2015",
					"804 - Zeus (Foil) | Cluj-Napoca 2015",
					"805 - Zeus (Gold) | Cluj-Napoca 2015",
					"806 - allu | Cluj-Napoca 2015",
					"807 - allu (Foil) | Cluj-Napoca 2015",
					"808 - allu (Gold) | Cluj-Napoca 2015",
					"809 - f0rest | Cluj-Napoca 2015",
					"810 - f0rest (Foil) | Cluj-Napoca 2015",
					"811 - f0rest (Gold) | Cluj-Napoca 2015",
					"812 - friberg | Cluj-Napoca 2015",
					"813 - friberg (Foil) | Cluj-Napoca 2015",
					"814 - friberg (Gold) | Cluj-Napoca 2015",
					"815 - GeT_RiGhT | Cluj-Napoca 2015",
					"816 - GeT_RiGhT (Foil) | Cluj-Napoca 2015",
					"817 - GeT_RiGhT (Gold) | Cluj-Napoca 2015",
					"818 - Xizt | Cluj-Napoca 2015",
					"819 - Xizt (Foil) | Cluj-Napoca 2015",
					"820 - Xizt (Gold) | Cluj-Napoca 2015",
					"821 - aizy | Cluj-Napoca 2015",
					"822 - aizy (Foil) | Cluj-Napoca 2015",
					"823 - aizy (Gold) | Cluj-Napoca 2015",
					"824 - Kjaerbye | Cluj-Napoca 2015",
					"825 - Kjaerbye (Foil) | Cluj-Napoca 2015",
					"826 - Kjaerbye (Gold) | Cluj-Napoca 2015",
					"827 - MSL | Cluj-Napoca 2015",
					"828 - MSL (Foil) | Cluj-Napoca 2015",
					"829 - MSL (Gold) | Cluj-Napoca 2015",
					"830 - Pimp | Cluj-Napoca 2015",
					"831 - Pimp (Foil) | Cluj-Napoca 2015",
					"832 - Pimp (Gold) | Cluj-Napoca 2015",
					"833 - tenzki | Cluj-Napoca 2015",
					"834 - tenzki (Foil) | Cluj-Napoca 2015",
					"835 - tenzki (Gold) | Cluj-Napoca 2015",
					"836 - adreN | Cluj-Napoca 2015",
					"837 - adreN (Foil) | Cluj-Napoca 2015",
					"838 - adreN (Gold) | Cluj-Napoca 2015",
					"839 - EliGE | Cluj-Napoca 2015",
					"840 - EliGE (Foil) | Cluj-Napoca 2015",
					"841 - EliGE (Gold) | Cluj-Napoca 2015",
					"842 - FugLy | Cluj-Napoca 2015",
					"843 - FugLy (Foil) | Cluj-Napoca 2015",
					"844 - FugLy (Gold) | Cluj-Napoca 2015",
					"845 - Hiko | Cluj-Napoca 2015",
					"846 - Hiko (Foil) | Cluj-Napoca 2015",
					"847 - Hiko (Gold) | Cluj-Napoca 2015",
					"848 - nitr0 | Cluj-Napoca 2015",
					"849 - nitr0 (Foil) | Cluj-Napoca 2015",
					"850 - nitr0 (Gold) | Cluj-Napoca 2015",
					"851 - Ex6TenZ | Cluj-Napoca 2015",
					"852 - Ex6TenZ (Foil) | Cluj-Napoca 2015",
					"853 - Ex6TenZ (Gold) | Cluj-Napoca 2015",
					"854 - RpK | Cluj-Napoca 2015",
					"855 - RpK (Foil) | Cluj-Napoca 2015",
					"856 - RpK (Gold) | Cluj-Napoca 2015",
					"857 - ScreaM | Cluj-Napoca 2015",
					"858 - ScreaM (Foil) | Cluj-Napoca 2015",
					"859 - ScreaM (Gold) | Cluj-Napoca 2015",
					"860 - shox | Cluj-Napoca 2015",
					"861 - shox (Foil) | Cluj-Napoca 2015",
					"862 - shox (Gold) | Cluj-Napoca 2015",
					"863 - SmithZz | Cluj-Napoca 2015",
					"864 - SmithZz (Foil) | Cluj-Napoca 2015",
					"865 - SmithZz (Gold) | Cluj-Napoca 2015",
					"866 - cajunb | Cluj-Napoca 2015",
					"867 - cajunb (Foil) | Cluj-Napoca 2015",
					"868 - cajunb (Gold) | Cluj-Napoca 2015",
					"869 - device | Cluj-Napoca 2015",
					"870 - device (Foil) | Cluj-Napoca 2015",
					"871 - device (Gold) | Cluj-Napoca 2015",
					"872 - dupreeh | Cluj-Napoca 2015",
					"873 - dupreeh (Foil) | Cluj-Napoca 2015",
					"874 - dupreeh (Gold) | Cluj-Napoca 2015",
					"875 - karrigan | Cluj-Napoca 2015",
					"876 - karrigan (Foil) | Cluj-Napoca 2015",
					"877 - karrigan (Gold) | Cluj-Napoca 2015",
					"878 - Xyp9x | Cluj-Napoca 2015",
					"879 - Xyp9x (Foil) | Cluj-Napoca 2015",
					"880 - Xyp9x (Gold) | Cluj-Napoca 2015",
					"881 - Furlan | Cluj-Napoca 2015",
					"882 - Furlan (Foil) | Cluj-Napoca 2015",
					"883 - Furlan (Gold) | Cluj-Napoca 2015",
					"884 - GruBy | Cluj-Napoca 2015",
					"885 - GruBy (Foil) | Cluj-Napoca 2015",
					"886 - GruBy (Gold) | Cluj-Napoca 2015",
					"887 - Hyper | Cluj-Napoca 2015",
					"888 - Hyper (Foil) | Cluj-Napoca 2015",
					"889 - Hyper (Gold) | Cluj-Napoca 2015",
					"890 - peet | Cluj-Napoca 2015",
					"891 - peet (Foil) | Cluj-Napoca 2015",
					"892 - peet (Gold) | Cluj-Napoca 2015",
					"893 - rallen | Cluj-Napoca 2015",
					"894 - rallen (Foil) | Cluj-Napoca 2015",
					"895 - rallen (Gold) | Cluj-Napoca 2015",
					"896 - byali | Cluj-Napoca 2015",
					"897 - byali (Foil) | Cluj-Napoca 2015",
					"898 - byali (Gold) | Cluj-Napoca 2015",
					"899 - NEO | Cluj-Napoca 2015",
					"900 - NEO (Foil) | Cluj-Napoca 2015",
					"901 - NEO (Gold) | Cluj-Napoca 2015",
					"902 - pashaBiceps | Cluj-Napoca 2015",
					"903 - pashaBiceps (Foil) | Cluj-Napoca 2015",
					"904 - pashaBiceps (Gold) | Cluj-Napoca 2015",
					"905 - Snax | Cluj-Napoca 2015",
					"906 - Snax (Foil) | Cluj-Napoca 2015",
					"907 - Snax (Gold) | Cluj-Napoca 2015",
					"908 - TaZ | Cluj-Napoca 2015",
					"909 - TaZ (Foil) | Cluj-Napoca 2015",
					"910 - TaZ (Gold) | Cluj-Napoca 2015",
					"911 - Ninjas in Pyjamas | Cluj-Napoca 2015",
					"912 - Ninjas in Pyjamas (Foil) | Cluj-Napoca 2015",
					"913 - Ninjas in Pyjamas (Gold) | Cluj-Napoca 2015",
					"914 - Team Dignitas | Cluj-Napoca 2015",
					"915 - Team Dignitas (Foil) | Cluj-Napoca 2015",
					"916 - Team Dignitas (Gold) | Cluj-Napoca 2015",
					"917 - Counter Logic Gaming | Cluj-Napoca 2015",
					"918 - Counter Logic Gaming (Foil) | Cluj-Napoca 2015",
					"919 - Counter Logic Gaming (Gold) | Cluj-Napoca 2015",
					"920 - Vexed Gaming | Cluj-Napoca 2015",
					"921 - Vexed Gaming (Foil) | Cluj-Napoca 2015",
					"922 - Vexed Gaming (Gold) | Cluj-Napoca 2015",
					"923 - Flipsid3 Tactics | Cluj-Napoca 2015",
					"924 - Flipsid3 Tactics (Foil) | Cluj-Napoca 2015",
					"925 - Flipsid3 Tactics (Gold) | Cluj-Napoca 2015",
					"926 - Team Liquid | Cluj-Napoca 2015",
					"927 - Team Liquid (Foil) | Cluj-Napoca 2015",
					"928 - Team Liquid (Gold) | Cluj-Napoca 2015",
					"929 - mousesports | Cluj-Napoca 2015",
					"930 - mousesports (Foil) | Cluj-Napoca 2015",
					"931 - mousesports (Gold) | Cluj-Napoca 2015",
					"932 - Natus Vincere | Cluj-Napoca 2015",
					"933 - Natus Vincere (Foil) | Cluj-Napoca 2015",
					"934 - Natus Vincere (Gold) | Cluj-Napoca 2015",
					"935 - Virtus.Pro | Cluj-Napoca 2015",
					"936 - Virtus.Pro (Foil) | Cluj-Napoca 2015",
					"937 - Virtus.Pro (Gold) | Cluj-Napoca 2015",
					"938 - Cloud9 | Cluj-Napoca 2015",
					"939 - Cloud9 (Foil) | Cluj-Napoca 2015",
					"940 - Cloud9 (Gold) | Cluj-Napoca 2015",
					"941 - G2 Esports | Cluj-Napoca 2015",
					"942 - G2 Esports (Foil) | Cluj-Napoca 2015",
					"943 - G2 Esports (Gold) | Cluj-Napoca 2015",
					"944 - Titan | Cluj-Napoca 2015",
					"945 - Titan (Foil) | Cluj-Napoca 2015",
					"946 - Titan (Gold) | Cluj-Napoca 2015",
					"947 - Team SoloMid | Cluj-Napoca 2015",
					"948 - Team SoloMid (Foil) | Cluj-Napoca 2015",
					"949 - Team SoloMid (Gold) | Cluj-Napoca 2015",
					"950 - Team EnVyUs | Cluj-Napoca 2015",
					"951 - Team EnVyUs (Foil) | Cluj-Napoca 2015",
					"952 - Team EnVyUs (Gold) | Cluj-Napoca 2015",
					"953 - Fnatic | Cluj-Napoca 2015",
					"954 - Fnatic (Foil) | Cluj-Napoca 2015",
					"955 - Fnatic (Gold) | Cluj-Napoca 2015",
					"956 - Luminosity Gaming | Cluj-Napoca 2015",
					"957 - Luminosity Gaming (Foil) | Cluj-Napoca 2015",
					"958 - Luminosity Gaming (Gold) | Cluj-Napoca 2015",
					"959 - DreamHack | Cluj-Napoca 2015",
					"960 - DreamHack (Foil) | Cluj-Napoca 2015",
					"961 - DreamHack (Gold) | Cluj-Napoca 2015",
					"962 - Ivette",
					"963 - Kimberly",
					"964 - Martha",
					"965 - Merietta",
					"966 - Sherry",
					"967 - Tamara",
					"968 - Ivette (Holo)",
					"969 - Kimberly (Holo)",
					"970 - Martha (Holo)",
					"971 - Merietta (Holo)",
					"972 - Sherry (Holo)",
					"973 - Tamara (Holo)",
					"974 - Boom",
					"975 - Boom (Holo)",
					"976 - Boom (Foil)",
					"977 - Countdown",
					"978 - Countdown (Holo)",
					"979 - Countdown (Foil)",
					"980 - Don't Worry",
					"981 - Don't Worry (Holo)",
					"982 - Don't Worry (Foil)",
					"983 - Hard Cluck Life",
					"984 - Hard Cluck Life (Holo)",
					"985 - Hard Cluck Life (Foil)",
					"986 - Move It",
					"987 - Move It (Holo)",
					"988 - Move It (Foil)",
					"989 - The Awper",
					"990 - The Baiter",
					"991 - The Bomber",
					"992 - The Bot",
					"993 - The Fragger",
					"994 - The Leader",
					"995 - The Lurker",
					"996 - The 'Nader",
					"997 - The Ninja",
					"998 - Support",
					"999 - The Awper (Foil)"
					"1000 - The Bomber (Foil)",
					"1001 - The Fragger (Foil)",
					"1002 - The Leader (Foil)",
					"1003 - The Nader (Foil)",
					"1004 - Ninja (Foil)",
					"1005 - The Pro (Foil)",
					"1006 - Support (Foil)",
					"1007 - Ninjas in Pyjamas | MLG Columbus 2016",
					"1008 - Ninjas in Pyjamas (Holo) | MLG Columbus 2016",
					"1009 - Ninjas in Pyjamas (Foil) | MLG Columbus 2016",
					"1010 - Ninjas in Pyjamas (Gold) | MLG Columbus 2016",
					"1011 - Splyce | MLG Columbus 2016",
					"1012 - Splyce (Holo) | MLG Columbus 2016",
					"1013 - Splyce (Foil) | MLG Columbus 2016",
					"1014 - Splyce (Gold) | MLG Columbus 2016",
					"1015 - Counter Logic Gaming | MLG Columbus 2016",
					"1016 - Counter Logic Gaming (Holo) | MLG Columbus 2016",
					"1017 - Counter Logic Gaming (Foil) | MLG Columbus 2016",
					"1018 - Counter Logic Gaming (Gold) | MLG Columbus 2016",
					"1019 - Gambit Gaming | MLG Columbus 2016",
					"1020 - Gambit Gaming (Holo) | MLG Columbus 2016",
					"1021 - Gambit Gaming (Foil) | MLG Columbus 2016",
					"1022 - Gambit Gaming (Gold) | MLG Columbus 2016",
					"1023 - Flipsid3 Tactics | MLG Columbus 2016",
					"1024 - Flipsid3 Tactics (Holo) | MLG Columbus 2016",
					"1025 - Flipsid3 Tactics (Foil) | MLG Columbus 2016",
					"1026 - Flipsid3 Tactics (Gold) | MLG Columbus 2016",
					"1027 - Team Liquid | MLG Columbus 2016",
					"1028 - Team Liquid (Holo) | MLG Columbus 2016",
					"1029 - Team Liquid (Foil) | MLG Columbus 2016",
					"1030 - Team Liquid (Gold) | MLG Columbus 2016",
					"1031 - mousesports | MLG Columbus 2016",
					"1032 - mousesports (Holo) | MLG Columbus 2016",
					"1033 - mousesports (Foil) | MLG Columbus 2016",
					"1034 - mousesports (Gold) | MLG Columbus 2016",
					"1035 - Natus Vincere | MLG Columbus 2016",
					"1036 - Natus Vincere (Holo) | MLG Columbus 2016",
					"1037 - Natus Vincere (Foil) | MLG Columbus 2016",
					"1038 - Natus Vincere (Gold) | MLG Columbus 2016",
					"1039 - Virtus.Pro | MLG Columbus 2016",
					"1040 - Virtus.Pro (Holo) | MLG Columbus 2016",
					"1041 - Virtus.Pro (Foil) | MLG Columbus 2016",
					"1042 - Virtus.Pro (Gold) | MLG Columbus 2016",
					"1043 - Cloud9 | MLG Columbus 2016",
					"1044 - Cloud9 (Holo) | MLG Columbus 2016",
					"1045 - Cloud9 (Foil) | MLG Columbus 2016",
					"1046 - Cloud9 (Gold) | MLG Columbus 2016",
					"1047 - G2 Esports | MLG Columbus 2016",
					"1048 - G2 Esports (Holo) | MLG Columbus 2016",
					"1049 - G2 Esports (Foil) | MLG Columbus 2016",
					"1050 - G2 Esports (Gold) | MLG Columbus 2016",
					"1051 - FaZe Clan | MLG Columbus 2016",
					"1052 - FaZe Clan (Holo) | MLG Columbus 2016",
					"1053 - FaZe Clan (Foil) | MLG Columbus 2016",
					"1054 - FaZe Clan (Gold) | MLG Columbus 2016",
					"1055 - Astralis | MLG Columbus 2016",
					"1056 - Astralis (Holo) | MLG Columbus 2016",
					"1057 - Astralis (Foil) | MLG Columbus 2016",
					"1058 - Astralis (Gold) | MLG Columbus 2016",
					"1059 - Team EnVyUs | MLG Columbus 2016",
					"1060 - Team EnVyUs (Holo) | MLG Columbus 2016",
					"1061 - Team EnVyUs (Foil) | MLG Columbus 2016",
					"1062 - Team EnVyUs (Gold) | MLG Columbus 2016",
					"1063 - Fnatic | MLG Columbus 2016",
					"1064 - Fnatic (Holo) | MLG Columbus 2016",
					"1065 - Fnatic (Foil) | MLG Columbus 2016",
					"1066 - Fnatic (Gold) | MLG Columbus 2016",
					"1067 - Luminosity Gaming | MLG Columbus 2016",
					"1068 - Luminosity Gaming (Holo) | MLG Columbus 2016",
					"1069 - Luminosity Gaming (Foil) | MLG Columbus 2016",
					"1070 - Luminosity Gaming (Gold) | MLG Columbus 2016",
					"1071 - MLG | MLG Columbus 2016",
					"1072 - MLG (Holo) | MLG Columbus 2016",
					"1073 - MLG (Foil) | MLG Columbus 2016",
					"1074 - MLG (Gold) | MLG Columbus 2016",
					"1075 - reltuC | MLG Columbus 2016",
					"1076 - reltuC (Foil) | MLG Columbus 2016",
					"1077 - reltuC (Gold) | MLG Columbus 2016",
					"1078 - FugLy | MLG Columbus 2016",
					"1079 - FugLy (Foil) | MLG Columbus 2016",
					"1080 - FugLy (Gold) | MLG Columbus 2016",
					"1081 - hazed | MLG Columbus 2016",
					"1082 - hazed (Foil) | MLG Columbus 2016",
					"1083 - hazed (Gold) | MLG Columbus 2016",
					"1084 - jdm64 | MLG Columbus 2016",
					"1085 - jdm64 (Foil) | MLG Columbus 2016",
					"1086 - jdm64 (Gold) | MLG Columbus 2016",
					"1087 - tarik | MLG Columbus 2016",
					"1088 - tarik (Foil) | MLG Columbus 2016",
					"1089 - tarik (Gold) | MLG Columbus 2016",
					"1090 - freakazoid | MLG Columbus 2016",
					"1091 - freakazoid (Foil) | MLG Columbus 2016",
					"1092 - freakazoid (Gold) | MLG Columbus 2016",
					"1093 - Stewie2K | MLG Columbus 2016",
					"1094 - Stewie2K (Foil) | MLG Columbus 2016",
					"1095 - Stewie2K (Gold) | MLG Columbus 2016",
					"1096 - shroud | MLG Columbus 2016",
					"1097 - shroud (Foil) | MLG Columbus 2016",
					"1098 - shroud (Gold) | MLG Columbus 2016",
					"1099 - Skadoodle | MLG Columbus 2016",
					"1100 - Skadoodle (Foil) | MLG Columbus 2016",
					"1101 - Skadoodle (Gold) | MLG Columbus 2016",
					"1102 - n0thing | MLG Columbus 2016",
					"1103 - n0thing (Foil) | MLG Columbus 2016",
					"1104 - n0thing (Gold) | MLG Columbus 2016",
					"1105 - apEX | MLG Columbus 2016",
					"1106 - apEX (Foil) | MLG Columbus 2016",
					"1107 - apEX (Gold) | MLG Columbus 2016",
					"1108 - Happy | MLG Columbus 2016",
					"1109 - Happy (Foil) | MLG Columbus 2016",
					"1110 - Happy (Gold) | MLG Columbus 2016",
					"1111 - DEVIL | MLG Columbus 2016",
					"1112 - DEVIL (Foil) | MLG Columbus 2016",
					"1113 - DEVIL (Gold) | MLG Columbus 2016",
					"1114 - kennyS | MLG Columbus 2016",
					"1115 - kennyS (Foil) | MLG Columbus 2016",
					"1116 - kennyS (Gold) | MLG Columbus 2016",
					"1117 - NBK- | MLG Columbus 2016",
					"1118 - NBK- (Foil) | MLG Columbus 2016",
					"1119 - NBK- (Gold) | MLG Columbus 2016",
					"1120 - B1ad3 | MLG Columbus 2016",
					"1121 - B1ad3 (Foil) | MLG Columbus 2016",
					"1122 - B1ad3 (Gold) | MLG Columbus 2016",
					"1123 - bondik | MLG Columbus 2016",
					"1124 - bondik (Foil) | MLG Columbus 2016",
					"1125 - bondik (Gold) | MLG Columbus 2016",
					"1126 - Shara | MLG Columbus 2016",
					"1127 - Shara (Foil) | MLG Columbus 2016",
					"1128 - Shara (Gold) | MLG Columbus 2016",
					"1129 - markeloff | MLG Columbus 2016",
					"1130 - markeloff (Foil) | MLG Columbus 2016",
					"1131 - markeloff (Gold) | MLG Columbus 2016",
					"1132 - WorldEdit | MLG Columbus 2016",
					"1133 - WorldEdit (Foil) | MLG Columbus 2016",
					"1134 - WorldEdit (Gold) | MLG Columbus 2016",
					"1135 - flusha | MLG Columbus 2016",
					"1136 - flusha (Foil) | MLG Columbus 2016",
					"1137 - flusha (Gold) | MLG Columbus 2016",
					"1138 - JW | MLG Columbus 2016",
					"1139 - JW (Foil) | MLG Columbus 2016",
					"1140 - JW (Gold) | MLG Columbus 2016",
					"1141 - KRIMZ | MLG Columbus 2016",
					"1142 - KRIMZ (Foil) | MLG Columbus 2016",
					"1143 - KRIMZ (Gold) | MLG Columbus 2016",
					"1144 - olofmeister | MLG Columbus 2016",
					"1145 - olofmeister (Foil) | MLG Columbus 2016",
					"1146 - olofmeister (Gold) | MLG Columbus 2016",
					"1147 - dennis | MLG Columbus 2016",
					"1148 - dennis (Foil) | MLG Columbus 2016",
					"1149 - dennis (Gold) | MLG Columbus 2016",
					"1150 - aizy | MLG Columbus 2016",
					"1151 - aizy (Foil) | MLG Columbus 2016",
					"1152 - aizy (Gold) | MLG Columbus 2016",
					"1153 - fox | MLG Columbus 2016",
					"1154 - fox (Foil) | MLG Columbus 2016",
					"1155 - fox (Gold) | MLG Columbus 2016",
					"1156 - Maikelele | MLG Columbus 2016",
					"1157 - Maikelele (Foil) | MLG Columbus 2016",
					"1158 - Maikelele (Gold) | MLG Columbus 2016",
					"1159 - rain | MLG Columbus 2016",
					"1160 - rain (Foil) | MLG Columbus 2016",
					"1161 - rain (Gold) | MLG Columbus 2016",
					"1162 - jkaem | MLG Columbus 2016",
					"1163 - jkaem (Foil) | MLG Columbus 2016",
					"1164 - jkaem (Gold) | MLG Columbus 2016",
					"1165 - fnx | MLG Columbus 2016",
					"1166 - fnx (Foil) | MLG Columbus 2016",
					"1167 - fnx (Gold) | MLG Columbus 2016",
					"1168 - coldzera | MLG Columbus 2016",
					"1169 - coldzera (Foil) | MLG Columbus 2016",
					"1170 - coldzera (Gold) | MLG Columbus 2016",
					"1171 - FalleN | MLG Columbus 2016",
					"1172 - FalleN (Foil) | MLG Columbus 2016",
					"1173 - FalleN (Gold) | MLG Columbus 2016",
					"1174 - fer | MLG Columbus 2016",
					"1175 - fer (Foil) | MLG Columbus 2016",
					"1176 - fer (Gold) | MLG Columbus 2016",
					"1177 - TACO | MLG Columbus 2016",
					"1178 - TACO (Foil) | MLG Columbus 2016",
					"1179 - TACO (Gold) | MLG Columbus 2016",
					"1180 - chrisJ | MLG Columbus 2016",
					"1181 - chrisJ (Foil) | MLG Columbus 2016",
					"1182 - chrisJ (Gold) | MLG Columbus 2016",
					"1183 - denis | MLG Columbus 2016",
					"1184 - denis (Foil) | MLG Columbus 2016",
					"1185 - denis (Gold) | MLG Columbus 2016",
					"1186 - Spiidi | MLG Columbus 2016",
					"1187 - Spiidi (Foil) | MLG Columbus 2016",
					"1188 - Spiidi (Gold) | MLG Columbus 2016",
					"1189 - nex | MLG Columbus 2016",
					"1190 - nex (Foil) | MLG Columbus 2016",
					"1191 - nex (Gold) | MLG Columbus 2016",
					"1192 - NiKo | MLG Columbus 2016",
					"1193 - NiKo (Foil) | MLG Columbus 2016",
					"1194 - NiKo (Gold) | MLG Columbus 2016",
					"1195 - Edward | MLG Columbus 2016",
					"1196 - Edward (Foil) | MLG Columbus 2016",
					"1197 - Edward (Gold) | MLG Columbus 2016",
					"1198 - flamie | MLG Columbus 2016",
					"1199 - flamie (Foil) | MLG Columbus 2016",
					"1200 - flamie (Gold) | MLG Columbus 2016",
					"1201 - GuardiaN | MLG Columbus 2016",
					"1202 - GuardiaN (Foil) | MLG Columbus 2016",
					"1203 - GuardiaN (Gold) | MLG Columbus 2016",
					"1204 - seized | MLG Columbus 2016",
					"1205 - seized (Foil) | MLG Columbus 2016",
					"1206 - seized (Gold) | MLG Columbus 2016",
					"1207 - Zeus | MLG Columbus 2016",
					"1208 - Zeus (Foil) | MLG Columbus 2016",
					"1209 - Zeus (Gold) | MLG Columbus 2016",
					"1210 - pyth | MLG Columbus 2016",
					"1211 - pyth (Foil) | MLG Columbus 2016",
					"1212 - pyth (Gold) | MLG Columbus 2016",
					"1213 - f0rest | MLG Columbus 2016",
					"1214 - f0rest (Foil) | MLG Columbus 2016",
					"1215 - f0rest (Gold) | MLG Columbus 2016",
					"1216 - friberg | MLG Columbus 2016",
					"1217 - friberg (Foil) | MLG Columbus 2016",
					"1218 - friberg (Gold) | MLG Columbus 2016",
					"1219 - GeT_RiGhT | MLG Columbus 2016",
					"1220 - GeT_RiGhT (Foil) | MLG Columbus 2016",
					"1221 - GeT_RiGhT (Gold) | MLG Columbus 2016",
					"1222 - Xizt | MLG Columbus 2016",
					"1223 - Xizt (Foil) | MLG Columbus 2016",
					"1224 - Xizt (Gold) | MLG Columbus 2016",
					"1225 - jasonR | MLG Columbus 2016",
					"1226 - jasonR (Foil) | MLG Columbus 2016",
					"1227 - jasonR (Gold) | MLG Columbus 2016",
					"1228 - arya | MLG Columbus 2016",
					"1229 - arya (Foil) | MLG Columbus 2016",
					"1230 - arya (Gold) | MLG Columbus 2016",
					"1231 - Professor_Chaos | MLG Columbus 2016",
					"1232 - Professor_Chaos (Foil) | MLG Columbus 2016",
					"1233 - Professor_Chaos (Gold) | MLG Columbus 2016",
					"1234 - DAVEY | MLG Columbus 2016",
					"1235 - DAVEY (Foil) | MLG Columbus 2016",
					"1236 - DAVEY (Gold) | MLG Columbus 2016",
					"1237 - abE | MLG Columbus 2016",
					"1238 - abE (Foil) | MLG Columbus 2016",
					"1239 - abE (Gold) | MLG Columbus 2016",
					"1240 - adreN | MLG Columbus 2016",
					"1241 - adreN (Foil) | MLG Columbus 2016",
					"1242 - adreN (Gold) | MLG Columbus 2016",
					"1243 - EliGE | MLG Columbus 2016",
					"1244 - EliGE (Foil) | MLG Columbus 2016",
					"1245 - EliGE (Gold) | MLG Columbus 2016",
					"1246 - s1mple | MLG Columbus 2016",
					"1247 - s1mple (Foil) | MLG Columbus 2016",
					"1248 - s1mple (Gold) | MLG Columbus 2016",
					"1249 - Hiko | MLG Columbus 2016",
					"1250 - Hiko (Foil) | MLG Columbus 2016",
					"1251 - Hiko (Gold) | MLG Columbus 2016",
					"1252 - nitr0 | MLG Columbus 2016",
					"1253 - nitr0 (Foil) | MLG Columbus 2016",
					"1254 - nitr0 (Gold) | MLG Columbus 2016",
					"1255 - Ex6TenZ | MLG Columbus 2016",
					"1256 - Ex6TenZ (Foil) | MLG Columbus 2016",
					"1257 - Ex6TenZ (Gold) | MLG Columbus 2016",
					"1258 - RpK | MLG Columbus 2016",
					"1259 - RpK (Foil) | MLG Columbus 2016",
					"1260 - RpK (Gold) | MLG Columbus 2016",
					"1261 - ScreaM | MLG Columbus 2016",
					"1262 - ScreaM (Foil) | MLG Columbus 2016",
					"1263 - ScreaM (Gold) | MLG Columbus 2016",
					"1264 - shox | MLG Columbus 2016",
					"1265 - shox (Foil) | MLG Columbus 2016",
					"1266 - shox (Gold) | MLG Columbus 2016",
					"1267 - SmithZz | MLG Columbus 2016",
					"1268 - SmithZz (Foil) | MLG Columbus 2016",
					"1269 - SmithZz (Gold) | MLG Columbus 2016",
					"1270 - cajunb | MLG Columbus 2016",
					"1271 - cajunb (Foil) | MLG Columbus 2016",
					"1272 - cajunb (Gold) | MLG Columbus 2016",
					"1273 - device | MLG Columbus 2016",
					"1274 - device (Foil) | MLG Columbus 2016",
					"1275 - device (Gold) | MLG Columbus 2016",
					"1276 - dupreeh | MLG Columbus 2016",
					"1277 - dupreeh (Foil) | MLG Columbus 2016",
					"1278 - dupreeh (Gold) | MLG Columbus 2016",
					"1279 - karrigan | MLG Columbus 2016",
					"1280 - karrigan (Foil) | MLG Columbus 2016",
					"1281 - karrigan (Gold) | MLG Columbus 2016",
					"1282 - Xyp9x | MLG Columbus 2016",
					"1283 - Xyp9x (Foil) | MLG Columbus 2016",
					"1284 - Xyp9x (Gold) | MLG Columbus 2016",
					"1285 - wayLander | MLG Columbus 2016",
					"1286 - wayLander (Foil) | MLG Columbus 2016",
					"1287 - wayLander (Gold) | MLG Columbus 2016",
					"1288 - Dosia | MLG Columbus 2016",
					"1289 - Dosia (Foil) | MLG Columbus 2016",
					"1290 - Dosia (Gold) | MLG Columbus 2016",
					"1291 - hooch | MLG Columbus 2016",
					"1292 - hooch (Foil) | MLG Columbus 2016",
					"1293 - hooch (Gold) | MLG Columbus 2016",
					"1294 - mou | MLG Columbus 2016",
					"1295 - mou (Foil) | MLG Columbus 2016",
					"1296 - mou (Gold) | MLG Columbus 2016",
					"1297 - AdreN  | MLG Columbus 2016",
					"1298 - AdreN (Foil)  | MLG Columbus 2016",
					"1299 - AdreN (Gold)  | MLG Columbus 2016",
					"1300 - byali | MLG Columbus 2016",
					"1301 - byali (Foil) | MLG Columbus 2016",
					"1302 - byali (Gold) | MLG Columbus 2016",
					"1303 - NEO | MLG Columbus 2016",
					"1304 - NEO (Foil) | MLG Columbus 2016",
					"1305 - NEO (Gold) | MLG Columbus 2016",
					"1306 - pashaBiceps | MLG Columbus 2016",
					"1307 - pashaBiceps (Foil) | MLG Columbus 2016",
					"1308 - pashaBiceps (Gold) | MLG Columbus 2016",
					"1309 - Snax | MLG Columbus 2016",
					"1310 - Snax (Foil) | MLG Columbus 2016",
					"1311 - Snax (Gold) | MLG Columbus 2016",
					"1312 - TaZ | MLG Columbus 2016",
					"1313 - TaZ (Foil) | MLG Columbus 2016",
					"1314 - TaZ (Gold) | MLG Columbus 2016",
					"1315 - All-Stars Orange (Holo)",
					"1316 - All-Stars Blue (Holo)",
					"1317 - Ninjas in Pyjamas | Cologne 2016",
					"1318 - Ninjas in Pyjamas (Holo) | Cologne 2016",
					"1319 - Ninjas in Pyjamas (Foil) | Cologne 2016",
					"1320 - Ninjas in Pyjamas (Gold) | Cologne 2016",
					"1321 - OpTic Gaming | Cologne 2016",
					"1322 - OpTic Gaming (Holo) | Cologne 2016",
					"1323 - OpTic Gaming (Foil) | Cologne 2016",
					"1324 - OpTic Gaming (Gold) | Cologne 2016",
					"1325 - Counter Logic Gaming | Cologne 2016",
					"1326 - Counter Logic Gaming (Holo) | Cologne 2016",
					"1327 - Counter Logic Gaming (Foil) | Cologne 2016",
					"1328 - Counter Logic Gaming (Gold) | Cologne 2016",
					"1329 - Gambit Gaming | Cologne 2016",
					"1330 - Gambit Gaming (Holo) | Cologne 2016",
					"1331 - Gambit Gaming (Foil) | Cologne 2016",
					"1332 - Gambit Gaming (Gold) | Cologne 2016",
					"1333 - Flipsid3 Tactics | Cologne 2016",
					"1334 - Flipsid3 Tactics (Holo) | Cologne 2016",
					"1335 - Flipsid3 Tactics (Foil) | Cologne 2016",
					"1336 - Flipsid3 Tactics (Gold) | Cologne 2016",
					"1337 - Team Liquid | Cologne 2016",
					"1338 - Team Liquid (Holo) | Cologne 2016",
					"1339 - Team Liquid (Foil) | Cologne 2016",
					"1340 - Team Liquid (Gold) | Cologne 2016",
					"1341 - mousesports | Cologne 2016",
					"1342 - mousesports (Holo) | Cologne 2016",
					"1343 - mousesports (Foil) | Cologne 2016",
					"1344 - mousesports (Gold) | Cologne 2016",
					"1345 - Natus Vincere | Cologne 2016",
					"1346 - Natus Vincere (Holo) | Cologne 2016",
					"1347 - Natus Vincere (Foil) | Cologne 2016",
					"1348 - Natus Vincere (Gold) | Cologne 2016",
					"1349 - Virtus.Pro | Cologne 2016",
					"1350 - Virtus.Pro (Holo) | Cologne 2016",
					"1351 - Virtus.Pro (Foil) | Cologne 2016",
					"1352 - Virtus.Pro (Gold) | Cologne 2016",
					"1353 - SK Gaming | Cologne 2016",
					"1354 - SK Gaming (Holo) | Cologne 2016",
					"1355 - SK Gaming (Foil) | Cologne 2016",
					"1356 - SK Gaming (Gold) | Cologne 2016",
					"1357 - G2 Esports | Cologne 2016",
					"1358 - G2 Esports (Holo) | Cologne 2016",
					"1359 - G2 Esports (Foil) | Cologne 2016",
					"1360 - G2 Esports (Gold) | Cologne 2016",
					"1361 - FaZe Clan | Cologne 2016",
					"1362 - FaZe Clan (Holo) | Cologne 2016",
					"1363 - FaZe Clan (Foil) | Cologne 2016",
					"1364 - FaZe Clan (Gold) | Cologne 2016",
					"1365 - Astralis | Cologne 2016",
					"1366 - Astralis (Holo) | Cologne 2016",
					"1367 - Astralis (Foil) | Cologne 2016",
					"1368 - Astralis (Gold) | Cologne 2016",
					"1369 - Team EnVyUs | Cologne 2016",
					"1370 - Team EnVyUs (Holo) | Cologne 2016",
					"1371 - Team EnVyUs (Foil) | Cologne 2016",
					"1372 - Team EnVyUs (Gold) | Cologne 2016",
					"1373 - Fnatic | Cologne 2016",
					"1374 - Fnatic (Holo) | Cologne 2016",
					"1375 - Fnatic (Foil) | Cologne 2016",
					"1376 - Fnatic (Gold) | Cologne 2016",
					"1377 - Team Dignitas | Cologne 2016",
					"1378 - Team Dignitas (Holo) | Cologne 2016",
					"1379 - Team Dignitas (Foil) | Cologne 2016",
					"1380 - Team Dignitas (Gold) | Cologne 2016",
					"1381 - ESL | Cologne 2016",
					"1382 - ESL (Holo) | Cologne 2016",
					"1383 - ESL (Foil) | Cologne 2016",
					"1384 - ESL (Gold) | Cologne 2016",
					"1385 - reltuC | Cologne 2016",
					"1386 - reltuC (Foil) | Cologne 2016",
					"1387 - reltuC (Gold) | Cologne 2016",
					"1388 - koosta | Cologne 2016",
					"1389 - koosta (Foil) | Cologne 2016",
					"1390 - koosta (Gold) | Cologne 2016",
					"1391 - hazed | Cologne 2016",
					"1392 - hazed (Foil) | Cologne 2016",
					"1393 - hazed (Gold) | Cologne 2016",
					"1394 - pita | Cologne 2016",
					"1395 - pita (Foil) | Cologne 2016",
					"1396 - pita (Gold) | Cologne 2016",
					"1397 - tarik | Cologne 2016",
					"1398 - tarik (Foil) | Cologne 2016",
					"1399 - tarik (Gold) | Cologne 2016",
					"1400 - daps | Cologne 2016",
					"1401 - daps (Foil) | Cologne 2016",
					"1402 - daps (Gold) | Cologne 2016",
					"1403 - mixwell | Cologne 2016",
					"1404 - mixwell (Foil) | Cologne 2016",
					"1405 - mixwell (Gold) | Cologne 2016",
					"1406 - NAF | Cologne 2016",
					"1407 - NAF (Foil) | Cologne 2016",
					"1408 - NAF (Gold) | Cologne 2016",
					"1409 - RUSH | Cologne 2016",
					"1410 - RUSH (Foil) | Cologne 2016",
					"1411 - RUSH (Gold) | Cologne 2016",
					"1412 - stanislaw | Cologne 2016",
					"1413 - stanislaw (Foil) | Cologne 2016",
					"1414 - stanislaw (Gold) | Cologne 2016",
					"1415 - apEX | Cologne 2016",
					"1416 - apEX (Foil) | Cologne 2016",
					"1417 - apEX (Gold) | Cologne 2016",
					"1418 - Happy | Cologne 2016",
					"1419 - Happy (Foil) | Cologne 2016",
					"1420 - Happy (Gold) | Cologne 2016",
					"1421 - DEVIL | Cologne 2016",
					"1422 - DEVIL (Foil) | Cologne 2016",
					"1423 - DEVIL (Gold) | Cologne 2016",
					"1424 - kennyS | Cologne 2016",
					"1425 - kennyS (Foil) | Cologne 2016",
					"1426 - kennyS (Gold) | Cologne 2016",
					"1427 - NBK- | Cologne 2016",
					"1428 - NBK- (Foil) | Cologne 2016",
					"1429 - NBK- (Gold) | Cologne 2016",
					"1430 - B1ad3 | Cologne 2016",
					"1431 - B1ad3 (Foil) | Cologne 2016",
					"1432 - B1ad3 (Gold) | Cologne 2016",
					"1433 - wayLander | Cologne 2016",
					"1434 - wayLander (Foil) | Cologne 2016",
					"1435 - wayLander (Gold) | Cologne 2016",
					"1436 - Shara | Cologne 2016",
					"1437 - Shara (Foil) | Cologne 2016",
					"1438 - Shara (Gold) | Cologne 2016",
					"1439 - markeloff | Cologne 2016",
					"1440 - markeloff (Foil) | Cologne 2016",
					"1441 - markeloff (Gold) | Cologne 2016",
					"1442 - WorldEdit | Cologne 2016",
					"1443 - WorldEdit (Foil) | Cologne 2016",
					"1444 - WorldEdit (Gold) | Cologne 2016",
					"1445 - flusha | Cologne 2016",
					"1446 - flusha (Foil) | Cologne 2016",
					"1447 - flusha (Gold) | Cologne 2016",
					"1448 - JW | Cologne 2016",
					"1449 - JW (Foil) | Cologne 2016",
					"1450 - JW (Gold) | Cologne 2016",
					"1451 - KRIMZ | Cologne 2016",
					"1452 - KRIMZ (Foil) | Cologne 2016",
					"1453 - KRIMZ (Gold) | Cologne 2016",
					"1454 - olofmeister | Cologne 2016",
					"1455 - olofmeister (Foil) | Cologne 2016",
					"1456 - olofmeister (Gold) | Cologne 2016",
					"1457 - dennis | Cologne 2016",
					"1458 - dennis (Foil) | Cologne 2016",
					"1459 - dennis (Gold) | Cologne 2016",
					"1460 - aizy | Cologne 2016",
					"1461 - aizy (Foil) | Cologne 2016",
					"1462 - aizy (Gold) | Cologne 2016",
					"1463 - fox | Cologne 2016",
					"1464 - fox (Foil) | Cologne 2016",
					"1465 - fox (Gold) | Cologne 2016",
					"1466 - kioShiMa | Cologne 2016",
					"1467 - kioShiMa (Foil) | Cologne 2016",
					"1468 - kioShiMa (Gold) | Cologne 2016",
					"1469 - rain | Cologne 2016",
					"1470 - rain (Foil) | Cologne 2016",
					"1471 - rain (Gold) | Cologne 2016",
					"1472 - jkaem | Cologne 2016",
					"1473 - jkaem (Foil) | Cologne 2016",
					"1474 - jkaem (Gold) | Cologne 2016",
					"1475 - coldzera | Cologne 2016",
					"1476 - coldzera (Foil) | Cologne 2016",
					"1477 - coldzera (Gold) | Cologne 2016",
					"1478 - FalleN | Cologne 2016",
					"1479 - FalleN (Foil) | Cologne 2016",
					"1480 - FalleN (Gold) | Cologne 2016",
					"1481 - fer | Cologne 2016",
					"1482 - fer (Foil) | Cologne 2016",
					"1483 - fer (Gold) | Cologne 2016",
					"1484 - fnx | Cologne 2016",
					"1485 - fnx (Foil) | Cologne 2016",
					"1486 - fnx (Gold) | Cologne 2016",
					"1487 - TACO | Cologne 2016",
					"1488 - TACO (Foil) | Cologne 2016",
					"1489 - TACO (Gold) | Cologne 2016",
					"1490 - chrisJ | Cologne 2016",
					"1491 - chrisJ (Foil) | Cologne 2016",
					"1492 - chrisJ (Gold) | Cologne 2016",
					"1493 - denis | Cologne 2016",
					"1494 - denis (Foil) | Cologne 2016",
					"1495 - denis (Gold) | Cologne 2016",
					"1496 - Spiidi | Cologne 2016",
					"1497 - Spiidi (Foil) | Cologne 2016",
					"1498 - Spiidi (Gold) | Cologne 2016",
					"1499 - nex | Cologne 2016",
					"1500 - nex (Foil) | Cologne 2016",
					"1501 - nex (Gold) | Cologne 2016",
					"1502 - NiKo | Cologne 2016",
					"1503 - NiKo (Foil) | Cologne 2016",
					"1504 - NiKo (Gold) | Cologne 2016",
					"1505 - Edward | Cologne 2016",
					"1506 - Edward (Foil) | Cologne 2016",
					"1507 - Edward (Gold) | Cologne 2016",
					"1508 - flamie | Cologne 2016",
					"1509 - flamie (Foil) | Cologne 2016",
					"1510 - flamie (Gold) | Cologne 2016",
					"1511 - GuardiaN | Cologne 2016",
					"1512 - GuardiaN (Foil) | Cologne 2016",
					"1513 - GuardiaN (Gold) | Cologne 2016",
					"1514 - seized | Cologne 2016",
					"1515 - seized (Foil) | Cologne 2016",
					"1516 - seized (Gold) | Cologne 2016",
					"1517 - Zeus | Cologne 2016",
					"1518 - Zeus (Foil) | Cologne 2016",
					"1519 - Zeus (Gold) | Cologne 2016",
					"1520 - pyth | Cologne 2016",
					"1521 - pyth (Foil) | Cologne 2016",
					"1522 - pyth (Gold) | Cologne 2016",
					"1523 - f0rest | Cologne 2016",
					"1524 - f0rest (Foil) | Cologne 2016",
					"1525 - f0rest (Gold) | Cologne 2016",
					"1526 - friberg | Cologne 2016",
					"1527 - friberg (Foil) | Cologne 2016",
					"1528 - friberg (Gold) | Cologne 2016",
					"1529 - GeT_RiGhT | Cologne 2016",
					"1530 - GeT_RiGhT (Foil) | Cologne 2016",
					"1531 - GeT_RiGhT (Gold) | Cologne 2016",
					"1532 - Xizt | Cologne 2016",
					"1533 - Xizt (Foil) | Cologne 2016",
					"1534 - Xizt (Gold) | Cologne 2016",
					"1535 - cajunb | Cologne 2016",
					"1536 - cajunb (Foil) | Cologne 2016",
					"1537 - cajunb (Gold) | Cologne 2016",
					"1538 - MSL | Cologne 2016",
					"1539 - MSL (Foil) | Cologne 2016",
					"1540 - MSL (Gold) | Cologne 2016",
					"1541 - TENZKI | Cologne 2016",
					"1542 - TENZKI (Foil) | Cologne 2016",
					"1543 - TENZKI (Gold) | Cologne 2016",
					"1544 - RUBINO | Cologne 2016",
					"1545 - RUBINO (Foil) | Cologne 2016",
					"1546 - RUBINO (Gold) | Cologne 2016",
					"1547 - k0nfig | Cologne 2016",
					"1548 - k0nfig (Foil) | Cologne 2016",
					"1549 - k0nfig (Gold) | Cologne 2016",
					"1550 - jdm64 | Cologne 2016",
					"1551 - jdm64 (Foil) | Cologne 2016",
					"1552 - jdm64 (Gold) | Cologne 2016",
					"1553 - EliGE | Cologne 2016",
					"1554 - EliGE (Foil) | Cologne 2016",
					"1555 - EliGE (Gold) | Cologne 2016",
					"1556 - s1mple | Cologne 2016",
					"1557 - s1mple (Foil) | Cologne 2016",
					"1558 - s1mple (Gold) | Cologne 2016",
					"1559 - Hiko | Cologne 2016",
					"1560 - Hiko (Foil) | Cologne 2016",
					"1561 - Hiko (Gold) | Cologne 2016",
					"1562 - nitr0 | Cologne 2016",
					"1563 - nitr0 (Foil) | Cologne 2016",
					"1564 - nitr0 (Gold) | Cologne 2016",
					"1565 - bodyy | Cologne 2016",
					"1566 - bodyy (Foil) | Cologne 2016",
					"1567 - bodyy (Gold) | Cologne 2016",
					"1568 - RpK | Cologne 2016",
					"1569 - RpK (Foil) | Cologne 2016",
					"1570 - RpK (Gold) | Cologne 2016",
					"1571 - ScreaM | Cologne 2016",
					"1572 - ScreaM (Foil) | Cologne 2016",
					"1573 - ScreaM (Gold) | Cologne 2016",
					"1574 - shox | Cologne 2016",
					"1575 - shox (Foil) | Cologne 2016",
					"1576 - shox (Gold) | Cologne 2016",
					"1577 - SmithZz | Cologne 2016",
					"1578 - SmithZz (Foil) | Cologne 2016",
					"1579 - SmithZz (Gold) | Cologne 2016",
					"1580 - gla1ve | Cologne 2016",
					"1581 - gla1ve (Foil) | Cologne 2016",
					"1582 - gla1ve (Gold) | Cologne 2016",
					"1583 - device | Cologne 2016",
					"1584 - device (Foil) | Cologne 2016",
					"1585 - device (Gold) | Cologne 2016",
					"1586 - dupreeh | Cologne 2016",
					"1587 - dupreeh (Foil) | Cologne 2016",
					"1588 - dupreeh (Gold) | Cologne 2016",
					"1589 - karrigan | Cologne 2016",
					"1590 - karrigan (Foil) | Cologne 2016",
					"1591 - karrigan (Gold) | Cologne 2016",
					"1592 - Xyp9x | Cologne 2016",
					"1593 - Xyp9x (Foil) | Cologne 2016",
					"1594 - Xyp9x (Gold) | Cologne 2016",
					"1595 - spaze | Cologne 2016",
					"1596 - spaze (Foil) | Cologne 2016",
					"1597 - spaze (Gold) | Cologne 2016",
					"1598 - Dosia | Cologne 2016",
					"1599 - Dosia (Foil) | Cologne 2016",
					"1600 - Dosia (Gold) | Cologne 2016",
					"1601 - hooch | Cologne 2016",
					"1602 - hooch (Foil) | Cologne 2016",
					"1603 - hooch (Gold) | Cologne 2016",
					"1604 - mou | Cologne 2016",
					"1605 - mou (Foil) | Cologne 2016",
					"1606 - mou (Gold) | Cologne 2016",
					"1607 - AdreN | Cologne 2016",
					"1608 - AdreN (Foil) | Cologne 2016",
					"1609 - AdreN (Gold) | Cologne 2016",
					"1610 - byali | Cologne 2016",
					"1611 - byali (Foil) | Cologne 2016",
					"1612 - byali (Gold) | Cologne 2016",
					"1613 - NEO | Cologne 2016",
					"1614 - NEO (Foil) | Cologne 2016",
					"1615 - NEO (Gold) | Cologne 2016",
					"1616 - pashaBiceps | Cologne 2016",
					"1617 - pashaBiceps (Foil) | Cologne 2016",
					"1618 - pashaBiceps (Gold) | Cologne 2016",
					"1619 - Snax | Cologne 2016",
					"1620 - Snax (Foil) | Cologne 2016",
					"1621 - Snax (Gold) | Cologne 2016",
					"1622 - TaZ | Cologne 2016",
					"1623 - TaZ (Foil) | Cologne 2016",
					"1624 - TaZ (Gold) | Cologne 2016",
					"1625 - Boris",
					"1626 - Max",
					"1627 - Stan",
					"1628 - Jack",
					"1629 - Perry",
					"1630 - Viggo",
					"1631 - Joan",
					"1632 - Boris (Holo)",
					"1633 - Max (Holo)",
					"1634 - Stan (Holo)",
					"1635 - Jack (Holo)",
					"1636 - Perry (Holo)",
					"1637 - Viggo (Holo)",
					"1638 - Joan (Holo)",
					"1639 - Basilisk",
					"1640 - Dragon",
					"1641 - Hippocamp",
					"1642 - Manticore",
					"1643 - Pegasus",
					"1644 - Phoenix Reborn",
					"1645 - Sphinx",
					"1646 - Hippocamp (Holo)",
					"1647 - Manticore (Holo)",
					"1648 - Pegasus (Holo)",
					"1649 - Sphinx (Holo)",
					"1650 - Basilisk (Foil)",
					"1651 - Dragon (Foil)",
					"1652 - Phoenix Reborn (Foil)",
					"1653 - Blood Boiler",
					"1654 - Chabo",
					"1655 - Drug War Veteran",
					"1656 - Flickshot",
					"1657 - Hamster Hawk",
					"1658 - Ivette",
					"1659 - Kawaii Killer CT",
					"1660 - Kawaii Killer Terrorist",
					"1661 - Martha",
					"1662 - Old School",
					"1663 - Pocket BBQ",
					"1664 - Rekt",
					"1665 - Shave Master",
					"1666 - Shooting Star Return",
					"1667 - Rising Skull",
					"1668 - Tamara",
					"1669 - Unicorn",
					"1670 - Winged Defuser",
					"1671 - Ace",
					"1672 - Banana",
					"1673 - Cerberus",
					"1674 - Clutch King",
					"1675 - Crown",
					"1676 - Guardian",
					"1677 - EZ",
					"1678 - Fire Serpent",
					"1679 - Howling Dawn",
					"1680 - Kisses",
					"1681 - Easy Peasy",
					"1682 - Nice Shot",
					"1683 - Phoenix",
					"1684 - Real MVP",
					"1685 - R.I.P.I.P.",
					"1686 - Skull n' Crosshairs",
					"1687 - Welcome to the Clutch",
					"1688 - Wings",
					"1697 - X-Axes",
					"1698 - Death Sentence",
					"1699 - Chess King",
					"1700 - King Me",
					"1701 - Keep the Change",
					"1702 - Double",
					"1703 - Eco",
					"1704 - Tilt",
					"1705 - Speechless",
					"1706 - QQ",
					"1707 - Mr. Teeth",
					"1708 - Ninja",
					"1709 - Worry",
					"1710 - Rage Mode",
					"1711 - Eye Spy",
					"1712 - GGEZ",
					"1713 - GGWP",
					"1714 - GLHF",
					"1715 - Quickdraw",
					"1716 - Backstab",
					"1717 - Loser",
					"1718 - Sheriff",
					"1719 - Tombstone",
					"1720 - Heart",
					"1721 - 8-Ball",
					"1722 - Lambda",
					"1723 - Still Happy",
					"1724 - Jump Shot",
					"1725 - Karambit",
					"1726 - X-Knives",
					"1727 - Toasted",
					"1728 - Bling",
					"1729 - Noscope",
					"1730 - Piggles",
					"1731 - Popdog",
					"1732 - Cocky",
					"1733 - NaCl",
					"1734 - Sorry",
					"1735 - Eat It",
					"1736 - Take Flight",
					"1737 - GTG",
					"1738 - Astralis | Atlanta 2017",
					"1739 - Astralis (Holo) | Atlanta 2017",
					"1740 - Astralis (Foil) | Atlanta 2017",
					"1741 - Astralis (Gold) | Atlanta 2017",
					"1742 - Team EnVyUs | Atlanta 2017",
					"1743 - Team EnVyUs (Holo) | Atlanta 2017",
					"1744 - Team EnVyUs (Foil) | Atlanta 2017",
					"1745 - Team EnVyUs (Gold) | Atlanta 2017",
					"1746 - FaZe Clan | Atlanta 2017",
					"1747 - FaZe Clan (Holo) | Atlanta 2017",
					"1748 - FaZe Clan (Foil) | Atlanta 2017",
					"1749 - FaZe Clan (Gold) | Atlanta 2017",
					"1750 - Flipsid3 Tactics | Atlanta 2017",
					"1751 - Flipsid3 Tactics (Holo) | Atlanta 2017",
					"1752 - Flipsid3 Tactics (Foil) | Atlanta 2017",
					"1753 - Flipsid3 Tactics (Gold) | Atlanta 2017",
					"1754 - Fnatic | Atlanta 2017",
					"1755 - Fnatic (Holo) | Atlanta 2017",
					"1756 - Fnatic (Foil) | Atlanta 2017",
					"1757 - Fnatic (Gold) | Atlanta 2017",
					"1758 - G2 Esports | Atlanta 2017",
					"1759 - G2 Esports (Holo) | Atlanta 2017",
					"1760 - G2 Esports (Foil) | Atlanta 2017",
					"1761 - G2 Esports (Gold) | Atlanta 2017",
					"1762 - Gambit Gaming | Atlanta 2017",
					"1763 - Gambit Gaming (Holo) | Atlanta 2017",
					"1764 - Gambit Gaming (Foil) | Atlanta 2017",
					"1765 - Gambit Gaming (Gold) | Atlanta 2017",
					"1766 - GODSENT | Atlanta 2017",
					"1767 - GODSENT (Holo) | Atlanta 2017",
					"1768 - GODSENT (Foil) | Atlanta 2017",
					"1769 - GODSENT (Gold) | Atlanta 2017",
					"1770 - HellRaisers | Atlanta 2017",
					"1771 - HellRaisers (Holo) | Atlanta 2017",
					"1772 - HellRaisers (Foil) | Atlanta 2017",
					"1773 - HellRaisers (Gold) | Atlanta 2017",
					"1774 - mousesports | Atlanta 2017",
					"1775 - mousesports (Holo) | Atlanta 2017",
					"1776 - mousesports (Foil) | Atlanta 2017",
					"1777 - mousesports (Gold) | Atlanta 2017",
					"1778 - Natus Vincere | Atlanta 2017",
					"1779 - Natus Vincere (Holo) | Atlanta 2017",
					"1780 - Natus Vincere (Foil) | Atlanta 2017",
					"1781 - Natus Vincere (Gold) | Atlanta 2017",
					"1782 - North | Atlanta 2017",
					"1783 - North (Holo) | Atlanta 2017",
					"1784 - North (Foil) | Atlanta 2017",
					"1785 - North (Gold) | Atlanta 2017",
					"1786 - OpTic Gaming | Atlanta 2017",
					"1787 - OpTic Gaming (Holo) | Atlanta 2017",
					"1788 - OpTic Gaming (Foil) | Atlanta 2017",
					"1789 - OpTic Gaming (Gold) | Atlanta 2017",
					"1790 - SK Gaming | Atlanta 2017",
					"1791 - SK Gaming (Holo) | Atlanta 2017",
					"1792 - SK Gaming (Foil) | Atlanta 2017",
					"1793 - SK Gaming (Gold) | Atlanta 2017",
					"1794 - Team Liquid | Atlanta 2017",
					"1795 - Team Liquid (Holo) | Atlanta 2017",
					"1796 - Team Liquid (Foil) | Atlanta 2017",
					"1797 - Team Liquid (Gold) | Atlanta 2017",
					"1798 - Virtus.Pro | Atlanta 2017",
					"1799 - Virtus.Pro (Holo) | Atlanta 2017",
					"1800 - Virtus.Pro (Foil) | Atlanta 2017",
					"1801 - Virtus.Pro (Gold) | Atlanta 2017",
					"1802 - ELEAGUE | Atlanta 2017",
					"1803 - ELEAGUE (Holo) | Atlanta 2017",
					"1804 - ELEAGUE (Foil) | Atlanta 2017",
					"1805 - ELEAGUE (Gold) | Atlanta 2017",
					"1806 - Astralis | Atlanta 2017",
					"1807 - Team EnVyUs | Atlanta 2017",
					"1808 - FaZe Clan | Atlanta 2017",
					"1809 - Flipsid3 Tactics | Atlanta 2017",
					"1810 - Fnatic | Atlanta 2017",
					"1811 - G2 Esports | Atlanta 2017",
					"1812 - Gambit Gaming | Atlanta 2017",
					"1813 - GODSENT | Atlanta 2017",
					"1814 - HellRaisers | Atlanta 2017",
					"1815 - mousesports | Atlanta 2017",
					"1816 - Natus Vincere | Atlanta 2017",
					"1817 - North | Atlanta 2017",
					"1818 - OpTic Gaming | Atlanta 2017",
					"1819 - SK Gaming | Atlanta 2017",
					"1820 - Team Liquid | Atlanta 2017",
					"1821 - Virtus.Pro | Atlanta 2017",
					"1822 - ELEAGUE | Atlanta 2017",
					"1823 - STYKO | Atlanta 2017",
					"1824 - STYKO (Foil) | Atlanta 2017",
					"1825 - STYKO (Gold) | Atlanta 2017",
					"1826 - Zero | Atlanta 2017",
					"1827 - Zero (Foil) | Atlanta 2017",
					"1828 - Zero (Gold) | Atlanta 2017",
					"1829 - DeadFox | Atlanta 2017",
					"1830 - DeadFox (Foil) | Atlanta 2017",
					"1831 - DeadFox (Gold) | Atlanta 2017",
					"1832 - bondik | Atlanta 2017",
					"1833 - bondik (Foil) | Atlanta 2017",
					"1834 - bondik (Gold) | Atlanta 2017",
					"1835 - ANGE1 | Atlanta 2017",
					"1836 - ANGE1 (Foil) | Atlanta 2017",
					"1837 - ANGE1 (Gold) | Atlanta 2017",
					"1838 - tarik | Atlanta 2017",
					"1839 - tarik (Foil) | Atlanta 2017",
					"1840 - tarik (Gold) | Atlanta 2017",
					"1841 - mixwell | Atlanta 2017",
					"1842 - mixwell (Foil) | Atlanta 2017",
					"1843 - mixwell (Gold) | Atlanta 2017",
					"1844 - NAF | Atlanta 2017",
					"1845 - NAF (Foil) | Atlanta 2017",
					"1846 - NAF (Gold) | Atlanta 2017",
					"1847 - RUSH | Atlanta 2017",
					"1848 - RUSH (Foil) | Atlanta 2017",
					"1849 - RUSH (Gold) | Atlanta 2017",
					"1850 - stanislaw | Atlanta 2017",
					"1851 - stanislaw (Foil) | Atlanta 2017",
					"1852 - stanislaw (Gold) | Atlanta 2017",
					"1853 - apEX | Atlanta 2017",
					"1854 - apEX (Foil) | Atlanta 2017",
					"1855 - apEX (Gold) | Atlanta 2017",
					"1856 - Happy | Atlanta 2017",
					"1857 - Happy (Foil) | Atlanta 2017",
					"1858 - Happy (Gold) | Atlanta 2017",
					"1859 - SIXER | Atlanta 2017",
					"1860 - SIXER (Foil) | Atlanta 2017",
					"1861 - SIXER (Gold) | Atlanta 2017",
					"1862 - kennyS | Atlanta 2017",
					"1863 - kennyS (Foil) | Atlanta 2017",
					"1864 - kennyS (Gold) | Atlanta 2017",
					"1865 - NBK- | Atlanta 2017",
					"1866 - NBK- (Foil) | Atlanta 2017",
					"1867 - NBK- (Gold) | Atlanta 2017",
					"1868 - B1ad3 | Atlanta 2017",
					"1869 - B1ad3 (Foil) | Atlanta 2017",
					"1870 - B1ad3 (Gold) | Atlanta 2017",
					"1871 - wayLander | Atlanta 2017",
					"1872 - wayLander (Foil) | Atlanta 2017",
					"1873 - wayLander (Gold) | Atlanta 2017",
					"1874 - electronic | Atlanta 2017",
					"1875 - electronic (Foil) | Atlanta 2017",
					"1876 - electronic (Gold) | Atlanta 2017",
					"1877 - markeloff | Atlanta 2017",
					"1878 - markeloff (Foil) | Atlanta 2017",
					"1879 - markeloff (Gold) | Atlanta 2017",
					"1880 - WorldEdit | Atlanta 2017",
					"1881 - WorldEdit (Foil) | Atlanta 2017",
					"1882 - WorldEdit (Gold) | Atlanta 2017",
					"1883 - twist | Atlanta 2017",
					"1884 - twist (Foil) | Atlanta 2017",
					"1885 - twist (Gold) | Atlanta 2017",
					"1886 - disco doplan | Atlanta 2017",
					"1887 - disco doplan (Foil) | Atlanta 2017",
					"1888 - disco doplan (Gold) | Atlanta 2017",
					"1889 - KRIMZ | Atlanta 2017",
					"1890 - KRIMZ (Foil) | Atlanta 2017",
					"1891 - KRIMZ (Gold) | Atlanta 2017",
					"1892 - olofmeister | Atlanta 2017",
					"1893 - olofmeister (Foil) | Atlanta 2017",
					"1894 - olofmeister (Gold) | Atlanta 2017",
					"1895 - dennis | Atlanta 2017",
					"1896 - dennis (Foil) | Atlanta 2017",
					"1897 - dennis (Gold) | Atlanta 2017",
					"1898 - aizy | Atlanta 2017",
					"1899 - aizy (Foil) | Atlanta 2017",
					"1900 - aizy (Gold) | Atlanta 2017",
					"1901 - allu | Atlanta 2017",
					"1902 - allu (Foil) | Atlanta 2017",
					"1903 - allu (Gold) | Atlanta 2017",
					"1904 - kioShiMa | Atlanta 2017",
					"1905 - kioShiMa (Foil) | Atlanta 2017",
					"1906 - kioShiMa (Gold) | Atlanta 2017",
					"1907 - rain | Atlanta 2017",
					"1908 - rain (Foil) | Atlanta 2017",
					"1909 - rain (Gold) | Atlanta 2017",
					"1910 - karrigan | Atlanta 2017",
					"1911 - karrigan (Foil) | Atlanta 2017",
					"1912 - karrigan (Gold) | Atlanta 2017",
					"1913 - coldzera | Atlanta 2017",
					"1914 - coldzera (Foil) | Atlanta 2017",
					"1915 - coldzera (Gold) | Atlanta 2017",
					"1916 - FalleN | Atlanta 2017",
					"1917 - FalleN (Foil) | Atlanta 2017",
					"1918 - FalleN (Gold) | Atlanta 2017",
					"1919 - fer | Atlanta 2017",
					"1920 - fer (Foil) | Atlanta 2017",
					"1921 - fer (Gold) | Atlanta 2017",
					"1922 - fox | Atlanta 2017",
					"1923 - fox (Foil) | Atlanta 2017",
					"1924 - fox (Gold) | Atlanta 2017",
					"1925 - TACO | Atlanta 2017",
					"1926 - TACO (Foil) | Atlanta 2017",
					"1927 - TACO (Gold) | Atlanta 2017",
					"1928 - chrisJ | Atlanta 2017",
					"1929 - chrisJ (Foil) | Atlanta 2017",
					"1930 - chrisJ (Gold) | Atlanta 2017",
					"1931 - denis | Atlanta 2017",
					"1932 - denis (Foil) | Atlanta 2017",
					"1933 - denis (Gold) | Atlanta 2017",
					"1934 - Spiidi | Atlanta 2017",
					"1935 - Spiidi (Foil) | Atlanta 2017",
					"1936 - Spiidi (Gold) | Atlanta 2017",
					"1937 - loWel | Atlanta 2017",
					"1938 - loWel (Foil) | Atlanta 2017",
					"1939 - loWel (Gold) | Atlanta 2017",
					"1940 - NiKo | Atlanta 2017",
					"1941 - NiKo (Foil) | Atlanta 2017",
					"1942 - NiKo (Gold) | Atlanta 2017",
					"1943 - Edward | Atlanta 2017",
					"1944 - Edward (Foil) | Atlanta 2017",
					"1945 - Edward (Gold) | Atlanta 2017",
					"1946 - flamie | Atlanta 2017",
					"1947 - flamie (Foil) | Atlanta 2017",
					"1948 - flamie (Gold) | Atlanta 2017",
					"1949 - GuardiaN | Atlanta 2017",
					"1950 - GuardiaN (Foil) | Atlanta 2017",
					"1951 - GuardiaN (Gold) | Atlanta 2017",
					"1952 - seized | Atlanta 2017",
					"1953 - seized (Foil) | Atlanta 2017",
					"1954 - seized (Gold) | Atlanta 2017",
					"1955 - s1mple | Atlanta 2017",
					"1956 - s1mple (Foil) | Atlanta 2017",
					"1957 - s1mple (Gold) | Atlanta 2017",
					"1958 - znajder | Atlanta 2017",
					"1959 - znajder (Foil) | Atlanta 2017",
					"1960 - znajder (Gold) | Atlanta 2017",
					"1961 - Lekr0 | Atlanta 2017",
					"1962 - Lekr0 (Foil) | Atlanta 2017",
					"1963 - Lekr0 (Gold) | Atlanta 2017",
					"1964 - pronax | Atlanta 2017",
					"1965 - pronax (Foil) | Atlanta 2017",
					"1966 - pronax (Gold) | Atlanta 2017",
					"1967 - JW | Atlanta 2017",
					"1968 - JW (Foil) | Atlanta 2017",
					"1969 - JW (Gold) | Atlanta 2017",
					"1970 - flusha | Atlanta 2017",
					"1971 - flusha (Foil) | Atlanta 2017",
					"1972 - flusha (Gold) | Atlanta 2017",
					"1973 - cajunb | Atlanta 2017",
					"1974 - cajunb (Foil) | Atlanta 2017",
					"1975 - cajunb (Gold) | Atlanta 2017",
					"1976 - MSL | Atlanta 2017",
					"1977 - MSL (Foil) | Atlanta 2017",
					"1978 - MSL (Gold) | Atlanta 2017",
					"1979 - Magisk | Atlanta 2017",
					"1980 - Magisk (Foil) | Atlanta 2017",
					"1981 - Magisk (Gold) | Atlanta 2017",
					"1982 - RUBINO | Atlanta 2017",
					"1983 - RUBINO (Foil) | Atlanta 2017",
					"1984 - RUBINO (Gold) | Atlanta 2017",
					"1985 - k0nfig | Atlanta 2017",
					"1986 - k0nfig (Foil) | Atlanta 2017",
					"1987 - k0nfig (Gold) | Atlanta 2017",
					"1988 - jdm64 | Atlanta 2017",
					"1989 - jdm64 (Foil) | Atlanta 2017",
					"1990 - jdm64 (Gold) | Atlanta 2017",
					"1991 - EliGE | Atlanta 2017",
					"1992 - EliGE (Foil) | Atlanta 2017",
					"1993 - EliGE (Gold) | Atlanta 2017",
					"1994 - Pimp | Atlanta 2017",
					"1995 - Pimp (Foil) | Atlanta 2017",
					"1996 - Pimp (Gold) | Atlanta 2017",
					"1997 - Hiko | Atlanta 2017",
					"1998 - Hiko (Foil) | Atlanta 2017",
					"1999 - Hiko (Gold) | Atlanta 2017",
					"2000 - nitr0 | Atlanta 2017",
					"2001 - nitr0 (Foil) | Atlanta 2017",
					"2002 - nitr0 (Gold) | Atlanta 2017",
					"2003 - bodyy | Atlanta 2017",
					"2004 - bodyy (Foil) | Atlanta 2017",
					"2005 - bodyy (Gold) | Atlanta 2017",
					"2006 - RpK | Atlanta 2017",
					"2007 - RpK (Foil) | Atlanta 2017",
					"2008 - RpK (Gold) | Atlanta 2017",
					"2009 - ScreaM | Atlanta 2017",
					"2010 - ScreaM (Foil) | Atlanta 2017",
					"2011 - ScreaM (Gold) | Atlanta 2017",
					"2012 - shox | Atlanta 2017",
					"2013 - shox (Foil) | Atlanta 2017",
					"2014 - shox (Gold) | Atlanta 2017",
					"2015 - SmithZz | Atlanta 2017",
					"2016 - SmithZz (Foil) | Atlanta 2017",
					"2017 - SmithZz (Gold) | Atlanta 2017",
					"2018 - gla1ve | Atlanta 2017",
					"2019 - gla1ve (Foil) | Atlanta 2017",
					"2020 - gla1ve (Gold) | Atlanta 2017",
					"2021 - device | Atlanta 2017",
					"2022 - device (Foil) | Atlanta 2017",
					"2023 - device (Gold) | Atlanta 2017",
					"2024 - dupreeh | Atlanta 2017",
					"2025 - dupreeh (Foil) | Atlanta 2017",
					"2026 - dupreeh (Gold) | Atlanta 2017",
					"2027 - Kjaerbye | Atlanta 2017",
					"2028 - Kjaerbye (Foil) | Atlanta 2017",
					"2029 - Kjaerbye (Gold) | Atlanta 2017",
					"2030 - Xyp9x | Atlanta 2017",
					"2031 - Xyp9x (Foil) | Atlanta 2017",
					"2032 - Xyp9x (Gold) | Atlanta 2017",
					"2033 - Zeus | Atlanta 2017",
					"2034 - Zeus (Foil) | Atlanta 2017",
					"2035 - Zeus (Gold) | Atlanta 2017",
					"2036 - Dosia | Atlanta 2017",
					"2037 - Dosia (Foil) | Atlanta 2017",
					"2038 - Dosia (Gold) | Atlanta 2017",
					"2039 - Hobbit | Atlanta 2017",
					"2040 - Hobbit (Foil) | Atlanta 2017",
					"2041 - Hobbit (Gold) | Atlanta 2017",
					"2042 - mou | Atlanta 2017",
					"2043 - mou (Foil) | Atlanta 2017",
					"2044 - mou (Gold) | Atlanta 2017",
					"2045 - AdreN | Atlanta 2017",
					"2046 - AdreN (Foil) | Atlanta 2017",
					"2047 - AdreN (Gold) | Atlanta 2017",
					"2048 - byali | Atlanta 2017",
					"2049 - byali (Foil) | Atlanta 2017",
					"2050 - byali (Gold) | Atlanta 2017",
					"2051 - NEO | Atlanta 2017",
					"2052 - NEO (Foil) | Atlanta 2017",
					"2053 - NEO (Gold) | Atlanta 2017",
					"2054 - pashaBiceps | Atlanta 2017",
					"2055 - pashaBiceps (Foil) | Atlanta 2017",
					"2056 - pashaBiceps (Gold) | Atlanta 2017",
					"2057 - Snax | Atlanta 2017",
					"2058 - Snax (Foil) | Atlanta 2017",
					"2059 - Snax (Gold) | Atlanta 2017",
					"2060 - TaZ | Atlanta 2017",
					"2061 - TaZ (Foil) | Atlanta 2017",
					"2062 - TaZ (Gold) | Atlanta 2017",
					"2063 - Astralis | Krakow 2017",
					"2064 - Astralis (Holo) | Krakow 2017",
					"2065 - Astralis (Foil) | Krakow 2017",
					"2066 - Astralis (Gold) | Krakow 2017",
					"2067 - Virtus.Pro | Krakow 2017",
					"2068 - Virtus.Pro (Holo) | Krakow 2017",
					"2069 - Virtus.Pro (Foil) | Krakow 2017",
					"2070 - Virtus.Pro (Gold) | Krakow 2017",
					"2071 - Fnatic | Krakow 2017",
					"2072 - Fnatic (Holo) | Krakow 2017",
					"2073 - Fnatic (Foil) | Krakow 2017",
					"2074 - Fnatic (Gold) | Krakow 2017",
					"2075 - SK Gaming | Krakow 2017",
					"2076 - SK Gaming (Holo) | Krakow 2017",
					"2077 - SK Gaming (Foil) | Krakow 2017",
					"2078 - SK Gaming (Gold) | Krakow 2017",
					"2079 - Natus Vincere | Krakow 2017",
					"2080 - Natus Vincere (Holo) | Krakow 2017",
					"2081 - Natus Vincere (Foil) | Krakow 2017",
					"2082 - Natus Vincere (Gold) | Krakow 2017",
					"2083 - Gambit | Krakow 2017",
					"2084 - Gambit (Holo) | Krakow 2017",
					"2085 - Gambit (Foil) | Krakow 2017",
					"2086 - Gambit (Gold) | Krakow 2017",
					"2087 - North | Krakow 2017",
					"2088 - North (Holo) | Krakow 2017",
					"2089 - North (Foil) | Krakow 2017",
					"2090 - North (Gold) | Krakow 2017",
					"2091 - FaZe Clan | Krakow 2017",
					"2092 - FaZe Clan (Holo) | Krakow 2017",
					"2093 - FaZe Clan (Foil) | Krakow 2017",
					"2094 - FaZe Clan (Gold) | Krakow 2017",
					"2095 - mousesports | Krakow 2017",
					"2096 - mousesports (Holo) | Krakow 2017",
					"2097 - mousesports (Foil) | Krakow 2017",
					"2098 - mousesports (Gold) | Krakow 2017",
					"2099 - G2 Esports | Krakow 2017",
					"2100 - G2 Esports (Holo) | Krakow 2017",
					"2101 - G2 Esports (Foil) | Krakow 2017",
					"2102 - G2 Esports (Gold) | Krakow 2017",
					"2103 - BIG | Krakow 2017",
					"2104 - BIG (Holo) | Krakow 2017",
					"2105 - BIG (Foil) | Krakow 2017",
					"2106 - BIG (Gold) | Krakow 2017",
					"2107 - Cloud9 | Krakow 2017",
					"2108 - Cloud9 (Holo) | Krakow 2017",
					"2109 - Cloud9 (Foil) | Krakow 2017",
					"2110 - Cloud9 (Gold) | Krakow 2017",
					"2111 - PENTA Sports | Krakow 2017",
					"2112 - PENTA Sports (Holo) | Krakow 2017",
					"2113 - PENTA Sports (Foil) | Krakow 2017",
					"2114 - PENTA Sports (Gold) | Krakow 2017",
					"2115 - Flipsid3 Tactics | Krakow 2017",
					"2116 - Flipsid3 Tactics (Holo) | Krakow 2017",
					"2117 - Flipsid3 Tactics (Foil) | Krakow 2017",
					"2118 - Flipsid3 Tactics (Gold) | Krakow 2017",
					"2119 - Immortals | Krakow 2017",
					"2120 - Immortals (Holo) | Krakow 2017",
					"2121 - Immortals (Foil) | Krakow 2017",
					"2122 - Immortals (Gold) | Krakow 2017",
					"2123 - Vega Squadron | Krakow 2017",
					"2124 - Vega Squadron (Holo) | Krakow 2017",
					"2125 - Vega Squadron (Foil) | Krakow 2017",
					"2126 - Vega Squadron (Gold) | Krakow 2017",
					"2127 - PGL | Krakow 2017",
					"2128 - PGL (Holo) | Krakow 2017",
					"2129 - PGL (Foil) | Krakow 2017",
					"2130 - PGL (Gold) | Krakow 2017",
					"2131 - Astralis | Krakow 2017",
					"2132 - Virtus.Pro | Krakow 2017",
					"2133 - Fnatic | Krakow 2017",
					"2134 - SK Gaming | Krakow 2017",
					"2135 - Natus Vincere | Krakow 2017",
					"2136 - Gambit | Krakow 2017",
					"2137 - North | Krakow 2017",
					"2138 - FaZe Clan | Krakow 2017",
					"2139 - mousesports | Krakow 2017",
					"2140 - G2 Esports | Krakow 2017",
					"2141 - BIG | Krakow 2017",
					"2142 - Cloud9 | Krakow 2017",
					"2143 - PENTA Sports | Krakow 2017",
					"2144 - Flipsid3 Tactics | Krakow 2017",
					"2145 - Immortals | Krakow 2017",
					"2146 - Vega Squadron | Krakow 2017",
					"2147 - PGL | Krakow 2017",
					"2148 - device | Krakow 2017",
					"2149 - device (Foil) | Krakow 2017",
					"2150 - device (Gold) | Krakow 2017",
					"2151 - dupreeh | Krakow 2017",
					"2152 - dupreeh (Foil) | Krakow 2017",
					"2153 - dupreeh (Gold) | Krakow 2017",
					"2154 - gla1ve | Krakow 2017",
					"2155 - gla1ve (Foil) | Krakow 2017",
					"2156 - gla1ve (Gold) | Krakow 2017",
					"2157 - Kjaerbye | Krakow 2017",
					"2158 - Kjaerbye (Foil) | Krakow 2017",
					"2159 - Kjaerbye (Gold) | Krakow 2017",
					"2160 - Xyp9x | Krakow 2017",
					"2161 - Xyp9x (Foil) | Krakow 2017",
					"2162 - Xyp9x (Gold) | Krakow 2017",
					"2163 - byali | Krakow 2017",
					"2164 - byali (Foil) | Krakow 2017",
					"2165 - byali (Gold) | Krakow 2017",
					"2166 - NEO | Krakow 2017",
					"2167 - NEO (Foil) | Krakow 2017",
					"2168 - NEO (Gold) | Krakow 2017",
					"2169 - pashaBiceps | Krakow 2017",
					"2170 - pashaBiceps (Foil) | Krakow 2017",
					"2171 - pashaBiceps (Gold) | Krakow 2017",
					"2172 - Snax | Krakow 2017",
					"2173 - Snax (Foil) | Krakow 2017",
					"2174 - Snax (Gold) | Krakow 2017",
					"2175 - TaZ | Krakow 2017",
					"2176 - TaZ (Foil) | Krakow 2017",
					"2177 - TaZ (Gold) | Krakow 2017",
					"2178 - dennis | Krakow 2017",
					"2179 - dennis (Foil) | Krakow 2017",
					"2180 - dennis (Gold) | Krakow 2017",
					"2181 - flusha | Krakow 2017",
					"2182 - flusha (Foil) | Krakow 2017",
					"2183 - flusha (Gold) | Krakow 2017",
					"2184 - JW | Krakow 2017",
					"2185 - JW (Foil) | Krakow 2017",
					"2186 - JW (Gold) | Krakow 2017",
					"2187 - KRIMZ | Krakow 2017",
					"2188 - KRIMZ (Foil) | Krakow 2017",
					"2189 - KRIMZ (Gold) | Krakow 2017",
					"2190 - olofmeister | Krakow 2017",
					"2191 - olofmeister (Foil) | Krakow 2017",
					"2192 - olofmeister (Gold) | Krakow 2017",
					"2193 - coldzera | Krakow 2017",
					"2194 - coldzera (Foil) | Krakow 2017",
					"2195 - coldzera (Gold) | Krakow 2017",
					"2196 - FalleN | Krakow 2017",
					"2197 - FalleN (Foil) | Krakow 2017",
					"2198 - FalleN (Gold) | Krakow 2017",
					"2199 - felps | Krakow 2017",
					"2200 - felps (Foil) | Krakow 2017",
					"2201 - felps (Gold) | Krakow 2017",
					"2202 - fer | Krakow 2017",
					"2203 - fer (Foil) | Krakow 2017",
					"2204 - fer (Gold) | Krakow 2017",
					"2205 - TACO | Krakow 2017",
					"2206 - TACO (Foil) | Krakow 2017",
					"2207 - TACO (Gold) | Krakow 2017",
					"2208 - Edward | Krakow 2017",
					"2209 - Edward (Foil) | Krakow 2017",
					"2210 - Edward (Gold) | Krakow 2017",
					"2211 - flamie | Krakow 2017",
					"2212 - flamie (Foil) | Krakow 2017",
					"2213 - flamie (Gold) | Krakow 2017",
					"2214 - GuardiaN | Krakow 2017",
					"2215 - GuardiaN (Foil) | Krakow 2017",
					"2216 - GuardiaN (Gold) | Krakow 2017",
					"2217 - s1mple | Krakow 2017",
					"2218 - s1mple (Foil) | Krakow 2017",
					"2219 - s1mple (Gold) | Krakow 2017",
					"2220 - seized | Krakow 2017",
					"2221 - seized (Foil) | Krakow 2017",
					"2222 - seized (Gold) | Krakow 2017",
					"2223 - AdreN | Krakow 2017",
					"2224 - AdreN (Foil) | Krakow 2017",
					"2225 - AdreN (Gold) | Krakow 2017",
					"2226 - Dosia | Krakow 2017",
					"2227 - Dosia (Foil) | Krakow 2017",
					"2228 - Dosia (Gold) | Krakow 2017",
					"2229 - Hobbit | Krakow 2017",
					"2230 - Hobbit (Foil) | Krakow 2017",
					"2231 - Hobbit (Gold) | Krakow 2017",
					"2232 - mou | Krakow 2017",
					"2233 - mou (Foil) | Krakow 2017",
					"2234 - mou (Gold) | Krakow 2017",
					"2235 - Zeus | Krakow 2017",
					"2236 - Zeus (Foil) | Krakow 2017",
					"2237 - Zeus (Gold) | Krakow 2017",
					"2238 - aizy | Krakow 2017",
					"2239 - aizy (Foil) | Krakow 2017",
					"2240 - aizy (Gold) | Krakow 2017",
					"2241 - cajunb | Krakow 2017",
					"2242 - cajunb (Foil) | Krakow 2017",
					"2243 - cajunb (Gold) | Krakow 2017",
					"2244 - k0nfig | Krakow 2017",
					"2245 - k0nfig (Foil) | Krakow 2017",
					"2246 - k0nfig (Gold) | Krakow 2017",
					"2247 - Magisk | Krakow 2017",
					"2248 - Magisk (Foil) | Krakow 2017",
					"2249 - Magisk (Gold) | Krakow 2017",
					"2250 - MSL | Krakow 2017",
					"2251 - MSL (Foil) | Krakow 2017",
					"2252 - MSL (Gold) | Krakow 2017",
					"2253 - allu | Krakow 2017",
					"2254 - allu (Foil) | Krakow 2017",
					"2255 - allu (Gold) | Krakow 2017",
					"2256 - karrigan | Krakow 2017",
					"2257 - karrigan (Foil) | Krakow 2017",
					"2258 - karrigan (Gold) | Krakow 2017",
					"2259 - kioShiMa | Krakow 2017",
					"2260 - kioShiMa (Foil) | Krakow 2017",
					"2261 - kioShiMa (Gold) | Krakow 2017",
					"2262 - NiKo | Krakow 2017",
					"2263 - NiKo (Foil) | Krakow 2017",
					"2264 - NiKo (Gold) | Krakow 2017",
					"2265 - rain | Krakow 2017",
					"2266 - rain (Foil) | Krakow 2017",
					"2267 - rain (Gold) | Krakow 2017",
					"2268 - chrisJ | Krakow 2017",
					"2269 - chrisJ (Foil) | Krakow 2017",
					"2270 - chrisJ (Gold) | Krakow 2017",
					"2271 - denis | Krakow 2017",
					"2272 - denis (Foil) | Krakow 2017",
					"2273 - denis (Gold) | Krakow 2017",
					"2274 - loWel | Krakow 2017",
					"2275 - loWel (Foil) | Krakow 2017",
					"2276 - loWel (Gold) | Krakow 2017",
					"2277 - oskar | Krakow 2017",
					"2278 - oskar (Foil) | Krakow 2017",
					"2279 - oskar (Gold) | Krakow 2017",
					"2280 - ropz | Krakow 2017",
					"2281 - ropz (Foil) | Krakow 2017",
					"2282 - ropz (Gold) | Krakow 2017",
					"2283 - apEX | Krakow 2017",
					"2284 - apEX (Foil) | Krakow 2017",
					"2285 - apEX (Gold) | Krakow 2017",
					"2286 - bodyy | Krakow 2017",
					"2287 - bodyy (Foil) | Krakow 2017",
					"2288 - bodyy (Gold) | Krakow 2017",
					"2289 - kennyS | Krakow 2017",
					"2290 - kennyS (Foil) | Krakow 2017",
					"2291 - kennyS (Gold) | Krakow 2017",
					"2292 - NBK- | Krakow 2017",
					"2293 - NBK- (Foil) | Krakow 2017",
					"2294 - NBK- (Gold) | Krakow 2017",
					"2295 - shox | Krakow 2017",
					"2296 - shox (Foil) | Krakow 2017",
					"2297 - shox (Gold) | Krakow 2017",
					"2298 - gob b | Krakow 2017",
					"2299 - gob b (Foil) | Krakow 2017",
					"2300 - gob b (Gold) | Krakow 2017",
					"2301 - keev | Krakow 2017",
					"2302 - keev (Foil) | Krakow 2017",
					"2303 - keev (Gold) | Krakow 2017",
					"2304 - LEGIJA | Krakow 2017",
					"2305 - LEGIJA (Foil) | Krakow 2017",
					"2306 - LEGIJA (Gold) | Krakow 2017",
					"2307 - nex | Krakow 2017",
					"2308 - nex (Foil) | Krakow 2017",
					"2309 - nex (Gold) | Krakow 2017",
					"2310 - tabseN | Krakow 2017",
					"2311 - tabseN (Foil) | Krakow 2017",
					"2312 - tabseN (Gold) | Krakow 2017",
					"2313 - autimatic | Krakow 2017",
					"2314 - autimatic (Foil) | Krakow 2017",
					"2315 - autimatic (Gold) | Krakow 2017",
					"2316 - n0thing | Krakow 2017",
					"2317 - n0thing (Foil) | Krakow 2017",
					"2318 - n0thing (Gold) | Krakow 2017",
					"2319 - shroud | Krakow 2017",
					"2320 - shroud (Foil) | Krakow 2017",
					"2321 - shroud (Gold) | Krakow 2017",
					"2322 - Skadoodle | Krakow 2017",
					"2323 - Skadoodle (Foil) | Krakow 2017",
					"2324 - Skadoodle (Gold) | Krakow 2017",
					"2325 - Stewie2K | Krakow 2017",
					"2326 - Stewie2K (Foil) | Krakow 2017",
					"2327 - Stewie2K (Gold) | Krakow 2017",
					"2328 - HS | Krakow 2017",
					"2329 - HS (Foil) | Krakow 2017",
					"2330 - HS (Gold) | Krakow 2017",
					"2331 - innocent | Krakow 2017",
					"2332 - innocent (Foil) | Krakow 2017",
					"2333 - innocent (Gold) | Krakow 2017",
					"2334 - kRYSTAL | Krakow 2017",
					"2335 - kRYSTAL (Foil) | Krakow 2017",
					"2336 - kRYSTAL (Gold) | Krakow 2017",
					"2337 - suNny | Krakow 2017",
					"2338 - suNny (Foil) | Krakow 2017",
					"2339 - suNny (Gold) | Krakow 2017",
					"2340 - zehN | Krakow 2017",
					"2341 - zehN (Foil) | Krakow 2017",
					"2342 - zehN (Gold) | Krakow 2017",
					"2343 - B1ad3 | Krakow 2017",
					"2344 - B1ad3 (Foil) | Krakow 2017",
					"2345 - B1ad3 (Gold) | Krakow 2017",
					"2346 - electronic | Krakow 2017",
					"2347 - electronic (Foil) | Krakow 2017",
					"2348 - electronic (Gold) | Krakow 2017",
					"2349 - markeloff | Krakow 2017",
					"2350 - markeloff (Foil) | Krakow 2017",
					"2351 - markeloff (Gold) | Krakow 2017",
					"2352 - wayLander | Krakow 2017",
					"2353 - wayLander (Foil) | Krakow 2017",
					"2354 - wayLander (Gold) | Krakow 2017",
					"2355 - WorldEdit | Krakow 2017",
					"2356 - WorldEdit (Foil) | Krakow 2017",
					"2357 - WorldEdit (Gold) | Krakow 2017",
					"2358 - boltz | Krakow 2017",
					"2359 - boltz (Foil) | Krakow 2017",
					"2360 - boltz (Gold) | Krakow 2017",
					"2361 - HEN1 | Krakow 2017",
					"2362 - HEN1 (Foil) | Krakow 2017",
					"2363 - HEN1 (Gold) | Krakow 2017",
					"2364 - kNgV- | Krakow 2017",
					"2365 - kNgV- (Foil) | Krakow 2017",
					"2366 - kNgV- (Gold) | Krakow 2017",
					"2367 - LUCAS1 | Krakow 2017",
					"2368 - LUCAS1 (Foil) | Krakow 2017",
					"2369 - LUCAS1 (Gold) | Krakow 2017",
					"2370 - steel | Krakow 2017",
					"2371 - steel (Foil) | Krakow 2017",
					"2372 - steel (Gold) | Krakow 2017",
					"2373 - chopper | Krakow 2017",
					"2374 - chopper (Foil) | Krakow 2017",
					"2375 - chopper (Gold) | Krakow 2017",
					"2376 - hutji | Krakow 2017",
					"2377 - hutji (Foil) | Krakow 2017",
					"2378 - hutji (Gold) | Krakow 2017",
					"2379 - jR | Krakow 2017",
					"2380 - jR (Foil) | Krakow 2017",
					"2381 - jR (Gold) | Krakow 2017",
					"2382 - keshandr | Krakow 2017",
					"2383 - keshandr (Foil) | Krakow 2017",
					"2384 - keshandr (Gold) | Krakow 2017",
					"2385 - mir | Krakow 2017",
					"2386 - mir (Foil) | Krakow 2017",
					"2387 - mir (Gold) | Krakow 2017",
					"2388 - Water Gun",
					"2389 - Cheongsam",
					"2390 - Cheongsam (Holo)",
					"2391 - Fancy Koi",
					"2392 - Fancy Koi (Foil)",
					"2393 - Guardian Dragon",
					"2394 - Guardian Dragon (Foil)",
					"2395 - Hotpot",
					"2396 - Noodles",
					"2397 - Rice Bomb",
					"2398 - Terror Rice",
					"2399 - Mahjong Fa",
					"2400 - Mahjong Rooster",
					"2401 - Mahjong Zhong",
					"2402 - Toy Tiger",
					"2403 - God of Fortune",
					"2404 - Huaji",
					"2405 - Nezha",
					"2406 - Rage",
					"2407 - Longevity",
					"2408 - Longevity (Foil)",
					"2409 - Non-Veg",
					"2410 - Pixiu",
					"2411 - Pixiu (Foil)",
					"2412 - Twin Koi",
					"2413 - Twin Koi (Holo)",
					"2414 - Shaolin",
					"2415 - Green Swallow",
					"2416 - Blue Swallow",
					"2417 - Zombie Hop",
					"2418 - Water Gun",
					"2419 - Cheongsam",
					"2420 - Guardian Dragon",
					"2421 - Rage",
					"2422 - God of Fortune",
					"2423 - Hotpot",
					"2424 - Fancy Koi",
					"2425 - Longevity",
					"2426 - Nezha",
					"2427 - Noodles",
					"2428 - Non-Veg",
					"2429 - Pixiu",
					"2430 - Twin Koi",
					"2431 - Rice Bomb",
					"2432 - Terror Rice",
					"2433 - Shaolin",
					"2434 - Toy Tiger",
					"2435 - Zombie Hop",
					"2436 - Gambit Esports | Boston 2018",
					"2437 - Gambit Esports (Holo) | Boston 2018",
					"2438 - Gambit Esports (Foil) | Boston 2018",
					"2439 - Gambit Esports (Gold) | Boston 2018",
					"2440 - 100 Thieves | Boston 2018",
					"2441 - 100 Thieves (Holo) | Boston 2018",
					"2442 - 100 Thieves (Foil) | Boston 2018",
					"2443 - 100 Thieves (Gold) | Boston 2018",
					"2444 - Astralis | Boston 2018",
					"2445 - Astralis (Holo) | Boston 2018",
					"2446 - Astralis (Foil) | Boston 2018",
					"2447 - Astralis (Gold) | Boston 2018",
					"2448 - Virtus.Pro | Boston 2018",
					"2449 - Virtus.Pro (Holo) | Boston 2018",
					"2450 - Virtus.Pro (Foil) | Boston 2018",
					"2451 - Virtus.Pro (Gold) | Boston 2018",
					"2452 - Fnatic | Boston 2018",
					"2453 - Fnatic (Holo) | Boston 2018",
					"2454 - Fnatic (Foil) | Boston 2018",
					"2455 - Fnatic (Gold) | Boston 2018",
					"2456 - SK Gaming | Boston 2018",
					"2457 - SK Gaming (Holo) | Boston 2018",
					"2458 - SK Gaming (Foil) | Boston 2018",
					"2459 - SK Gaming (Gold) | Boston 2018",
					"2460 - BIG | Boston 2018",
					"2461 - BIG (Holo) | Boston 2018",
					"2462 - BIG (Foil) | Boston 2018",
					"2463 - BIG (Gold) | Boston 2018",
					"2464 - North | Boston 2018",
					"2465 - North (Holo) | Boston 2018",
					"2466 - North (Foil) | Boston 2018",
					"2467 - North (Gold) | Boston 2018",
					"2468 - G2 Esports | Boston 2018",
					"2469 - G2 Esports (Holo) | Boston 2018",
					"2470 - G2 Esports (Foil) | Boston 2018",
					"2471 - G2 Esports (Gold) | Boston 2018",
					"2472 - Cloud9 | Boston 2018",
					"2473 - Cloud9 (Holo) | Boston 2018",
					"2474 - Cloud9 (Foil) | Boston 2018",
					"2475 - Cloud9 (Gold) | Boston 2018",
					"2476 - Flipsid3 Tactics | Boston 2018",
					"2477 - Flipsid3 Tactics (Holo) | Boston 2018",
					"2478 - Flipsid3 Tactics (Foil) | Boston 2018",
					"2479 - Flipsid3 Tactics (Gold) | Boston 2018",
					"2480 - Natus Vincere | Boston 2018",
					"2481 - Natus Vincere (Holo) | Boston 2018",
					"2482 - Natus Vincere (Foil) | Boston 2018",
					"2483 - Natus Vincere (Gold) | Boston 2018",
					"2484 - mousesports | Boston 2018",
					"2485 - mousesports (Holo) | Boston 2018",
					"2486 - mousesports (Foil) | Boston 2018",
					"2487 - mousesports (Gold) | Boston 2018",
					"2488 - Sprout Esports | Boston 2018",
					"2489 - Sprout Esports (Holo) | Boston 2018",
					"2490 - Sprout Esports (Foil) | Boston 2018",
					"2491 - Sprout Esports (Gold) | Boston 2018",
					"2492 - FaZe Clan | Boston 2018",
					"2493 - FaZe Clan (Holo) | Boston 2018",
					"2494 - FaZe Clan (Foil) | Boston 2018",
					"2495 - FaZe Clan (Gold) | Boston 2018",
					"2496 - Vega Squadron | Boston 2018",
					"2497 - Vega Squadron (Holo) | Boston 2018",
					"2498 - Vega Squadron (Foil) | Boston 2018",
					"2499 - Vega Squadron (Gold) | Boston 2018",
					"2500 - Space Soldiers | Boston 2018",
					"2501 - Space Soldiers (Holo) | Boston 2018",
					"2502 - Space Soldiers (Foil) | Boston 2018",
					"2503 - Space Soldiers (Gold) | Boston 2018",
					"2504 - Team Liquid | Boston 2018",
					"2505 - Team Liquid (Holo) | Boston 2018",
					"2506 - Team Liquid (Foil) | Boston 2018",
					"2507 - Team Liquid (Gold) | Boston 2018",
					"2508 - Avangar | Boston 2018",
					"2509 - Avangar (Holo) | Boston 2018",
					"2510 - Avangar (Foil) | Boston 2018",
					"2511 - Avangar (Gold) | Boston 2018",
					"2512 - Renegades | Boston 2018",
					"2513 - Renegades (Holo) | Boston 2018",
					"2514 - Renegades (Foil) | Boston 2018",
					"2515 - Renegades (Gold) | Boston 2018",
					"2516 - Team EnVyUs | Boston 2018",
					"2517 - Team EnVyUs (Holo) | Boston 2018",
					"2518 - Team EnVyUs (Foil) | Boston 2018",
					"2519 - Team EnVyUs (Gold) | Boston 2018",
					"2520 - Misfits Gaming | Boston 2018",
					"2521 - Misfits Gaming (Holo) | Boston 2018",
					"2522 - Misfits Gaming (Foil) | Boston 2018",
					"2523 - Misfits Gaming (Gold) | Boston 2018",
					"2524 - Quantum Bellator Fire | Boston 2018",
					"2525 - Quantum Bellator Fire (Holo) | Boston 2018",
					"2526 - Quantum Bellator Fire (Foil) | Boston 2018",
					"2527 - Quantum Bellator Fire (Gold) | Boston 2018",
					"2528 - Tyloo | Boston 2018",
					"2529 - Tyloo (Holo) | Boston 2018",
					"2530 - Tyloo (Foil) | Boston 2018",
					"2531 - Tyloo (Gold) | Boston 2018",
					"2532 - ELEAGUE | Boston 2018",
					"2533 - ELEAGUE (Holo) | Boston 2018",
					"2534 - ELEAGUE (Foil) | Boston 2018",
					"2535 - ELEAGUE (Gold) | Boston 2018",
					"2536 - Gambit Esports | Boston 2018",
					"2537 - 100 Thieves | Boston 2018",
					"2538 - Astralis | Boston 2018",
					"2539 - Virtus.Pro | Boston 2018",
					"2540 - Fnatic | Boston 2018",
					"2541 - SK Gaming | Boston 2018",
					"2542 - BIG | Boston 2018",
					"2543 - North | Boston 2018",
					"2544 - G2 Esports | Boston 2018",
					"2545 - Cloud9 | Boston 2018",
					"2546 - Flipsid3 Tactics | Boston 2018",
					"2547 - Natus Vincere | Boston 2018",
					"2548 - mousesports | Boston 2018",
					"2549 - Sprout Esports | Boston 2018",
					"2550 - FaZe Clan | Boston 2018",
					"2551 - Vega Squadron | Boston 2018",
					"2552 - Space Soldiers | Boston 2018",
					"2553 - Team Liquid | Boston 2018",
					"2554 - Avangar | Boston 2018",
					"2555 - Renegades | Boston 2018",
					"2556 - Team EnVyUs | Boston 2018",
					"2557 - Misfits Gaming | Boston 2018",
					"2558 - Quantum Bellator Fire | Boston 2018",
					"2559 - Tyloo | Boston 2018",
					"2560 - ELEAGUE | Boston 2018",
					"2561 - AdreN | Boston 2018",
					"2562 - AdreN (Foil) | Boston 2018",
					"2563 - AdreN (Gold) | Boston 2018",
					"2564 - Dosia | Boston 2018",
					"2565 - Dosia (Foil) | Boston 2018",
					"2566 - Dosia (Gold) | Boston 2018",
					"2567 - fitch | Boston 2018",
					"2568 - fitch (Foil) | Boston 2018",
					"2569 - fitch (Gold) | Boston 2018",
					"2570 - Hobbit | Boston 2018",
					"2571 - Hobbit (Foil) | Boston 2018",
					"2572 - Hobbit (Gold) | Boston 2018",
					"2573 - mou | Boston 2018",
					"2574 - mou (Foil) | Boston 2018",
					"2575 - mou (Gold) | Boston 2018",
					"2576 - BIT | Boston 2018",
					"2577 - BIT (Foil) | Boston 2018",
					"2578 - BIT (Gold) | Boston 2018",
					"2579 - fnx | Boston 2018",
					"2580 - fnx (Foil) | Boston 2018",
					"2581 - fnx (Gold) | Boston 2018",
					"2582 - HEN1 | Boston 2018",
					"2583 - HEN1 (Foil) | Boston 2018",
					"2584 - HEN1 (Gold) | Boston 2018",
					"2585 - kNgV- | Boston 2018",
					"2586 - kNgV- (Foil) | Boston 2018",
					"2587 - kNgV- (Gold) | Boston 2018",
					"2588 - LUCAS1 | Boston 2018",
					"2589 - LUCAS1 (Foil) | Boston 2018",
					"2590 - LUCAS1 (Gold) | Boston 2018",
					"2591 - device | Boston 2018",
					"2592 - device (Foil) | Boston 2018",
					"2593 - device (Gold) | Boston 2018",
					"2594 - dupreeh | Boston 2018",
					"2595 - dupreeh (Foil) | Boston 2018",
					"2596 - dupreeh (Gold) | Boston 2018",
					"2597 - gla1ve | Boston 2018",
					"2598 - gla1ve (Foil) | Boston 2018",
					"2599 - gla1ve (Gold) | Boston 2018",
					"2600 - Kjaerbye | Boston 2018",
					"2601 - Kjaerbye (Foil) | Boston 2018",
					"2602 - Kjaerbye (Gold) | Boston 2018",
					"2603 - Xyp9x | Boston 2018",
					"2604 - Xyp9x (Foil) | Boston 2018",
					"2605 - Xyp9x (Gold) | Boston 2018",
					"2606 - byali | Boston 2018",
					"2607 - byali (Foil) | Boston 2018",
					"2608 - byali (Gold) | Boston 2018",
					"2609 - NEO | Boston 2018",
					"2610 - NEO (Foil) | Boston 2018",
					"2611 - NEO (Gold) | Boston 2018",
					"2612 - pashaBiceps | Boston 2018",
					"2613 - pashaBiceps (Foil) | Boston 2018",
					"2614 - pashaBiceps (Gold) | Boston 2018",
					"2615 - Snax | Boston 2018",
					"2616 - Snax (Foil) | Boston 2018",
					"2617 - Snax (Gold) | Boston 2018",
					"2618 - TaZ | Boston 2018",
					"2619 - TaZ (Foil) | Boston 2018",
					"2620 - TaZ (Gold) | Boston 2018",
					"2621 - flusha | Boston 2018",
					"2622 - flusha (Foil) | Boston 2018",
					"2623 - flusha (Gold) | Boston 2018",
					"2624 - Golden | Boston 2018",
					"2625 - Golden (Foil) | Boston 2018",
					"2626 - Golden (Gold) | Boston 2018",
					"2627 - JW | Boston 2018",
					"2628 - JW (Foil) | Boston 2018",
					"2629 - JW (Gold) | Boston 2018",
					"2630 - KRIMZ | Boston 2018",
					"2631 - KRIMZ (Foil) | Boston 2018",
					"2632 - KRIMZ (Gold) | Boston 2018",
					"2633 - Lekr0 | Boston 2018",
					"2634 - Lekr0 (Foil) | Boston 2018",
					"2635 - Lekr0 (Gold) | Boston 2018",
					"2636 - coldzera | Boston 2018",
					"2637 - coldzera (Foil) | Boston 2018",
					"2638 - coldzera (Gold) | Boston 2018",
					"2639 - FalleN | Boston 2018",
					"2640 - FalleN (Foil) | Boston 2018",
					"2641 - FalleN (Gold) | Boston 2018",
					"2642 - felps | Boston 2018",
					"2643 - felps (Foil) | Boston 2018",
					"2644 - felps (Gold) | Boston 2018",
					"2645 - fer | Boston 2018",
					"2646 - fer (Foil) | Boston 2018",
					"2647 - fer (Gold) | Boston 2018",
					"2648 - TACO | Boston 2018",
					"2649 - TACO (Foil) | Boston 2018",
					"2650 - TACO (Gold) | Boston 2018",
					"2651 - gob b | Boston 2018",
					"2652 - gob b (Foil) | Boston 2018",
					"2653 - gob b (Gold) | Boston 2018",
					"2654 - keev | Boston 2018",
					"2655 - keev (Foil) | Boston 2018",
					"2656 - keev (Gold) | Boston 2018",
					"2657 - LEGIJA | Boston 2018",
					"2658 - LEGIJA (Foil) | Boston 2018",
					"2659 - LEGIJA (Gold) | Boston 2018",
					"2660 - nex | Boston 2018",
					"2661 - nex (Foil) | Boston 2018",
					"2662 - nex (Gold) | Boston 2018",
					"2663 - tabseN | Boston 2018",
					"2664 - tabseN (Foil) | Boston 2018",
					"2665 - tabseN (Gold) | Boston 2018",
					"2666 - aizy | Boston 2018",
					"2667 - aizy (Foil) | Boston 2018",
					"2668 - aizy (Gold) | Boston 2018",
					"2669 - cajunb | Boston 2018",
					"2670 - cajunb (Foil) | Boston 2018",
					"2671 - cajunb (Gold) | Boston 2018",
					"2672 - k0nfig | Boston 2018",
					"2673 - k0nfig (Foil) | Boston 2018",
					"2674 - k0nfig (Gold) | Boston 2018",
					"2675 - MSL | Boston 2018",
					"2676 - MSL (Foil) | Boston 2018",
					"2677 - MSL (Gold) | Boston 2018",
					"2678 - v4lde | Boston 2018",
					"2679 - v4lde (Foil) | Boston 2018",
					"2680 - v4lde (Gold) | Boston 2018",
					"2681 - apEX | Boston 2018",
					"2682 - apEX (Foil) | Boston 2018",
					"2683 - apEX (Gold) | Boston 2018",
					"2684 - bodyy | Boston 2018",
					"2685 - bodyy (Foil) | Boston 2018",
					"2686 - bodyy (Gold) | Boston 2018",
					"2687 - kennyS | Boston 2018",
					"2688 - kennyS (Foil) | Boston 2018",
					"2689 - kennyS (Gold) | Boston 2018",
					"2690 - NBK- | Boston 2018",
					"2691 - NBK- (Foil) | Boston 2018",
					"2692 - NBK- (Gold) | Boston 2018",
					"2693 - shox | Boston 2018",
					"2694 - shox (Foil) | Boston 2018",
					"2695 - shox (Gold) | Boston 2018",
					"2696 - autimatic | Boston 2018",
					"2697 - autimatic (Foil) | Boston 2018",
					"2698 - autimatic (Gold) | Boston 2018",
					"2699 - RUSH | Boston 2018",
					"2700 - RUSH (Foil) | Boston 2018",
					"2701 - RUSH (Gold) | Boston 2018",
					"2702 - Skadoodle | Boston 2018",
					"2703 - Skadoodle (Foil) | Boston 2018",
					"2704 - Skadoodle (Gold) | Boston 2018",
					"2705 - Stewie2K | Boston 2018",
					"2706 - Stewie2K (Foil) | Boston 2018",
					"2707 - Stewie2K (Gold) | Boston 2018",
					"2708 - tarik | Boston 2018",
					"2709 - tarik (Foil) | Boston 2018",
					"2710 - tarik (Gold) | Boston 2018",
					"2711 - B1ad3 | Boston 2018",
					"2712 - B1ad3 (Foil) | Boston 2018",
					"2713 - B1ad3 (Gold) | Boston 2018",
					"2714 - markeloff | Boston 2018",
					"2715 - markeloff (Foil) | Boston 2018",
					"2716 - markeloff (Gold) | Boston 2018",
					"2717 - seized | Boston 2018",
					"2718 - seized (Foil) | Boston 2018",
					"2719 - seized (Gold) | Boston 2018",
					"2720 - wayLander | Boston 2018",
					"2721 - wayLander (Foil) | Boston 2018",
					"2722 - wayLander (Gold) | Boston 2018",
					"2723 - WorldEdit | Boston 2018",
					"2724 - WorldEdit (Foil) | Boston 2018",
					"2725 - WorldEdit (Gold) | Boston 2018",
					"2726 - Edward | Boston 2018",
					"2727 - Edward (Foil) | Boston 2018",
					"2728 - Edward (Gold) | Boston 2018",
					"2729 - electronic | Boston 2018",
					"2730 - electronic (Foil) | Boston 2018",
					"2731 - electronic (Gold) | Boston 2018",
					"2732 - flamie | Boston 2018",
					"2733 - flamie (Foil) | Boston 2018",
					"2734 - flamie (Gold) | Boston 2018",
					"2735 - s1mple | Boston 2018",
					"2736 - s1mple (Foil) | Boston 2018",
					"2737 - s1mple (Gold) | Boston 2018",
					"2738 - Zeus | Boston 2018",
					"2739 - Zeus (Foil) | Boston 2018",
					"2740 - Zeus (Gold) | Boston 2018",
					"2741 - chrisJ | Boston 2018",
					"2742 - chrisJ (Foil) | Boston 2018",
					"2743 - chrisJ (Gold) | Boston 2018",
					"2744 - oskar | Boston 2018",
					"2745 - oskar (Foil) | Boston 2018",
					"2746 - oskar (Gold) | Boston 2018",
					"2747 - ropz | Boston 2018",
					"2748 - ropz (Foil) | Boston 2018",
					"2749 - ropz (Gold) | Boston 2018",
					"2750 - STYKO | Boston 2018",
					"2751 - STYKO (Foil) | Boston 2018",
					"2752 - STYKO (Gold) | Boston 2018",
					"2753 - suNny | Boston 2018",
					"2754 - suNny (Foil) | Boston 2018",
					"2755 - suNny (Gold) | Boston 2018",
					"2756 - denis | Boston 2018",
					"2757 - denis (Foil) | Boston 2018",
					"2758 - denis (Gold) | Boston 2018",
					"2759 - innocent | Boston 2018",
					"2760 - innocent (Foil) | Boston 2018",
					"2761 - innocent (Gold) | Boston 2018",
					"2762 - kRYSTAL | Boston 2018",
					"2763 - kRYSTAL (Foil) | Boston 2018",
					"2764 - kRYSTAL (Gold) | Boston 2018",
					"2765 - Spiidi | Boston 2018",
					"2766 - Spiidi (Foil) | Boston 2018",
					"2767 - Spiidi (Gold) | Boston 2018",
					"2768 - zehN | Boston 2018",
					"2769 - zehN (Foil) | Boston 2018",
					"2770 - zehN (Gold) | Boston 2018",
					"2771 - GuardiaN | Boston 2018",
					"2772 - GuardiaN (Foil) | Boston 2018",
					"2773 - GuardiaN (Gold) | Boston 2018",
					"2774 - karrigan | Boston 2018",
					"2775 - karrigan (Foil) | Boston 2018",
					"2776 - karrigan (Gold) | Boston 2018",
					"2777 - NiKo | Boston 2018",
					"2778 - NiKo (Foil) | Boston 2018",
					"2779 - NiKo (Gold) | Boston 2018",
					"2780 - olofmeister | Boston 2018",
					"2781 - olofmeister (Foil) | Boston 2018",
					"2782 - olofmeister (Gold) | Boston 2018",
					"2783 - rain | Boston 2018",
					"2784 - rain (Foil) | Boston 2018",
					"2785 - rain (Gold) | Boston 2018",
					"2786 - chopper | Boston 2018",
					"2787 - chopper (Foil) | Boston 2018",
					"2788 - chopper (Gold) | Boston 2018",
					"2789 - hutji | Boston 2018",
					"2790 - hutji (Foil) | Boston 2018",
					"2791 - hutji (Gold) | Boston 2018",
					"2792 - jR | Boston 2018",
					"2793 - jR (Foil) | Boston 2018",
					"2794 - jR (Gold) | Boston 2018",
					"2795 - keshandr | Boston 2018",
					"2796 - keshandr (Foil) | Boston 2018",
					"2797 - keshandr (Gold) | Boston 2018",
					"2798 - mir | Boston 2018",
					"2799 - mir (Foil) | Boston 2018",
					"2800 - mir (Gold) | Boston 2018",
					"2801 - Calyx | Boston 2018",
					"2802 - Calyx (Foil) | Boston 2018",
					"2803 - Calyx (Gold) | Boston 2018",
					"2804 - MAJ3R | Boston 2018",
					"2805 - MAJ3R (Foil) | Boston 2018",
					"2806 - MAJ3R (Gold) | Boston 2018",
					"2807 - ngiN | Boston 2018",
					"2808 - ngiN (Foil) | Boston 2018",
					"2809 - ngiN (Gold) | Boston 2018",
					"2810 - paz | Boston 2018",
					"2811 - paz (Foil) | Boston 2018",
					"2812 - paz (Gold) | Boston 2018",
					"2813 - XANTARES | Boston 2018",
					"2814 - XANTARES (Foil) | Boston 2018",
					"2815 - XANTARES (Gold) | Boston 2018",
					"2816 - EliGE | Boston 2018",
					"2817 - EliGE (Foil) | Boston 2018",
					"2818 - EliGE (Gold) | Boston 2018",
					"2819 - jdm64 | Boston 2018",
					"2820 - jdm64 (Foil) | Boston 2018",
					"2821 - jdm64 (Gold) | Boston 2018",
					"2822 - nitr0 | Boston 2018",
					"2823 - nitr0 (Foil) | Boston 2018",
					"2824 - nitr0 (Gold) | Boston 2018",
					"2825 - stanislaw | Boston 2018",
					"2826 - stanislaw (Foil) | Boston 2018",
					"2827 - stanislaw (Gold) | Boston 2018",
					"2828 - Twistzz | Boston 2018",
					"2829 - Twistzz (Foil) | Boston 2018",
					"2830 - Twistzz (Gold) | Boston 2018",
					"2831 - buster | Boston 2018",
					"2832 - buster (Foil) | Boston 2018",
					"2833 - buster (Gold) | Boston 2018",
					"2834 - dimasick | Boston 2018",
					"2835 - dimasick (Foil) | Boston 2018",
					"2836 - dimasick (Gold) | Boston 2018",
					"2837 - Jame | Boston 2018",
					"2838 - Jame (Foil) | Boston 2018",
					"2839 - Jame (Gold) | Boston 2018",
					"2840 - KrizzeN | Boston 2018",
					"2841 - KrizzeN (Foil) | Boston 2018",
					"2842 - KrizzeN (Gold) | Boston 2018",
					"2843 - qikert | Boston 2018",
					"2844 - qikert (Foil) | Boston 2018",
					"2845 - qikert (Gold) | Boston 2018",
					"2846 - AZR | Boston 2018",
					"2847 - AZR (Foil) | Boston 2018",
					"2848 - AZR (Gold) | Boston 2018",
					"2849 - jks | Boston 2018",
					"2850 - jks (Foil) | Boston 2018",
					"2851 - jks (Gold) | Boston 2018",
					"2852 - NAF | Boston 2018",
					"2853 - NAF (Foil) | Boston 2018",
					"2854 - NAF (Gold) | Boston 2018",
					"2855 - Nifty | Boston 2018",
					"2856 - Nifty (Foil) | Boston 2018",
					"2857 - Nifty (Gold) | Boston 2018",
					"2858 - USTILO | Boston 2018",
					"2859 - USTILO (Foil) | Boston 2018",
					"2860 - USTILO (Gold) | Boston 2018",
					"2861 - Happy | Boston 2018",
					"2862 - Happy (Foil) | Boston 2018",
					"2863 - Happy (Gold) | Boston 2018",
					"2864 - RpK | Boston 2018",
					"2865 - RpK (Foil) | Boston 2018",
					"2866 - RpK (Gold) | Boston 2018",
					"2867 - ScreaM | Boston 2018",
					"2868 - ScreaM (Foil) | Boston 2018",
					"2869 - ScreaM (Gold) | Boston 2018",
					"2870 - SIXER | Boston 2018",
					"2871 - SIXER (Foil) | Boston 2018",
					"2872 - SIXER (Gold) | Boston 2018",
					"2873 - xms | Boston 2018",
					"2874 - xms (Foil) | Boston 2018",
					"2875 - xms (Gold) | Boston 2018",
					"2876 - AmaNEk | Boston 2018",
					"2877 - AmaNEk (Foil) | Boston 2018",
					"2878 - AmaNEk (Gold) | Boston 2018",
					"2879 - devoduvek | Boston 2018",
					"2880 - devoduvek (Foil) | Boston 2018",
					"2881 - devoduvek (Gold) | Boston 2018",
					"2882 - seang@res | Boston 2018",
					"2883 - seang@res (Foil) | Boston 2018",
					"2884 - seang@res (Gold) | Boston 2018",
					"2885 - ShahZaM | Boston 2018",
					"2886 - ShahZaM (Foil) | Boston 2018",
					"2887 - ShahZaM (Gold) | Boston 2018",
					"2888 - SicK | Boston 2018",
					"2889 - SicK (Foil) | Boston 2018",
					"2890 - SicK (Gold) | Boston 2018",
					"2891 - balblna | Boston 2018",
					"2892 - balblna (Foil) | Boston 2018",
					"2893 - balblna (Gold) | Boston 2018",
					"2894 - Boombl4 | Boston 2018",
					"2895 - Boombl4 (Foil) | Boston 2018",
					"2896 - Boombl4 (Gold) | Boston 2018",
					"2897 - jmqa | Boston 2018",
					"2898 - jmqa (Foil) | Boston 2018",
					"2899 - jmqa (Gold) | Boston 2018",
					"2900 - Kvik | Boston 2018",
					"2901 - Kvik (Foil) | Boston 2018",
					"2902 - Kvik (Gold) | Boston 2018",
					"2903 - waterfaLLZ | Boston 2018",
					"2904 - waterfaLLZ (Foil) | Boston 2018",
					"2905 - waterfaLLZ (Gold) | Boston 2018",
					"2906 - BnTeT | Boston 2018",
					"2907 - BnTeT (Foil) | Boston 2018",
					"2908 - BnTeT (Gold) | Boston 2018",
					"2909 - bondik | Boston 2018",
					"2910 - bondik (Foil) | Boston 2018",
					"2911 - bondik (Gold) | Boston 2018",
					"2912 - captainMo | Boston 2018",
					"2913 - captainMo (Foil) | Boston 2018",
					"2914 - captainMo (Gold) | Boston 2018",
					"2915 - DD | Boston 2018",
					"2916 - DD (Foil) | Boston 2018",
					"2917 - DD (Gold) | Boston 2018",
					"2918 - somebody | Boston 2018",
					"2919 - somebody (Foil) | Boston 2018",
					"2920 - somebody (Gold) | Boston 2018",
					"2935 - Flash Gaming | Boston 2018",
					"2936 - Flash Gaming (Holo) | Boston 2018",
					"2937 - Flash Gaming (Foil) | Boston 2018",
					"2938 - Flash Gaming (Gold) | Boston 2018",
					"2939 - Flash Gaming | Boston 2018",
					"2940 - Attacker | Boston 2018",
					"2941 - Attacker (Foil) | Boston 2018",
					"2942 - Attacker (Gold) | Boston 2018",
					"2943 - Karsa | Boston 2018",
					"2944 - Karsa (Foil) | Boston 2018",
					"2945 - Karsa (Gold) | Boston 2018",
					"2946 - Kaze | Boston 2018",
					"2947 - Kaze (Foil) | Boston 2018",
					"2948 - Kaze (Gold) | Boston 2018",
					"2949 - LoveYY | Boston 2018",
					"2950 - LoveYY (Foil) | Boston 2018",
					"2951 - LoveYY (Gold) | Boston 2018",
					"2952 - Summer | Boston 2018",
					"2953 - Summer (Foil) | Boston 2018",
					"2954 - Summer (Gold) | Boston 2018",
				};
				for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
					if (filter.PassFilter(lines[i]))
						ImGui::Text("%s", lines[i]);
			}
			ImGui::EndChild();

			ImGui::End();
		}

		ImGui::GetStyle().WindowMinSize = OldMinSize;

	}


	if (bIsGuiVisible)
	{
		int pX, pY;
		Interfaces::InputSystem()->GetCursorPosition(&pX, &pY);

		ImGui::GetIO().MousePos.x = (float)pX;
		ImGui::GetIO().MousePos.y = (float)pY;

		Client::OnRenderGUI();
	} 



	g_pGui->GUI_End_Render();
}




void CGui::DefaultSheme()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1294117647f, 0.1294117647f, 0.1294117647f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22745098039f, 0.2431372549f, 0.8431372549f, 0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.05882352941f, 0.2431372549f, 0.8431372549f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.22745098039f, 0.39607843137f, 0.94117647058f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.89f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.25098039215f, 0.25098039215f, 0.25098039215f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22745098039f, 0.39607843137f, 0.94117647058f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.04705882352f, 0.2f, 0.65098039215f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.1294117647f, 0.1294117647f, 0.1294117647f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 0.18823529411f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.25098039215f, 0.25098039215f, 0.25098039215f, 0.51f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 0.05f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 0.05f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 0.18823529411f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 0.18823529411f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 0.18823529411f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.18823529411f, 0.18823529411f, 0.18823529411f, 0.18823529411f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
}

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	IMGUI_API bool ComboBoxArray(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}
	IMGUI_API bool TabLabels(const char ** tabLabels, int tabSize, int & tabIndex, int * tabOrder)
	{
		ImGuiStyle & style = ImGui::GetStyle();
		const ImVec2 itemSpacing = style.ItemSpacing;
		const ImVec4 color = style.Colors[ImGuiCol_Button];
		const ImVec4 colorActive = style.Colors[ImGuiCol_ButtonActive];
		const ImVec4 colorHover = style.Colors[ImGuiCol_ButtonHovered];
		const ImVec4 colorText = style.Colors[ImGuiCol_Text];
		style.ItemSpacing.x = 1;
		style.ItemSpacing.y = 1;
		const ImVec4 colorSelectedTab = ImVec4(color.x, color.y, color.z, color.w * 0.5f);
		const ImVec4 colorSelectedTabHovered = ImVec4(colorHover.x, colorHover.y, colorHover.z, colorHover.w * 0.5f);
		const ImVec4 colorSelectedTabText = ImVec4(colorText.x * 0.8f, colorText.y * 0.8f, colorText.z * 0.8f, colorText.w * 0.8f);
		if (tabSize > 0 && (tabIndex < 0 || tabIndex >= tabSize))
		{
			if (!tabOrder)
			{
				tabIndex = 0;
			}
			else
			{
				tabIndex = -1;
			}
		}
		float windowWidth = 0.f, sumX = 0.f;
		windowWidth = ImGui::GetWindowWidth() - style.WindowPadding.x - (ImGui::GetScrollMaxY() > 0 ? style.ScrollbarSize : 0.f);
		static int draggingTabIndex = -1; int draggingTabTargetIndex = -1;
		static ImVec2 draggingtabSize(0, 0);
		static ImVec2 draggingTabOffset(0, 0);
		const bool isMMBreleased = ImGui::IsMouseReleased(2);
		const bool isMouseDragging = ImGui::IsMouseDragging(0, 2.f);
		int justClosedTabIndex = -1, newtabIndex = tabIndex;
		bool selection_changed = false; bool noButtonDrawn = true;
		for (int j = 0, i; j < tabSize; j++)
		{
			i = tabOrder ? tabOrder[j] : j;
			if (i == -1) continue;
			if (sumX > 0.f)
			{
				sumX += style.ItemSpacing.x;
				sumX += ImGui::CalcTextSize(tabLabels[i]).x + 2.f * style.FramePadding.x;
				if (sumX > windowWidth)
				{
					sumX = 0.f;
				}
				else
				{
					ImGui::SameLine();
				}
			}
			if (i != tabIndex)
			{
				// Push the style
				style.Colors[ImGuiCol_Button] = colorSelectedTab;
				style.Colors[ImGuiCol_ButtonActive] = colorSelectedTab;
				style.Colors[ImGuiCol_ButtonHovered] = colorSelectedTabHovered;
				style.Colors[ImGuiCol_Text] = colorSelectedTabText;
			}
			// Draw the button
			ImGui::PushID(i); // otherwise two tabs with the same name would clash.
			if (ImGui::Button(tabLabels[i], ImVec2(92.f, 27.f))) { selection_changed = (tabIndex != i); newtabIndex = i; } // if you want to change the button width and hegiht x = width, y = height .;
			ImGui::PopID();
			if (i != tabIndex)
			{
				// Reset the style
				style.Colors[ImGuiCol_Button] = color;
				style.Colors[ImGuiCol_ButtonActive] = colorActive;
				style.Colors[ImGuiCol_ButtonHovered] = colorHover;
				style.Colors[ImGuiCol_Text] = colorText;
			}
			noButtonDrawn = false;
			if (sumX == 0.f) sumX = style.WindowPadding.x + ImGui::GetItemRectSize().x; // First element of a line
			if (ImGui::IsItemHoveredRect())
			{
				if (tabOrder)
				{
					// tab reordering
					if (isMouseDragging)
					{
						if (draggingTabIndex == -1)
						{
							draggingTabIndex = j;
							draggingtabSize = ImGui::GetItemRectSize();
							const ImVec2 & mp = ImGui::GetIO().MousePos;
							const ImVec2 draggingTabCursorPos = ImGui::GetCursorPos();
							draggingTabOffset = ImVec2(
								mp.x + draggingtabSize.x * 0.5f - sumX + ImGui::GetScrollX(),
								mp.y + draggingtabSize.y * 0.5f - draggingTabCursorPos.y + ImGui::GetScrollY()
							);
						}
					}
					else if (draggingTabIndex >= 0 && draggingTabIndex < tabSize && draggingTabIndex != j)
					{
						draggingTabTargetIndex = j; // For some odd reasons this seems to get called only when draggingTabIndex < i ! (Probably during mouse dragging ImGui owns the mouse someway and sometimes ImGui::IsItemHovered() is not getting called)
					}
				}
			}
		}
		tabIndex = newtabIndex;
		// Restore the style
		style.Colors[ImGuiCol_Button] = color;
		style.Colors[ImGuiCol_ButtonActive] = colorActive;
		style.Colors[ImGuiCol_ButtonHovered] = colorHover;
		style.Colors[ImGuiCol_Text] = colorText;
		style.ItemSpacing = itemSpacing;
		return selection_changed;
	}
}

























































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































