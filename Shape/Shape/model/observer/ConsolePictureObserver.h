#pragma once

#include "../../model/Color.h"
#include "../Picture.h"
#include "../Shape.h"
#include <ostream>

class ConsolePictureObserver
{
public:
	ConsolePictureObserver(std::ostream& out);
	void OnShapeAdded(const Shape& shape);
	void OnShapeRemoved(const Shape& shape);
	void OnShapeMoved(const Shape& shape, double dx, double dy);
	void OnShapeColorChanged(const Shape& shape, const Color& newColor);
	void OnShapeParamsChanged(const Shape& shape);

private:
	std::ostream& m_out;
};