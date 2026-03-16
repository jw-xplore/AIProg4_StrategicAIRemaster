#include "Task.h"

Task::Task(Worker* worker, std::initializer_list<Subtask*> subtasks)
{
	running = true;
	currentSubTask = 0;
	repeat = false;
	assignee = worker;

	this->subtasks = subtasks;
}

Task::Task(const Task& rhs)
{

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
		if (repeat)
		{
			// Repeat
			currentSubTask = 0;
		}
		else
		{
			// Stop execution - All subtasks done
			running = false;
			finished = true;
		}

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