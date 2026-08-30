#include "winpch.h"
#include "monsterai.h"
#include "guy.h"
#include "sound.h"
#include "bullet.h"
#include "player.h"



void DoMove(Guy* me, int move, int frmAdv, byte busy, int dx, int dy)
{
	me->seq = move;
	me->frm = 0;
	me->frmTimer = 0;
	me->frmAdvance = frmAdv;
	if (busy)
		me->action = ACTION_BUSY;
	else
		me->action = ACTION_IDLE;
	me->dx = dx;
	me->dy = dy;
}

void BasicAI(Guy* me, byte ouchSound, byte dieSound, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(ouchSound, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		else
			MakeSound(dieSound, me->x, me->y, SND_CUTOFF, 1200);
	}
}

void WanderAI(Guy* me, int speed, int wanderRate, int wanderTime, int unWanderRate, Map* map, world_t* world, Guy* goodguy)
{
	if (me->action == ACTION_BUSY)
		return;

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		FaceGoodguy(me, goodguy);

		me->dx = Cosine(me->facing * 32)*speed;
		me->dy = Sine(me->facing * 32)*speed;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
		if (Random(wanderRate) == 0)
		{
			me->mind = 1;		// occasionally wander
			me->mind1 = 1;
		}
	}
	else if (me->mind == 1)	// random wandering
	{
		if (!(me->mind1--))	// time to get a new direction
		{
			if ((goodguy) && Random(unWanderRate) == 0)
				me->mind = 0;	// get back on track
			else
				me->facing = (byte)Random(8);
			me->mind1 = (byte)Random(wanderTime) + 1;
		}

		me->dx = (Cosine(me->facing * 32) * speed) / FIXAMT;
		me->dy = (Sine(me->facing * 32) * speed) / FIXAMT;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
	}
}

void StartAnimation(Guy* me, byte seq, word adv, bool force)
{
	if (!force && seq == me->seq) // don't restart the animation if it's already playing
		return;
	me->seq			= seq;
	me->frm			= 0;
	me->frmTimer	= 0;
	me->frmAdvance	= adv;
	if (me->seq > ANIM_MOVE) // animations 2-9 are busy animations, so set the action to busy
		me->action = ACTION_BUSY;
	else
		me->action = ACTION_IDLE;
}

void SetMoveFacing(Guy* me, int speed)
{
	me->dx = Cosine(me->facing * 32) * speed;
	me->dy = Sine(me->facing * 32) * speed;
}

void StartMoveAnimation(Guy* me, word speed)
{
	if (me->seq != ANIM_MOVE)
		StartAnimation(me, ANIM_MOVE, speed);
}

void StartIdleAnimation(Guy* me, word speed)
{
	if (me->seq != ANIM_IDLE)
		StartAnimation(me, ANIM_IDLE, speed);
}

bool IsAnimationFrame(Guy* me, byte seq, byte frame)
{
	return me->seq == seq && me->frm == frame;
}

bool IsAnimationFrames(Guy* me, byte seq, byte frameFirst, byte frameLast)
{
	return me->seq == seq && (me->frm >= frameFirst && me->frm <= frameLast);
}

bool TargetWithinRange(Guy* me, Guy* target, int range)
{
	return (abs(me->x - goodguy->x) + abs(me->y - goodguy->y)) <= range * FIXAMT;
}

// used for moss, to check if tile has an obstacle that would prevent spawning
byte TileHasObstacle(Map* map, int x, int y)
{
	switch (GetItem(map->GetTile(x + 1, y)->item)->passability)
	{
	case ITP_SOLID:
	case ITP_BULLETPROOF:
		return 1;
	default:
		return 0;
	}
}

// can the vehicle drive on the terrain?
byte TerrainIsDrivable(Guy* me, world_t* world, Map* map, int mapx, int mapy)
{
	terrain_t* terrain = GetTerrain(world, map->map[mapx + mapy * map->width].floor);

	if (TileHasObstacle(map, mapx, mapy)); // obstacle = no path!
	{
		return 0; // only allow path if ghost??
	}

	switch (terrain->pathType)
	{
		case TRN_BUNNY:
			return me->aiType == (short)EntityType::BunnyPath;
		case TRN_MINECART:
			return me->aiType == (short)EntityType::Minecart;
		case TRN_DRIVE:
			return me->aiType == (short)EntityType::YouGo;
		default:
			return 0;
	}
}
