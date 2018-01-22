#pragma once

#include <string>

#define OPTION(type, var, val) type var = val

class Config
{
public:
#pragma region Visuals
	OPTION(bool, VIS_Chams, false);
	OPTION(bool, VIS_ESP, false);
	OPTION(bool, VIS_ESP_Box, false);
	OPTION(bool, VIS_ESP_Bone, false);
	OPTION(bool, VIS_ESP_Name, false);
	OPTION(bool, VIS_ESP_Health, false);
	OPTION(bool, VIS_ESP_Weapon, false);
	OPTION(bool, VIS_ESP_Snaplines, false);
#pragma endregion
#pragma region Misc
	OPTION(bool, MISC_Bunnyhop, false);
	OPTION(bool, MISC_AutoAccept, true);
	OPTION(bool, MISC_RankReveal, false);
	OPTION(bool, MISC_Backtrack, true);
	OPTION(int, MISC_ViewmodelFOV, 0);
	OPTION(std::wstring, MISC_ClanTag, L"");
#pragma endregion
};

extern Config g_Options;
