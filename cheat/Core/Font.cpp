#include "Font.h"

void Font::Create()
{
	Overlay = g_VGuiSurface->CreateFont_();
	MenuText = g_VGuiSurface->CreateFont_();
	ESP = g_VGuiSurface->CreateFont_();

	g_VGuiSurface->SetFontGlyphSet(Overlay, "Tahoma", 14, 900, 0, 0, FONTFLAG_ANTIALIAS);
	g_VGuiSurface->SetFontGlyphSet(MenuText, "Tahoma", 12, 550, 0, 0, FONTFLAG_ANTIALIAS);
	g_VGuiSurface->SetFontGlyphSet(ESP, "Arial", 12, 700, 0, 0, FONTFLAG_DROPSHADOW);
}
