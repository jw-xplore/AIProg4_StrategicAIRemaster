#pragma once
/*
Takes care of high-level decision making and assigning tasks
*/

#include <vector>
#include <queue>
#include <map>
#include <algorithm>

class Worker;
enum EWorkerRole;
class Task;
enum EBuildingType;

struct Node;
struct NodeRecordAs;
struct NodeRecordAsCompare;
class PathFinding;

class Commander
{
public:
	std::vector<std::queue<Worker*>> freeWorkers; // Queue of free workers divided by roles
	std::queue<Task*> pendingTasks;
	std::vector<Task*> activeTasks;
	std::queue<EBuildingType> neededBuildings;

	std::map<Node*, NodeRecordAs>* searchResult;
	std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare>* open;

	Commander();
	~Commander();

	void Update(float dTime);

	void RegisterFreeWorker(Worker* worker);
};

