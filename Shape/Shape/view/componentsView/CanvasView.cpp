#include "CanvasView.h"
#include "../../visitor/SfmlRenderer.h"

CanvasView::CanvasView(sf::RenderWindow& window, const sf::Font& font,
	Picture& picture, EditorState& state, EditorController& controller)
	: m_window(window)
	, m_font(font)
	, m_picture(picture)
	, m_state(state)
	, m_controller(controller)
{
}

void CanvasView::Draw()
{
	DrawCanvas();
	DrawDraft();
	DrawGhost();
	DrawSelectionBox();
}

void CanvasView::DrawCanvas()
{
	SfmlRenderer renderer(m_window);
	for (const auto* shape : m_picture.GetShapes())
	{
		shape->Accept(renderer);
	}
}

void CanvasView::DrawDraft()
{
	if (m_state.draft.isDrafting && m_controller.GetDraftShape())
	{
		SfmlRenderer renderer(m_window);
		m_controller.GetDraftShape()->Accept(renderer);
	}
}

void CanvasView::DrawGhost()
{
	SfmlRenderer renderer(m_window);

	if (m_state.preview.hasMovePreview)
	{
		for (const auto& id : m_state.SelectedIds)
		{
			auto* shape = m_picture.GetShape(id);
			if (!shape)
			{
				continue;
			}

			auto ghost = shape->Clone(shape->GetId() + "_ghost");
			ghost->Move(m_state.preview.movePreviewDx, m_state.preview.movePreviewDy);
			ghost->Accept(renderer);
		}
	}
	else if (m_state.preview.hasResizePreview)
	{
		for (const auto& [id, bounds] : m_state.preview.resizePreviewBounds)
		{
			auto* shape = m_picture.GetShape(id);
			if (!shape)
				continue;

			auto ghost = shape->Clone(shape->GetId() + "_ghost");
			ghost->SetBounds(bounds);
			ghost->Accept(renderer);
		}
	}
}

void CanvasView::DrawSelectionBox()
{
	if (m_state.SelectedIds.empty())
	{
		return;
	}

	for (const auto& id : m_state.SelectedIds)
	{
		auto* selectedShape = m_picture.GetShape(id);
		if (!selectedShape)
		{
			continue;
		}

		Bounds bounds = selectedShape->GetBounds();

		if (m_state.preview.hasResizePreview)
		{
			auto it = m_state.preview.resizePreviewBounds.find(id);
			if (it != m_state.preview.resizePreviewBounds.end())
			{
				bounds = it->second;
			}
		}
		else if (m_state.preview.hasMovePreview)
		{
			bounds.left += m_state.preview.movePreviewDx;
			bounds.top += m_state.preview.movePreviewDy;
		}

		const float sizeOutline = 3.0f;
		bounds.height -= 2 * sizeOutline;
		bounds.left += sizeOutline;
		bounds.width -= 2 * sizeOutline;
		bounds.top += sizeOutline;

		sf::RectangleShape rect(sf::Vector2f(
			static_cast<float>(bounds.width),
			static_cast<float>(bounds.height)));
		rect.setPosition(sf::Vector2f(
			static_cast<float>(bounds.left),
			static_cast<float>(bounds.top)));
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineColor(sf::Color::Black);
		rect.setOutlineThickness(sizeOutline);
		m_window.draw(rect);

		float size = 8.0f;
		float half = size / 2.0f;
		sf::RectangleShape marker(sf::Vector2f(size, size));
		marker.setFillColor(sf::Color::White);
		marker.setOutlineColor(sf::Color::Black);
		marker.setOutlineThickness(1.0f);
		marker.setPosition(sf::Vector2f(
			static_cast<float>(bounds.left) - half,
			static_cast<float>(bounds.top) - half));

		float minX = 0.f;
		float maxX = static_cast<float>(m_state.GetUsableCanvasWidth()) - size;
		float minY = static_cast<float>(m_state.toolbarHeight);
		float maxY = static_cast<float>(m_state.canvasHeight) - size;

		auto clampMarker = [&](float x, float y) {
			float cx = std::max(minX, std::min(x, maxX));
			float cy = std::max(minY, std::min(y, maxY));
			marker.setPosition({ cx, cy });
			m_window.draw(marker);
		};

		clampMarker(static_cast<float>(bounds.left) - half, static_cast<float>(bounds.top) - half);
		clampMarker(static_cast<float>(bounds.Right()) - half, static_cast<float>(bounds.top) - half);
		clampMarker(static_cast<float>(bounds.Right()) - half, static_cast<float>(bounds.Bottom()) - half);
		clampMarker(static_cast<float>(bounds.left) - half, static_cast<float>(bounds.Bottom()) - half);
	}
}

void CanvasView::UpdateCursor(sf::Vector2f mousePos)
{
	sf::Cursor::Type cursorType = sf::Cursor::Type::Arrow;

	if (!m_state.SelectedIds.empty())
	{
		auto* shape = m_picture.GetShape(m_state.SelectedIds[0]);
		if (shape)
		{
			Bounds b = shape->GetBounds();
			const float tol = 6.0f;

			auto near = [&](float hx, float hy) {
				return std::abs(mousePos.x - hx) <= tol && std::abs(mousePos.y - hy) <= tol;
			};

			if (near(static_cast<float>(b.left), static_cast<float>(b.top)))
			{
				cursorType = sf::Cursor::Type::SizeTopLeft;
			}
			else if (near(static_cast<float>(b.Right()), static_cast<float>(b.Bottom())))
			{
				cursorType = sf::Cursor::Type::SizeBottomRight;
			}
			else if (near(static_cast<float>(b.Right()), static_cast<float>(b.top)))
			{
				cursorType = sf::Cursor::Type::SizeTopRight;
			}
			else if (near(static_cast<float>(b.left), static_cast<float>(b.Bottom())))
			{
				cursorType = sf::Cursor::Type::SizeBottomLeft;
			}
		}
	}

	m_window.setMouseCursor(sf::Cursor(cursorType));
}