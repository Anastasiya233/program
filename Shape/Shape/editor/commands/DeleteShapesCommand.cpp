#include "DeleteShapesCommand.h"

#include <algorithm>

DeleteShapesCommand::DeleteShapesCommand(Picture& picture, std::vector<std::string> ids)
	: m_picture(picture)
	, m_ids(std::move(ids))
{
}

void DeleteShapesCommand::Execute()
{
	m_removed.clear();

	std::vector<std::pair<int, std::string>> existing;
	existing.reserve(m_ids.size());
	for (const auto& id : m_ids)
	{
		if (m_picture.GetShape(id) != nullptr)
		{
			existing.emplace_back(m_picture.GetShapeIndex(id), id);
		}
	}

	std::sort(existing.begin(), existing.end(),
		[](const auto& a, const auto& b) { return a.first > b.first; });

	for (const auto& [index, id] : existing)
	{
		auto* shape = m_picture.GetShape(id);
		RemovedShape removed;
		removed.prototype = shape->Clone(shape->GetId());
		removed.index = index;
		m_removed.push_back(std::move(removed));

		m_picture.DeleteShape(id);
	}
}

void DeleteShapesCommand::Unexecute()
{
	std::sort(m_removed.begin(), m_removed.end(),
		[](const RemovedShape& a, const RemovedShape& b) { return a.index < b.index; });

	for (const auto& removed : m_removed)
	{
		m_picture.InsertShape(
			removed.prototype->Clone(removed.prototype->GetId()),
			removed.index);
	}
}

const std::vector<std::string>& DeleteShapesCommand::GetShapeIds() const
{
	return m_ids;
}