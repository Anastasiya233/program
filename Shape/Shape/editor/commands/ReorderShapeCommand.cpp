#include "ReorderShapeCommand.h"

ReorderShapeCommand::ReorderShapeCommand(Picture& picture, std::string id, int oldIndex, int newIndex)
	: m_picture(picture)
	, m_id(std::move(id))
	, m_oldIndex(oldIndex)
	, m_newIndex(newIndex)
{
}

void ReorderShapeCommand::Execute()
{
	m_picture.MoveShapeTo(m_id, m_newIndex);
}

void ReorderShapeCommand::Unexecute()
{
	m_picture.MoveShapeTo(m_id, m_oldIndex);
}

std::vector<std::string> ReorderShapeCommand::GetAffectedIds() const
{
	return { m_id };
}