#pragma once
#include "../../model/shapes/PrototypeImpl.h"
#include "IShapeStrategy.h"

class TriangleStrategy : public PrototypeImpl<TriangleStrategy, IShapeStrategy>
{
public:
	TriangleStrategy(double x1, double y1, double x2, double y2, double x3, double y3);
	void Move(double dx, double dy) override;
	std::string GetTypeName() const override;
	std::string GetParamsToString() const override;
	bool HitTest(double x, double y) const;
	Bounds GetBounds() const;
	void SetBounds(const Bounds& b);
	void Accept(const Shape& shape, IShapeVisitor& visitor) const override;

private:
	double m_x1, m_y1, m_x2, m_y2, m_x3, m_y3;
};