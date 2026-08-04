#ifndef PATHFINDING_H
#define PATHFINDING_H
#include "map.h"
#include "pathnode.h"

class Pathfinder final
{
	public:
		Pathfinder();
		bool FindPath(Guy *me, int startX, int startY, int endX, int endY, std::vector<PathNode*> &path);
		void SetMap(Map* map, world_t* world);
	private:
		Map* map;
		world_t* world;
		int Heuristic(int x1, int y1, int x2, int y2);
		int MovementCost(PathNode* from, PathNode* to);
		void BuildPath(PathNode* endNode, std::vector<PathNode*>& path);
		void ResetNodes(Map* map);
};

Pathfinder* GetPathfinder(void);

#endif
