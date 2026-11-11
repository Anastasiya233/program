#pragma once
#include "../../model/Picture.h"
#include "../history/CommandHistory.h"
#include <vector>

class ChangeColorCommand : public ICommand
{
public:
	struct Change
	{
		std::string id;
		Color before;
		Color after;
	};

	ChangeColorCommand(Picture& picture, std::vector<Change> changes);

	void Execute() override;
	void Unexecute() override;
	std::vector<std::string> AffectedIds() const override
	{
		std::vector<std::string> ids;
		for (const auto& c: m_changes) ids.push_back(c.id);
		return ids;
	}

	const std::vector<std::string> GetShapeIds() const;

private:
	Picture& m_picture;
	std::vector<Change> m_changes;
};