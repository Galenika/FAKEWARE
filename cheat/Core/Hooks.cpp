#include "Hooks.h"
#include "InputSys.h"
#include "Font.h"
#include "math.h"
#include "../Menu/Menu.h"

namespace Hooks
{
	vfunc_hook HLClientHook;
	vfunc_hook Direct3DHook;
	vfunc_hook VGUIPanelHook;
	vfunc_hook VGUISurfaceHook;
	vfunc_hook MDLRenderHook;
	vfunc_hook ClientModeHook;
	vfunc_hook RenderViewHook;
	vfunc_hook GameEventManagerHook;
	vfunc_hook MatSystemHook;
	RecvPropHook* SequenceHook;

	void Initialize()
	{
		// Set up virtual function hooks
		HLClientHook.setup(g_CHLClient);
		Direct3DHook.setup(g_D3DDevice9);
		VGUIPanelHook.setup(g_VGuiPanel);
		VGUISurfaceHook.setup(g_VGuiSurface);
		MDLRenderHook.setup(g_ModelRender);
		ClientModeHook.setup(g_ClientMode);
		GameEventManagerHook.setup(g_GameEvents);
		MatSystemHook.setup(g_MatSystem);

		// Hook functions

		HLClientHook.hook_index(Index::FrameStageNotify, hkFrameStageNotify);
		HLClientHook.hook_index(Index::CreateMove, hkCreateMove_Proxy);

		Direct3DHook.hook_index(Index::EndScene, hkEndScene);
		Direct3DHook.hook_index(Index::Reset, hkReset);

		VGUIPanelHook.hook_index(Index::PaintTraverse, hkPaintTraverse);

		VGUISurfaceHook.hook_index(Index::PlaySound, hkPlaySound);
		VGUISurfaceHook.hook_index(Index::LockCursor, hkLockCursor);

		MDLRenderHook.hook_index(Index::DrawModelExecute, hkDrawModelExecute);

		ClientModeHook.hook_index(Index::GetViewmodelFOV, hkGetViewmodelFOV);

		GameEventManagerHook.hook_index(Index::FireEventClientSide, hkFireEventClientSide);

		auto SequenceProp = C_BaseViewModel::GetSequenceProp();
		SequenceHook = new RecvPropHook(SequenceProp, hkSequenceProxyFn);

		g_VGuiSurface->PlaySound_("ui\\mm_success_lets_roll.wav");

		Font::Get().Create();
		Menu::Get().Init();
		g_ClientState->ForceFullUpdate();
		Skinchanger::Get().Dump();
	}

	void Kill()
	{
		Menu::Get().Kill();

		// Kill all hooks
		HLClientHook.unhook_all();
		Direct3DHook.unhook_all();
		VGUIPanelHook.unhook_all();
		VGUISurfaceHook.unhook_all();
		MDLRenderHook.unhook_all();
		ClientModeHook.unhook_all();
		GameEventManagerHook.unhook_all();
		MatSystemHook.unhook_all();
	}

	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* device)
	{
		/*
		Spoofed ConVars
		*/
		{
			static auto mat_postprocess_enable = g_CVar->FindVar("mat_postprocess_enable");
			mat_postprocess_enable->m_fnChangeCallbacks.m_Size = 0;
			mat_postprocess_enable->SetValue(0);

			static auto fog_override = g_CVar->FindVar("fog_override");
			fog_override->m_fnChangeCallbacks.m_Size = 0;
			fog_override->SetValue(1);

			static auto fog_enable = g_CVar->FindVar("fog_enable");
			fog_enable->m_fnChangeCallbacks.m_Size = 0;
			fog_enable->SetValue(0);

			static auto fog_enableskybox = g_CVar->FindVar("fog_enableskybox");
			fog_enableskybox->m_fnChangeCallbacks.m_Size = 0;
			fog_enableskybox->SetValue(0);
		}

		if (g_Options.MISC_RankReveal && InputSys::Get().IsKeyDown(VK_TAB))
			Utilities::RevealRanks();

		auto ofunc = Direct3DHook.get_original<EndScene>(Index::EndScene);
		return ofunc(device);
	}

	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		auto ofunc = Direct3DHook.get_original<Reset>(Index::Reset);

		auto hr = ofunc(device, pPresentationParameters);

		if (hr >= 0)
		{
			Font::Get().Create();
		}

		return hr;
	}

	//--------------------------------------------------------------------------------
	void __stdcall hkPlaySound(const char* name)
	{
		static auto ofunc = VGUISurfaceHook.get_original<PlaySound>(Index::PlaySound);
		ofunc(g_VGuiSurface, name);

		if (g_Options.MISC_AutoAccept)
		{
			// Auto Accept
			if (strstr(name, "!UI/competitive_accept_beep.wav"))
			{
				static auto fnAccept = (void(*)())Utilities::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 83 BE");

				fnAccept();

				//This will flash the CSGO window on the taskbar
				//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
				FLASHWINFO fi;
				fi.cbSize = sizeof(FLASHWINFO);
				fi.hwnd = InputSys::Get().GetMainWindow();
				fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
				fi.uCount = 0;
				fi.dwTimeout = 0;
				FlashWindowEx(&fi);
			}
		}
	}

	//--------------------------------------------------------------------------------
	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		switch (stage)
		{
		case FRAME_UNDEFINED:
			break;
		case FRAME_START:
			break;
		case FRAME_NET_UPDATE_START:
			break;
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
			if (!Menu::Get().IsVisible())
				Utilities::SetClantag(g_Options.MISC_ClanTag);
			Skinchanger::Get().Run();
			break;
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
			break;
		case FRAME_NET_UPDATE_END:
			break;
		case FRAME_RENDER_START:
			break;
		case FRAME_RENDER_END:
			break;
		}

		static auto ofunc = HLClientHook.get_original<FrameStageNotify>(Index::FrameStageNotify);
		ofunc(g_CHLClient, stage);
	}

	//--------------------------------------------------------------------------------
	void __stdcall hkLockCursor()
	{
		static auto ofunc = VGUISurfaceHook.get_original<LockCursor>(Index::LockCursor);

		if (Menu::Get().IsVisible()) g_VGuiSurface->UnlockCursor();
		else ofunc();
	}

	//--------------------------------------------------------------------------------
	float __stdcall hkGetViewmodelFOV()
	{
		static auto ofunc = ClientModeHook.get_original<GetViewmodelFOV>(Index::GetViewmodelFOV);
		if (g_EngineClient->IsTakingScreenshot()) return ofunc();
		else return ofunc() + g_Options.MISC_ViewmodelFOV;
	}
}