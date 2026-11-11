#include "SfmlRenderer.h"

void SfmlRenderer::Visit(const Shape&, const GroupStrategy& strategy)
{
	for (const Shape* child : strategy.GetChildren()) child->Accept(*this);
}

SfmlRenderer::SfmlRenderer(sf::RenderTarget& target)
	: m_target(target)
{
}

void SfmlRenderer::Visit(const Shape& shape, const RectangleStrategy& strategy)
{
	const Bounds b = strategy.GetBounds();
	const Color c = shape.GetColor();

	sf::RectangleShape rect({ static_cast<float>(b.width), static_cast<float>(b.height) });
	rect.setPosition({ static_cast<float>(b.left), static_cast<float>(b.top) });
	rect.setFillColor(sf::Color(c.GetR(), c.GetG(), c.GetB()));
	m_target.draw(rect);
}

void SfmlRenderer::Visit(const Shape& shape, const EllipseStrategy& strategy)
{
	const Bounds b = strategy.GetBounds();
	const Color c = shape.GetColor();
	const float rx = static_cast<float>(b.width / 2.0);
	const float ry = static_cast<float>(b.height / 2.0);

	sf::CircleShape ellipse(1.f, 64);
	ellipse.setScale({ rx, ry });
	ellipse.setPosition({ static_cast<float>(b.left), static_cast<float>(b.top) });
	ellipse.setFillColor(sf::Color(c.GetR(), c.GetG(), c.GetB()));
	m_target.draw(ellipse);
}

void SfmlRenderer::Visit(const Shape& shape, const TriangleStrategy& strategy)
{
	const Bounds b = strategy.GetBounds();
	const Color c = shape.GetColor();

	sf::ConvexShape triangle(3);
	triangle.setPoint(0, { static_cast<float>(b.left), static_cast<float>(b.top + b.height) });
	triangle.setPoint(1, { static_cast<float>(b.left + b.width), static_cast<float>(b.top + b.height) });
	triangle.setPoint(2, { static_cast<float>(b.left + b.width / 2.0), static_cast<float>(b.top) });
	triangle.setFillColor(sf::Color(c.GetR(), c.GetG(), c.GetB()));
	m_target.draw(triangle);
}

void SfmlRenderer::Visit(const Shape& shape, const PentagonStrategy& strategy)
{
	auto v = strategy.GetVertices();
	const Color c = shape.GetColor();

	sf::ConvexShape pentagon(5);
	for (int i = 0; i < 5; ++i)
		pentagon.setPoint(i, { static_cast<float>(v[i].first), static_cast<float>(v[i].second) });
	pentagon.setFillColor(sf::Color(c.GetR(), c.GetG(), c.GetB()));
	m_target.draw(pentagon);
}

void SfmlRenderer::Visit(const Shape& shape, const StarStrategy& strategy)
{
	auto v = strategy.GetVertices();
	const Color c = shape.GetColor();
	sf::Color sfColor(c.GetR(), c.GetG(), c.GetB());

	sf::VertexArray triangles(sf::PrimitiveType::Triangles);

	auto b = strategy.GetBounds();
	float cx = static_cast<float>(b.left + b.width / 2.0);
	float cy = static_cast<float>(b.top + b.height / 2.0);

	for (int i = 0; i < 10; ++i)
	{
		int j = (i + 1) % 10;
		triangles.append(sf::Vertex({ cx, cy }, sfColor));
		triangles.append(sf::Vertex({ static_cast<float>(v[i].first), static_cast<float>(v[i].second) }, sfColor));
		triangles.append(sf::Vertex({ static_cast<float>(v[j].first), static_cast<float>(v[j].second) }, sfColor));
	}
	m_target.draw(triangles);
}