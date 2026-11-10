#include "Picture.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

Picture::Iterator Picture::FindShape(const std::string& id)
{
	return std::find_if(m_shapes.begin(), m_shapes.end(),
		[&id](const DataShape& ds) {
			return ds.shape->GetId() == id;
		});
}

Picture::ConstIterator Picture::FindShape(const std::string& id) const
{
	return std::find_if(m_shapes.begin(), m_shapes.end(),
		[&id](const DataShape& ds) {
			return ds.shape->GetId() == id;
		});
}

void Picture::AddShape(std::unique_ptr<Shape> shape)
{
	const std::string& id = shape->GetId();
	if (GetShape(id) != nullptr)
	{
		throw std::runtime_error("Shape '" + id + "' already exists.");
	}

	DataShape entry;
	entry.shape = std::move(shape);
	Shape* s = entry.shape.get();

	entry.subMoved = s->SubscribeToMoved(
		[this](const Shape& s, double dx, double dy) {
			m_shapeMoved(s, dx, dy);
		});

	entry.subColor = s->SubscribeToColorChanged(
		[this](const Shape& s, const Color& c) {
			m_shapeColorChanged(s, c);
		});

	entry.subParams = s->SubscribeToParamsChanged(
		[this](const Shape& s) {
			m_shapeParamsChanged(s);
		});
	m_shapes.push_back(std::move(entry));
	m_shapeAdded(*s);
}

void Picture::DeleteShape(const std::string& id)
{
	auto it = FindShape(id);
	if (it == m_shapes.end())
	{
		throw std::runtime_error("Shape '" + id + "' not found.");
	}

	m_shapeRemoved(*it->shape);
	m_shapes.erase(it);
}

void Picture::MovePicture(double dx, double dy)
{
	for (auto& dataShape : m_shapes)
	{
		dataShape.shape->Move(dx, dy);
	}
}

void Picture::MoveShapeTo(const std::string& id, int newIndex)
{
	auto it = FindShape(id);
	if (it == m_shapes.end())
	{
		return;
	}

	DataShape temp = std::move(*it);
	m_shapes.erase(it);

	if (newIndex < 0)
	{
		newIndex = 0;
	}
	if (newIndex > m_shapes.size())
	{
		newIndex = m_shapes.size();
	}

	m_shapes.insert(m_shapes.begin() + newIndex, std::move(temp));
}

Shape* Picture::GetShape(const std::string& id)
{
	auto it = FindShape(id);
	return it == m_shapes.end() ? nullptr : it->shape.get();
}

const Shape* Picture::GetShape(const std::string& id) const
{
	auto it = FindShape(id);
	;
	return it == m_shapes.end() ? nullptr : it->shape.get();
}

const std::vector<Shape*> Picture::GetShapes() const
{
	std::vector<Shape*> result;
	result.reserve(m_shapes.size());
	for (const auto& entry : m_shapes)
	{
		result.push_back(entry.shape.get());
	}
	return result;
}

std::unique_ptr<Picture> Picture::Clone() const
{
	auto copy = std::make_unique<Picture>();
	for (const auto& dataShape : m_shapes)
	{
		copy->AddShape(dataShape.shape->Clone(dataShape.shape->GetId()));
	}
	return copy;
}

void Picture::Clear()
{
	while (!m_shapes.empty())
	{
		m_shapeRemoved(*m_shapes.back().shape);
		m_shapes.pop_back();
	}
}

boost::signals2::scoped_connection Picture::SubscribeToShapeAdded(std::function<void(const Shape&)> handler)
{
	return m_shapeAdded.connect(std::move(handler));
}

boost::signals2::scoped_connection Picture::SubscribeToShapeRemoved(std::function<void(const Shape&)> handler)
{
	return m_shapeRemoved.connect(std::move(handler));
}

boost::signals2::scoped_connection Picture::SubscribeToShapeMoved(std::function<void(const Shape&, double, double)> handler)
{
	return m_shapeMoved.connect(std::move(handler));
}

boost::signals2::scoped_connection Picture::SubscribeToShapeColorChanged(std::function<void(const Shape&, const Color&)> handler)
{
	return m_shapeColorChanged.connect(std::move(handler));
}

boost::signals2::scoped_connection Picture::SubscribeToShapeParamsChanged(std::function<void(const Shape&)> handler)
{
	return m_shapeParamsChanged.connect(std::move(handler));
}

void Picture::OnShapeMoved(const Shape& shape, double dx, double dy)
{
	m_shapeMoved(shape, dx, dy);
}

void Picture::OnShapeColorChanged(const Shape& shape, const Color& newColor)
{
	m_shapeColorChanged(shape, newColor);
}

void Picture::OnShapeParamsChanged(const Shape& shape)
{
	m_shapeParamsChanged(shape);
}

Shape* Picture::GetTopShapeAt(double x, double y) const
{
	for (auto it = m_shapes.rbegin(); it != m_shapes.rend(); ++it)
	{
		if (it->shape->HitTest(x, y))
		{
			return it->shape.get();
		}
	}
	return nullptr;
}

void Picture::InsertShape(std::unique_ptr<Shape> shape, int index)
{
	if (!shape)
	{
		throw std::invalid_argument("Shape cannot be null");
	}

	const std::string& id = shape->GetId();
	if (GetShape(id) != nullptr)
	{
		throw std::runtime_error("Shape '" + id + "' already exists.");
	}

	if (index < 0)
	{
		index = 0;
	}
	if (index > static_cast<int>(m_shapes.size()))
	{
		index = static_cast<int>(m_shapes.size());
	}

	DataShape entry;
	entry.shape = std::move(shape);
	Shape* s = entry.shape.get();

	entry.subMoved = s->SubscribeToMoved(
		[this](const Shape& s, double dx, double dy) { m_shapeMoved(s, dx, dy); });
	entry.subColor = s->SubscribeToColorChanged(
		[this](const Shape& s, const Color& c) { m_shapeColorChanged(s, c); });
	entry.subParams = s->SubscribeToParamsChanged(
		[this](const Shape& s) { m_shapeParamsChanged(s); });

	m_shapes.insert(m_shapes.begin() + index, std::move(entry));
	m_shapeAdded(*s);
}

int Picture::GetShapeIndex(const std::string& id) const
{
	auto it = FindShape(id);
	if (it == m_shapes.end())
	{
		return -1;
	}
	return static_cast<int>(std::distance(m_shapes.cbegin(), it));
}