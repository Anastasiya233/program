#include "Shape.h"
#include <utility>

Shape::Shape(std::string id, Color color, std::unique_ptr<IShapeStrategy> strategy)
	: m_id(id)
	, m_color(color)
	, m_strategy(std::move(strategy))
{
}

void Shape::Move(double dx, double dy)
{
	if (dx == 0.0 && dy == 0.0)
	{
		return;
	}
	m_strategy->Move(dx, dy);
	m_moved(*this, dx, dy);
}

std::unique_ptr<Shape> Shape::Clone(const std::string& newId) const
{
	return std::make_unique<Shape>(newId, m_color, m_strategy->Clone());
}

void Shape::SetColor(const Color& color)
{
	if (m_color.GetHexStr() == color.GetHexStr())
	{
		return;
	}
	m_color = color;
	m_colorChanged(*this, color);
}

void Shape::SetStrategy(std::unique_ptr<IShapeStrategy> strategy)
{
	if (!strategy)
	{
		return;
	}
	m_strategy = std::move(strategy);
	m_paramsChanged(*this);
}

const std::string& Shape::GetId() const
{
	return m_id;
}

const Color& Shape::GetColor() const
{
	return m_color;
}

std::string Shape::GetTypeName() const
{
	return m_strategy->GetTypeName();
}

std::string Shape::GetParamsToString() const
{
	return m_strategy->GetParamsToString();
}

boost::signals2::scoped_connection Shape::SubscribeToMoved(std::function<void(const Shape&, double, double)> handler)
{
	return m_moved.connect(std::move(handler));
}

boost::signals2::scoped_connection Shape::SubscribeToColorChanged(std::function<void(const Shape&, const Color&)> handler)
{
	return m_colorChanged.connect(std::move(handler));
}

boost::signals2::scoped_connection Shape::SubscribeToParamsChanged(std::function<void(const Shape&)> handler)
{
	return m_paramsChanged.connect(std::move(handler));
}

bool Shape::HitTest(double x, double y) const
{
	return m_strategy->HitTest(x, y);
}

Bounds Shape::GetBounds() const
{
	return m_strategy->GetBounds();
}

void Shape::SetBounds(const Bounds& b)
{
	m_strategy->SetBounds(b);
	m_paramsChanged(*this);
}

void Shape::Accept(IShapeVisitor& visitor) const
{
	m_strategy->Accept(*this, visitor);
}