#pragma once

#include "../../editor/EditorState.h"
#include "../Shape.h"
#include "EllipseStrategy.h"
#include "IShapeStrategy.h"
#include "RectangleStrategy.h"
#include "TriangleStrategy.h"
#include "PentagonStrategy.h"
#include "StarStrategy.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

class ShapeStrategyFactory
{
public:
	static std::unique_ptr<IShapeStrategy> CreateShapeFromString(const std::string& type, std::istringstream& iss);
	static std::unique_ptr<Shape> CreateDraftShape(Tool tool, double left, double top, double width, double height);
};
