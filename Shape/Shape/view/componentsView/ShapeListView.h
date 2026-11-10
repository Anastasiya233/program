#pragma once

#include "../../editor/EditorController.h"
#include "../../editor/EditorState.h"
#include "../../model/Picture.h"
#include <SFML/Graphics.hpp>

class ShapeListView
{
public:
	ShapeListView(float x, float y, float width, float height, const sf::Font& font);

	void Draw(sf::RenderWindow& window, const Picture& picture, const EditorState& state);

	bool HandleClick(sf::Vector2f mousePos, Picture& picture, EditorState& state);
	void HandleMouseMoved(sf::Vector2f mousePos, const Picture& picture, EditorState& state);
	void HandleMouseReleased(Picture& picture, EditorState& state, EditorController& controller);
	bool HandleScroll(float delta, sf::Vector2f mousePos, int shapeCount);

private:
	float m_x, m_y, m_width, m_height;
	const sf::Font& m_font;
	const float m_rowHeight = 30.f;

	float m_scrollOffset = 0.f;
	const float m_iconSize = 24.f;
	sf::Vector2f m_dragMousePos{ 0.f, 0.f };

	void DrawPanel(sf::RenderWindow& window) const;
	void DrawRow(sf::RenderWindow& window, const Shape& shape, float rowY, bool isSelected);
	void DrawIcon(sf::RenderWindow& window, const Shape& shape, float iconX, float iconY);
	void DrawDragOverlay(sf::RenderWindow& window, const Picture& picture, const EditorState& state);
	bool IsRowVisible(float rowY) const;
};