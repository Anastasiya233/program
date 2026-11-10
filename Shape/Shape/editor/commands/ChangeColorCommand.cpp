#include "ChangeColorCommand.h"

ChangeColorCommand::ChangeColorCommand(Picture& picture, std::vector<Change> changes)
	: m_picture(picture)
	, m_changes(std::move(changes))
{
}

void ChangeColorCommand::Execute()
{
	for (const auto& c : m_changes)
	{
		if (auto* shape = m_picture.GetShape(c.id))
		{
			shape->SetColor(c.after);
		}
	}
}

void ChangeColorCommand::Unexecute()
{
	for (const auto& c : m_changes)
	{
		if (auto* shape = m_picture.GetShape(c.id))
		{
			shape->SetColor(c.before);
		}
	}
}

const std::vector<std::string> ChangeColorCommand::GetShapeIds() const
{
	std::vector<std::string> ids;
	ids.reserve(m_changes.size());
	for (const auto& c : m_changes)
	{
		ids.push_back(c.id);
	}
	return ids;
}