#include "AddShapeCommand.h"

AddShapeCommand::AddShapeCommand(Picture& picture, std::unique_ptr<Shape> shape, int index)
	: m_picture(picture)
	, m_shape(std::move(shape))
	, m_index(index)
{
}

void AddShapeCommand::Execute()
{
	m_picture.InsertShape(m_shape->Clone(m_shape->GetId()), m_index);
}

void AddShapeCommand::Unexecute()
{
	m_picture.DeleteShape(m_shape->GetId());
}

const std::string& AddShapeCommand::GetShapeId() const
{
	return m_shape->GetId();
}