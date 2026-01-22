#pragma once
#include <vector>
#include <functional>

class Worker;

enum ESubtaskState
{
	Running,
	Finnished,
	Skipped,
	Canceled,
	ESubtaskStateCount
};

using SubTaskFn = std::function<ESubtaskState(Worker&, float dTime)>;

// Holds several subtasks and manage their step by step execution
class Task
{
public:
	bool running;
	int currentSubTask = 0;
	std::vector<SubTaskFn> subtasks;
	bool repeat;

	Worker* assignee;

	Task(std::initializer_list<SubTaskFn> subtasks);
	~Task();

	void Update(float dTime);
	void Cancel();
};