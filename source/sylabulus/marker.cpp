#include "winpch.h"
#include "special.h"
#include "marker.h"
#include "monster.h"
#include "repair.h"
#include "guy.h"
#include "message.h"
#include "sound.h"
#include "game.h"
#include "dialogbits.h"
#include "vars.h"
#include "editor.h"
#include "world.h"
#include "shop.h"
#include "goal.h"
#include "palettes.h"
#include "chat.h"
#include "world.h"
#include "worldselect.h"
#include "particle.h"
#include "display.h"
#include <climits>

static std::span<marker_t> mrkr;		// Local specials
static int numMarkers;                  // Specials >= this aren't set.

byte GetMarkerCount()
{
	return (byte)numMarkers;
}

void SetMarkerCount(int count)
{
	numMarkers = 0;

	for (int i = 0; i < count && i < (int)mrkr.size(); i++)
	{
		if (mrkr[i].x1 != 255)
			numMarkers = i + 1;
	}
	printf("markers = %d\n", numMarkers);
}

void InitMarkers(std::span<marker_t> list)
{
	mrkr = list;
	numMarkers = 0;

	for (marker_t& me : mrkr)
	{
		me.x1 = 255;
	}
}

int NewMarker(byte x,byte y)
{
	for (int i = 0;i < (int)mrkr.size();i++)
		if (mrkr[i].x1 == 255)
		{
			memset(&mrkr[i], 0, sizeof(special_t));
			mrkr[i].x1 = x;
			mrkr[i].y1 = y;
			if (i >= numMarkers)
				numMarkers = i + 1;
			return i;
		}

	return -1;
}

int GetMarker(byte x,byte y)
{
	for (int i = 0; i < numMarkers; i++)
	{
		if (mrkr[i].x1 == x && mrkr[i].y1 == y)
			return i;
	}

	return -1;
}

marker_t *GetMarker(int i)
{
	return (i >= 0 && i < mrkr.size()) ? &mrkr[i] : NULL;
}

void DeleteMarker(int i)
{
	int j;

	if (i >= 0 && i < numMarkers)
	{
		mrkr[i].x1 = 255;
		for (j = i;j < numMarkers - 1;j++)
			mrkr[j] = mrkr[j + 1];
		mrkr[numMarkers - 1].x1 = 255;
		numMarkers--;
	}
}

void SlideMarkers(Map *map,int dx,int dy)
{
	for (marker_t &me : map->marker)
	{
		if(me.x1!=255)
		{
			int offX=-me.x1;
			int offY=-me.y1;

			me.x1=(byte)SlideCoord(me.x1,dx,map->width);
			me.y1=(byte)SlideCoord(me.y1,dy,map->height);
			me.x2=(byte)SlideCoord(me.x2,dx,map->width);
			me.y2=(byte)SlideCoord(me.y2,dy,map->height);

			offX+=me.x1;
			offY+=me.y1;
		}
	}
}

void CheckMarkers(Map* map)
{
	int i;

	for (i = 0;i < numMarkers;i++)
	{
		if (mrkr[i].x1 == 255)
			continue;

	}
}

bool CheckMarker(const marker_t *me)
{
	return (me->type > 0);
}

word GetPathMarker(Guy* me, byte pathValue, byte currentPriority)
{
	if (!me)
		return 65535;

	int bestPriority = 256;
	int lowestPriority = 256;

	int bestIndex = -1;
	int bestDistance = INT_MAX;

	int gx = me->x / FIXAMT;
	int gy = me->y / FIXAMT;

	// Find the next available priority.
	for (int i = 0; i < numMarkers; i++)
	{
		marker_t* current = &mrkr[i];

		if (current->type != MRK_PATH || current->value != pathValue)
			continue;

		int priority = current->value2;

		if (priority < lowestPriority)
			lowestPriority = priority;

		if (priority > currentPriority && priority < bestPriority)
			bestPriority = priority;
	}

	// Wrap around to the lowest priority.
	if (bestPriority == 256)
		bestPriority = lowestPriority;

	if (bestPriority == 256)
		return 65535;

	// Find the closest marker with the selected priority.
	for (int i = 0; i < numMarkers; i++)
	{
		marker_t* current = &mrkr[i];

		if (current->type != MRK_PATH ||
			current->value != pathValue ||
			current->value2 != bestPriority)
			continue;

		int dx = current->x1 * TILE_WIDTH - gx;
		int dy = current->y1 * TILE_HEIGHT - gy;
		int dist = dx * dx + dy * dy;

		if (dist < bestDistance)
		{
			bestDistance = dist;
			bestIndex = i;
		}
	}

	return (bestIndex >= 0) ? (word)bestIndex : 65535;
}
