#pragma once

#include "../../editor/EditorController.h"
#include "../../editor/EditorState.h"
#include "../../model/Picture.h"
#include <SFML/Graphics.hpp>

class CanvasView
{
public:
	CanvasView(sf::RenderWindow& window, const sf::Font& font,
		Picture& picture, EditorState& state, EditorController& controller);

	void Draw();
	void UpdateCursor(sf::Vector2f mousePos);

private:
	void DrawCanvas();
	void DrawGhost();
	void DrawSelectionBox();
	void DrawDraft();

	sf::RenderWindow& m_window;
	const sf::Font& m_font;
	Picture& m_picture;
	EditorState& m_state;
	EditorController& m_controller;
};