#pragma once

#include "../model/Color.h"
#include "shapes/IShapeStrategy.h"
#include <boost/signals2.hpp>
#include <functional>
#include <memory>
#include <string>

class Shape
{
public:
	Shape(std::string id, Color color, std::unique_ptr<IShapeStrategy> strategy);

	void Move(double dx, double dy);
	void SetColor(const Color& color);
	void SetStrategy(std::unique_ptr<IShapeStrategy> strategy);
	std::unique_ptr<Shape> Clone(const std::string& newId) const;

	const std::string& GetId() const;
	const Color& GetColor() const;
	std::string GetTypeName() const;
	std::string GetParamsToString() const;

	boost::signals2::scoped_connection SubscribeToMoved(std::function<void(const Shape&, double, double)> handler);
	boost::signals2::scoped_connection SubscribeToColorChanged(std::function<void(const Shape&, const Color&)> handler);
	boost::signals2::scoped_connection SubscribeToParamsChanged(std::function<void(const Shape&)> handler);

	bool HitTest(double x, double y) const;
	Bounds GetBounds() const;
	void SetBounds(const Bounds& b);
	void Accept(IShapeVisitor& visitor) const;

private:
	std::string m_id;
	Color m_color;
	std::unique_ptr<IShapeStrategy> m_strategy;

	boost::signals2::signal<void(const Shape&, double, double)> m_moved;
	boost::signals2::signal<void(const Shape&, const Color&)> m_colorChanged;
	boost::signals2::signal<void(const Shape&)> m_paramsChanged;
};