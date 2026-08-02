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
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_YERFOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_YERFDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

}

void AI_Lantern(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_ZOMBIEOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_LANTERNDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if (me->mind == 0) // doing NOTHING!
	{

	}

}

void AI_Hardhat(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_HARDHATOUCH, me->x, me->y, SND_CUTOFF|SND_RANDOM, 1200);
		else
			MakeSound(SND_HARDHATDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->frmTimer<32)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			FireBullet(x,y,me->facing*32,BLT_BIGSHELL,me->friendly);
			me->reload = 20;
		}
		return;	// can't do nothin' right now
	}

	// randomly decide to point at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	if (!me->mind && !Random(100))
	{
		MakeSound(SND_HARDHATTAUNT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		me->seq = ANIM_A1;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 128;
		me->action = ACTION_BUSY;
		me->dx = 0;
		me->dy = 0;
		return;
	}

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < (48 * FIXAMT) && Random(8) == 0)
			{
				// get him!
				MakeSound(SND_HARDHATSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
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

			me->dx = Cosine(me->facing * 32) * 3;
			me->dy = Sine(me->facing * 32) * 3;
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
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < (512 * FIXAMT) && Random(32) == 0)
			{
				// spit at him
				MakeSound(SND_HARDHATSHOOT, me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_ATTACK;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
				me->action = ACTION_BUSY;
				me->dx = 0;
				me->dy = 0;
				me->reload = 0;
				FaceGoodguy(me, goodguy);
				return;
			}
		}
		if (!(me->mind1--))	// time to get a new direction
		{
			if ((goodguy) && Random(3) == 0)
				me->mind = 0;	// get back on track
			else
				me->facing = (byte)Random(8);
			me->mind1 = Random(40) + 1;
		}

		me->dx = Cosine(me->facing * 32) * 3;
		me->dy = Sine(me->facing * 32) * 3;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
	}
}

void AI_Pelican(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_PELICANOUCH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
		else
			MakeSound(SND_PELICANDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK)
		{
			if (me->frm > 3 && me->frm < 8 && me->frmTimer<32)
			{
				x = me->x + Cosine(me->facing * 32) * 16;
				y = me->y + Sine(me->facing * 32) * 16;
				for (i = 0;i < 2;i++)
					FireBulletZ(x, y, FIXAMT * 12, me->facing * 32 - 16 + (byte)Random(33), BLT_SHARK, me->friendly);
			}
			if (me->frm == 8)
				me->reload = 10;
		}

		if (me->seq == ANIM_DIE)
		{
			me->frmAdvance = 192;
			me->facing = (me->facing + 1) & 7;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 2)		// when mind=2, pursue relentlessly
	{
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < (128 * FIXAMT))
			{
				me->mind = 1;	// start circling
			}
			FaceGoodguy2(me, goodguy);

			me->dx = Cosine(me->facing * 32) * 3;
			me->dy = Sine(me->facing * 32) * 3;
			if (me->seq != ANIM_MOVE)
			{
				me->seq = ANIM_MOVE;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 192;
			}
		}
		else
		{
			me->mind = 0;	// if there's no goodguy, get random
			me->mind1 = 1;
		}
	}
	else if (me->mind == 0)	// random wandering
	{
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < (128 * FIXAMT))
			{
				me->mind = 1;	// circling mode
			}
		}
		if (!(me->mind1--))	// time to get a new direction
		{
			me->facing = (byte)Random(8);
			me->mind1 = Random(40) + 1;
		}

		me->dx = Cosine(me->facing * 32) * 3;
		me->dy = Sine(me->facing * 32) * 3;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 192;
		}
	}
	else if (me->mind == 1)	// circling
	{
		FaceGoodguy2(me, goodguy);

		// move sideways
		me->dx = Cosine((me->facing * 32 + 64) & 255) * 3;
		me->dy = Sine((me->facing * 32 + 64) & 255) * 3;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 192;
		}

		if (goodguy && RangeToTarget(me, goodguy) > 128 * FIXAMT)
		{
			me->mind = 2;	// chase him down
		}

		if ((!me->reload) && Random(32) == 0)
		{
			MakeSound(SND_PELICANSPEW, me->x, me->y, SND_CUTOFF, 1000);
			me->seq = ANIM_ATTACK;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
		}
	}
}

void AI_Ninja(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->action == ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_NINJAOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_NINJADIE, me->x, me->y, SND_CUTOFF, 1200);
	}
}

void AI_Kappa(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->action == ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_UMBRELLOOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_UMBRELLODIE, me->x, me->y, SND_CUTOFF, 1200);
	}
}

void AI_Tsuchizoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_SERPENTOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_SERPENTDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 6 && me->reload == 0 && goodguy)
		{
			for(i=0;i<2;i++)
			{
				byte b = (me->facing * 32 + i * 16 - 8) & 255;
				x = me->x + Cosine(b) * 8;
				y = me->y + Sine(b) * 8;
				FireBullet(x, y, b, BLT_ENERGY, me->friendly);
			}
			me->reload = 15;
			if (RangeToTarget(me, goodguy) < (512 * FIXAMT))
				me->mind = 0;
		}
		if (me->seq == ANIM_A1 && me->frm == 4 && goodguy)
		{
			if (Guy *g = FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 72, 0, 0, 10, map, world, me->friendly))
			{
				SetPoisonFrames(g, g->poison+30*10);
				me->mind = 0;
				MakeSound(SND_LIGHTNING, me->x, me->y, SND_CUTOFF, 1000);
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (!me->reload && RangeToTarget(me, goodguy)<(512*FIXAMT) && !Random(32))
			{
				// spit at him
				MakeSound(SND_SERPENTSPIT, me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_ATTACK;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 96;
				me->action = ACTION_BUSY;
				me->dx = 0;
				me->dy = 0;
				me->reload = 0;
				FaceGoodguy(me, goodguy);
				return;
			}

			me->dx = -Cosine(me->facing * 32) * 4;
			me->dy = -Sine(me->facing * 32) * 4;
			if (me->seq != ANIM_MOVE)
			{
				me->seq = ANIM_MOVE;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
			}

			if (RangeToTarget(me, goodguy) < (192*FIXAMT)) // heat up, start pursuing if Bouapha stays around too long
			{
				me->mind2+=2;
				if (me->mind2 % 4 == 0)
				{
					BlowSmoke(me->x, me->y, me->z, FIXAMT);
				}
				if (me->mind2 > 30*2)
				{
					me->mind=2;
					me->mind2=0;
				}
			}
			else if (me->mind2>0) // cool down
				me->mind2--;

			if (RangeToTarget(me, goodguy)>=(200*FIXAMT) && !Random(64)) // when safe, wander!
			{
				me->mind = 1;	// if there's no goodguy, get random
				me->mind1 = 1;
			}
		}
		else
		{
			me->mind = 1;	// if there's no goodguy, get random
			me->mind1 = 1;
		}
	}
	else if (me->mind == 2)	// pursue
	{

		me->dx = Cosine(me->facing * 32) * 8;
		me->dy = Sine(me->facing * 32) * 8;
		FaceGoodguy3(me, goodguy);

		if(me->frm%2==0)
		{
			BlowSmoke(me->x, me->y, me->z, FIXAMT);
		}

		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 256;
		}

		if (RangeToTarget(me, goodguy)<(72*FIXAMT) && !Random(4)) // CHOMP!
		{
			FaceGoodguy(me, goodguy);
			MakeSound(SND_HAPPYCHOMP, me->x, me->y, SND_CUTOFF, 1500);
			me->seq = ANIM_A1;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 256;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 0;
			return;
		}

		if (RangeToTarget(me, goodguy) > (256*FIXAMT)) // cool down if no longer a threat
		{
			me->mind2++;
			if (me->mind2 > 30 * 2)
			{
				me->mind = 0;
				me->mind2 = 0;
			}
		}
		else if (me->mind2 > 0) // cool down
			me->mind2--;
	}
	else if (me->mind == 1)	// random wandering
	{
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy)<(512*FIXAMT) && !Random(32))
			{
				// spit at him
				MakeSound(SND_SERPENTSPIT, me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_ATTACK;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 96;
				me->action = ACTION_BUSY;
				me->dx = 0;
				me->dy = 0;
				me->reload = 0;
				FaceGoodguy(me, goodguy);
				return;
			}
		}
		if (!(me->mind1--))	// time to get a new direction
		{
			if ((goodguy) && Random(3) == 0)
				me->mind = 0;	// get back on track
			else
				me->facing = (byte)Random(8);
			me->mind1 = Random(40) + 1;
		}

		me->dx = Cosine(me->facing * 32) * 4;
		me->dy = Sine(me->facing * 32) * 4;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
	}
}

void AI_Jimbo(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_JIMBOOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_JIMBODIE, me->x, me->y, SND_CUTOFF, 1200);
	}
}

void AI_Umbrello(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_UMBRELLOOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_UMBRELLODIE, me->x, me->y, SND_CUTOFF, 1200);
	}

}

void AI_Kabocha(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp == 0)
			MakeSound(SND_PUMPKINDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

}

void AI_NohMannComedy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

}

void AI_NohMannTragedy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

}

void AI_NohMannArm(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_ARMORHIT, me->x, me->y, SND_CUTOFF, 1200);
	}

}

void AI_EyeballTree(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_RICHIEOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_RICHIEDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

}

void AI_StickNinja(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_STICKNINJAOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_STICKNINJADIE, me->x, me->y, SND_CUTOFF, 1200);
	}

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
	{
		if (me->hp > 0)
			MakeSound(SND_ZOMBIEOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_ZOMBIEDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->seq == ANIM_DIE && me->reload == 0)
	{
		ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 8);
		me->reload = 1;
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_A1)
		{
			if (me->frm == 10)
			{
				me->dx = Cosine(me->facing*32)*12;
				me->dy = Sine(me->facing*32)*12;
				me->dz=20*FIXAMT;
			}
			if (me->frm > 10)
			{
				if (!me->reload && goodguy && (me->AttackCheck(8, (Cosine(me->facing) * 16) >> FIXSHIFT, (Sine(me->facing) * 16) >> FIXSHIFT, goodguy)))
				{
					goodguy->GetShot(Cosine(me->facing*32)*4, Sine(me->facing*32)*4, 3, map, world);
					me->reload = 2;
				}
				if (me->z < 1 && !me->reload)
				{
					me->reload=3;
					// oh no...
					ShakeScreen(120);
					for(i=0;i<Random(3)+1;i++)
					{
						x=me->x+((64-Random(129))<<FIXSHIFT);
						y=me->y+((64-Random(129))<<FIXSHIFT);
						FireBullet(x,y,0,BLT_ROCKETBOOM,me->friendly);
					}
					me->hp=1;
					me->GetShot(0,0,1,map,world);
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
			if (RangeToTarget(me,goodguy)<128*FIXAMT && Random(32)==0)
			{
				me->seq = ANIM_A1;
				me->frmAdvance = 192;
				me->frm = 0;
				me->frmTimer = 0;
				me->action = ACTION_BUSY;
				me->dx = 0;
				me->dy = 0;
				MakeSound(SND_BOMBIELEAP, me->x, me->y, SND_CUTOFF|SND_RANDOM, 1200); // random for the funnies
				return;
			}
			FaceGoodguy(me, goodguy);
			me->dx = Cosine(me->facing*32)*2;
			me->dy = Sine(me->facing*32)*2;
			if (me->seq != ANIM_MOVE)
			{
				me->seq = ANIM_MOVE;
				me->frmAdvance = 128;
				me->frm = 0;
				me->frmTimer = 0;
			}
			if (Random(64)==0)
			{
				me->mind=1;
				me->mind1=1;
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
			me->frmAdvance = 96;
		}
	}
}

void AI_Deathcap(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if(me->hp>0)
			MakeSound(SND_BUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_BUSHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 5 && !me->reload && goodguy)
		{
			MakeSound(SND_MISSILELAUNCH, me->x, me->y, SND_CUTOFF, 1200);
			for (i = 0;i < 4;i++)
				FireBullet(me->x, me->y, i*2 + me->mind2, BLT_ROCKET, me->friendly);
			me->mind2 = me->mind2==0; // flip!
			me->reload = 40;
		}
		return;	// can't do nothin' right now
	}

	if (RangeToTarget(me, goodguy) < (160 * FIXAMT) && Random(16) == 0 && !me->reload)
	{
		// get him!
		me->seq = ANIM_ATTACK;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 256;
		me->action = ACTION_BUSY;
		me->dx = 0;
		me->dy = 0;
		me->reload = 0;
		return;
	}
	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			FaceGoodguy(me, goodguy);

			me->dx = Cosine(me->facing * 32) * 1;
			me->dy = Sine(me->facing * 32) * 1;
			if (me->seq != ANIM_MOVE)
			{
				me->seq = ANIM_MOVE;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
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

		me->dx = Cosine(me->facing * 32) * 1;
		me->dy = Sine(me->facing * 32) * 1;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
	}
}

void AI_GlueTrap(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, a;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_ROBOOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_ROBODIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frmTimer < 63)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)
	{
		if (goodguy && RangeToTarget(me, goodguy) < 800 * FIXAMT)
		{
			me->mind = 1;
			me->reload = 30;
		}
	}
	else
	{
		if (!me->reload)
		{
			x = me->x + Cosine(me->facing * 32) * 12;
			y = me->y + Sine(me->facing * 32) * 12;
			MakeSound(SND_MISSILELAUNCH, me->x, me->y, SND_CUTOFF, 1200);
			FireBullet(x, y, me->facing*32, BLT_GLUESHOT, me->friendly);
			me->reload = 80;
			return;
		}
		FaceGoodguy3(me, goodguy);
	}
}

void AI_Gingersnap(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_ZOMBIEOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_ZOMBIEDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frmTimer < 63)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)
	{
		me->mind	= 1;
		me->mind2	= me->mapx-4;
		me->mind3	= me->mapy;
	}
	else if (me->mind == 1)
	{
		x = (me->mind2 << FIXSHIFT) + Cosine(me->mind1) * 60;
		y = (me->mind3 << FIXSHIFT) + Sine(me->mind1) * 60;

		if (!me->reload && me->CanWalk(x,y,map,world))
		{
			me->x = x;
			me->y = y;
			me->mind1 = (me->mind1+4)&255;
			me->reload = 1;
		}
	}
}

void AI_PurpleKappa(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_UMBRELLOOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_UMBRELLODIE, me->x, me->y, SND_CUTOFF, 1200);
	}

}

void AI_GreaterIsozoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_SERPENTOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_SERPENTDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

}

void AI_CroquetBird(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_ZOMBIEOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_ZOMBIEDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

}

void AI_NastyTree(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_ZOMBIEOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_ZOMBIEDIE, me->x, me->y, SND_CUTOFF, 1200);
		if (me->hp > 0)
		{
			me->seq = ANIM_A1;	//stunned
			me->frm = 0;
			me->action = ACTION_BUSY;
			me->frmTimer = 0;
			me->frmAdvance = 32;
		}
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 6 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (me->AttackCheck(35, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(Cosine(me->facing * 32) * 8, Sine(me->facing * 32) * 8, 10, map, world);
				MakeSound(SND_MISSILEBOOM, me->x, me->y, SND_CUTOFF, 1200);
			}
			me->reload = 20;
		}
		if (me->seq == ANIM_A1 && me->frm%2==0)
		{
			Dampen(&me->dx, FIXAMT / 4);
			Dampen(&me->dy, FIXAMT / 4);

		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// waiting for Bouapha
	{
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < (300 * FIXAMT) && map->CheckLOS(me->mapx, me->mapy, 10, goodguy->mapx, goodguy->mapy))
			{
				// wake up!
				me->mind = 1;
			}
		}
	}
	else if (me->mind == 1)
	{
		if (me->mind2)
			me->mind2--;
		else
		{
			me->mind2 = 3;
			FaceGoodguy2(me, goodguy);
		}

		i = RangeToTarget(me, goodguy);
		if (i < (60 * FIXAMT) && Random(4) == 0 && me->reload == 0)
		{
			// get him! (chomp)
			MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1500);
			me->seq = ANIM_ATTACK;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 256;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			return;
		}

		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}

		me->dx = Cosine(me->facing * 32) * 6;
		me->dy = Sine(me->facing * 32) * 6;
	}
}

void AI_SpikeBall(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp == 0)
			MakeSound(SND_PUMPKINDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->mind2)
		me->mind2--;
	else
	{
		me->mind2 = 3;
		FaceGoodguy2(me, goodguy);
	}

	Dampen(&me->dx, FIXAMT / 8);
	Dampen(&me->dy, FIXAMT / 8);

	me->dx += Cosine(me->facing * 32) / 2;
	me->dy += Sine(me->facing * 32) / 2;

	if (!me->reload && RangeToTarget(me, goodguy) < 64 * FIXAMT)
	{
		x = me->x + Cosine(me->facing * 32) * 16;
		y = me->y + Sine(me->facing * 32) * 16;

		if (me->AttackCheck(35, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
		{
			goodguy->GetShot(Cosine(me->facing * 32) * 8, Sine(me->facing * 32) * 8, 10, map, world);
			me->facing = (me->facing + 4) & 7;
			me->dx/=2;
			me->dy/=2;
			ReverseClamp(&me->dx, 2 * FIXAMT);
			me->mind1 = 0;
			me->reload = 3;
			MakeSound(SND_SPIKECLANK, me->x, me->y, SND_CUTOFF, 800);
		}
	}
	if (me->mind1)	// hit a wall
	{
		if (me->mind1 & 1)
			me->dx = -me->dx / 2;
		if (me->mind1 & 2)
			me->dy = -me->dy / 2;
		me->mind1 = 0;
		me->GetShot(0, 0, x, map, world);
		MakeSound(SND_SPIKEBOUNCE, me->x, me->y, SND_CUTOFF, 800);
		if(me->facing%2==0)
			me->facing = (me->facing+4)&7;
		else if (!Random(2))
			me->facing = (me->facing+2)&7;
		else
			me->facing = (me->facing-2)&7;
	}

	Clamp(&me->dx, 10 * FIXAMT);
	Clamp(&me->dy, 10 * FIXAMT);

	if (me->seq != ANIM_MOVE)
	{
		me->seq = ANIM_MOVE;
		me->frm = 0;
		me->frmTimer = 0;
		MakeSound(SND_SPIKECLANK, me->x, me->y, SND_CUTOFF, 200);
	}
	if (me->seq == ANIM_MOVE)
	{
		me->frmAdvance = (abs(me->dx) + abs(me->dy))/FIXAMT*32;
	}
}

void AI_MiniZombie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF|SND_DOUBLESPEED,1200);
		else
			MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF|SND_DOUBLESPEED,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==1 && me->frmTimer<32)
			me->frmAdvance*=2;
		if(me->seq==ANIM_ATTACK && me->frm==5)
		{
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
		}
		if(me->seq==ANIM_ATTACK && me->frm>6)
		{
			Dampen(&me->dx,FIXAMT/2);
			Dampen(&me->dy,FIXAMT/2);
		}
		if(me->seq==ANIM_ATTACK && me->frm>=7 && me->frm<13 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(8,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,1,map,world);
				me->reload=2;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(96*FIXAMT) && Random(32)==0)
			{
				// get him!
				MakeSound(SND_ZOMBIELEAP,me->x,me->y,SND_CUTOFF|SND_DOUBLESPEED,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}

			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(30)+1;
		}

		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_MiniBonehead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF|SND_DOUBLESPEED,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF|SND_DOUBLESPEED,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==1 && me->frmTimer<32)
			me->frmAdvance*=2;
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,2,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(x,y,me->facing*32,BLT_SPINE,me->friendly);
			me->reload=5;
			me->mind1=1;
		}
		return;	// can't do nothin' right now
	}

	// randomly decide to point at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	if((!Random(100)) && me->mind==0)
	{
		me->seq=ANIM_A2;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		return;
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT) && Random(8)==0)
			{
				// get him!
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=96;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=192;
			}
			if(Random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(32)==0)
			{
				// spit at him
				MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF|SND_DOUBLESPEED,1200);
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=192;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				FaceGoodguy(me,goodguy);
				return;
			}
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(30)+1;
		}

		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=192;
		}
	}
}
