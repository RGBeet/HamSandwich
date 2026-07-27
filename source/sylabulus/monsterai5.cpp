#include "winpch.h"
#include "monsterai.h"
#include "guy.h"
#include "sound.h"
#include "bullet.h"
#include "player.h"
#include "editor.h"
#include "shop.h"
#include "goal.h"

void AI_Yerfboss(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_Lantern(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_Hardhat(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_Pelican(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_Ninja(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_Kappa(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_Tsuchizoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_Jimbo(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_Umbrello(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_Kabocha(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_NohMannComedy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_NohMannTragedy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_NohMannArm(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_EyeballTree(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_StickNinja(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_CrazyBush(Guy* me, Map* map, world_t* world, Guy* goodguy)
{ 
	int x,y,i,j;
	if(me->reload)
		me->reload--;
	if(me->mind2)
		me->mind2--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_BUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_BUSHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(50*FIXAMT) && Random(8)==0 && map->FindGuy(me->mapx,me->mapy,5,goodguy))
		{
			// get him!
			goodguy->GetShot(me->dx/2,me->dy/2,1,map,world);
		}
		FaceGoodguy2(me,goodguy);

		me->dx=Cosine(me->facing*32)*5;
		me->dy=Sine(me->facing*32)*5;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Bombie(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_ZOMBIEOUCH, me->x, me->y, SND_CUTOFF|SND_RANDOM, 1200);
		else
			MakeSound(SND_ZOMBIEDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->mind3)
		me->mind3--;
	else
	{
		me->mind3 = 10;
		if (me->hp < me->maxHP && me->hp>0)
			me->hp++;
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm < 7)
		{
			for (x = 0;x < 5;x++)
				FireBullet(me->x - FIXAMT * 20 + Random(FIXAMT * 40), me->y - FIXAMT * 20 + Random(FIXAMT * 40), Random(256), BLT_SPORE, me->friendly);
		}
		if (me->seq == ANIM_ATTACK && me->frm == 5)
		{
			me->dx = Cosine(me->facing * 32) * 12;
			me->dy = Sine(me->facing * 32) * 12;
		}
		if (me->seq == ANIM_ATTACK && me->frm > 6)
		{
			Dampen(&me->dx, FIXAMT / 2);
			Dampen(&me->dy, FIXAMT / 2);
			if (me->frm == 13 && me->frmTimer < 64)
			{
				for (x = 0;x < 256;x += 8)
				{
					FireExactBullet(me->x, me->y, FIXAMT * 10, Cosine(x) * 12, Sine(x) * 12, 0, 0, 16, x, BLT_SPORE, me->friendly);
					FireExactBullet(me->x, me->y, FIXAMT * 10, Cosine(x) * 6, Sine(x) * 6, 0, 0, 16, x, BLT_SPORE, me->friendly);
				}
			}
		}
		if (me->seq == ANIM_ATTACK && me->frm >= 7 && me->frm < 13 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (me->AttackCheck(8, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 1, map, world);
				me->reload = 2;
			}
		}
		return;	// can't do nothin' right now
	}

	if (Random(10) == 0)
		FireBullet(me->x - FIXAMT * 20 + Random(FIXAMT * 40), me->y - FIXAMT * 20 + Random(FIXAMT * 40), Random(256), BLT_SPORE, me->friendly);

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < (200 * FIXAMT) && Random(5) == 0)
			{
				// get him!
				MakeSound(SND_ZOMBIELEAP, me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_ATTACK;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
				me->action = ACTION_BUSY;
				me->dx = 0;
				me->dy = 0;
				me->reload = 0;
				return;
			}

			FaceGoodguy(me, goodguy);

			me->dx = Cosine(me->facing * 32) * 2;
			me->dy = Sine(me->facing * 32) * 2;
			if (me->seq != ANIM_MOVE)
			{
				me->seq = ANIM_MOVE;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
			}
			if (Random(64) == 0)
			{
				me->mind = 1;		// occasionally wander
				me->mind1 = 1;
			}
		}
		else
		{
			me->mind = 1;	// if there's no goodguy, get random
			me->mind1 = 1;
		}
	}
	else if (me->mind == 1)	// random wandering
	{
		if (!(me->mind1--))	// time to get a new direction
		{
			if ((goodguy) && Random(3) == 0)
				me->mind = 0;	// get back on track
			else
				me->facing = (byte)Random(8);
			me->mind1 = Random(40) + 1;
		}

		me->dx = Cosine(me->facing * 32) * 2;
		me->dy = Sine(me->facing * 32) * 2;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
	}
}

void AI_Deathcap(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_GlueTrap(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_Gingersnap(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_PurpleKappa(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_GreaterIsozoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_CroquetBird(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_NastyTree(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}

void AI_SpikeBall(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

}
