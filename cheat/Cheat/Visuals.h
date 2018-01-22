#pragma once
#include "../Core/stdafx.h"
#include "../SDK/csgostructs.h"
#include "../Core/singleton.h"

class Visuals : public Singleton<Visuals>
{
	void Overlay();
	void SniperCrosshair();
	void ESP_Box(int x, int y, int w, int h, C_BasePlayer* pEntity);
	void ESP_Name(int x, int y, int w, int h, C_BasePlayer* pEntity);
	void ESP_Health(int x, int y, int w, int h, C_BasePlayer* pEntity);
	void ESP_Weapon(int x, int y, int w, int h, C_BasePlayer* pEntity);
	void ESP_Bone(C_BasePlayer* pEntity);
	void ESP_Snapline(C_BasePlayer* pEntity);
public:
	void PaintTraverse();
	void DrawModelExecute(IMatRenderContext * ctx, const DrawModelState_t &state, const ModelRenderInfo_t &info, matrix3x4_t *matrix);
};
