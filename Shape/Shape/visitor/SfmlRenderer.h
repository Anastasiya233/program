#pragma once
#include "../model/Shape.h"
#include "../model/shapes/EllipseStrategy.h"
#include "../model/shapes/PentagonStrategy.h"
#include "../model/shapes/RectangleStrategy.h"
#include "../model/shapes/StarStrategy.h"
#include "../model/shapes/TriangleStrategy.h"
#include "../visitor/IShapeVisitor.h"
#include <SFML/Graphics.hpp>

class Shape;
class RectangleStrategy;
class EllipseStrategy;
class TriangleStrategy;
class PentagonStrategy;
class StarStrategy;

class SfmlRenderer : public IShapeVisitor
{
public:
	explicit SfmlRenderer(sf::RenderTarget& target);

	void Visit(const Shape& shape, const RectangleStrategy& strategy) override;
	void Visit(const Shape& shape, const EllipseStrategy& strategy) override;
	void Visit(const Shape& shape, const TriangleStrategy& strategy) override;
	void Visit(const Shape& shape, const PentagonStrategy& strategy) override;
	void Visit(const Shape& shape, const StarStrategy& strategy) override;

private:
	sf::RenderTarget& m_target;
};