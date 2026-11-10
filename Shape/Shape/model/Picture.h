#pragma once

#include "Shape.h"
#include <boost/signals2.hpp>
#include <memory>
#include <string>
#include <vector>

class Picture
{
public:
	Picture() = default;

	void AddShape(std::unique_ptr<Shape> shape);
	void InsertShape(std::unique_ptr<Shape> shape, int index);
	void DeleteShape(const std::string& id);
	void MovePicture(double dx, double dy);
	void MoveShapeTo(const std::string& id, int newIndex);
	Shape* GetShape(const std::string& id);
	const Shape* GetShape(const std::string& id) const;
	const std::vector<Shape*> GetShapes() const;
	std::unique_ptr<Picture> Clone() const;
	void Clear();

	int GetShapeIndex(const std::string& id) const;

	boost::signals2::scoped_connection SubscribeToShapeAdded(std::function<void(const Shape&)> handler);
	boost::signals2::scoped_connection SubscribeToShapeRemoved(std::function<void(const Shape&)> handler);
	boost::signals2::scoped_connection SubscribeToShapeMoved(std::function<void(const Shape&, double, double)> handler);
	boost::signals2::scoped_connection SubscribeToShapeColorChanged(std::function<void(const Shape&, const Color&)> handler);
	boost::signals2::scoped_connection SubscribeToShapeParamsChanged(std::function<void(const Shape&)> handler);

	void OnShapeMoved(const Shape& shape, double dx, double dy);
	void OnShapeColorChanged(const Shape& shape, const Color& newColor);
	void OnShapeParamsChanged(const Shape& shape);

	Shape* GetTopShapeAt(double x, double y) const;

private:
	struct DataShape
	{
		std::unique_ptr<Shape> shape;
		boost::signals2::scoped_connection subMoved;
		boost::signals2::scoped_connection subColor;
		boost::signals2::scoped_connection subParams;
	};
	std::vector<DataShape> m_shapes;

	using Iterator = std::vector<DataShape>::iterator;
	using ConstIterator = std::vector<DataShape>::const_iterator;
	Iterator FindShape(const std::string& id);
	ConstIterator FindShape(const std::string& id) const;

	boost::signals2::signal<void(const Shape&)> m_shapeAdded;
	boost::signals2::signal<void(const Shape&)> m_shapeRemoved;
	boost::signals2::signal<void(const Shape&, double, double)> m_shapeMoved;
	boost::signals2::signal<void(const Shape&, const Color&)> m_shapeColorChanged;
	boost::signals2::signal<void(const Shape&)> m_shapeParamsChanged;
};