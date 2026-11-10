#pragma once

#include "../../model/Picture.h"
#include "../history/CommandHistory.h"
#include <memory>
#include <string>

class AddShapeCommand : public ICommand
{
public:
	AddShapeCommand(Picture& picture, std::unique_ptr<Shape> shape, int index);

	void Execute() override;
	void Unexecute() override;

	const std::string& GetShapeId() const;

private:
	Picture& m_picture;
	std::unique_ptr<Shape> m_shape;
	int m_index;
};