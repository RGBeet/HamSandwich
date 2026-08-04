#include <algorithm>
#include <cmath>
#include <climits>
#include <queue>
#include <vector>

#include "map.h"
#include "pathfinding.h"
#include "pathnode.h"
#include "guy.h"


Pathfinder pathfinder;

Pathfinder *GetPathfinder()
{
	return &pathfinder;
}

struct CompareNodes
{
	bool operator()(const PathNode* a, const PathNode* b) const
	{
		return a->fcost > b->fcost;
	}
};

Pathfinder::Pathfinder()
{
	map		= nullptr;
	world	= nullptr;
}

void Pathfinder::SetMap(Map* m, world_t* w)
{
	this->map	= m;
	this->world = w;
}

int Pathfinder::Heuristic(int x1, int y1, int x2, int y2)
{
	int dx = abs(x1 - x2);
	int dy = abs(y1 - y2);

	int diagonal = std::min(dx, dy);
	int straight = std::max(dx, dy) - diagonal;

	return diagonal * 14 + straight * 10;
}

int Pathfinder::MovementCost(PathNode* from, PathNode* to)
{
	if (from->x != to->x && from->y != to->y)
		return 14; // diagonal

	return 10; // straight
}

void Pathfinder::ResetNodes(Map* map)
{
	if (!map)
	{
		printf("WARNING! No map for Pathfinder\n");
		return;
	}
	for (auto& node : map->nodes)
	{
		node.opened = false;
		node.closed = false;
		node.gcost = 0;
		node.hcost = 0;
		node.fcost = 0;
		node.parent = nullptr;
	}
}

void Pathfinder::BuildPath(PathNode* endNode, std::vector<PathNode*>& path)
{
	int safety = 0;
	while (endNode)
	{
		path.push_back(endNode);
		endNode = endNode->parent;
		safety++;

		if (safety > 1000)
		{
			printf("BAD PATH LOOP!\n");
			break;
		}
	}
	std::reverse(path.begin(),path.end());
}

bool Pathfinder::FindPath(Guy *me, int startX, int startY, int endX, int endY, std::vector<PathNode*>& path)
{
	if (!map)
	{
		printf("ERROR: Pathfinder has no map\n");
		return false;
	}

	path.clear();
	ResetNodes(map);

	PathNode* startNode = map->GetNode(startX, startY);
	PathNode* endNode	= map->GetNode(endX, endY);

	if (!startNode || !endNode || !endNode->walkable)
		return false;

	std::priority_queue<PathNode*, std::vector<PathNode*>, CompareNodes> openSet;

	startNode->gcost	= 0;
	startNode->hcost	= Heuristic(startX, startY, endX, endY);
	startNode->fcost	= startNode->gcost + startNode->hcost; // ???
	startNode->opened	= true;

	openSet.push(startNode);

	const int offsets[8][2] =
	{
		{ 1, 0},
		{ 1, 1},
		{ 0, 1},
		{-1, 1},
		{-1, 0},
		{-1,-1},
		{ 0,-1},
		{ 1,-1}
	};

	while (!openSet.empty())
	{
		PathNode* current = openSet.top();
		openSet.pop();

		if (current->closed)
			continue;

		current->closed = true;

		if (current == endNode)
		{
			BuildPath(endNode, path);
			return true;
		}

		for (int i = 0; i < 8; i++)
		{
			int nx = current->x + offsets[i][0];
			int ny = current->y + offsets[i][1];

			PathNode* neighbor	= map->GetNode(nx, ny);
			byte canWalk		= me->CanWalk(nx, ny, map, world);

			if (!neighbor || !neighbor->walkable || neighbor->closed)
				continue;

			// Only check when moving diagonally
			if (nx != current->x && ny != current->y)
			{
				PathNode* horizontal	= map->GetNode(nx, current->y);
				PathNode* vertical		= map->GetNode(current->x, ny);

				// Prevent cutting through corners
				if (!horizontal || !vertical ||
					!horizontal->walkable ||
					!vertical->walkable)
				{
					continue;
				}
			}

			int newCost = current->gcost + MovementCost(current, neighbor);

			if (neighbor->opened && newCost >= neighbor->gcost)
				continue;

			neighbor->gcost		= newCost;
			neighbor->hcost		= Heuristic(nx, ny, endX, endY);
			neighbor->fcost		= neighbor->gcost + neighbor->hcost;

			neighbor->parent	= current;
			neighbor->opened	= true;

			openSet.push(neighbor);
		}
	}
	return false; // No path found
}
