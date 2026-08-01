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
