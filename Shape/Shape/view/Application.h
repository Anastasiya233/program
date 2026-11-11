#pragma once
#include "../editor/EditorController.h"
#include "../editor/EditorState.h"
#include "../model/Picture.h"
#include "EditorView.h"
#include <memory>
#include <vector>

struct SharedDocument
{
    std::shared_ptr<Picture> picture = std::make_shared<Picture>();
    CommandHistory history;
};

struct DocumentContext
{
    std::shared_ptr<SharedDocument> document;
    std::shared_ptr<EditorState> state;
    std::unique_ptr<EditorController> controller;
    std::unique_ptr<EditorView> view;
    explicit DocumentContext(std::shared_ptr<SharedDocument> shared = {});
};

class Application
{
public:
    Application();
    void Run();
private:
    void CreateNewWindow();
    void CreateSharedWindow(std::shared_ptr<SharedDocument> doc);
    std::vector<std::unique_ptr<DocumentContext>> m_documents;
};
