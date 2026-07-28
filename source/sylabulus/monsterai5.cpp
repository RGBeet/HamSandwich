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
	int x,y,i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_ZOMBIEOUCH, SND_ZOMBIEDIE);

	if (me->seq == ANIM_DIE && me->reload == 0)
	{
		if (me->frm == 0)
		{
			FireBullet(me->x, me->y, 0, BLT_ROCKETBOOM, me->friendly);
		}
		ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 8);
		me->reload = 2;
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK, 10, false))
		{
			me->SetNewSpeed(8);
			me->dz = 8;
		}
		if (me->seq == ANIM_A1)
		{
			if (me->frm == 10)
			{
				me->SetNewSpeed(8);
				me->dz=20*FIXAMT;
			}
			if (me->frm > 10)
			{
				if (!me->reload && goodguy && (me->AttackCheck(8, me->GetSpaceInFrontX(16) >> FIXSHIFT, me->GetSpaceInFrontY(16) >> FIXSHIFT, goodguy)))
				{
					goodguy->GetShot(me->GetSpaceInFrontX(4), me->GetSpaceInFrontY(4), 3, map, world);
					me->reload = 2;
				}
				if (me->z < 1)
				{
					// oh no...
					ShakeScreen(120);
					for(i=0;i<Random(3)+1;i++)
					{
						x=me->x+((64-Random(129))<<FIXSHIFT);
						y=me->y+((64-Random(129))<<FIXSHIFT);
						FireBullet(x,y,0,BLT_ROCKETBOOM,me->friendly);
					}
					me->SelfDestruct(map,world);
				}
			}
			if (me->frm > 16)
			{ // stay like this until death
				me->frm = 17;
				me->frmAdvance = 0;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0) // when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(128) && me->CheckRoll(32, ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_A1, 192, ACTION_BUSY, 0, 0, 0); // get him!
				MakeSound(SND_BOMBIELEAP, me->x, me->y, SND_CUTOFF|SND_RANDOM, 1200); // random for the funnies
				return;
			}
			FaceGoodguy(me, goodguy);
			me->SetNewSpeed(1);
			if (me->seq != ANIM_MOVE)
				me->StartAnimMove(64);
			if (me->CheckRoll(64, ROLL_MOVE))
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if (me->mind == 1)	// random wandering
	{
		me->TryGetNewDirection(&me->mind1, 40, goodguy, true);
		me->SetNewSpeed(1);
		me->StartAnimMove(64);
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
