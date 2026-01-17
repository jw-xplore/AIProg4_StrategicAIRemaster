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

std::vector<Node>* PathFinding::AStarDivided(Vector2 start, Vector2 end, std::map<Node*, NodeRecordAs>& searchResult, std::priority_queue<NodeRecordAs, std::vector<NodeRecordAs>, NodeRecordAsCompare>& open)
{
	int visitsCounter = 0;

	// Find start and end
	Node* startNode = nullptr;

	int ex = (int)end.x / GlobalVars::TILE_SIZE;
	int ey = (int)end.y / GlobalVars::TILE_SIZE;
	Node* endNode = &nodes[ey][ex];;

	if (searchResult.empty())
	{
		// Find start
		int sx = (int)start.x / GlobalVars::TILE_SIZE;
		int sy = (int)start.y / GlobalVars::TILE_SIZE;
		startNode = &nodes[sy][sx];

		// Initialize start node
		NodeRecordAs startRecord;
		startRecord.node = startNode;
		startRecord.costSoFar = 0;
		startRecord.costEstimated = ManhattanHeuristic(startNode, endNode);
		startRecord.state = ENodeRecordState::Open;

		open.push(startRecord);
	}

	NodeRecordAs current;

	// Visit nodes
	while (open.size() != 0)
	{
		visitsCounter++;
		if (visitsCounter > 1000)
			break;

		// Pick node with smallest estimate cost
		current = open.top();
		open.pop();

		// Is at the end?
		if (current.node == endNode)
			break;

		int connectionsCounter = 0;

		// Loop through connections
		for (auto& connection : current.node->connections)
		{
			connectionsCounter++;
			Node* connectionNode = connection.node;
			NodeRecordAs connectionRecord;
			connectionRecord.state = ENodeRecordState::Unvisited;

			if (searchResult.find(connectionNode) != searchResult.end())
				connectionRecord = searchResult[connectionNode];

			float connectionCost = current.costSoFar + connection.weight;
			float connectionHeuristics;

			// Setup based on visited state
			if (connectionRecord.state == ENodeRecordState::Unvisited)
			{
				connectionRecord = NodeRecordAs();
				connectionRecord.node = connectionNode;

				connectionHeuristics = ManhattanHeuristic(connectionNode, endNode);
			}
			else if (connectionRecord.state == ENodeRecordState::Closed)
			{
				// Check if there is shorter route
				if (connectionRecord.costSoFar <= connectionCost)
					continue;

				// Reopen if leads to shorter path
				connectionRecord.state = ENodeRecordState::Open;

				connectionHeuristics = connectionRecord.costEstimated - connectionRecord.costSoFar;
			}
			else if (connectionRecord.state == ENodeRecordState::Open)
			{
				// Skip if route is not better
				if (connectionRecord.costSoFar <= connectionCost)
					continue;

				connectionHeuristics = connection.weight - connectionRecord.costSoFar;
			}

			// Update node cost, estimate and connection
			connectionRecord.connection = &connection;
			connectionRecord.costEstimated = connectionCost + connectionHeuristics;

			// Add to open list 
			if (connectionRecord.state != ENodeRecordState::Open)
			{
				connectionRecord.state = ENodeRecordState::Open;
				open.push(connectionRecord);
				searchResult[connectionRecord.node] = connectionRecord;
			}
		}

		// Close current node
		current.state = ENodeRecordState::Closed;
		searchResult[current.node] = current;
	}

	// Return empty path if search wasn't finished
	if (current.node != endNode)
		return {};

	// Format path
	std::vector<Node>* path = new std::vector<Node>;

	// Failed to find end?
	if (current.node != endNode)
		return path; // Empty path

	if (!startNode)
	{
		int sx = (int)start.x / GlobalVars::TILE_SIZE;
		int sy = (int)start.y / GlobalVars::TILE_SIZE;
		startNode = &nodes[sy][sx];
	}

	// Track path
	while (current.node != startNode)
	{
		path->push_back(*current.node);
		current = searchResult[current.connection->fromNode];
	}

	// Cleanup
	searchResult.clear();
	while (!open.empty())
		open.pop();

	return path;
}

inline float PathFinding::ManhattanHeuristic(const Node* start, const Node* end)
{
	float noSq = std::powf(end->x - start->x, 2) + std::powf(end->y - start->y, 2);
	return std::sqrt(noSq);
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