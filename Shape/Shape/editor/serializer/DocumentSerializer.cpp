#include "DocumentSerializer.h"
#include "../../model/shapes/ShapeStrategyFactory.h"
#include "../../model/shapes/GroupStrategy.h"
#include <functional>
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
	std::function<std::unique_ptr<Shape>(const std::string&, std::istream&)> parse;
	parse = [&parse](const std::string& source, std::istream& in) -> std::unique_ptr<Shape>
		{
			std::istringstream iss(source);
			std::string type, id, hex;
			if (!(iss >> type >> id >> hex) || type == "endgroup")
				throw std::runtime_error("Invalid shape: " + source);
			const Color color = Color::GetRGB(hex);
			if (type == "group")
			{
				std::vector<std::unique_ptr<Shape>> children;
				std::string childLine;
				bool ended = false;
				while (std::getline(in, childLine))
				{
					if (childLine.empty()) continue;
					if (childLine == "endgroup") { ended = true; break; }
					children.push_back(parse(childLine, in));
				}
				if (!ended) throw std::runtime_error("Unterminated group: " + id);
				return std::make_unique<Shape>(id, color,
					std::make_unique<GroupStrategy>(std::move(children)));
			}
			return std::make_unique<Shape>(id, color,
				ShapeStrategyFactory::CreateShapeFromString(type, iss));
		};
	while (std::getline(file, line))
	{
		if (!line.empty()) newPicture->AddShape(parse(line, file));
	}
	return newPicture;
}