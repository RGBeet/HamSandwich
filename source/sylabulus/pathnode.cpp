#include "map.h"
#include "pathfinding.h"
#include "pathnode.h"
#include "guy.h"
#include "winpch.h"

// Can the guy fit within the path node? Ensures larger Guys can move around with ease.
bool PathNode::CanFit(Guy* guy, Map* map, world_t* world)
{
	int centerX = (x * TILE_WIDTH + TILE_WIDTH / 2) << FIXSHIFT;
	int centerY = (y * TILE_HEIGHT + TILE_HEIGHT / 2) << FIXSHIFT;

	int checks[5][2] =
	{
		{0,0},
		{16,0},
		{-16,0},
		{0,12},
		{0,-12}
	};

	for (int i = 0; i < 5; i++)
	{
		int xx = centerX + (checks[i][0] << FIXSHIFT);
		int yy = centerY + (checks[i][1] << FIXSHIFT);

		if (!guy->CanWalkPath(xx, yy, map, world))
			return false;
	}

	return true;
}
