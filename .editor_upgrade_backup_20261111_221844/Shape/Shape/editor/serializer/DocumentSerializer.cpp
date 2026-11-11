#include "DocumentSerializer.h"
#include "../../model/shapes/ShapeStrategyFactory.h"
#include "../../visitor/SaveVisitor.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

void DocumentSerializer::Save(const Picture& picture, const std::string& filename)
{
	std::ofstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file for writing: " + filename);
	}

	file << "RESULT!\n";
	SaveVisitor visitor(file);

	for (const auto* shape : picture.GetShapes())
	{
		shape->Accept(visitor);
	}
}

std::unique_ptr<Picture> DocumentSerializer::Load(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file for reading: " + filename);
	}

	std::string line;
	if (!std::getline(file, line) || line != "RESULT!")
	{
		throw std::runtime_error("Invalid file format: missing version");
	}

	auto newPicture = std::make_unique<Picture>();

	while (std::getline(file, line))
	{
		if (line.empty())
		{
			continue;
		}

		std::istringstream iss(line);
		std::string type, id, colorHex;

		if (!(iss >> type >> id >> colorHex))
		{
			throw std::runtime_error("Invalid line format: " + line);
		}

		auto strategy = ShapeStrategyFactory::CreateShapeFromString(type, iss);
		if (!strategy)
		{
			throw std::runtime_error("Unknown shape type: " + type);
		}

		Color color = Color::GetRGB(colorHex);
		auto shape = std::make_unique<Shape>(id, color, std::move(strategy));

		newPicture->AddShape(std::move(shape));
	}

	return newPicture;
}