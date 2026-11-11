#include "Application.h"

DocumentContext::DocumentContext()
{
	picture = std::make_shared<Picture>();
	state = std::make_shared<EditorState>();
	controller = std::make_unique<EditorController>(*picture, *state, history);
	view = std::make_unique<EditorView>(*picture, *state, *controller);
}

Application::Application()
{
	CreateNewWindow();
}

void Application::CreateNewWindow()
{
	m_documents.push_back(std::make_unique<DocumentContext>());
}

void Application::Run()
{
	while (!m_documents.empty())
	{
		std::vector<std::string> filesToOpen;
		int newEmptyWindowsCount = 0;

		for (auto it = m_documents.begin(); it != m_documents.end();)
		{
			if ((*it)->view->WantsNewWindow())
			{
				newEmptyWindowsCount++;
				(*it)->view->ClearRequests();
			}
			else if ((*it)->view->WantsOpenWindow())
			{
				filesToOpen.push_back((*it)->view->GetOpenPath());
				(*it)->view->ClearRequests();
			}

			if ((*it)->view->Update())
			{
				++it;
			}
			else
			{
				it = m_documents.erase(it);
			}
		}

		for (int i = 0; i < newEmptyWindowsCount; ++i)
		{
			CreateNewWindow();
		}
		for (const auto& path : filesToOpen)
		{
			auto doc = std::make_unique<DocumentContext>();
			doc->controller->LoadDocument(path);
			m_documents.push_back(std::move(doc));
		}
	}
}