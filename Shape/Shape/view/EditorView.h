#pragma once
#include "../editor/EditorController.h"
#include "../editor/EditorState.h"
#include "../model/Picture.h"
#include "../view/componentsView/Button.h"
#include "../view/componentsView/CanvasView.h"
#include "../view/componentsView/ShapeListView.h"
#include "../view/componentsView/TimelineView.h"
#include "../view/dialog/FileDialog.h"
#include "dialog/ColorDialog.h"
#include <SFML/Graphics.hpp>

class EditorView
{
public:
	EditorView(Picture& picture, EditorState& state, EditorController& controller);
	bool Update();
	bool WantsNewWindow() const;
	bool WantsSharedWindow() const;
	bool WantsOpenWindow() const;
	std::string GetOpenPath() const;
	void ClearRequests();

private:
	bool m_wantsNewWindow = false;
	bool m_wantsSharedWindow = false;
	bool m_timelineDragging = false;
	std::string m_timelineShapeId;
	TimelineView m_timeline;
	bool m_wantsOpenWindow = false;
	std::string m_openPath;
	sf::RenderWindow m_window;
	sf::Font m_font;

	FileDialog m_fileDialog;
	ColorDialog m_colorDialog;
	Picture& m_picture;
	EditorState& m_state;
	EditorController& m_controller;
	ShapeListView m_listView;
	CanvasView m_canvasView;
	std::vector<Button> m_buttons;

	void ProcessEvents();
	void Render();
	sf::View CanvasCamera() const;
	sf::Vector2f ToCanvas(sf::Vector2i pixel) const;

	void ClickButton(const sf::Event::MouseButtonPressed* placeClick);
	void HandleKeyPressed(const sf::Event::KeyPressed* key);
	void OpenWindow();
	void SaveWindow();

	void DrawToolbar();
};