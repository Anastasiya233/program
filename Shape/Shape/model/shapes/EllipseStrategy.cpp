#include "EllipseStrategy.h"

EllipseStrategy::EllipseStrategy(double cx, double cy, double rx, double ry)
	: m_centerX(cx)
	, m_centerY(cy)
	, m_radiusX(rx)
	, m_radiusY(ry)
{
}

void EllipseStrategy::Move(double dx, double dy)
{
	m_centerX += dx;
	m_centerY += dy;
}

std::string EllipseStrategy::GetTypeName() const
{
	return "circle";
}

std::string EllipseStrategy::GetParamsToString() const
{
	std::ostringstream os;
	os << m_centerX << ' ' << m_centerY << ' ' << m_radiusX << ' ' << m_radiusY;
	return os.str();
}

bool EllipseStrategy::HitTest(double x, double y) const
{
	if (m_radiusX <= 0.0 || m_radiusY <= 0.0)
	{
		return false;
	}
	double dx = (x - m_centerX) / m_radiusX;
	double dy = (y - m_centerY) / m_radiusY;
	return dx * dx + dy * dy <= 1.0;
}

Bounds EllipseStrategy::GetBounds() const
{
	return { m_centerX - m_radiusX, m_centerY - m_radiusY, 2.0 * m_radiusX, 2.0 * m_radiusY };
}

void EllipseStrategy::SetBounds(const Bounds& b)
{
	m_centerX = b.left + b.width / 2.0;
	m_centerY = b.top + b.height / 2.0;
	m_radiusX = b.width / 2.0;
	m_radiusY = b.height / 2.0;
}

void EllipseStrategy::Accept(const Shape& shape, IShapeVisitor& visitor) const
{
	visitor.Visit(shape, *this);
}
