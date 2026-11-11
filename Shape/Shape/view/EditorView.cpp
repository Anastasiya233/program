#include "EditorView.h"
#include "../editor/EditorController.h"
#include "../editor/EditorState.h"
#include "../model/Picture.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <stdexcept>

sf::Font LoadGameFont()
{
	sf::Font font;
	if (!font.openFromFile("assets/arial.ttf"))
	{
		throw std::runtime_error("Cannot load font");
	}
	return font;
}

EditorView::EditorView(Picture& picture, EditorState& state, EditorController& controller)
	: m_window(sf::VideoMode({ static_cast<unsigned>(state.canvasWidth),
				   static_cast<unsigned>(state.canvasHeight) }),
		  "Graphic Editor",
		  sf::Style::Titlebar | sf::Style::Close)
	, m_picture(picture)
	, m_state(state)
	, m_controller(controller)
	, m_font(LoadGameFont())
	, m_canvasView(m_window, m_font, m_picture, m_state, m_controller)
	, m_listView(static_cast<float>(state.GetUsableCanvasWidth()),
		  0.f,
		  state.listPanelWidth,
		  static_cast<float>(state.canvasHeight),
		  m_font)
	, m_fileDialog(m_window.getNativeHandle())
	, m_colorDialog(m_window.getNativeHandle())
{
	float x = 10.f;
	float gap = 11.f;
	float y = 10.f;
	float h = 30.f;

	m_buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(70.f, h), "+ Rect", m_font, [this]() { m_controller.SetTool(Tool::DrawRect); });
	x += 70.f + gap;

	m_buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(80.f, h), "+ Ellipse", m_font, [this]() { m_controller.SetTool(Tool::DrawEllipse); });
	x += 80.f + gap;

	m_buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(90.f, h), "+ Triangle", m_font, [this]() { m_controller.SetTool(Tool::DrawTriangle); });
	x += 90.f + gap;

	m_buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(95.f, h), "+ Pentagon", m_font, [this]() { m_controller.SetTool(Tool::DrawPentagon); });
	x += 95.f + gap;

	m_buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(70.f, h), "+ Star", m_font, [this]() { m_controller.SetTool(Tool::DrawStar); });
	x += 70.f + gap;

	m_buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(75.f, h), "Delete", m_font, [this]() { m_controller.DeleteShape(); });
	x += 75.f + gap;

	m_buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(70.f, h), "Color", m_font, [this]() {
		if (m_state.SelectedIds.empty())
		{
			return;
		}

		m_state.drag.isDrag = false;
		m_state.resize.isResize = false;
		m_state.ClearPreview();

		Color currentColor = m_picture.GetShape(m_state.SelectedIds[0])->GetColor();
		auto newColor = m_colorDialog.AskColor(currentColor);

		if (newColor.has_value())
		{
			m_controller.SetSelectedShapesColor(newColor.value());
		}
	});
	x += 70.f + gap;

	m_buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(70.f, h), "Open", m_font, [this]() { OpenWindow(); });
	x += 70.f + gap;

	m_buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(70.f, h), "Save", m_font, [this]() { SaveWindow(); });
}

bool EditorView::Update()
{
	if (!m_window.isOpen())
	{
		return false;
	}
	if (!m_state.lastError.empty())
	{
		m_fileDialog.ShowError(m_state.lastError);
		m_state.lastError.clear();
	}

	// Windows use stable IDs, never borrowed Shape pointers. Prune deleted IDs.
	m_state.SelectedIds.erase(std::remove_if(m_state.SelectedIds.begin(),
		m_state.SelectedIds.end(), [this](const std::string& id) {
			return m_picture.GetShape(id) == nullptr;
		}), m_state.SelectedIds.end());
	ProcessEvents();
	Render();
	return m_window.isOpen();
}

void EditorView::ClickButton(const sf::Event::MouseButtonPressed* placeClick)
{
	if (placeClick->button != sf::Mouse::Button::Left)
	{
		return;
	}

	float x = placeClick->position.x;
	float y = placeClick->position.y;

	for (const auto& btn : m_buttons)
	{
		if (btn.HitTest({ x, y }))
		{
			btn.Click();
			return;
		}
	}

	if (y > m_state.toolbarHeight)
	{
		bool isCtrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
			|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
		const auto world = ToCanvas(placeClick->position);
		m_controller.OnMousePressed(world.x, world.y, isCtrl);
	}
}

void EditorView::HandleKeyPressed(const sf::Event::KeyPressed* key)
{
	bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
		|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
	if (isCtrlPressed)
	{
		if (key->code == sf::Keyboard::Key::Z && !key->shift)
		{
			m_controller.Undo();
		}
		else if (key->code == sf::Keyboard::Key::Y
			|| (key->code == sf::Keyboard::Key::Z && key->shift))
		{
			m_controller.Redo();
		}
		else if (key->code == sf::Keyboard::Key::G)
		{
			if (key->shift) m_controller.UngroupSelection();
			else m_controller.GroupSelection();
		}
		else if (key->code == sf::Keyboard::Key::S)
		{
			SaveWindow();
		}
		else if (key->code == sf::Keyboard::Key::O)
		{
			OpenWindow();
		}
		else if (key->code == sf::Keyboard::Key::N)
		{
			if (key->shift) m_wantsSharedWindow = true;
			else m_wantsNewWindow = true;
		}
	}
	else if (key->code == sf::Keyboard::Key::Delete)
	{
		m_controller.DeleteShape();
	}
	else if (key->code == sf::Keyboard::Key::Escape)
	{
		m_controller.CancelDraft();
		m_controller.SetTool(Tool::Select);
	}
}

void EditorView::OpenWindow()
{
	auto path = m_fileDialog.GetOpenPath();
	if (path)
	{
		m_wantsOpenWindow = true;
		m_openPath = *path;
	}
}

void EditorView::SaveWindow()
{
	if (m_state.currentFileName.empty())
	{
		auto path = m_fileDialog.GetSavePath();
		if (!path)
		{
			return;
		}
		m_state.currentFileName = *path;
	}
	m_controller.SaveDocument(m_state.currentFileName);
}

void EditorView::ProcessEvents()
{
	auto toFloat = [](sf::Vector2i v) {
		return sf::Vector2f(static_cast<float>(v.x), static_cast<float>(v.y));
	};

	while (const std::optional event = m_window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			m_window.close();
		}
		else if (auto* placeClick = event->getIf<sf::Event::MouseButtonPressed>())
		{
			sf::Vector2f mousePos = toFloat(placeClick->position);

			if (m_timeline.Contains(mousePos, m_state))
			{
				m_timelineDragging = true;
				m_timelineShapeId = m_timeline.SelectAt(mousePos, m_picture, m_state);
			}
			else if (!m_listView.HandleClick(mousePos, m_picture, m_state))
			{
				ClickButton(placeClick);
			}
		}
		else if (auto* mm = event->getIf<sf::Event::MouseMoved>())
		{
			sf::Vector2f mousePos = toFloat(mm->position);

			if (m_timelineDragging)
				m_timeline.UpdateTime(mousePos.x, m_state);
			else
			{
				const auto world = ToCanvas(mm->position);
				m_controller.OnMouseMoved(world.x, world.y);
				m_listView.HandleMouseMoved(mousePos, m_picture, m_state);
				m_canvasView.UpdateCursor(world);
			}
		}
		else if (auto* mr = event->getIf<sf::Event::MouseButtonReleased>())
		{
			if (mr->button == sf::Mouse::Button::Left)
			{
				sf::Vector2f mousePos = toFloat(mr->position);

				if (m_timelineDragging)
				{
					if (!m_timelineShapeId.empty())
					{
						const int index = m_timeline.ShapeRowAt(mousePos, m_picture, m_state);
						if (index >= 0) m_controller.ReorderShape(m_timelineShapeId, index);
					}
					m_timelineDragging = false;
					m_timelineShapeId.clear();
				}
				else
				{
					m_listView.HandleMouseReleased(m_picture, m_state, m_controller);
					const auto world = ToCanvas(mr->position);
					m_controller.OnMouseReleased(world.x, world.y);
				}
			}
		}
		else if (auto* kp = event->getIf<sf::Event::KeyPressed>())
		{
			HandleKeyPressed(kp);
		}
		else if (auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>())
		{
			sf::Vector2f mousePos = toFloat(scroll->position);

			const bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
				|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
			const bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)
				|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
			if (ctrl)
			{
				double& zoom = shift ? m_state.timelineZoom : m_state.canvasZoom;
				zoom = std::clamp(zoom * std::pow(1.1, scroll->delta), 0.5, 4.0);
			}
			else
			{
				int shapeCount = static_cast<int>(m_picture.GetShapes().size());
				m_listView.HandleScroll(scroll->delta, mousePos, shapeCount);
			}
		}
	}
}

sf::View EditorView::CanvasCamera() const
{
    // Canvas is clipped to its own viewport; UI and timeline keep screen pixels.
    sf::View view;
    view.setCenter({400.f, 320.f});
    view.setSize({static_cast<float>(800.0/m_state.canvasZoom),
        static_cast<float>(560.0/m_state.canvasZoom)});
    view.setViewport(sf::FloatRect({0.f,40.f/600.f}, {0.8f,430.f/600.f}));
    return view;
}

sf::Vector2f EditorView::ToCanvas(sf::Vector2i pixel) const
{
    return m_window.mapPixelToCoords(pixel, CanvasCamera());
}

void EditorView::Render()
{
	m_window.clear(sf::Color::White);

	m_window.setView(CanvasCamera());
	m_canvasView.Draw();
	m_window.setView(m_window.getDefaultView());
	DrawToolbar();
	m_listView.Draw(m_window, m_picture, m_state);
	m_timeline.Draw(m_window, m_picture, m_state, m_font);

	m_window.display();
}

void EditorView::DrawToolbar()
{
	const Tool tools[] = {
		Tool::DrawRect,
		Tool::DrawEllipse,
		Tool::DrawTriangle,
		Tool::DrawPentagon,
		Tool::DrawStar
	};
	const size_t toolCount = 5;

	for (size_t i = 0; i < m_buttons.size(); ++i)
	{
		bool active = false;
		if (i < toolCount)
		{
			active = (m_state.activeTool == tools[i]);
		}
		m_buttons[i].SetActive(active);
		m_buttons[i].Draw(m_window);
	}
}

bool EditorView::WantsSharedWindow() const
{
	return m_wantsSharedWindow;
}

bool EditorView::WantsNewWindow() const
{
	return m_wantsNewWindow;
}

bool EditorView::WantsOpenWindow() const
{
	return m_wantsOpenWindow;
}

std::string EditorView::GetOpenPath() const
{
	return m_openPath;
}

void EditorView::ClearRequests()
{
	m_wantsNewWindow = false;
	m_wantsSharedWindow = false;
	m_wantsOpenWindow = false;
}