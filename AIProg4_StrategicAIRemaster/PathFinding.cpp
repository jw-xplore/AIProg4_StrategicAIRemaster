#include "PathFinding.h"
#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "World.h";
#include "Constants.h"
#include "Database.h"

PathFinding::PathFinding(World& world)
{
	this->world = &world;
	width = world.width;
	height = world.height;

	nodes = new Node * [height];

	// Define node graph
	for (int y = 0; y < height; y++)
	{
		nodes[y] = new Node[width];

		for (int x = 0; x < width; x++)
		{
			nodes[y][x].x = x;
			nodes[y][x].y = y;

			ETerrainType type = world.mapTerrain[x][y];
			float cost = GameDB::Database::Instance()->terrains[type].cost;

			nodes[y][x].travelCost = cost;
		}
	}

	// Define connections
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (nodes[y][x].travelCost != -1)
				AddConnectionsToNode(&nodes[y][x], x, y);
		}
	}
}

PathFinding::~PathFinding()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int ns = nodes[y][x].connections.size();

			/*
			for (int n = 0; n < ns; n++)
			{
				delete nodes[y][x].connections[n];
			}
			*/

			nodes[y][x].connections.clear();
		}
	}

	for (int y = 0; y < height; y++)
	{
		delete[] nodes[y];
	}

	delete nodes;
	lastSearch.clear();
}

/*
Add connections to all neightbouring nodes
*/
void PathFinding::AddConnectionsToNode(Node* node, int x, int y)
{
	int w = width;
	int h = height;

	// Top
	if (y < h - 1 && nodes[y + 1][x].travelCost != -1)
	{
		Connection link;
		link.fromNode = node;
		link.node = &nodes[y + 1][x];
		link.weight = link.node->travelCost;

		node->connections.push_back(link);
	}

	if (y < h - 1 && x < w - 1 && nodes[y + 1][x + 1].travelCost != -1 && nodes[y + 1][x].travelCost != -1 && nodes[y][x + 1].travelCost != -1)
	{
		Connection link;
		link.fromNode = node;
		link.node = &nodes[y + 1][x + 1];
		link.weight = 1.4f * link.node->travelCost;

		node->connections.push_back(link);
	}

	// Right
	if (x < w - 1 && nodes[y][x + 1].travelCost != -1)
	{
		Connection link;
		link.fromNode = node;
		link.node = &nodes[y][x + 1];
		link.weight = link.node->travelCost;

		node->connections.push_back(link);
	}

	if (y > 0 && x < w - 1 && nodes[y - 1][x + 1].travelCost != -1 && nodes[y - 1][x].travelCost != -1 && nodes[y][x + 1].travelCost != -1)
	{
		Connection link;
		link.fromNode = node;
		link.node = &nodes[y - 1][x + 1];
		link.weight = 1.4f * link.node->travelCost;

		node->connections.push_back(link);
	}

	// Bottom
	if (y > 0 && nodes[y - 1][x].travelCost != -1)
	{
		Connection link;
		link.fromNode = node;
		link.node = &nodes[y - 1][x];
		link.weight = link.node->travelCost;

		node->connections.push_back(link);
	}

	if (y > 0 && x > 0 && nodes[y - 1][x - 1].travelCost != -1 && nodes[y - 1][x].travelCost != -1 && nodes[y][x - 1].travelCost != -1)
	{
		Connection link;
		link.fromNode = node;
		link.node = &nodes[y - 1][x - 1];
		link.weight = 1.4f * link.node->travelCost;

		node->connections.push_back(link);
	}

	// Left
	if (x > 0 && nodes[y][x - 1].travelCost != -1)
	{
		Connection link;
		link.fromNode = node;
		link.node = &nodes[y][x - 1];
		link.weight = link.node->travelCost;

		node->connections.push_back(link);
	}

	if (y < h - 1 && x > 0 && nodes[y + 1][x - 1].travelCost != -1 && nodes[y][x - 1].travelCost != -1 && nodes[y + 1][x].travelCost != -1)
	{
		Connection link;
		link.fromNode = node;
		link.node = &nodes[y + 1][x - 1];
		link.weight = 1.4f * link.node->travelCost;

		node->connections.push_back(link);
	}
}

/*
Debug draw to display connectections in the graph
*/

void PathFinding::DrawGraph()
{
	int tileSize = GlobalVars::TILE_SIZE;
	int halfSize = GlobalVars::TILE_SIZE / 2;

	//std::vector<Connection>& cons;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Vector2 nodePos = { x * tileSize + halfSize, y * tileSize + halfSize };

			std::vector<Connection>& cons = nodes[y][x].connections;

			for (int n = 0; n < cons.size(); n++)
			{
				Node* xt = cons[n].node;
				Vector2 pos = { cons[n].node->x * tileSize + halfSize, cons[n].node->y * tileSize + halfSize };

				Color col = YELLOW;
				if (cons[n].weight > 1.5f)
					col = RED;

				DrawLine(nodePos.x, nodePos.y, pos.x, pos.y, col);
			}
		}
	}
}

std::vector<Node>* PathFinding::AStar(Vector2 start, Vector2 end)
{
	//lastSearch.clear();
	int visitsCounter = 0;

	// Find start and end
	int sx = (int)start.x / GlobalVars::TILE_SIZE;
	int sy = (int)start.y / GlobalVars::TILE_SIZE;
	Node* startNode = &nodes[sy][sx];

	int ex = (int)end.x / GlobalVars::TILE_SIZE;
	int ey = (int)end.y / GlobalVars::TILE_SIZE;
	Node* endNode = &nodes[ey][ex];

	std::cout << "start: " << sx << ", " << sy << " | end: " << ex << ", " << ey << "\n";

	// Initialize start node
	NodeRecordAs startRecord;
	startRecord.node = startNode;
	startRecord.costSoFar = 0;
	startRecord.costEstimated = ManhattanHeuristic(startNode, endNode);

	// Setup open and closed list
	std::vector<NodeRecordAs> open;
	open.push_back(startRecord);
	//std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare> open;

	std::vector<NodeRecordAs> closed;
	NodeRecordAs current;
	
	//int counter = 0;

	while (open.size() != 0)
	{
		visitsCounter++;
		//counter++;

		// Find smallest record - smallest estimated cost
		current = SmallestAsRecord(open);
		lastSearch.push_back(*current.node);

		// Is at the end?
		if (current.node == endNode)
			break;

		// Loop through connections
		for (int i = 0; i < current.node->connections.size(); i++)
		{
			Connection* connection = &current.node->connections[i];

			Node* currentNode = connection->node;
			NodeRecordAs currentNodeRecord;
			float currentNodeCost = current.costSoFar + connection->weight;

			float currentNodeHeuristics;

			// Check node in closed list
			if (ContainsAsRecord(closed, currentNode, currentNodeRecord))
			{
				// Check if there is shorter route
				if (currentNodeRecord.costSoFar <= currentNodeCost)
					continue;

				// Remove from closed list if it is shortest path
				closed.erase(std::remove(closed.begin(), closed.end(), currentNodeRecord), closed.end());

				currentNodeHeuristics = currentNodeRecord.costEstimated - currentNodeRecord.costSoFar;
			}
			else if (ContainsAsRecord(open, currentNode, currentNodeRecord)) // Skip if the node is open and we ve not found a better route
			{
				// Skip if route is not better
				if (currentNodeRecord.costSoFar <= currentNodeCost)
					continue;

				currentNodeHeuristics = connection->weight - currentNodeRecord.costSoFar;
			}
			else // Record unvisited node
			{
				currentNodeRecord = NodeRecordAs();
				currentNodeRecord.node = currentNode;

				currentNodeHeuristics = ManhattanHeuristic(startNode, endNode);
			}

			// Update node cost, estimate and connection
			currentNodeRecord.connection = connection;
			currentNodeRecord.costEstimated = currentNodeCost + currentNodeHeuristics;

			// Add to open list 
			if (!ContainsAsRecord(open, currentNode, currentNodeRecord))
			{
				open.push_back(currentNodeRecord);
			}
		}

		// Release current node when done with iterating through connections
		open.erase(std::remove(open.begin(), open.end(), current), open.end());
		closed.push_back(current);
	}

	// Format path
	std::vector<Node>* path = new std::vector<Node>;

	// Failed to find end?
	if (current.node != endNode)
		return path; // Empty path

	// Track path
	while (current.node != startNode)
	{
		path->push_back(*current.node);
		current = *FindAsRecordFromNode(closed, current.connection->fromNode);
	}

	return path;
}

inline int PathFinding::ManhattanHeuristic(const Node* start, const Node* end)
{
	//return std::abs(end->x - start->x) + std::abs(end->y - start->y);
	return std::sqrt((end->x - start->x) ^ 2 + (end->y - start->y) ^ 2);
	//int x = end->x - start->x;
	//int y = end->y - start->y;

	//return (end->x - start->x) ^ 2 + (end->y - start->y) ^ 2;

	//return std::pow(end->x - start->x, 2) + std::pow(end->y - start->y, 2);
}

NodeRecordAs PathFinding::SmallestAsRecord(std::vector<NodeRecordAs>& list)
{
	NodeRecordAs record = list[0];
	int size = list.size();

	for (int i = 1; i < size; i++)
	{
		if (record.costEstimated > list[i].costEstimated)
			record = list[i];
	}

	return record;
}

bool PathFinding::ContainsAsRecord(const std::vector<NodeRecordAs>& list, Node* node, NodeRecordAs& outRecord)
{
	for (auto& record : list)
	{
		if (record.node == node)
		{
			outRecord = record;
			return true;
		}
	}

	return false;
}

NodeRecordAs* PathFinding::FindAsRecordFromNode(std::vector<NodeRecordAs>& list, Node* node)
{
	for (auto& record : list)
	{
		if (record.node == node)
			return &record;
	}

	return nullptr;
}