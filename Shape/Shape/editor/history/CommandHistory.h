#pragma once

#include <cstddef>
#include <deque>
#include <memory>

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void Execute() = 0;
	virtual void Unexecute() = 0;
};

class CommandHistory
{
public:
	CommandHistory() = default;

	CommandHistory(const CommandHistory&) = delete;
	CommandHistory& operator=(const CommandHistory&) = delete;

	void Execute(std::unique_ptr<ICommand> command);
	void Undo();
	void Redo();
	void Clear();

	bool CanUndo() const;
	bool CanRedo() const;
	void PushExecuted(std::unique_ptr<ICommand> command);

private:
	std::deque<std::unique_ptr<ICommand>> m_history;
	std::size_t m_nextCommandIndex = 0;
};