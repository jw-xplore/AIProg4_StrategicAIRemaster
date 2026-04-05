#include "GoalSystem.h"
#include "SystemsHolder.h"
#include "World.h";
#include "EntityManager.h"
#include "Building.h"

//--------------------------------------------------------------
// Task
//--------------------------------------------------------------

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

bool Task::IsInputSatisfied()
{
	EntityManager* entityMngr = SystemsHolder::GetInstance()->entityMananger;

	for (const TaskAttribute& attribute : input)
	{
		// Capital
		if (attribute.category == ETaskAttributeCategory::Capital)
		{
			Capital::ECapitalType type = (Capital::ECapitalType)attribute.type;

			// For world
			if (!attribute.source)
			{
				// Find single free pickup - In world we don't care about mutliple items
				if (!entityMngr->FindFreePickupOfType(type))
					return false;
			}
			else
			{
				// For building
				if (attribute.source->storedCapital[type] > attribute.amount)
					return false;
			}

			continue;
		}

		// Worker
		if (attribute.category == ETaskAttributeCategory::Worker)
		{
			if (!entityMngr->FindWorkerOfRole((EWorkerRole)attribute.type))
				return false;

			continue;
		}

		// Building
		if (attribute.category == ETaskAttributeCategory::Building)
		{
			if (!entityMngr->FindFinishedBuildingOfType((EBuildingType)attribute.type))
				return false;

			continue;
		}
	}

	// All fullfilled
	return true;
}

//--------------------------------------------------------------
// Goal
//--------------------------------------------------------------

Goal::Goal(Task& finalTask, std::vector<Task>& availableTasks)
{
	this->finalTask = &finalTask;

	DefineTaskChain(*this->finalTask, availableTasks);
}

void Goal::DefineTaskChain(Task& currentTask, std::vector<Task>& availableTasks)
{
	// No input criteria needed = Regular worker can do immediately
	if (currentTask.input.empty())
		return;

	// Define previous task for each input to safisfy current criteria
	for (const TaskAttribute& input : currentTask.input)
	{
		for (Task& task : availableTasks)
		{
			// Found task satisfying input criteria
			if (task.output.category == input.category && task.output.type == input.type && task.output.source == input.source)
			{
				currentTask.previousTasks.push_back(&task);
				DefineTaskChain(task, availableTasks);
				break;
			}
		}
	}
}

Task* Goal::NextAvailableTask()
{
	return NextAvailableTask(*finalTask);
}

Task* Goal::NextAvailableTask(Task& currentTask)
{
	// This is done - Should work only with last one
	if (currentTask.finished)
		return nullptr;

	// Check not started previous tasks
	int inputId = 0;

	for (Task*& task : currentTask.previousTasks)
	{
		if (task->IsInputSatisfied())
		{
			if (!task->finished && !task->running)
				return NextAvailableTask(*task);
		}

		inputId++;
	}

	// If some of tasks is in progress but unfinished - return null and wait for completion
	for (Task*& task : currentTask.previousTasks)
	{
		if (task->running)
			return nullptr;
	}

	// All previous tasks are done
	return &currentTask;
}

void Goal::DebugDraw(Task& task, int posX = 0, int posY = 0)
{
	std::string strZoom = "Zoom";
	char const* cZoom = strZoom.c_str();

	Color coloring = RED;
	if (task.running)
		coloring = YELLOW;
	if (task.finished)
		coloring = GREEN;

	DrawText(cZoom, posX * 50, posY * 50, 20, coloring);

	int addY = 0;

	for (Task*& prev : task.previousTasks)
	{
		DebugDraw(*prev, posX + 1, posY + addY);
		addY++;
	}
}