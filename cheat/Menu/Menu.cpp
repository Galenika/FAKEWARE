#include "Menu.h"

#include "../Core/Font.h"
#include "../Core/options.h"
#include "../Cheat/Skinchanger.h"

void Menu::Init()
{
	m_isVisible = false;

	CWindow Main(100, 100, 345, 150);

#pragma region Visuals
	auto Visuals = new CChild(0, 0, 120, L"Visuals");

	Visuals->AddControl(new CCheckBox(L"Chams", &g_Options.VIS_Chams));
	Visuals->AddControl(new CCheckBox(L"ESP", &g_Options.VIS_ESP));
	Visuals->AddControl(new CCheckBox(L"Box", &g_Options.VIS_ESP_Box));
	Visuals->AddControl(new CCheckBox(L"Skeleton", &g_Options.VIS_ESP_Bone));
	Visuals->AddControl(new CCheckBox(L"Name", &g_Options.VIS_ESP_Name));
	Visuals->AddControl(new CCheckBox(L"Health", &g_Options.VIS_ESP_Health));
	Visuals->AddControl(new CCheckBox(L"Weapon", &g_Options.VIS_ESP_Weapon));
	Visuals->AddControl(new CCheckBox(L"Snap Lines", &g_Options.VIS_ESP_Snaplines));

	Main.AddControl(Visuals);
#pragma endregion
#pragma region Misc
	auto Misc = new CChild(130, 0, 190, L"Misc");

	Misc->AddControl(new CCheckBox(L"Backtrack", &g_Options.MISC_Backtrack));
	Misc->AddControl(new CCheckBox(L"Auto-Accept", &g_Options.MISC_AutoAccept));
	Misc->AddControl(new CCheckBox(L"Bunnyhop", &g_Options.MISC_Bunnyhop));
	Misc->AddControl(new CCheckBox(L"Reveal Ranks", &g_Options.MISC_RankReveal));
	Misc->AddControl(new CSlider<int>(L"Viewmodel FOV", 0, 40, &g_Options.MISC_ViewmodelFOV));
	Misc->AddControl(new CTextBox(80, 16, L"Clan Tag", &g_Options.MISC_ClanTag));
	Misc->AddControl(new CButton(80, 16, L"Load Skins", []() { Skinchanger::Get().LoadSkins(); }));

	Main.AddControl(Misc);
#pragma endregion

	MenuForm.AddWindow(Main);
}

void Menu::Kill()
{}

void Menu::PaintTraverse()
{
	if (m_isVisible)
		MenuForm.Paint();
}

void Menu::Click()
{
	if (m_isVisible)
		MenuForm.Click();
}

void Menu::HandleInput(WPARAM vk)
{
	if (m_isVisible)
		MenuForm.HandleInput(vk);
}

void Menu::Toggle()
{
	m_isVisible = !m_isVisible;
}
