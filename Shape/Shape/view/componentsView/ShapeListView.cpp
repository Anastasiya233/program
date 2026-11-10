#include "ShapeListView.h"
#include "../../visitor/SfmlRenderer.h"

ShapeListView::ShapeListView(float x, float y, float width, float height, const sf::Font& font)
	: m_x(x)
	, m_y(y)
	, m_width(width)
	, m_height(height)
	, m_font(font)
{
}

void ShapeListView::Draw(sf::RenderWindow& window, const Picture& picture, const EditorState& state)
{
	DrawPanel(window);

	auto shapes = picture.GetShapes();
	int count = static_cast<int>(shapes.size());

	for (int i = 0; i < count; ++i)
	{
		int listIndex = count - 1 - i;
		if (state.draggedListIndex == listIndex)
		{
			continue;
		}

		const Shape* shape = shapes[listIndex];
		float rowY = m_y + i * m_rowHeight - m_scrollOffset;

		if (!IsRowVisible(rowY))
		{
			continue;
		}

		bool isSelected = state.IsSelected(shape->GetId());
		DrawRow(window, *shape, rowY, isSelected);
	}

	DrawDragOverlay(window, picture, state);
}

void ShapeListView::DrawPanel(sf::RenderWindow& window) const
{
	sf::RectangleShape panel(sf::Vector2f(m_width, m_height));
	panel.setPosition(sf::Vector2f(m_x, m_y));
	panel.setFillColor(sf::Color(240, 240, 240));
	window.draw(panel);
}

void ShapeListView::DrawRow(sf::RenderWindow& window, const Shape& shape, float rowY, bool isSelected)
{
	const float m_padding = 10.f;
	const float m_halfPadding = 5.f;
	const sf::Color m_selectedColor = sf::Color(180, 200, 255);
	const sf::Color m_normalColor = sf::Color::White;

	sf::RectangleShape row(sf::Vector2f(m_width - m_padding, m_rowHeight));
	row.setPosition(sf::Vector2f(m_x + m_halfPadding, rowY));
	row.setFillColor(isSelected ? m_selectedColor : m_normalColor);
	window.draw(row);

	float iconX = m_x + m_padding;
	float iconY = rowY + (m_rowHeight - m_iconSize) / 2.f;
	DrawIcon(window, shape, iconX, iconY);

	sf::Text text(m_font);
	text.setCharacterSize(14);
	text.setFillColor(sf::Color::Black);
	text.setString(shape.GetTypeName() + " (" + shape.GetId() + ")");
	text.setPosition(sf::Vector2f(iconX + m_iconSize + m_halfPadding, rowY + m_halfPadding));
	window.draw(text);
}

void ShapeListView::DrawIcon(sf::RenderWindow& window, const Shape& shape, float iconX, float iconY)
{
	Bounds bounds = shape.GetBounds();

	sf::RenderTexture renderTexture;
	if (!renderTexture.resize({ 24, 24 }))
	{
		return;
	}

	sf::View view;
	view.setCenter({ static_cast<float>(bounds.left + bounds.width / 2.0),
		static_cast<float>(bounds.top + bounds.height / 2.0) });
	view.setSize({ static_cast<float>(bounds.width), static_cast<float>(bounds.height) });
	renderTexture.setView(view);

	renderTexture.clear(sf::Color::White);

	SfmlRenderer renderer(renderTexture);
	shape.Accept(renderer);

	renderTexture.display();

	sf::Sprite sprite(renderTexture.getTexture());
	sprite.setScale({ m_iconSize / 24.f, m_iconSize / 24.f });
	sprite.setPosition({ iconX, iconY });
	window.draw(sprite);
}

void ShapeListView::DrawDragOverlay(sf::RenderWindow& window, const Picture& picture, const EditorState& state)
{
	if (state.draggedListIndex < 0)
	{
		return;
	}

	auto shapes = picture.GetShapes();
	int count = shapes.size();

	if (state.dropListIndex >= 0 && state.draggedListIndex != state.dropListIndex)
	{
		float insertY = m_y + (count - 1 - state.dropListIndex) * m_rowHeight - m_scrollOffset;
		if (insertY >= m_y && insertY <= m_y + m_height)
		{
			sf::RectangleShape line(sf::Vector2f(m_width - 20.f, 2.f));
			line.setPosition(sf::Vector2f(m_x + 10.f, insertY));
			line.setFillColor(sf::Color::Blue);
			window.draw(line);
		}
	}

	if (m_dragMousePos.x >= m_x && m_dragMousePos.x <= m_x + m_width && m_dragMousePos.y >= m_y && m_dragMousePos.y <= m_y + m_height)
	{
		float ghostY = m_dragMousePos.y - m_rowHeight / 2.f;

		if (state.draggedListIndex < count)
		{
			const Shape* shape = shapes[state.draggedListIndex];
			DrawRow(window, *shape, ghostY, false);
		}
	}
}

bool ShapeListView::IsRowVisible(float rowY) const
{
	return !(rowY + m_rowHeight < m_y || rowY > m_y + m_height);
}

bool ShapeListView::HandleClick(sf::Vector2f mousePos, Picture& picture, EditorState& state)
{
	if (mousePos.x < m_x || mousePos.x > m_x + m_width
		|| mousePos.y < m_y || mousePos.y > m_y + m_height)
	{
		return false;
	}

	float relativeY = mousePos.y - m_y + m_scrollOffset;
	int rowIndex = static_cast<int>(relativeY / m_rowHeight);

	auto shapes = picture.GetShapes();
	int cnt = shapes.size();
	int listIndex = cnt - 1 - rowIndex;

	if (listIndex >= 0 && listIndex < cnt)
	{
		state.ClearSelection();
		state.SelectedIds.push_back(shapes[listIndex]->GetId());
		state.draggedListIndex = listIndex;
		state.dropListIndex = listIndex;
		m_dragMousePos = mousePos;
	}
	return true;
}

void ShapeListView::HandleMouseMoved(sf::Vector2f mousePos, const Picture& picture, EditorState& state)
{
	bool isOverPanel = (mousePos.x >= m_x && mousePos.x <= m_x + m_width);
	if (state.draggedListIndex < 0 || !isOverPanel)
	{
		return;
	}

	m_dragMousePos = mousePos;

	int cntShapes = picture.GetShapes().size();
	const float totalHeight = cntShapes * m_rowHeight;
	const float maxScroll = std::max(0.f, totalHeight - m_height);
	const float m_scrollSpeed = 5.f;
	const float m_scrollZone = 20.f;

	if (mousePos.y > m_y + m_height - m_scrollZone)
	{
		m_scrollOffset += m_scrollSpeed;
	}
	else if (mousePos.y < m_y + m_scrollZone)
	{
		m_scrollOffset -= m_scrollSpeed;
	}

	if (m_scrollOffset < 0.f)
	{
		m_scrollOffset = 0.f;
	}
	if (m_scrollOffset > maxScroll)
	{
		m_scrollOffset = maxScroll;
	}

	float clampedY = std::clamp(mousePos.y, m_y, m_y + m_height);
	float relativeY = clampedY - m_y + m_scrollOffset;
	int rowIndex = static_cast<int>(relativeY / m_rowHeight);

	auto shapes = picture.GetShapes();
	int listIndex = cntShapes - 1 - rowIndex;

	if (listIndex >= 0 && listIndex < cntShapes)
	{
		state.dropListIndex = listIndex;
	}
}

void ShapeListView::HandleMouseReleased(Picture& picture, EditorState& state, EditorController& controller)
{
	if (state.draggedListIndex != -1 && state.dropListIndex != -1)
	{
		auto shapes = picture.GetShapes();
		if (state.draggedListIndex < static_cast<int>(shapes.size()))
		{
			std::string id = shapes[state.draggedListIndex]->GetId();
			controller.ReorderShape(id, state.dropListIndex);
		}
	}
	state.draggedListIndex = -1;
	state.dropListIndex = -1;
}

bool ShapeListView::HandleScroll(float delta, sf::Vector2f mousePos, int shapeCount)
{
	if (mousePos.x < m_x || mousePos.x > m_x + m_width
		|| mousePos.y < m_y || mousePos.y > m_y + m_height)
	{
		return false;
	}

	float totalHeight = shapeCount * m_rowHeight;
	float maxScroll = std::max(0.f, totalHeight - m_height);

	m_scrollOffset -= delta * m_rowHeight;

	if (m_scrollOffset < 0.f)
	{
		m_scrollOffset = 0.f;
	}
	if (m_scrollOffset > maxScroll)
	{
		m_scrollOffset = maxScroll;
	}
	return true;
}