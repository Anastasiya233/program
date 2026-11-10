#include "FileDialog.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <commdlg.h>

static std::wstring ToWideString(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	std::wstring result(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, result.data(), size);
	return result;
}

static OPENFILENAMEW CreateDialogParams(void* owner, wchar_t* buffer)
{
	OPENFILENAMEW dialog{};
	dialog.lStructSize = sizeof(dialog);
	dialog.hwndOwner = static_cast<HWND>(owner);
	dialog.lpstrFilter = L"Текстовые файлы (*.txt)\0*.txt\0Все файлы\0*.*\0";
	dialog.lpstrFile = buffer;
	dialog.nMaxFile = MAX_PATH;
	dialog.lpstrDefExt = L"txt";
	return dialog;
}

FileDialog::FileDialog(void* windowHandle)
	: m_owner(windowHandle)
{
}

std::optional<std::string> FileDialog::GetSavePath()
{
	wchar_t buffer[MAX_PATH] = L"";

	OPENFILENAMEW dialog = CreateDialogParams(m_owner, buffer);
	dialog.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

	if (!GetSaveFileNameW(&dialog))
	{
		return std::nullopt;
	}

	int size = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
	std::string result(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, buffer, -1, result.data(), size, nullptr, nullptr);

	if (!result.empty() && result.back() == '\0')
	{
		result.pop_back();
	}
	return result;
}

std::optional<std::string> FileDialog::GetOpenPath()
{
	wchar_t buffer[MAX_PATH] = L"";

	OPENFILENAMEW dialog = CreateDialogParams(m_owner, buffer);
	dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if (!GetOpenFileNameW(&dialog))
	{
		return std::nullopt;
	}

	int size = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
	std::string result(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, buffer, -1, result.data(), size, nullptr, nullptr);

	if (!result.empty() && result.back() == '\0')
	{
		result.pop_back();
	}
	return result;
}

void FileDialog::ShowError(const std::string& message) const
{
	std::wstring wideMessage = ToWideString(message);
	MessageBoxW(static_cast<HWND>(m_owner), wideMessage.c_str(), L"Ошибка", MB_OK | MB_ICONERROR);
}