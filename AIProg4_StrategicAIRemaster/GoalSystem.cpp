#include "GoalSystem.h"
#include "SystemsHolder.h"
#include "World.h";
#include "EntityManager.h"
#include "Building.h"
#include <stdexcept>
#include "Commander.h"
#include "Worker.h"

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

			for (auto& listener : onFinishedListeners)
			{
				listener(this);
			}
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

//--------------------------------------------------------------
// Goal step
//--------------------------------------------------------------

GoalStep::GoalStep(GoalStep& source)
{
	this->name = source.name;

	this->requirements = source.requirements;
	this->output = source.output;

	this->taskFunc = std::function<Task* (Worker*)>(source.taskFunc);
}

GoalStep::GoalStep(std::string name, std::initializer_list<TaskAttribute> requirements, TaskAttribute output, std::function<Task* (Worker*)> taskFunc)
{
	this->name = name;

	this->requirements = requirements;
	this->output = output;

	this->taskFunc = taskFunc;
}

bool GoalStep::IsInputSatisfied()
{
	EntityManager* entityMngr = SystemsHolder::GetInstance()->entityMananger;

	for (const TaskAttribute& attribute : requirements)
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
				if (attribute.source->storedCapital[type] < attribute.amount)
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

bool GoalStep::IsActive()
{
	if (totalTasks <= 0)
		return false;

	return activeTasks.size() > 0 && finishedTasks < totalTasks;
}

bool GoalStep::IsDone()
{
	if (totalTasks == 0)
		return false;

	if (finishedTasks > totalTasks)
		throw std::runtime_error("Goal step finished tasks overflow!");

	return finishedTasks == totalTasks;
}

void GoalStep::AssignTask()
{
	Commander* commander = SystemsHolder::GetInstance()->commander;
	Worker* worker = commander->FindFreeWorker(roleConstrain);

	if (worker)
	{
		Task* task = taskFunc(worker);
		task->parentGoalStep = this;
		commander->AssignTask(worker, task);

		// Push active task
		activeTasks.push_back(task);
		task->onFinishedListeners.push_back(
			[&](Task* task) { OnTaskFinished(task); }
		);
	}
}

void GoalStep::OnTaskFinished(Task* task)
{
	activeTasks.erase(std::remove(std::begin(activeTasks), std::end(activeTasks), task), std::end(activeTasks));
}

//--------------------------------------------------------------
// Goal
//--------------------------------------------------------------

Goal::Goal()
{

}

Goal::Goal(GoalStep& finalStep, std::vector<GoalStep*>& availableSteps)
{
	this->finalStep = new GoalStep(finalStep);
	this->finalStep->totalTasks = 1;

	DefineTaskChain(*this->finalStep, availableSteps);
}

void Goal::DefineTaskChain(GoalStep& currentStep, std::vector<GoalStep*>& availableSteps)
{
	// No input criteria needed = Regular worker can do immediately
	if (currentStep.requirements.empty())
		return;

	// Define previous task for each input to safisfy current criteria
	for (const TaskAttribute& input : currentStep.requirements)
	{
		for (GoalStep*& step : availableSteps)
		{
			// Compare required source
			bool sourcePass = step->output.source == input.source;
			if (!sourcePass)
				sourcePass = step->output.source == nullptr && step->dynamicSource;

			// Found task satisfying input criteria
			if (step->output.category == input.category && step->output.type == input.type && sourcePass)
			{
				GoalStep* newStep = new GoalStep(*step);

				// Dynamic source building
				if (newStep->output.source == nullptr && step->dynamicSource)
					newStep->output.source = input.source;

				newStep->totalTasks = input.amount * currentStep.totalTasks; // TODO: Multipliying with previous steps amounts

				currentStep.previousSteps.push_back(newStep);
				DefineTaskChain(*newStep, availableSteps);
				break;
			}
		}
	}
}

GoalStep* Goal::NextAvailableStep()
{
	return NextAvailableStep(*finalStep);
}

GoalStep* Goal::NextAvailableStep(GoalStep& currentStep)
{
	// This is done - Should work only with first step in goal chain = goal is done
	if (currentStep.IsDone())
		return nullptr;

	// Can perform this step?
	if (currentStep.IsInputSatisfied())
	{
		// Is there work to be done? 
		int assigned = currentStep.activeTasks.size() + currentStep.finishedTasks;
		if (assigned < currentStep.totalTasks)
			return &currentStep;
	}

	// Check not started previous tasks
	for (GoalStep*& step : currentStep.previousSteps)
	{
		if (!step->IsDone())
		{
			return NextAvailableStep(*step);
		}
	}

	// Is active and fully occupied - wait as nothing can be done now
	return nullptr;
}

void Goal::DebugDraw(GoalStep& step, int posX, int posY)
{
	std::string buildingStr = "Non";
	if (step.output.source)
		buildingStr = std::to_string(step.output.source->type);

	std::string str = step.name + " - " + buildingStr;
	char const* cZoom = str.c_str();

	Color coloring = RED;
	if (step.IsActive())
		coloring = YELLOW;
	if (step.IsDone())
		coloring = GREEN;

	DrawText(cZoom, posX * 250, posY * 50, 20, coloring);

	int addY = 0;

	for (GoalStep*& prev : step.previousSteps)
	{
		DebugDraw(*prev, posX + 1, posY + addY);
		addY++;
	}
}