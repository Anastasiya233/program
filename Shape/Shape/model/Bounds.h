#pragma once

struct Bounds
{
	double left = 0.0;
	double top = 0.0;
	double width = 0.0;
	double height = 0.0;

	double Right() const { return left + width; }
	double Bottom() const { return top + height; }

	bool IsEmpty() const { return width <= 0.0 || height <= 0.0; }
};