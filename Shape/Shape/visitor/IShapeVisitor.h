#pragma once

class Shape;
class RectangleStrategy;
class EllipseStrategy;
class TriangleStrategy;
class PentagonStrategy;
class StarStrategy;
class GroupStrategy;

//TODO::   

class IShapeVisitor
{
public:
	virtual ~IShapeVisitor() = default;

	virtual void Visit(const Shape& shape, const RectangleStrategy& strategy) = 0;
	virtual void Visit(const Shape& shape, const EllipseStrategy& strategy) = 0;
	virtual void Visit(const Shape& shape, const TriangleStrategy& strategy) = 0;
	virtual void Visit(const Shape& shape, const PentagonStrategy& strategy) = 0;
	virtual void Visit(const Shape& shape, const StarStrategy& strategy) = 0;
	virtual void Visit(const Shape& shape, const GroupStrategy& strategy) = 0;
};