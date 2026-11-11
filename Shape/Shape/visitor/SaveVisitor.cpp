#include "SaveVisitor.h"
#include <sstream>

void SaveVisitor::Visit(const Shape& shape, const GroupStrategy& strategy)
{
	m_out << "group " << shape.GetId() << " " << shape.GetColor().GetHexStr() << "\n";
	for (const Shape* child : strategy.GetChildren()) child->Accept(*this);
	m_out << "endgroup\n";
}

SaveVisitor::SaveVisitor(std::ostream& out)
	: m_out(out)
{
}

void SaveVisitor::Visit(const Shape& shape, const RectangleStrategy& strategy)
{
	m_out << "rectangle " << shape.GetId() << " "
		  << shape.GetColor().GetHexStr() << " "
		  << strategy.GetParamsToString() << "\n";
}

void SaveVisitor::Visit(const Shape& shape, const EllipseStrategy& strategy)
{
	m_out << "circle " << shape.GetId() << " "
		  << shape.GetColor().GetHexStr() << " "
		  << strategy.GetParamsToString() << "\n";
}

void SaveVisitor::Visit(const Shape& shape, const TriangleStrategy& strategy)
{
	m_out << "triangle " << shape.GetId() << " "
		  << shape.GetColor().GetHexStr() << " "
		  << strategy.GetParamsToString() << "\n";
}

void SaveVisitor::Visit(const Shape& shape, const PentagonStrategy& strategy)
{
	m_out << "pentagon " << shape.GetId() << " "
		  << shape.GetColor().GetHexStr() << " "
		  << strategy.GetParamsToString() << "\n";
}

void SaveVisitor::Visit(const Shape& shape, const StarStrategy& strategy)
{
	m_out << "star " << shape.GetId() << " "
		  << shape.GetColor().GetHexStr() << " "
		  << strategy.GetParamsToString() << "\n";
}