#pragma once
#include "../../model/Picture.h"
#include "../history/CommandHistory.h"
#include <string>
#include <vector>

class ReorderShapeCommand : public ICommand
{
public:
	ReorderShapeCommand(Picture& picture, std::string id, int oldIndex, int newIndex);
	void Execute() override;
	void Unexecute() override;
	std::vector<std::string> GetAffectedIds() const;

private:
	Picture& m_picture;
	std::string m_id;
	int m_oldIndex;
	int m_newIndex;
};