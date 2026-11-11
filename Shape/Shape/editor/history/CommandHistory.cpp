#include "CommandHistory.h"

#include <stdexcept>

void CommandHistory::Execute(std::unique_ptr<ICommand> command)
{
	if (!command)
	{
		throw std::invalid_argument("Command cannot be null");
	}

	if (m_nextCommandIndex < m_history.size())
	{
		m_history.erase(m_history.begin() + m_nextCommandIndex, m_history.end());
	}

	command->Execute();
	m_history.push_back(std::move(command));
	m_nextCommandIndex = m_history.size();
}

std::vector<std::string> CommandHistory::Undo()
{
	if (m_nextCommandIndex == 0) return {};
	const auto ids = m_history[m_nextCommandIndex - 1]->AffectedIds();
	m_history[m_nextCommandIndex - 1]->Unexecute();
	--m_nextCommandIndex;
	return ids;
}

std::vector<std::string> CommandHistory::Redo()
{
	if (m_nextCommandIndex >= m_history.size()) return {};
	const auto ids = m_history[m_nextCommandIndex]->AffectedIds();
	m_history[m_nextCommandIndex]->Execute();
	++m_nextCommandIndex;
	return ids;
}

void CommandHistory::Clear()
{
	m_history.clear();
	m_nextCommandIndex = 0;
}

bool CommandHistory::CanUndo() const
{
	return m_nextCommandIndex > 0;
}

bool CommandHistory::CanRedo() const
{
	return m_nextCommandIndex < m_history.size();
}

void CommandHistory::PushExecuted(std::unique_ptr<ICommand> command)
{
	if (!command)
	{
		throw std::invalid_argument("Command cannot be null");
	}

	if (m_nextCommandIndex < m_history.size())
	{
		m_history.erase(m_history.begin() + static_cast<std::ptrdiff_t>(m_nextCommandIndex),
			m_history.end());
	}

	m_history.push_back(std::move(command));
	m_nextCommandIndex = m_history.size();
}