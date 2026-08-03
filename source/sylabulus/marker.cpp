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
	numSpecials = 0;
	for (special_t &me : list)
	{
		me.x1 = 255;
	}
}

int NewMarker(byte x,byte y)
{
	for(int i=0;i<(int)spcl.size();i++)
		if(mrkr[i].x1==255)
		{
			memset(&spcl[i],0,sizeof(marker_t));
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
		if(mrkr[i].x==x && mrkr[i].y==y)
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
		mrkr[i].x=255;
		for(j=i;j<numMarkers-1;j++)
			mrkr[j]=mrkr[j+1];
		mrkr[numMarkers-1].x=255;
		numMarkers--;
	}
}

void SlideMarkers(Map *map,int dx,int dy)
{
	for (marker_t &me : map->marker)
	{
		if(me.x!=255)
		{
			int offX=-me.x;
			int offY=-me.y;

			me.x=(byte)SlideCoord(me.x,dx,map->width);
			me.y=(byte)SlideCoord(me.y,dy,map->height);

			offX+=me.x;
			offY+=me.y;

			AdjustSpecialCoords(&me,offX,offY);
			AdjustSpecialEffectCoords(&me,offX,offY);
		}
	}
}