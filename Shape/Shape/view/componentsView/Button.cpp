#include "Button.h"

Button::Button(sf::Vector2f position, sf::Vector2f size, const std::string& text,
	const sf::Font& font, std::function<void()> onClick)
	: m_shape(size)
	, m_text(font)
	, m_onClick(onClick)
{
	m_shape.setPosition(position);
	m_shape.setFillColor(sf::Color(200, 200, 200));
	m_shape.setOutlineColor(sf::Color::Black);

	m_text.setString(text);
	m_text.setCharacterSize(16);
	m_text.setFillColor(sf::Color::Black);

	sf::FloatRect textBounds = m_text.getLocalBounds();
	m_text.setOrigin({ textBounds.position.x + textBounds.size.x / 2.f,
		textBounds.position.y + textBounds.size.y / 2.f });
	m_text.setPosition(position + size / 2.f);
}

void Button::Draw(sf::RenderWindow& window) const
{
	sf::RectangleShape shape = m_shape;
	if (m_active)
	{
		shape.setFillColor(sf::Color(150, 180, 255));
		shape.setOutlineColor(sf::Color::Blue);
		shape.setOutlineThickness(2.f);
	}
	window.draw(shape);
	window.draw(m_text);
}

bool Button::HitTest(sf::Vector2f point) const
{
	return m_shape.getGlobalBounds().contains(point);
}

void Button::Click() const
{
	if (m_onClick)
	{
		m_onClick();
	}
}

void Button::SetActive(bool active)
{
	m_active = active;
}

