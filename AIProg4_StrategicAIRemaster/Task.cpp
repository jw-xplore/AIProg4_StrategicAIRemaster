#include "Task.h"

Task::Task(std::initializer_list<SubTaskFn> subtasks)
{
	running = true;
	currentSubTask = 0;
	this->subtasks = subtasks;
	repeat = false;
}

Task::~Task()
{
	subtasks.clear();
}

void Task::Update(float dTime)
{
	if (!running)
		return;

	if (currentSubTask >= subtasks.size())
	{
		// Repeat
		if (repeat)
			currentSubTask = 0;

		// Stop execution
		return;
	}

	// Run and progress subtask
	ESubtaskState state = subtasks[currentSubTask](*assignee, dTime);

	if (state == ESubtaskState::Canceled)
	{
		// End task execution
		running = false;
		return;
	}

	if (state == ESubtaskState::Finnished || state == ESubtaskState::Skipped)
		currentSubTask++;
}

void Task::Cancel()
{
	running = true;
}