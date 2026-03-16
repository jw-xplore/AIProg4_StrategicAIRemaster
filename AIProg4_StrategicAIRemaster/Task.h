#pragma once
#include <vector>
#include <functional>
#include "Capital.h"

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

class Subtask
{
public:
	Subtask() {}
	~Subtask() {}

	virtual ESubtaskState Execute(Worker& worker, float dTime) { return ESubtaskState::Finnished; }

};

// Holds several subtasks and manage their step by step execution
class Task
{
public:
	bool finished;
	bool running;
	int currentSubTask = 0;
	bool repeat;

	Worker* assignee;
	std::vector<Subtask*> subtasks;
	Capital::CapitalAmounts rewardCapital;

	Task(Worker* worker, std::initializer_list<Subtask*> subtasks);
	Task(const Task& rhs);
	~Task();

	void Update(float dTime);
	void Cancel();
};