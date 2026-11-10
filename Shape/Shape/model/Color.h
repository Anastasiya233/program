#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iomanip>

class Color
{
public:
	Color() = default;
	Color(std::uint8_t r, std::uint8_t g, std::uint8_t b);
	std::uint8_t GetR() const;
	std::uint8_t GetG() const;
	std::uint8_t GetB() const;
	static Color GetRGB(const std::string& hex);
	std::string GetHexStr() const;

private:
	std::uint8_t m_r = 0;
	std::uint8_t m_g = 0;
	std::uint8_t m_b = 0;
};
