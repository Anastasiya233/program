#include "ShapeStrategyFactory.h"

std::unique_ptr<IShapeStrategy> ShapeStrategyFactory::CreateShapeFromString(const std::string& type, std::istringstream& iss)
{
	if (type == "rectangle" || type == "rect")
	{
		double x, y, w, h;
		if (!(iss >> x >> y >> w >> h))
		{
			throw std::runtime_error("Invalid rectangle parameters");
		}
		return std::make_unique<RectangleStrategy>(x, y, w, h);
	}
	else if (type == "ellipse" || type == "circle")
	{
		double cx, cy, rx, ry;
		if (!(iss >> cx >> cy >> rx >> ry))
		{
			throw std::runtime_error("Invalid ellipse parameters");
		}
		return std::make_unique<EllipseStrategy>(cx, cy, rx, ry);
	}
	else if (type == "triangle")
	{
		double x1, y1, x2, y2, x3, y3;
		if (!(iss >> x1 >> y1 >> x2 >> y2 >> x3 >> y3))
		{
			throw std::runtime_error("Invalid triangle parameters");
		}
		return std::make_unique<TriangleStrategy>(x1, y1, x2, y2, x3, y3);
	}
	else if (type == "pentagon")
	{
		double left, top, w, h;
		if (!(iss >> left >> top >> w >> h))
			throw std::runtime_error("Invalid pentagon parameters");
		return std::make_unique<PentagonStrategy>(left, top, w, h);
	}
	else if (type == "star")
	{
		double left, top, w, h;
		if (!(iss >> left >> top >> w >> h))
			throw std::runtime_error("Invalid star parameters");
		return std::make_unique<StarStrategy>(left, top, w, h);
	}
	else
	{
		throw std::runtime_error("Unknown shape type: " + type);
	}
}

std::unique_ptr<Shape> ShapeStrategyFactory::CreateDraftShape(Tool tool, double left, double top, double width, double height)
{
	std::string id = "draft";

	if (tool == Tool::DrawRect)
	{
		return std::make_unique<Shape>(id, Color(220, 50, 50),
			std::make_unique<RectangleStrategy>(left, top, width, height));
	}
	else if (tool == Tool::DrawEllipse)
	{
		return std::make_unique<Shape>(id, Color(50, 120, 220),
			std::make_unique<EllipseStrategy>(left + width / 2, top + height / 2, width / 2, height / 2));
	}
	else if (tool == Tool::DrawTriangle)
	{
		return std::make_unique<Shape>(id, Color(50, 180, 80),
			std::make_unique<TriangleStrategy>(left, top + height, left + width, top + height, left + width / 2, top));
	}
	else if (tool == Tool::DrawPentagon)
	{
		return std::make_unique<Shape>(id, Color(255, 165, 0),
			std::make_unique<PentagonStrategy>(left, top, width, height));
	}
	else if (tool == Tool::DrawStar)
	{
		return std::make_unique<Shape>(id, Color(255, 215, 0),
			std::make_unique<StarStrategy>(left, top, width, height));
	}

	return nullptr;
}
