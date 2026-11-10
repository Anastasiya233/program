#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button
{
public:
	Button(sf::Vector2f position, sf::Vector2f size, const std::string& text,
		const sf::Font& font, std::function<void()> onClick);

	void Draw(sf::RenderWindow& window) const;
	bool HitTest(sf::Vector2f point) const;
	void Click() const;
	void SetActive(bool active);

private:
	sf::RectangleShape m_shape;
	sf::Text m_text;
	std::function<void()> m_onClick;
	bool m_active = false;
};