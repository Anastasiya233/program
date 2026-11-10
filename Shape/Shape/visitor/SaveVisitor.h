#pragma once
#include "../model/Shape.h"
#include "../model/shapes/EllipseStrategy.h"
#include "../model/shapes/PentagonStrategy.h"
#include "../model/shapes/RectangleStrategy.h"
#include "../model/shapes/StarStrategy.h"
#include "../model/shapes/TriangleStrategy.h"
#include "../visitor/IShapeVisitor.h"
#include <ostream>

class SaveVisitor : public IShapeVisitor
{
public:
	explicit SaveVisitor(std::ostream& out);

	void Visit(const Shape& shape, const RectangleStrategy& strategy) override;
	void Visit(const Shape& shape, const EllipseStrategy& strategy) override;
	void Visit(const Shape& shape, const TriangleStrategy& strategy) override;
	void Visit(const Shape& shape, const PentagonStrategy& strategy) override;
	void Visit(const Shape& shape, const StarStrategy& strategy) override;

private:
	std::ostream& m_out;
};