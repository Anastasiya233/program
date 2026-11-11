#include "Application.h"

DocumentContext::DocumentContext(std::shared_ptr<SharedDocument> shared)
    : document(shared ? std::move(shared) : std::make_shared<SharedDocument>())
    , state(std::make_shared<EditorState>())
{
    controller = std::make_unique<EditorController>(
        *document->picture, *state, document->history);
    view = std::make_unique<EditorView>(*document->picture, *state, *controller);
}

Application::Application() { CreateNewWindow(); }

void Application::CreateNewWindow()
{
    m_documents.push_back(std::make_unique<DocumentContext>());
}

void Application::CreateSharedWindow(std::shared_ptr<SharedDocument> doc)
{
    m_documents.push_back(std::make_unique<DocumentContext>(std::move(doc)));
}

void Application::Run()
{
    while (!m_documents.empty())
    {
        std::vector<std::string> filesToOpen;
        std::vector<std::shared_ptr<SharedDocument>> sharedWindows;
        int newEmptyWindowsCount = 0;
        for (auto it = m_documents.begin(); it != m_documents.end(); )
        {
            // Consume requests produced in the preceding frame.
            if ((*it)->view->WantsNewWindow())
                ++newEmptyWindowsCount;
            if ((*it)->view->WantsSharedWindow())
                sharedWindows.push_back((*it)->document);
            if ((*it)->view->WantsOpenWindow())
                filesToOpen.push_back((*it)->view->GetOpenPath());
            (*it)->view->ClearRequests();
            if ((*it)->view->Update()) ++it;
            else it = m_documents.erase(it);
        }
        for (int i=0; i<newEmptyWindowsCount; ++i) CreateNewWindow();
        for (const auto& doc : sharedWindows) CreateSharedWindow(doc);
        for (const auto& path : filesToOpen)
        {
            auto win = std::make_unique<DocumentContext>();
            win->controller->LoadDocument(path);
            m_documents.push_back(std::move(win));
        }
    }
}
