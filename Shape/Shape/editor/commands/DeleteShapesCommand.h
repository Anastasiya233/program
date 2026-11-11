#pragma once

#include "../../model/Picture.h"
#include "../history/CommandHistory.h"
#include <memory>
#include <string>
#include <vector>

class DeleteShapesCommand : public ICommand
{
public:
	DeleteShapesCommand(Picture& picture, std::vector<std::string> ids);

	void Execute() override;
	void Unexecute() override;
	std::vector<std::string> AffectedIds() const override { return m_ids; }

	const std::vector<std::string>& GetShapeIds() const;

private:
	struct RemovedShape
	{
		std::unique_ptr<Shape> prototype;
		int index = -1;
	};

	Picture& m_picture;
	std::vector<std::string> m_ids;
	std::vector<RemovedShape> m_removed;
};