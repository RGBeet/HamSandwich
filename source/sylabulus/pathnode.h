#ifndef PATHNODE_H
#define PATHNODE_H

struct PathNode
{
	int x, y;

	int gcost;	// Cost from start node
	int hcost;	// Heuristic cost to end node
	int fcost;	// g + h;

	bool walkable;
	bool opened;
	bool closed;
	byte clearance;

	PathNode* parent;

	PathNode()
	{
		walkable = true;
		gcost = hcost = fcost = 0;
		opened = closed = false;
		parent = nullptr;
	}

	public:
		bool CanFit(Guy* guy, Map* map, world_t* world);
};

#endif
