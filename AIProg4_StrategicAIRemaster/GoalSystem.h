#pragma once
#include <vector>
#include <map>
#include <tuple>
#include <functional>
#include "Capital.h"
#include <string>

class Worker;
class Building;

enum ESubtaskState
{
	Running,
	Finnished,
	Skipped,
	Canceled,
	ESubtaskStateCount
};

using SubTaskFn = std::function<ESubtaskState(Worker&, float dTime)>;

/// <summary>
/// Single purpose action like arrive, pickup, craft/create a material
/// </summary>
class Subtask
{
public:
	Subtask() {}
	~Subtask() {}

	virtual ESubtaskState Execute(Worker& worker, float dTime) { return ESubtaskState::Finnished; }

};

enum class ETaskAttributeCategory
{
	None = -1,
	Capital,
	Worker,
	Building,
};

struct TaskAttribute
{
	ETaskAttributeCategory category = ETaskAttributeCategory::None;
	int type = -1; // Id of type to given category
	int amount = 0;
	Building* source = nullptr; // Nullptr will result in checking availability in world
};

/// <summary>
/// Holds several subtasks and manage their step by step execution
/// Define required inputs and rewards
/// </summary>
class Task
{
public:
	bool finished;
	bool running;
	int currentSubTask = 0;
	bool repeat;
	std::string name = "";

	Worker* assignee;
	std::vector<Subtask*> subtasks;

	std::vector<TaskAttribute> input;
	std::vector<TaskAttribute> potentialInput;
	TaskAttribute output;

	std::vector<Task*> previousTasks;
	Task* nextTask;

	Task(Worker* worker, std::initializer_list<Subtask*> subtasks);
	Task(const Task& rhs);
	~Task();

	void Update(float dTime);
	void Cancel();

	bool IsInputSatisfied();
};

/// <summary>
/// 
/// </summary>
class Goal
{
public:
	unsigned short priority;
	Task* finalTask; // Linked list to previous tasks, finishing this task will complete the goal 

	Goal() {}
	Goal(Task& finalTask, std::vector<Task>& availableTasks);

	void DefineTaskChain(Task& currentTask, std::vector<Task>& availableTasks);
	Task* NextAvailableTask();
	Task* NextAvailableTask(Task& currentTask);
	bool IsCompleted();

	void DebugDraw(Task& task, int posX, int posY);
};

