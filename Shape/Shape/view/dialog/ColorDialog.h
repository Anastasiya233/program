#pragma once

#include "../../model/Color.h"
#include <array>
#include <optional>

class ColorDialog
{
public:
	explicit ColorDialog(void* owner);

	std::optional<Color> AskColor(const Color& initialColor);

private:
	void* m_owner;
	std::array<unsigned long, 16> m_customColors{};
};