#pragma once
#include "../../model/Color.h"
#include "../../model/shapes/PrototypeImpl.h"
#include "IShapeStrategy.h"
#include <array>
#include <utility>

class PentagonStrategy : public PrototypeImpl<PentagonStrategy, IShapeStrategy>
{
public:
	PentagonStrategy(double left, double top, double width, double height);
	void Move(double dx, double dy) override;
	std::string GetTypeName() const override;
	std::string GetParamsToString() const override;
	bool HitTest(double x, double y) const;
	Bounds GetBounds() const;
	void SetBounds(const Bounds& b);
	void Accept(const Shape& shape, IShapeVisitor& visitor) const override;

	std::array<std::pair<double, double>, 5> GetVertices() const;

private:
	double m_left, m_top, m_width, m_height;
};