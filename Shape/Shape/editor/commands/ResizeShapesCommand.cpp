#include "ResizeShapesCommand.h"

ResizeShapesCommand::ResizeShapesCommand(Picture& picture, std::vector<Change> changes)
	: m_picture(picture)
	, m_changes(std::move(changes))
{
}

void ResizeShapesCommand::Execute()
{
	for (const auto& c : m_changes)
	{
		if (auto* shape = m_picture.GetShape(c.id))
		{
			shape->SetBounds(c.after);
		}
	}
}

void ResizeShapesCommand::Unexecute()
{
	for (const auto& c : m_changes)
	{
		if (auto* shape = m_picture.GetShape(c.id))
		{
			shape->SetBounds(c.before);
		}
	}
}

const std::vector<std::string> ResizeShapesCommand::GetShapeIds() const
{
	std::vector<std::string> ids;
	ids.reserve(m_changes.size());
	for (const auto& c : m_changes)
	{
		ids.push_back(c.id);
	}
	return ids;
}