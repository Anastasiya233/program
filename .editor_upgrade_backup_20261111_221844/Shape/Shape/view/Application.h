#pragma once

#include "../editor/EditorController.h"
#include "../editor/EditorState.h"
#include "../editor/Logger.h"
#include "../model/Picture.h"
#include "EditorView.h"
#include <memory>
#include <vector>

struct DocumentContext
{
	std::shared_ptr<Picture> picture;
	std::shared_ptr<EditorState> state;
	CommandHistory history;
	std::unique_ptr<EditorController> controller;
	std::unique_ptr<EditorView> view;
	DocumentContext();
};

class Application
{
public:
	Application();
	void Run();

private:
	void CreateNewWindow();
	std::vector<std::unique_ptr<DocumentContext>> m_documents;
};