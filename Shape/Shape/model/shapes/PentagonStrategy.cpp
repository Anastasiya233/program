#include "PentagonStrategy.h"
#include <cmath>
#include <sstream>
#include <stdexcept>

PentagonStrategy::PentagonStrategy(double left, double top, double width, double height)
	: m_left(left)
	, m_top(top)
	, m_width(width)
	, m_height(height)
{
	if (width < 0.0 || height < 0.0)
		throw std::invalid_argument("Pentagon width and height must be non-negative");
}

void PentagonStrategy::Move(double dx, double dy)
{
	m_left += dx;
	m_top += dy;
}

std::string PentagonStrategy::GetTypeName() const
{
	return "pentagon";
}

std::string PentagonStrategy::GetParamsToString() const
{
	std::ostringstream os;
	os << m_left << ' ' << m_top << ' ' << m_width << ' ' << m_height;
	return os.str();
}

std::array<std::pair<double, double>, 5> PentagonStrategy::GetVertices() const
{
	const double PI = 3.14159265358979323846;
	std::array<std::pair<double, double>, 5> v;
	double cx = m_left + m_width / 2.0;
	double cy = m_top + m_height / 2.0;

	double rx = m_width / 2.0;
	double ry = m_height / 2.0;

	for (int i = 0; i < 5; ++i)
	{
		double angle = -PI / 2.0 + i * 2.0 * PI / 5.0;
		v[i] = { cx + rx * std::cos(angle), cy + ry * std::sin(angle) };
	}
	return v;
}

bool PentagonStrategy::HitTest(double x, double y) const
{
	auto v = GetVertices();
	bool hasNeg = false, hasPos = false;

	for (int i = 0; i < 5; ++i)
	{
		int j = (i + 1) % 5;
		double cross = (v[j].first - v[i].first) * (y - v[i].second)
			- (v[j].second - v[i].second) * (x - v[i].first);

		if (cross < 0)
			hasNeg = true;
		if (cross > 0)
			hasPos = true;
	}

	return !(hasNeg && hasPos);
}

Bounds PentagonStrategy::GetBounds() const
{
	return { m_left, m_top, m_width, m_height };
}

void PentagonStrategy::SetBounds(const Bounds& b)
{
	m_left = b.left;
	m_top = b.top;
	m_width = b.width;
	m_height = b.height;
}

void PentagonStrategy::Accept(const Shape& shape, IShapeVisitor& visitor) const
{
	visitor.Visit(shape, *this);
}