#pragma once

#include <cstddef>
#include <deque>
#include <memory>
#include <vector>
#include <string>

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void Execute() = 0;
	virtual void Unexecute() = 0;
	virtual std::vector<std::string> AffectedIds() const { return {}; }
};

class CommandHistory
{
public:
	CommandHistory() = default;

	CommandHistory(const CommandHistory&) = delete;
	CommandHistory& operator=(const CommandHistory&) = delete;

	void Execute(std::unique_ptr<ICommand> command);
	std::vector<std::string> Undo();
	std::vector<std::string> Redo();
	void Clear();

	bool CanUndo() const;
	bool CanRedo() const;
	void PushExecuted(std::unique_ptr<ICommand> command);

private:
	std::deque<std::unique_ptr<ICommand>> m_history;
	std::size_t m_nextCommandIndex = 0;
};