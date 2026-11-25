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
static byte markerActive[MAX_MARKER];	// 0=inactive, 1=active (TODO: add this to markers)
static int numMarkers;                  // Specials >= this aren't set.

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
			memset(&mrkr[i], 0, sizeof(marker_t));
			mrkr[i].x1 = x;
			mrkr[i].y1 = y;
			mrkr[i].x2 = x;
			mrkr[i].y2 = y;
			if (i >= numMarkers)
				numMarkers = i + 1;
			return i;
		}

	return -1;
}

int GetMarker(byte x,byte y)
{
	for (int i = 0; i < numMarkers; i++)
		if (mrkr[i].x1 == x && mrkr[i].y1 == y)
			return i;

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

void PickDimensions(marker_t *marker, int& xx, int& yy)
{
	int minX = std::min(marker->x1, marker->x2);
	int maxX = std::max(marker->x1, marker->x2);
	int minY = std::min(marker->y1, marker->y2);
	int maxY = std::max(marker->y1, marker->y2);
	int spotX = minX + Random(maxX - minX + 1);
	int spotY = minY + Random(maxY - minY + 1);
	xx = spotX;
	yy = spotY;
}

void SpawnParticlesAtMarker(marker_t* spawner)
{
	int spotX, spotY;
	PickDimensions(spawner, spotX, spotY);
	DoParticleEffect(spotX * TILE_WIDTH + TILE_WIDTH / 2, spotY * TILE_HEIGHT + TILE_HEIGHT / 2, spawner->value);
}

byte SpawnMonsterAtMarker(marker_t *spawner)
{

	// these types CANNOT be spawned!!!
	switch (spawner->type)
	{
		case ENT_NONE:
		case ENT_NOBODY:
		case ENT_BOUAPHA:
			return 0;
	}

	if (CountMonstersInRect(spawner->value, spawner->x1, spawner->y1, spawner->x2, spawner->y2) > 16) // too many guys!!
		return 0;

	int spotX, spotY;
	PickDimensions(spawner, spotX, spotY);

	Guy *newGuy = AddGuy((spotX * TILE_WIDTH + (TILE_WIDTH / 2)) << FIXSHIFT,
		(spotY * TILE_HEIGHT + (TILE_HEIGHT / 2)) << FIXSHIFT,
		0, spawner->value, 0);

	if (newGuy && (!newGuy->CanWalk(newGuy->x, newGuy->y, curMap, &curWorld)))
		RemoveGuy(newGuy);

	if (!newGuy)
		return 0;

	// fix friendly
	switch (spawner->type)
	{
		case ENT_BOUAPHA:
		case ENT_BUNNY:
		case ENT_BUNNYPATH:
		case ENT_BUNNYFOLLOW:
		case ENT_BUNNYJIMBO:
		case ENT_GOODTURRET:
		case ENT_PUNKBUNNY:
		case ENT_MINECART:
		case ENT_YOUGO:
			player.totalEnemies--;
			newGuy->friendly = 1;
			break;
	}

	// spawn
	if(newGuy->friendly)
		TeamChangeRing(4, newGuy->x, newGuy->y, 0, 8, 2);
	else
		TeamChangeRing(3, newGuy->x, newGuy->y, 0, 8, 2);

	return 1;
}

void CheckMarkers(Map* map)
{
	int i;
	byte active = 0;

	for (i = 0;i < numMarkers;i++)
	{
		if (!markerActive[i] || mrkr[i].x1 == 255)
			continue;

		switch (mrkr[i].type)
		{
			case MRK_PART:
				active = !Random(mrkr[i].value2);
				if (active)
					SpawnParticlesAtMarker(&mrkr[i]);
				break;
			case MRK_SPAWN: // check clock to spawn enemy
				switch (mrkr[i].value2) // spawn type
				{
					case 0:
						active = !Random(128);
						break;
					case 1:
						active = !Random(64); // quick
						break;
					case 2:
						active = !Random(256); // slow
						break;
					case 3:
						active = !(player.clock % (30*5));
						break;
					case 4:
						active = !(player.clock % (30*2)); // quick
						break;
					case 5:
						active = !(player.clock % (30*10)); // slow
						break;
				}
				if (active)
					SpawnMonsterAtMarker(&mrkr[i]);
				break;
		}
	}
}

bool CheckMarker(const marker_t *me)
{
	return (me->type > 0);
}

void GetMarkersFromMap(std::span<marker_t> list)
{
	mrkr = list;
	numMarkers = 0;
	for (size_t i = 0; i < mrkr.size(); i++)
	{
		marker_t& me = mrkr[i];
		if (me.x1 == 255)
			continue;

		numMarkers = i + 1;
		markerActive[i] = 1; // starts off active
	}
}

void SetMarkerActive(int index, byte state)
{
	markerActive[index] = state;
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
