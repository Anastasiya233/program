#pragma once

#include "../../model/Bounds.h"
#include "../../model/Picture.h"
#include "../history/CommandHistory.h"
#include <string>
#include <vector>

class ResizeShapesCommand : public ICommand
{
public:
	struct Change
	{
		std::string id;
		Bounds before;
		Bounds after;
	};

	ResizeShapesCommand(Picture& picture, std::vector<Change> changes);

	void Execute() override;
	void Unexecute() override;

	const std::vector<std::string> GetShapeIds() const;

private:
	Picture& m_picture;
	std::vector<Change> m_changes;
};