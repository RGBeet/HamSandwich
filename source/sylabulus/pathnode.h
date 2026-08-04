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

	PathNode* parent;

	PathNode()
	{
		walkable = true;
		gcost = hcost = fcost = 0;
		opened = closed = false;
		parent = nullptr;
	}
};

#endif
