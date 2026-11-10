#pragma once

#include "../../visitor/IShapeVisitor.h"
#include "../Bounds.h"
#include <memory>
#include <string>

class IShapeStrategy
{
public:
	virtual ~IShapeStrategy() = default;

	virtual bool HitTest(double x, double y) const = 0;
	virtual Bounds GetBounds() const = 0;
	virtual void SetBounds(const Bounds& b) = 0;
	virtual void Move(double dx, double dy) = 0;
	virtual std::unique_ptr<IShapeStrategy> Clone() const = 0;
	virtual std::string GetTypeName() const = 0;
	virtual std::string GetParamsToString() const = 0;
	virtual void Accept(const Shape& shape, IShapeVisitor& visitor) const = 0;
};