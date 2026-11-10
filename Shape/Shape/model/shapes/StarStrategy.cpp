#include "StarStrategy.h"
#include <cmath>
#include <sstream>

StarStrategy::StarStrategy(double left, double top, double width, double height)
	: m_left(left)
	, m_top(top)
	, m_width(width)
	, m_height(height)
{
	if (width < 0.0 || height < 0.0)
		throw std::invalid_argument("Star width and height must be non-negative");
}

void StarStrategy::Move(double dx, double dy)
{
	m_left += dx;
	m_top += dy;
}

std::string StarStrategy::GetTypeName() const
{
	return "star";
}

std::string StarStrategy::GetParamsToString() const
{
	std::ostringstream os;
	os << m_left << ' ' << m_top << ' ' << m_width << ' ' << m_height;
	return os.str();
}

bool StarStrategy::HitTest(double x, double y) const
{
	auto v = GetVertices();
	bool inside = false;

	for (int i = 0, j = 9; i < 10; j = i++)
	{
		double xi = v[i].first, yi = v[i].second;
		double xj = v[j].first, yj = v[j].second;

		bool intersect = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi);

		if (intersect)
		{
			inside = !inside;
		}
	}
	return inside;
}

std::array<std::pair<double, double>, 10> StarStrategy::GetVertices() const
{
	const double PI = 3.14159265358979323846;
	std::array<std::pair<double, double>, 10> v;
	double cx = m_left + m_width / 2.0;
	double cy = m_top + m_height / 2.0;

	double rxOuter = m_width / 2.0;
	double ryOuter = m_height / 2.0;
	double rxInner = rxOuter * 0.4;
	double ryInner = ryOuter * 0.4;

	for (int i = 0; i < 10; ++i)
	{
		double angle = -PI / 2.0 + i * PI / 5.0;
		double rx = (i % 2 == 0) ? rxOuter : rxInner;
		double ry = (i % 2 == 0) ? ryOuter : ryInner;
		v[i] = { cx + rx * std::cos(angle), cy + ry * std::sin(angle) };
	}
	return v;
}

Bounds StarStrategy::GetBounds() const
{
	return { m_left, m_top, m_width, m_height };
}

void StarStrategy::SetBounds(const Bounds& b)
{
	m_left = b.left;
	m_top = b.top;
	m_width = b.width;
	m_height = b.height;
}

void StarStrategy::Accept(const Shape& shape, IShapeVisitor& visitor) const
{
	visitor.Visit(shape, *this);
}