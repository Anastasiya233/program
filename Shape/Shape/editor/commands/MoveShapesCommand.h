#pragma once

#include "../../model/Picture.h"
#include "../history/CommandHistory.h"
#include <string>
#include <vector>

class MoveShapesCommand : public ICommand
{
public:
	MoveShapesCommand(Picture& picture, std::vector<std::string> ids, double dx, double dy);

	void Execute() override;
	void Unexecute() override;
	std::vector<std::string> AffectedIds() const override { return m_ids; }

	const std::vector<std::string>& GetShapeIds() const;

private:
	Picture& m_picture;
	std::vector<std::string> m_ids;
	double m_dx;
	double m_dy;
};