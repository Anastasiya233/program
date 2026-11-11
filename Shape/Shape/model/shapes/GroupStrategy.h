#pragma once
#include "IShapeStrategy.h"
#include "../Shape.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

// Composite: a group itself is a normal Shape through the IShapeStrategy interface.
// Children are stored in z-order, bottom first. Nested groups work recursively.
class GroupStrategy final : public IShapeStrategy
{
public:
	explicit GroupStrategy(std::vector<std::unique_ptr<Shape>> children)
		: m_children(std::move(children))
	{
		if (m_children.empty())
			throw std::invalid_argument("Group cannot be empty");
	}

	bool HitTest(double x, double y) const override
	{
		for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
			if ((*it)->HitTest(x, y)) return true;
		return false;
	}

	Bounds GetBounds() const override
	{
		const Bounds first = m_children.front()->GetBounds();
		double left = first.left, top = first.top;
		double right = first.Right(), bottom = first.Bottom();
		for (const auto& child : m_children)
		{
			const Bounds b = child->GetBounds();
			left = std::min(left, b.left);
			top = std::min(top, b.top);
			right = std::max(right, b.Right());
			bottom = std::max(bottom, b.Bottom());
		}
		return { left, top, right - left, bottom - top };
	}

	void SetBounds(const Bounds& newBounds) override
	{
		if (newBounds.IsEmpty()) throw std::invalid_argument("Invalid group bounds");
		const Bounds old = GetBounds();
		if (old.IsEmpty()) throw std::invalid_argument("Cannot scale empty group bounds");
		const double sx = newBounds.width / old.width, sy = newBounds.height / old.height;
		for (auto& child : m_children)
		{
			const Bounds b = child->GetBounds();
			child->SetBounds({ newBounds.left + (b.left - old.left) * sx,
				newBounds.top + (b.top - old.top) * sy, b.width * sx, b.height * sy });
		}
	}

	void Move(double dx, double dy) override
	{
		for (auto& child : m_children) child->Move(dx, dy);
	}

	std::unique_ptr<IShapeStrategy> Clone() const override
	{
		std::vector<std::unique_ptr<Shape>> copies;
		for (const auto& child : m_children)
			copies.push_back(child->Clone(child->GetId()));
		return std::make_unique<GroupStrategy>(std::move(copies));
	}

	std::string GetTypeName() const override { return "Group"; }
	std::string GetParamsToString() const override { return std::to_string(m_children.size()); }
	void Accept(const Shape& shape, IShapeVisitor& visitor) const override
	{
		visitor.Visit(shape, *this);
	}

	void SetChildrenColor(const Color& color) override
	{
		for (auto& child : m_children) child->SetColorRecursive(color);
	}

	std::vector<const Shape*> GetChildren() const override
	{
		std::vector<const Shape*> result;
		for (const auto& child : m_children) result.push_back(child.get());
		return result;
	}

private:
	std::vector<std::unique_ptr<Shape>> m_children;
};
