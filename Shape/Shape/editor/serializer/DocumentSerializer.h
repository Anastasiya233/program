#pragma once
#include "../../model/Picture.h"
#include <memory>
#include <string>

class DocumentSerializer
{
public:
	static void Save(const Picture& picture, const std::string& filename);
	static std::unique_ptr<Picture> Load(const std::string& filename);
};