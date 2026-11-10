#include "Color.h"

Color::Color(std::uint8_t r, std::uint8_t g, std::uint8_t b)
	: m_r(r)
	, m_g(g)
	, m_b(b)
{
}

std::uint8_t Color::GetR() const
{
	return m_r;
}
std::uint8_t Color::GetG() const
{
	return m_g;
}
std::uint8_t Color::GetB() const
{
	return m_b;
}

Color Color::GetRGB(const std::string& hex)
{
	if (hex.size() != 7 || hex[0] != '#')
	{
		throw std::invalid_argument("Color must be #RRGGBB");
	}
	auto parse = [&](std::size_t pos) -> std::uint8_t {
		return static_cast<std::uint8_t>(std::stoul(hex.substr(pos, 2), nullptr, 16));
		};
	return Color{ parse(1), parse(3), parse(5) };
}

std::string Color::GetHexStr() const
{
	std::ostringstream os;
	os << '#' << std::hex << std::setfill('0')
		<< std::setw(2) << static_cast<int>(m_r)
		<< std::setw(2) << static_cast<int>(m_g)
		<< std::setw(2) << static_cast<int>(m_b);
	return os.str();
}