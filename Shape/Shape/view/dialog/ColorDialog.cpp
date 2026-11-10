#include "ColorDialog.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <commdlg.h>

ColorDialog::ColorDialog(void* owner)
	: m_owner(owner)
{
	m_customColors.fill(RGB(255, 255, 255));
}

std::optional<Color> ColorDialog::AskColor(const Color& initialColor)
{
	CHOOSECOLORW dialog{};
	dialog.lStructSize = sizeof(dialog);
	dialog.hwndOwner = static_cast<HWND>(m_owner);
	dialog.rgbResult = RGB(initialColor.GetR(), initialColor.GetG(), initialColor.GetB());
	dialog.lpCustColors = m_customColors.data();
	dialog.Flags = CC_RGBINIT | CC_FULLOPEN;

	if (!ChooseColorW(&dialog))
	{
		return std::nullopt;
	}

	return Color{
		static_cast<uint8_t>(GetRValue(dialog.rgbResult)),
		static_cast<uint8_t>(GetGValue(dialog.rgbResult)),
		static_cast<uint8_t>(GetBValue(dialog.rgbResult))
	};
}