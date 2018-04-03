#include "Client.h"
#include <ctime>
//#include "../GranadeHelper/CGrenadeAPI.h"

//[enc_string_enable /]
//[junk_enable /]

namespace Client
{
	//[swap_lines]
	int	iScreenWidth = 0;
	int	iScreenHeight = 0;

	string BaseDir = "";
	string LogFile = "";
	string GuiFile = "";
	string IniFile = "";

	vector<string> ConfigList;

	Vector2D	g_vCenterScreen = Vector2D(0.f, 0.f);

	CPlayers*	g_pPlayers = nullptr;
	CRender*	g_pRender = nullptr;
	CGui*		g_pGui = nullptr;

	CAimbot*	g_pAimbot = nullptr;
	CEsp*		g_pEsp = nullptr;
	CSkin*		g_pSkin = nullptr;
	CMisc*		g_pMisc = nullptr;
	CInventoryChanger* g_pInventoryChanger = nullptr;
	CKnifebot*	g_pKnifebot = nullptr;


	static char clantag[128] = { 0 };
	static char spamchat[128] = { 0 };

	bool		bC4Timer = false;
	float shirina = 0;
	float dlina = 0;
	int			iC4Timer = 40;
	int			iWeaponID = 0;
	int			iWeaponSelectIndex = WEAPON_DEAGLE;
	int			iWeaponSelectSkinIndex = -1;
	static int iConfigSelect = 0;
	static int iMenuSheme = 1;
	static char ConfigName[64] = { 0 };
	//[/swap_lines]

	void ReadConfigs(LPCTSTR lpszFileName)
	{
		if (!strstr(lpszFileName, "gui.ini"))
		{
			ConfigList.push_back(lpszFileName);
		}
	}

	void RefreshConfigs()
	{
		ConfigList.clear();
		string ConfigDir = "C:/SNAPSHOOK/*.ini";
		SearchFiles(ConfigDir.c_str(), ReadConfigs, FALSE);
	}


	void SendMMHello()
	{
		CMsgGCCStrike15_v2_MatchmakingClient2GCHello Message;
		void* ptr = malloc(Message.ByteSize() + 8);
		if (!ptr)
			return;
		auto unMsgType = k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31);
		((uint32_t*)ptr)[0] = unMsgType;
		((uint32_t*)ptr)[1] = 0;
		Message.SerializeToArray((void*)((DWORD)ptr + 8), Message.ByteSize());
		bool result = Interfaces::SteamGameCoordinator()->SendMessage(unMsgType, ptr, Message.ByteSize() + 8) == k_EGCResultOK;
		free(ptr);
	}
	void SendClientHello()
	{
		CMsgClientHello Message;
		Message.set_client_session_need(1);
		Message.clear_socache_have_versions();
		void* ptr = malloc(Message.ByteSize() + 8);
		if (!ptr)
			return;
		((uint32_t*)ptr)[0] = k_EMsgGCClientHello | ((DWORD)1 << 31);
		((uint32_t*)ptr)[1] = 0;
		Message.SerializeToArray((void*)((DWORD)ptr + 8), Message.ByteSize());
		bool result = Interfaces::SteamGameCoordinator()->SendMessage(k_EMsgGCClientHello | ((DWORD)1 << 31), ptr, Message.ByteSize() + 8) == k_EGCResultOK;
		free(ptr);
	}


	bool Initialize(IDirect3DDevice9* pDevice)
	{
		g_pPlayers = new CPlayers();
		g_pRender = new CRender(pDevice);
		g_pGui = new CGui();

		g_pAimbot = new CAimbot();
		g_pEsp = new CEsp();
		g_pSkin = new CSkin();
		g_pMisc = new CMisc();
		g_pInventoryChanger = new CInventoryChanger();
		GuiFile = "C:\SNAPSHOOK\gui.ini";
		IniFile = "C:\SNAPSHOOK\settings.ini";

		g_pSkin->InitalizeSkins();

		Settings::LoadSettings(IniFile);

		iWeaponSelectSkinIndex = GetWeaponSkinIndexFromPaintKit(g_SkinChangerCfg[iWeaponSelectIndex].nFallbackPaintKit);

		g_pGui->GUI_Init(pDevice);

		RefreshConfigs();
		SendClientHello();
		SendMMHello();
		return true;
	}

	void Shutdown()
	{
		DELETE_MOD(g_pPlayers);
		DELETE_MOD(g_pRender);
		DELETE_MOD(g_pGui);

		DELETE_MOD(g_pAimbot);
		DELETE_MOD(g_pEsp);
		DELETE_MOD(g_pSkin);
		DELETE_MOD(g_pMisc);
		DELETE_MOD(g_pKnifebot);
	}

	void OnRender()
	{
		if (g_pRender && !Interfaces::Engine()->IsTakingScreenshot() && Interfaces::Engine()->IsActiveApp())
		{
			// Открытие меню без net_graph 1
			g_pRender->BeginRender();

			if (g_pGui)
				g_pGui->GUI_Draw_Elements();

			Interfaces::Engine()->GetScreenSize(iScreenWidth, iScreenHeight);

			g_vCenterScreen.x = iScreenWidth / 2.f;
			g_vCenterScreen.y = iScreenHeight / 2.f;

			if (Settings::Esp::esp_Watermark)
			{
				g_pRender->Text(5, 5, false, true, Color::Aquamarine(), "SnapsHook | Build: %s : %s", __DATE__, __TIME__);
			}


			// Ватермарк //removed watermark for now XDDDDDDDD
			//g_pRender->Text(3, 3, false, true, Color::White(), "SnapsHook [beta framework]");

			if (Client::g_pPlayers && Client::g_pPlayers->GetLocal() && Interfaces::Engine()->IsInGame())
			{
				if (g_pEsp)
					g_pEsp->OnRender();

				if (g_pMisc)
				{
					g_pMisc->OnRender();
					g_pMisc->OnRenderSpectatorList();
				}
			}

			g_pRender->EndRender();
		}
	}

	void OnLostDevice()
	{
		if (g_pRender)
			g_pRender->OnLostDevice();

		if (g_pGui)
			ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	void OnResetDevice()
	{
		if (g_pRender)
			g_pRender->OnResetDevice();

		if (g_pGui)
			ImGui_ImplDX9_CreateDeviceObjects();
	}

	void OnRetrieveMessage(void* ecx, void* edx, uint32_t *punMsgType, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize)
	{
		g_pInventoryChanger->PostRetrieveMessage(punMsgType, pubDest, cubDest, pcubMsgSize);
	}
	void OnSendMessage(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData)
	{
		void* pubDataMutable = const_cast<void*>(pubData);
		g_pInventoryChanger->PreSendMessage(unMsgType, pubDataMutable, cubData);
	}

	void OnCreateMove(CUserCmd* pCmd)
	{
		if (g_pPlayers && Interfaces::Engine()->IsInGame())
		{
			g_pPlayers->Update();

			if (g_pEsp)
				g_pEsp->OnCreateMove(pCmd);

			if (IsLocalAlive())
			{
				if (!bIsGuiVisible)
				{
					int iWeaponSettingsSelectID = GetWeaponSettingsSelectID();

					if (iWeaponSettingsSelectID >= 0)
						iWeaponID = iWeaponSettingsSelectID;
				}

				if (g_pAimbot)
					g_pAimbot->OnCreateMove(pCmd, g_pPlayers->GetLocal());

				if (g_pMisc)
					g_pMisc->OnCreateMove(pCmd);
			}
		}
	}

	void OnFireEventClientSideThink(IGameEvent* pEvent)
	{
		if (!strcmp(pEvent->GetName(), "player_connect_full") ||
			!strcmp(pEvent->GetName(), "round_start") ||
			!strcmp(pEvent->GetName(), "cs_game_disconnected"))
		{
			if (g_pPlayers)
				g_pPlayers->Clear();

			if (g_pEsp)
				g_pEsp->OnReset();
		}

		if (Interfaces::Engine()->IsConnected())
		{
			hitmarker::singleton()->initialize();

			if (g_pEsp)
				g_pEsp->OnEvents(pEvent);

			if (g_pSkin)
				g_pSkin->OnEvents(pEvent);
		}
	}

	void OnFrameStageNotify(ClientFrameStage_t Stage)
	{
		if (Interfaces::Engine()->IsInGame())
		{
			Skin_OnFrameStageNotify(Stage);
			Gloves_OnFrameStageNotify(Stage);
		}
	}

	void OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t &state,
		const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
	{
		if (Interfaces::Engine()->IsInGame() && ctx && pCustomBoneToWorld)
		{
			if (g_pEsp)
				g_pEsp->OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

			if (g_pMisc)
				g_pMisc->OnDrawModelExecute();
		}
	}

	void OnPlaySound(const Vector* pOrigin, const char* pszSoundName)
	{
		if (!pszSoundName || !Interfaces::Engine()->IsInGame())
			return;

		if (!strstr(pszSoundName, "bulletLtoR") &&
			!strstr(pszSoundName, "rics/ric") &&
			!strstr(pszSoundName, "impact_bullet"))
		{
			if (g_pEsp && IsLocalAlive() && Settings::Esp::esp_Sound && pOrigin)
			{
				if (!GetVisibleOrigin(*pOrigin))
					g_pEsp->SoundEsp.AddSound(*pOrigin);
			}
		}
	}

	void OnPlaySound(const char* pszSoundName)
	{
		if (g_pMisc)
			g_pMisc->OnPlaySound(pszSoundName);
	}

	void OnOverrideView(CViewSetup* pSetup)
	{
		if (g_pMisc)
			g_pMisc->OnOverrideView(pSetup);
	}

	void OnGetViewModelFOV(float& fov)
	{
		if (g_pMisc)
			g_pMisc->OnGetViewModelFOV(fov);
	}

	void ImDrawRectRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow)
	{
		ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

		Color colColor(0, 0, 0, 255);

		flRainbow += flSpeed;
		if (flRainbow > 1.f) flRainbow = 0.f;//1 0 

		for (int i = 0; i < width; i = i + 1)
		{
			float hue = (1.f / (float)width) * i;
			hue -= flRainbow;
			if (hue < 0.f) hue += 1.f;

			Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
			windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), colRainbow.GetU32());
		}
	}

	void OnRenderGUI()
	{
		//Color MenuBackGround = Color(int(Settings::Misc::MenuBackGround[0] * 255.f),
		//int(Settings::Misc::MenuBackGround[1] * 255.f),
		//int(Settings::Misc::MenuBackGround[2] * 255.f),
		//int(Settings::Misc::MenuBackGround[3] * 255.f));

		//g_pRender->DrawFillBox(0, 0, iScreenWidth, iScreenHeight, MenuBackGround);

		float SpaceLineOne = 140.f;
		float SpaceLineTwo = 270.f;
		float SpaceLineThr = 390.f;
		float SpaceLineTwo2 = 225.f;


		float SpaceLine1 = 170.f;
		float SpaceLine228 = 150.f;
		float SpaceLine2 = 360.f;
		float SpaceLine5 = 120.f;

		CLicense lic;
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();


		ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(465, 575)); // MainMenu Size




		if (ImGui::Begin("SNAPSHOOK.XYZ", &bIsGuiVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{

			if (Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_FovType > 1)
				Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_FovType = 1;

			if (Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_BestHit > 1)
				Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_BestHit = 1;

			if (Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Spot > 5)
				Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Spot = 5;

			const char* tabNames[] = {
				u8"LegitBot" , u8"Visuals" , u8"Skinchanger" , u8"Misc" ,
				u8"Colors" };

			static int tabOrder[] = { 0 , 1 , 2 , 3 , 4 };
			static int tabSelected = 0;
			const bool tabChanged = ImGui::TabLabels(tabNames,
				sizeof(tabNames) / sizeof(tabNames[0]),
				tabSelected, tabOrder);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
			static float flRainbow;
			float flSpeed = 0.002f;
			int curWidth = 4;
			ImVec2 curPos = ImGui::GetCursorPos();
			ImVec2 curWindowPos = ImGui::GetWindowPos();
			curPos.x += curWindowPos.x;
			curPos.y += curWindowPos.y;
			int size;
			int y;
			Interfaces::Engine()->GetScreenSize(y, size);
			ImDrawRectRainbow(curPos.x - 10, curPos.y - 8, ImGui::GetWindowSize().x + size, curPos.y + -4, flSpeed, flRainbow);//10 5 4



			float SpaceLineOne = 140.f;
			float SpaceLineTwo = 270.f;
			float SpaceLineThr = 390.f;
			float SpaceLineTwo2 = 225.f;

			float SpaceLine1 = 170.f;
			float SpaceLine228 = 150.f;
			float SpaceLine2 = 360.f;
			float SpaceLine5 = 120.f;




			if (tabSelected == 0)
			{
				ImGui::BeginChild("secoad child", ImVec2(450, 495), true);
				{


					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox("Friendly Fire", &Settings::Aimbot::aim_Deathmatch);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Auto Wall", &Settings::Aimbot::aim_WallAttack);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Check Visibility", &Settings::Aimbot::aim_CheckSmoke);

					ImGui::Checkbox("In-Air Check", &Settings::Aimbot::aim_AntiJump);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Display Fov", &Settings::Aimbot::aim_DrawFov);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Visualise Aimbot", &Settings::Aimbot::aim_DrawSpot);

					ImGui::Checkbox("Backtrack", &Settings::Aimbot::aim_Backtrack);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Draw Ticks", &Settings::Aimbot::aim_DrawBacktrack);

					ImGui::SliderInt("Ticks", &Settings::Aimbot::aim_Backtracktickrate, 1, 12);

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::PushItemWidth(110.f);
					ImGui::Text("Active Weapon: ");
					ImGui::SameLine();
					ImGui::Combo("##AimWeapon", &iWeaponID, pWeaponData, IM_ARRAYSIZE(pWeaponData));
					ImGui::PopItemWidth();

					ImGui::Checkbox("Enable", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Active);

					if (iWeaponID <= 9)
					{
						ImGui::SameLine(SpaceLineOne);
						ImGui::Checkbox("RapidFire", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_AutoPistol);
					}


					ImGui::PushItemWidth(362.f);
					ImGui::SliderInt("Humanize", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Smooth, 1, 300);
					ImGui::SliderInt("Range", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Fov, 1, 300);
					ImGui::PopItemWidth();

					const char* AimFovType[] = { "Body" , "Crosshair" };
					ImGui::PushItemWidth(362.f);
					ImGui::Combo("Method", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_FovType, AimFovType, IM_ARRAYSIZE(AimFovType));
					ImGui::PopItemWidth();

					const char* BestHit[] = { "Disable" , "Enable" };
					ImGui::PushItemWidth(362.f);
					ImGui::Combo("Nearest", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_BestHit, BestHit, IM_ARRAYSIZE(BestHit));

					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("If disabled then use Priority");

					ImGui::PopItemWidth();

					const char* Aimspot[] = { "Head" , "Neck" , "Low Neck" , "Body" , "Thorax" , "Chest" };
					ImGui::PushItemWidth(362.f);
					ImGui::Combo("Priority", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Spot, Aimspot, IM_ARRAYSIZE(Aimspot));
					ImGui::PopItemWidth();

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::PushItemWidth(362.f);
					ImGui::SliderInt("Delay", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Delay, 0, 200);
					ImGui::SliderInt("Recoil", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Rcs, 0, 100);
					ImGui::PopItemWidth();

					ImGui::Spacing();
					ImGui::Separator();

					if (iWeaponID >= 10 && iWeaponID <= 30)
					{
						ImGui::PushItemWidth(362.f);
						ImGui::SliderInt("RCS Range", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_RcsFov, 1, 300);
						ImGui::SliderInt("RCS Smooth", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_RcsSmooth, 1, 300);
						ImGui::PopItemWidth();

						const char* ClampType[] = { "All Enemies" , "Nearest" , "Spray Transfer" };
						ImGui::PushItemWidth(362.f);
						ImGui::Combo("RCS Type", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_RcsClampType, ClampType, IM_ARRAYSIZE(ClampType));
						ImGui::PopItemWidth();

						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();
					}

					// Knife bot Needs Fixing Nigger Laech if you find this ss XDDD
					/*
					ImGui::Checkbox("Active", &Settings::Knifebot::knf_Active);
					ImGui::Checkbox("Attack Team", &Settings::Knifebot::knf_Team);

					ImGui::Separator();

					string attack_1 = "Attack 1";
					string attack_2 = "Attack 2";
					string attack_3 = "Attack 1 + Attack 2";

					const char* items[] = { attack_1.c_str() , attack_2.c_str() , attack_3.c_str() };
					ImGui::Combo("Attack type", &Settings::Knifebot::knf_Attack, items, IM_ARRAYSIZE(items));

					ImGui::Separator();

					ImGui::SliderInt("Dist to attack 1", &Settings::Knifebot::knf_DistAttack, 1, 100);
					ImGui::SliderInt("Dist to attack 2", &Settings::Knifebot::knf_DistAttack2, 1, 100);
					ImGui::Spacing();
					ImGui::Spacing(); */
					ImGui::EndChild();
				}

			} // AImbot - Completed Finnaly Credits - Snaps and Phobia

			if (tabSelected == 1) // visualstab completed creits - Snaps and Phobia
			{


				ImGui::BeginChild("secoad nikld", ImVec2(450, 495), true);
				{
					ImGui::BeginGroup();
					const char* iHitSound[] =
					{
						"Off",
						"Default",
						"Anime",
						"Roblox",
						"Gamesense",
					};

					const char* material_items[] =
					{
						"Glass",
						"Crystal",
						"Gold",
						"Dark Chrome",
						"Plastic Glass",
						"Velvet",
						"Crystal Blue",
						"Detailed Gold"
					};

					const char* armtype_items[] =
					{
						"Arms Only",
						"Arms + Weapon"
					};

					string style_1 = "None";
					string style_2 = "Box";
					string style_3 = "Corners";

					const char* items1[] = { style_1.c_str() , style_2.c_str(), style_3.c_str() };

					ImGui::PushItemWidth(339.f);
					ImGui::Combo("Type", &Settings::Esp::esp_Style, items1, IM_ARRAYSIZE(items1));
					ImGui::PopItemWidth();

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox("Team", &Settings::Esp::esp_Team);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Enemy", &Settings::Esp::esp_Enemy);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Bomb", &Settings::Esp::esp_Bomb);



					ImGui::Checkbox("Footsteps", &Settings::Esp::esp_Sound);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Aim Lines", &Settings::Esp::esp_Line);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("OutLine", &Settings::Esp::esp_Outline);



					ImGui::Checkbox("Rank", &Settings::Esp::esp_Rank);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Weapon", &Settings::Esp::esp_Weapon);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Ammo", &Settings::Esp::esp_Ammo);



					ImGui::Checkbox("Distance", &Settings::Esp::esp_Distance);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Skeleton", &Settings::Esp::esp_Skeleton);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("World Weapon", &Settings::Esp::esp_WorldWeapons);



					ImGui::Checkbox("World Grenade", &Settings::Esp::esp_WorldGrenade);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Grenade ESP", &Settings::Esp::esp_BoxNade);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Chicken", &Settings::Esp::esp_Chicken);



					ImGui::Checkbox("Nickname", &Settings::Esp::esp_Name);
					ImGui::SameLine(SpaceLineOne);
					//ImGui::Checkbox("Time", &Settings::Esp::esp_Time);
					//ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Watermark", &Settings::Esp::esp_Watermark);



					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					string visible_1 = "Enemy";
					string visible_2 = "Team";
					string visible_3 = "All";

					// Removed Seperator from here

					const char* items2[] = { visible_1.c_str() , visible_2.c_str() , visible_3.c_str() };

					string chams_1 = "None";
					string chams_2 = "Flat";
					string chams_3 = "Texture";

					const char* items5[] = { chams_1.c_str() , chams_2.c_str() , chams_3.c_str() };
					ImGui::Combo("Chams", &Settings::Esp::esp_Chams, items5, IM_ARRAYSIZE(items5));
					ImGui::Combo("Chams Visible", &Settings::Esp::esp_ChamsVisible, items2, IM_ARRAYSIZE(items2));
					ImGui::Checkbox("Wall Hack Chams", &Settings::Esp::esp_XQZ);
					// Removed Seperator

					ImGui::PushItemWidth(339.f);
					ImGui::Combo("Visible", &Settings::Esp::esp_Visible, items2, IM_ARRAYSIZE(items2));

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					//ImGui::SliderInt("Size", &Settings::Esp::esp_Size, 0, 10);
					//ImGui::SliderInt("Bomb Timer", &Settings::Esp::esp_BombTimer, 0, 65);

					string hpbar_1 = "None";
					string hpbar_2 = "Number";
					string hpbar_3 = "Bottom";
					string hpbar_4 = "Left";


					const char* items4[] = { hpbar_1.c_str() , hpbar_2.c_str() , hpbar_3.c_str() , hpbar_4.c_str() };
					ImGui::Combo("Health", &Settings::Esp::esp_Health, items4, IM_ARRAYSIZE(items4));

					string arbar_1 = "None";
					string arbar_2 = "Number";
					string arbar_3 = "Bottom";
					string arbar_4 = "Right";


					const char* items3[] = { arbar_1.c_str() , arbar_2.c_str() , arbar_3.c_str() , arbar_4.c_str() };
					ImGui::Combo("Armor", &Settings::Esp::esp_Armor, items3, IM_ARRAYSIZE(items3));

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox("Chams Materials", &Settings::Misc::misc_ChamsMaterials);
					ImGui::PushItemWidth(362.f);
					ImGui::Combo("##CHAMSMATERIALS", &Settings::Misc::misc_ChamsMaterialsList, material_items, ARRAYSIZE(material_items));

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox("Arms Materials", &Settings::Misc::misc_ArmMaterials);
					ImGui::PushItemWidth(362.f);
					ImGui::Combo("##ARMMATERIALTYPE", &Settings::Misc::misc_ArmMaterialsType, armtype_items, ARRAYSIZE(armtype_items));
					ImGui::SameLine();
					ImGui::Text("Type");
					ImGui::PushItemWidth(362.f);
					ImGui::Combo("##ARMMATERIALS", &Settings::Misc::misc_ArmMaterialsList, material_items, ARRAYSIZE(material_items));
					ImGui::SameLine();
					ImGui::Text("Material");

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox("Hitmarker", &Settings::Esp::esp_HitMarker);
					ImGui::Combo("##HITSOUND", &Settings::Esp::esp_HitMarkerSound, iHitSound, ARRAYSIZE(iHitSound));
					ImGui::SameLine();
					ImGui::Text("Sound");

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::PushItemWidth(339.f);


					ImGui::PopItemWidth();
					ImGui::EndGroup();
					ImGui::EndChild();
				}
			}

			else if (tabSelected == 2)
			{
				const char* knife_models_items[] =
				{
					"Default","Bayonet","Flip","Gut","Karambit" ,"M9 Bayonet",
					"Huntsman","Falchion","Bowie","Butterfly","Shadow Daggers"
				};
				const char* gloves_listbox_items[49] =
				{
					"Default",
					"Sport Gloves | Superconductor",
					"Sport Gloves | Pandora's Box",
					"Sport Gloves | Hedge Maze",
					"Sport Gloves | Arid",
					"Sport Gloves | Vice",
					"Sport Gloves | Omega",
					"Sport Gloves | Bronze Morph",
					"Sport Gloves | Amphibious",
					"Moto Gloves | Eclipse",
					"Moto Gloves | Spearmint",
					"Moto Gloves | Boom!",
					"Moto Gloves | Cool Mint",
					"Moto Gloves | Polygon",
					"Moto Gloves | Transport",
					"Moto Gloves | Turtle",
					"Moto Gloves | Pow",
					"Specialist Gloves | Crimson Kimono",
					"Specialist Gloves | Emerald Web",
					"Specialist Gloves | Foundation",
					"Specialist Gloves | Forest DDPAT",
					"Specialist Gloves | Mogul",
					"Specialist Gloves | Fade",
					"Specialist Gloves | Buckshot",
					"Specialist Gloves | Crimson Web",
					"Driver Gloves | Lunar Weave",
					"Driver Gloves | Convoy",
					"Driver Gloves | Crimson Weave",
					"Driver Gloves | Diamondback",
					"Driver Gloves | Racing Green",
					"Driver Gloves | Overtake",
					"Driver Gloves | Imperial Plad",
					"Driver Gloves | King Snake",
					"Hand Wraps | Leather",
					"Hand Wraps | Spruce DDPAT",
					"Hand Wraps | Badlands",
					"Hand Wraps | Slaughter",
					"Hand Wraps | Aboreal",
					"Hand Wraps | Duct Tape",
					"Hand Wraps | Overprint",
					"Hand Wraps | Cobalt Skulls",
					"Bloodhound Gloves | Charred",
					"Bloodhound Gloves | Snakebite",
					"Bloodhound Gloves | Bronzed",
					"Bloodhound Gloves | Guerrilla",
					"Hydra Gloves | Case Hardened",
					"Hydra Gloves | Rattler",
					"Hydra Gloves | Mangrove",
					"Hydra Gloves | Emerald",
				};

				static int iSelectKnifeCTSkinIndex = -1;
				static int iSelectKnifeTTSkinIndex = -1;

				int iKnifeCTModelIndex = Settings::Skin::knf_ct_model;
				int iKnifeTTModelIndex = Settings::Skin::knf_tt_model;

				static int iOldKnifeCTModelIndex = -1;
				static int iOldKnifeTTModelIndex = -1;

				if (iOldKnifeCTModelIndex != iKnifeCTModelIndex && Settings::Skin::knf_ct_model)
					iSelectKnifeCTSkinIndex = GetKnifeSkinIndexFromPaintKit(Settings::Skin::knf_ct_skin, false);

				if (iOldKnifeTTModelIndex != iKnifeTTModelIndex && Settings::Skin::knf_tt_model)
					iSelectKnifeTTSkinIndex = GetKnifeSkinIndexFromPaintKit(Settings::Skin::knf_ct_skin, true);

				iOldKnifeCTModelIndex = iKnifeCTModelIndex;
				iOldKnifeTTModelIndex = iKnifeTTModelIndex;

				string KnifeCTModel = knife_models_items[Settings::Skin::knf_ct_model];
				string KnifeTTModel = knife_models_items[Settings::Skin::knf_tt_model];



				ImGui::BeginChild("##hild", ImVec2(450, 495), true);
				{
					ImGui::PushItemWidth(230.f);
					ImGui::Text(u8"Knife");
					KnifeTTModel += " ##KTT";
					ImGui::Combo(u8"Knife CT", &Settings::Skin::knf_ct_model, knife_models_items, IM_ARRAYSIZE(knife_models_items));
					ImGui::ComboBoxArray(KnifeCTModel.c_str(), &iSelectKnifeCTSkinIndex, KnifeSkins[iKnifeCTModelIndex].SkinNames);
					ImGui::SliderFloat(u8"Wear Value CT", &g_SkinChangerCfg[WEAPON_KNIFE].flFallbackWear, 0.001f, 1.f, "%.3f");

					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();

					KnifeTTModel += " ##KTT";
					ImGui::Combo(u8"Knife TT", &Settings::Skin::knf_tt_model, knife_models_items, IM_ARRAYSIZE(knife_models_items));
					ImGui::ComboBoxArray(KnifeTTModel.c_str(), &iSelectKnifeTTSkinIndex, KnifeSkins[iKnifeTTModelIndex].SkinNames);
					ImGui::SliderFloat(u8"Wear Value T", &g_SkinChangerCfg[WEAPON_KNIFE_T].flFallbackWear, 0.001f, 1.f, "%.3f");


					if (ImGui::Button(u8"Apply Knife ##Skin"))
					{

						if (iSelectKnifeCTSkinIndex >= 0 && Settings::Skin::knf_ct_model > 0) {
							Settings::Skin::knf_ct_skin = KnifeSkins[iKnifeCTModelIndex].SkinPaintKit[iSelectKnifeCTSkinIndex];
						}
						else
						{
							Settings::Skin::knf_ct_skin = 0;
							iSelectKnifeCTSkinIndex = -1;
						}

						if (iSelectKnifeTTSkinIndex >= 0 && Settings::Skin::knf_tt_model > 0) {
							Settings::Skin::knf_tt_skin = KnifeSkins[iKnifeTTModelIndex].SkinPaintKit[iSelectKnifeTTSkinIndex];
						}
						else
						{
							Settings::Skin::knf_tt_skin = 0;
							iSelectKnifeTTSkinIndex = -1;
						}
						ForceFullUpdate();
					}


					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					static int iOldWeaponID = -1;

					ImGui::Combo("Weapon##WeaponSelect", &iWeaponID, pWeaponData, IM_ARRAYSIZE(pWeaponData));

					iWeaponSelectIndex = pWeaponItemIndexData[iWeaponID];

					if (iOldWeaponID != iWeaponID)
						iWeaponSelectSkinIndex = GetWeaponSkinIndexFromPaintKit(g_SkinChangerCfg[iWeaponSelectIndex].nFallbackPaintKit);

					iOldWeaponID = iWeaponID;

					string WeaponSkin = pWeaponData[iWeaponID];
					WeaponSkin += u8" Skin";

					ImGui::ComboBoxArray(WeaponSkin.c_str(), &iWeaponSelectSkinIndex, WeaponSkins[iWeaponID].SkinNames);
					ImGui::SliderFloat(u8"Wear", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].flFallbackWear, 0.001f, 1.f, "%.3f");
					ImGui::InputInt("StatTrack", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].nFallbackStatTrak, 1, 100, ImGuiInputTextFlags_CharsDecimal);

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Combo(u8"Gloves", &Settings::Skin::gloves_skin, gloves_listbox_items, IM_ARRAYSIZE(gloves_listbox_items));


					if (ImGui::Button(u8"Apply##Skin"))
					{

						if (iWeaponSelectSkinIndex >= 0) {
							g_SkinChangerCfg[iWeaponSelectIndex].nFallbackPaintKit = WeaponSkins[iWeaponID].SkinPaintKit[iWeaponSelectSkinIndex];
						}
						ForceFullUpdate();
					}

					Settings::Sticker::stickerSafeWeapon = SafeWeaponID();
					Stick[Settings::Sticker::stickerSafeWeapon].StickersEnabled = true;
					ImGui::Separator();
					ImGui::Text("Sticker Changer");
					ImGui::Separator();
					static int iSlot = 0;
					const char* slots[] =
					{
						"Back", "Back-Mid", "Front-Mid", "Front"
					};

					ImGui::Combo("Slot", &iSlot, slots, IM_ARRAYSIZE(slots));
					ImGui::SliderFloat("Wear ", &Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear, 0.f, 1.f, "%.3f");
					ImGui::SliderFloat("Size", &Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale, 0.f, 1.f, "%.3f");
					ImGui::SliderInt("Rotate", &Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation, 0, 360);
					ImGui::InputInt("ID", &Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID);

					if (ImGui::Button("Accept Stickers##Skin"))
					{
						switch (Settings::Sticker::stickerSafeWeapon)
						{
						case 1:
							Settings::Sticker::sticker_deserteagle_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_deserteagle_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_deserteagle_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_deserteagle_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_deserteagle_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 2:
							Settings::Sticker::sticker_elites_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_elites_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_elites_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_elites_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_elites_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 3:
							Settings::Sticker::sticker_fiveseven_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_fiveseven_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_fiveseven_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_fiveseven_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_fiveseven_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 4:
							Settings::Sticker::sticker_glock_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_glock_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_glock_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_glock_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_glock_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 7:
							Settings::Sticker::sticker_ak_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_ak_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_ak_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_ak_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_ak_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 8:
							Settings::Sticker::sticker_aug_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_aug_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_aug_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_aug_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_aug_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 9:
							Settings::Sticker::sticker_awp_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_awp_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_awp_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_awp_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_awp_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 10:
							Settings::Sticker::sticker_famas_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_famas_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_famas_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_famas_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_famas_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 11:
							Settings::Sticker::sticker_g35g1_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_g35g1_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_g35g1_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_g35g1_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_g35g1_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 13:
							Settings::Sticker::sticker_galil_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_galil_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_galil_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_galil_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_galil_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 14:
							Settings::Sticker::sticker_m249_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_m249_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_m249_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_m249_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_m249_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 16:
							Settings::Sticker::sticker_m4a4_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_m4a4_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_m4a4_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_m4a4_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_m4a4_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 17:
							Settings::Sticker::sticker_mac10_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_mac10_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_mac10_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_mac10_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_mac10_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 19:
							Settings::Sticker::sticker_p90_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_p90_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_p90_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_p90_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_p90_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 24:
							Settings::Sticker::sticker_ump_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_ump_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_ump_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_ump_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_ump_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 25:
							Settings::Sticker::sticker_xm_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_xm_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_xm_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_xm_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_xm_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 26:
							Settings::Sticker::sticker_bizon_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_bizon_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_bizon_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_bizon_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_bizon_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 27:
							Settings::Sticker::sticker_mag7_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_mag7_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_mag7_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_mag7_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_mag7_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 28:
							Settings::Sticker::sticker_negev_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_negev_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_negev_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_negev_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_negev_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 29:
							Settings::Sticker::sticker_sawedoff_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_sawedoff_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_sawedoff_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_sawedoff_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_sawedoff_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 30:
							Settings::Sticker::sticker_tec_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_tec_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_tec_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_tec_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_tec_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 32:
							Settings::Sticker::sticker_p2000_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_p2000_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_p2000_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_p2000_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_p2000_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 33:
							Settings::Sticker::sticker_mp7_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_mp7_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_mp7_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_mp7_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_mp7_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 34:
							Settings::Sticker::sticker_mp9_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_mp9_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_mp9_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_mp9_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_mp9_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 35:
							Settings::Sticker::sticker_nova_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_nova_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_nova_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_nova_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_nova_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 36:
							Settings::Sticker::sticker_p250_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_p250_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_p250_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_p250_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_p250_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 38:
							Settings::Sticker::sticker_scar_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_scar_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_scar_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_scar_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_scar_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 39:
							Settings::Sticker::sticker_sg_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_sg_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_sg_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_sg_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_sg_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						case 40:
							Settings::Sticker::sticker_ssg_iSlot[iSlot] = iSlot;
							Settings::Sticker::sticker_ssg_id[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iID;
							Settings::Sticker::sticker_ssg_wear[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flWear;
							Settings::Sticker::sticker_ssg_size[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].flScale;
							Settings::Sticker::sticker_ssg_rotate[iSlot] = Stick[Settings::Sticker::stickerSafeWeapon].Stickers[iSlot].iRotation;
							break;
						}

						ForceFullUpdate();
					}





					ImGui::EndChild();

				}
			} // Skins Tab Complete Credit Snaps
			else if (tabSelected == 3)
			{



				ImGui::BeginChild("first child", ImVec2(450, 495), true);
				{
					const char* skybox_items[] =
					{
						"cs_baggage_skybox_",
						"cs_tibet",
						"embassy",
						"italy",
						"jungle",
						"nukeblank",
						"office",
						"sky_cs15_daylight01_hdr",
						"sky_cs15_daylight02_hdr",
						"sky_cs15_daylight03_hdr",
						"sky_cs15_daylight04_hdr",
						"sky_csgo_cloudy01",
						"sky_csgo_night02",
						"sky_csgo_night02b",
						"sky_csgo_night_flat",
						"sky_day02_05_hdr",
						"sky_day02_05",
						"sky_dust",
						"sky_l4d_rural02_ldr",
						"sky_venice",
						"vertigo_hdr",
						"vertigoblue_hdr",
						"vertigo",
						"vietnam"
					};

					//		ImGui::Checkbox("Skin Changer", &Settings::Misc::misc_SkinChanger);
					//		ImGui::Checkbox("Knife Changer", &Settings::Misc::misc_KnifeChanger);
					ImGui::Checkbox("Auto Accept", &Settings::Misc::misc_AutoAccept);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Bunnyhop", &Settings::Misc::misc_Bhop);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Strafe", &Settings::Misc::misc_AutoStrafe);

					ImGui::Checkbox("Spec List", &Settings::Misc::misc_Spectators);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Recoil Dot", &Settings::Misc::misc_Punch);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Sniper Crosshair", &Settings::Misc::misc_AwpAim);

					ImGui::Checkbox("No Flash", &Settings::Misc::misc_NoFlash);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("No Smoke", &Settings::Misc::misc_NoSmoke);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("No Hands", &Settings::Misc::misc_NoHands);

					//ImGui::Checkbox("NightMode", &Settings::Esp::esp_NightMode);
					ImGui::Spacing();
					ImGui::Checkbox("Slide Walk", &Settings::Misc::misc_Moonwalk);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Wire Hands", &Settings::Misc::misc_WireHands);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox(u8"Grenade Trajectory", &Settings::Esp::esp_GrenadePrediction);

					ImGui::Checkbox("Dynamic Light", &Settings::Esp::esp_Dlightz);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Radar", &Settings::Misc::misc_Radar);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Wireframe Smoke", &Settings::Misc::esp_wireframesmoke);

					/*ImGui::Checkbox("Grenade Helper", &Settings::Esp::helper);
					ImGui::SameLine();
					if (ImGui::Button("Update Map"))
					cGrenade.bUpdateGrenadeInfo(Interfaces::Engine()->GetLevelNameShort());*/

					ImGui::Separator();
					ImGui::Text("Chat Spam");
					ImGui::Checkbox("Normal", &Settings::Misc::misc_spamregular);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Random", &Settings::Misc::misc_spamrandom);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Name Spam", &Settings::Misc::misc_namespamidkmemes_static);
					//		ImGui::SameLine(180);
					//		ImGui::Checkbox("Custom Name Spam", &Settings::Misc::misc_namespamidkmemes);
					//		if (Settings::Misc::misc_namespamidkmemes)
					//		{
					//			ImGui::InputText("First", Settings::Misc::First, IM_ARRAYSIZE(Settings::Misc::First));
					//		}

					ImGui::Separator();
					ImGui::Text("World Effects");
					ImGui::Checkbox("Chrome World", &Settings::Esp::esp_ChromeWorld);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Minecraft", &Settings::Esp::esp_MinecraftMode);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("LSD World", &Settings::Esp::esp_LSDMode);

					ImGui::Separator();
					ImGui::Text("Fake Lag/Ping");
					ImGui::Checkbox("Enable Fakelatency", &Settings::Misc::misc_fakeping);

					if (Settings::Misc::misc_fakeping)
					{
						ImGui::SliderInt("Fakelatency value", &Settings::Misc::misc_fakeping_value, 50, 1000);
					}

					ImGui::SliderFloat("##FAKELAG_AMOUNT", &Settings::Misc::misc_fakelag_amount, 0.f, 20.f, "Fake Lag Amount: %0.f");

					ImGui::Separator();
					ImGui::Text("Custom Changers");
					static char buf2[128] = { 0 };
					ImGui::InputText("##CustomName", buf2, 16, Settings::Misc::misc_NameChanger);
					if (ImGui::Button("Apply Custom Name")) {
						ConVar* Name = Interfaces::GetConVar()->FindVar("name");
						*(int*)((DWORD)&Name->fnChangeCallback + 0xC) = 0;
						Name->SetValue(buf2);
					}
					ImGui::Separator();
					static char misc_CustomClanTag[128] = { 0 };
					ImGui::PushItemWidth(300.f);
					ImGui::InputText("Clan Tag Changer", misc_CustomClanTag, 128);
					ImGui::PopItemWidth();
					if (ImGui::Button("Apply Custom ClanTag"))
					{
						Engine::ClanTagApply(misc_CustomClanTag);
					}

					ImGui::Separator();
					ImGui::Checkbox("FOV Changer", &Settings::Misc::misc_FovChanger);
					ImGui::PushItemWidth(362.f);
					ImGui::SliderInt("FOV View", &Settings::Misc::misc_FovView, 1, 170);
					ImGui::SliderInt("FOV Model View", &Settings::Misc::misc_FovModelView, 1, 190);
					ImGui::Separator();

					//ImGui::Checkbox("Inventory Changer", &Settings::Misc::misc_InvChanger);
					ImGui::Checkbox("Medal Changer", &Settings::Misc::misc_MedalChanger);
					ImGui::Checkbox("SNAPSHOOK Search Engine", &Settings::Misc::misc_FilterList);
					//ImGui::Checkbox("Rank , Wins and Commends Changer", &Settings::Misc::misc_RankSpoof);

					//ImGui::Checkbox("Third Person", &Settings::Misc::misc_ThirdPerson);
					//ImGui::SliderFloat("##THIRDPERSONRANGE", &Settings::Misc::misc_ThirdPersonRange, 0.f, 500.f, "Third Person Range: %0.f");



					

					ImGui::Separator();

					string clan_1 = "None";
					string clan_2 = "Clear";
					string clan_3 = "Static";
					string clan_4 = "Static No-name";
					string clan_5 = "Valve";
					string clan_6 = "Valve No-name";
					string clan_7 = "Animation";
					const char* items5[] = { clan_1.c_str() , clan_2.c_str() , clan_3.c_str() , clan_4.c_str() , clan_5.c_str() , clan_6.c_str() , clan_7.c_str() };
					ImGui::Combo("Clantag", &Settings::Misc::misc_Clan, items5, IM_ARRAYSIZE(items5));
					ImGui::Separator();
					ImGui::Spacing();

					if (ImGui::Combo("", &Settings::Misc::misc_CurrentSky, skybox_items, IM_ARRAYSIZE(skybox_items)))
					{
						Settings::Misc::misc_SkyName = skybox_items[Settings::Misc::misc_CurrentSky];
					}
					ImGui::SameLine();
					ImGui::Text("SkyBox");

					ImGui::Checkbox("No Sky", &Settings::Misc::misc_NoSky);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Disable Postprocess", &Settings::Misc::misc_EPostprocess);

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
					ImGui::PushItemWidth(220.f);
					ImGui::ComboBoxArray(u8"CFG List", &iConfigSelect, ConfigList);

					if (ImGui::Button(u8"Load"))
					{

						Settings::LoadSettings("C:/SNAPSHOOK/" + ConfigList[iConfigSelect]);
					}
					ImGui::SameLine();
					if (ImGui::Button(u8"Save"))
					{

						Settings::SaveSettings("C:/SNAPSHOOK/" + ConfigList[iConfigSelect]);
					}
					ImGui::SameLine();
					if (ImGui::Button(u8"Refresh CFG"))
					{
						RefreshConfigs();
					}

					ImGui::InputText("Config Name", ConfigName, 64);

					if (ImGui::Button(u8"Create and Save Your Config"))
					{
						string ConfigFileName = ConfigName;

						if (ConfigFileName.size() < 1)
						{
							ConfigFileName = "Default";
						}

						Settings::SaveSettings("C:/SNAPSHOOK/" + ConfigFileName + ".ini");
						RefreshConfigs();
					}
				}
				ImGui::EndChild();
			} // Misc Tab Compelte Credits Snaps

			else if (tabSelected == 4)
			{
				ImGui::BeginGroup();
				ImGui::PushItemWidth(170.f);



				ImGui::BeginChild("first child", ImVec2(450, 495), true);
				{
					ImGui::MyColorEdit3("Hit Marker Color", Settings::Esp::esp_HitMarkerColor);
					ImGui::MyColorEdit3("CT ESP Color", Settings::Esp::esp_Color_CT);
					ImGui::MyColorEdit3("T ESP Color", Settings::Esp::esp_Color_TT);
					ImGui::MyColorEdit3("Visible CT Color ", Settings::Esp::esp_Color_VCT);
					ImGui::MyColorEdit3("Visible T Color ", Settings::Esp::esp_Color_VTT);
					ImGui::MyColorEdit3("Chams Color CT", Settings::Esp::chams_Color_CT);
					ImGui::MyColorEdit3("Chams Color T", Settings::Esp::chams_Color_TT);
					ImGui::MyColorEdit3("Chams Color Visible CT", Settings::Esp::chams_Color_VCT);
					ImGui::MyColorEdit3("Chams Color Visible T", Settings::Esp::chams_Color_VTT);
					ImGui::Spacing();

				}
				ImGui::EndChild();
				ImGui::EndGroup();
				ImGui::PopItemWidth();
			} // Colors Tab Complete Credits Snaps

		};
		ImGui::End();
	}
}










































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































