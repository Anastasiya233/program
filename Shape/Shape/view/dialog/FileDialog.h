#pragma once

#include <optional>
#include <string>

class FileDialog
{
public:
	FileDialog(void* owner);

	std::optional<std::string> GetSavePath();
	std::optional<std::string> GetOpenPath();
	void ShowError(const std::string& message) const;

private:
	void* m_owner;
};