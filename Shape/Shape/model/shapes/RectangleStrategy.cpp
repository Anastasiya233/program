#include "RectangleStrategy.h"
#include <sstream>
#include <stdexcept>

RectangleStrategy::RectangleStrategy(double left, double top, double width, double height)
	: m_left(left)
	, m_top(top)
	, m_width(width)
	, m_height(height)
{
	if (width < 0.0 || height < 0.0)
	{
		throw std::invalid_argument("Rectangle width and height must be non-negative");
	}
}

void RectangleStrategy::Move(double dx, double dy)
{
	m_left += dx;
	m_top += dy;
}

std::string RectangleStrategy::GetTypeName() const
{
	return "rectangle";
}

std::string RectangleStrategy::GetParamsToString() const
{
	std::ostringstream os;
	os << m_left << ' ' << m_top << ' '
	   << m_width << ' ' << m_height;
	return os.str();
}

bool RectangleStrategy::HitTest(double x, double y) const
{
	return x >= m_left && x <= m_left + m_width && y >= m_top && y <= m_top + m_height;
}

Bounds RectangleStrategy::GetBounds() const
{
	return { m_left, m_top, m_width, m_height };
}

void RectangleStrategy::SetBounds(const Bounds& b)
{
	m_left = b.left;
	m_top = b.top;
	m_width = b.width;
	m_height = b.height;
}

void RectangleStrategy::Accept(const Shape& shape, IShapeVisitor& visitor) const
{
	visitor.Visit(shape, *this);
}