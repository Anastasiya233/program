#include "ConsolePictureObserver.h"

#include <ostream>

ConsolePictureObserver::ConsolePictureObserver(std::ostream& out)
	: m_out(out)
{
}

void ConsolePictureObserver::OnShapeAdded(const Shape& shape)
{
	m_out << "New shape " << shape.GetId() << '\n';
}

void ConsolePictureObserver::OnShapeRemoved(const Shape& shape)
{
	m_out << "Delete shape " << shape.GetId() << " " << shape.GetTypeName() << '\n';
}

void ConsolePictureObserver::OnShapeMoved(const Shape& shape, double dx, double dy)
{
	m_out << "Move shape " << shape.GetId() << " " << dx << " " << dy << '\n';
}

void ConsolePictureObserver::OnShapeColorChanged(const Shape& shape, const Color& newColor)
{
	m_out << "Change color " << shape.GetId() << " " << newColor.GetHexStr() << '\n';
}

void ConsolePictureObserver::OnShapeParamsChanged(const Shape& shape)
{
	m_out << "ChangeShape " << shape.GetId()
		  << " " << shape.GetTypeName() << " "
		  << shape.GetParamsToString() << '\n';
}