#pragma once
#include "../../model/Color.h"
#include "../../model/shapes/PrototypeImpl.h"
#include "IShapeStrategy.h"
#include <cmath>
#include <sstream>
#include <stdexcept>

class EllipseStrategy : public PrototypeImpl<EllipseStrategy, IShapeStrategy>
{
public:
	EllipseStrategy(double x, double y, double rx, double ry);
	void Move(double dx, double dy) override;
	std::string GetTypeName() const override;
	std::string GetParamsToString() const override;
	bool HitTest(double x, double y) const;
	Bounds GetBounds() const;
	void SetBounds(const Bounds& b);
	void Accept(const Shape& shape, IShapeVisitor& visitor) const override;

private:
	double m_centerX, m_centerY, m_radiusX, m_radiusY;
};