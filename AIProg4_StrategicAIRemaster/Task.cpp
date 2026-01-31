#include "Task.h"

Task::Task(std::initializer_list<Subtask*> subtasks)
{
	running = true;
	currentSubTask = 0;
	repeat = false;

	this->subtasks = subtasks;
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
	//ESubtaskState state = subtasks[currentSubTask](*assignee, dTime);
	ESubtaskState state = subtasks[currentSubTask]->Execute(*assignee, dTime);

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