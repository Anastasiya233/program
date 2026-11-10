
#include "TriangleStrategy.h"
#include <sstream>

TriangleStrategy::TriangleStrategy(double x1, double y1, double x2, double y2, double x3, double y3)
	: m_x1(x1)
	, m_y1(y1)
	, m_x2(x2)
	, m_y2(y2)
	, m_x3(x3)
	, m_y3(y3)
{
}

void TriangleStrategy::Move(double dx, double dy)
{
	m_x1 += dx;
	m_y1 += dy;
	m_x2 += dx;
	m_y2 += dy;
	m_x3 += dx;
	m_y3 += dy;
}

std::string TriangleStrategy::GetTypeName() const
{
	return "triangle";
}

std::string TriangleStrategy::GetParamsToString() const
{
	std::ostringstream os;
	os << m_x1 << ' ' << m_y1 << ' '
	   << m_x2 << ' ' << m_y2 << ' '
	   << m_x3 << ' ' << m_y3;
	return os.str();
}

bool TriangleStrategy::HitTest(double x, double y) const
{
	auto positionPoint = [](double ax, double ay, double bx, double by,
							 double cx, double cy) {
		return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
	};
	double d1 = positionPoint(m_x1, m_y1, m_x2, m_y2, x, y);
	double d2 = positionPoint(m_x2, m_y2, m_x3, m_y3, x, y);
	double d3 = positionPoint(m_x3, m_y3, m_x1, m_y1, x, y);

	bool hasNeg = (d1 < 0) && (d2 < 0) && (d3 < 0);
	bool hasPos = (d1 > 0) && (d2 > 0) && (d3 > 0);
	return hasNeg || hasPos;
}

Bounds TriangleStrategy::GetBounds() const
{
	double l = std::min({ m_x1, m_x2, m_x3 });
	double t = std::min({ m_y1, m_y2, m_y3 });
	double r = std::max({ m_x1, m_x2, m_x3 });
	double b = std::max({ m_y1, m_y2, m_y3 });
	return { l, t, r - l, b - t };
}

void TriangleStrategy::SetBounds(const Bounds& newBounds)
{
	Bounds oldBound = GetBounds();
	if (oldBound.width <= 0.0 || oldBound.height <= 0.0)
	{
		return;
	}
	double ratioX = newBounds.width / oldBound.width;
	double ratioY = newBounds.height / oldBound.height;
	auto newCoordinates = [&](double& x, double& y) {
		x = newBounds.left + (x - oldBound.left) * ratioX;
		y = newBounds.top + (y - oldBound.top) * ratioY;
	};
	newCoordinates(m_x1, m_y1);
	newCoordinates(m_x2, m_y2);
	newCoordinates(m_x3, m_y3);
}

void TriangleStrategy::Accept(const Shape& shape, IShapeVisitor& visitor) const
{
	visitor.Visit(shape, *this);
}