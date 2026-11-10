#include "MoveShapesCommand.h"

// TODO:делать через паттерн макро команды

MoveShapesCommand::MoveShapesCommand(Picture& picture, std::vector<std::string> ids, double dx, double dy)
	: m_picture(picture)
	, m_ids(std::move(ids))
	, m_dx(dx)
	, m_dy(dy)
{
}

void MoveShapesCommand::Execute()
{
	for (const auto& id : m_ids)
	{
		if (auto* shape = m_picture.GetShape(id))
		{
			shape->Move(m_dx, m_dy);
		}
	}
}

void MoveShapesCommand::Unexecute()
{
	for (const auto& id : m_ids)
	{
		if (auto* shape = m_picture.GetShape(id))
		{
			shape->Move(-m_dx, -m_dy);
		}
	}
}

const std::vector<std::string>& MoveShapesCommand::GetShapeIds() const
{
	return m_ids;
}