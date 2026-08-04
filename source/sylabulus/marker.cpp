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

static std::span<marker_t> mrkr;		// Local specials
static int numMarkers;                  // Specials >= this aren't set.

void InitMarkers(std::span<marker_t> list)
{
	mrkr = list;
	numMarkers = 0;
	for (marker_t &me : list)
	{
		me.x1 = 255;
	}
}

int NewMarker(byte x,byte y)
{
	for(int i=0;i<(int)mrkr.size();i++)
		if(mrkr[i].x1==255)
		{
			memset(&mrkr[i],0,sizeof(marker_t));
			mrkr[i].x1=x;
			mrkr[i].y1=y;
            mrkr[i].x2=x;
			mrkr[i].y2=y;
			if(i>=numMarkers)
				numMarkers=i+1;
			return i;
		}

	return -1;
}

int GetMarker(byte x,byte y)
{
	int i;

	for(i=0;i<numMarkers;i++)
		if(mrkr[i].x1==x && mrkr[i].y1==y)
			return i;

	return -1;
}

marker_t *GetMarker(int i)
{
	return (i >= 0 && i < numMarkers) ? &mrkr[i] : NULL;
}

void DeleteMarker(int i)
{
	int j;

	if(i>=0 && i<numMarkers)
	{
		mrkr[i].x1=255;
		for(j=i;j<numMarkers-1;j++)
			mrkr[j]=mrkr[j+1];
		mrkr[numMarkers-1].x1=255;
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
	// do stuff i guess
}

bool CheckMarker(const marker_t *me)
{
	return (me->type > 0);
}
