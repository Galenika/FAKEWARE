#pragma once

#include "../Core/SDK.h"
#include "../Core/singleton.h"
#include "GUI.h"

class Menu : public Singleton<Menu>
{
	bool m_isVisible;
	CForm MenuForm;
public:
	void Init();
	void Kill();

	void PaintTraverse();
	void Click();
	void HandleInput(WPARAM vk);

	void Toggle();

	bool IsVisible() const { return m_isVisible; }
};