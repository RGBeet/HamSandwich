#include "winpch.h"
#include "monsterai.h"
#include "guy.h"
#include "sound.h"
#include "bullet.h"
#include "player.h"

static int pickupX, pickupY;

void AI_Bonehead(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SKELOUCH, SND_SKELDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,3) && goodguy)
		{
			x = me->GetSpaceInFrontX(16);
			y = me->GetSpaceInFrontY(16);
			if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
				goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 4, map, world);
			me->reload = 5;
		}
		if (me->CheckSequenceFrame(ANIM_A1,3) && goodguy)
		{
			me->DoFireBullet(BLT_ENERGY, 5, 16);
			me->mind1 = 1;
		}
		return;	// can't do nothin' right now
	}

	// randomly decide to point at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	if ((!Random(100)) && me->mind == 0)
	{
		me->StartNewAnimation(ANIM_A2,64,ACTION_BUSY,0,0); // point
		return;
	}

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(48) && me->CheckRoll(8,ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_ATTACK,64,ACTION_BUSY,0,0,0,SND_SKELKICK);
				return;
			}
			FaceGoodguy(me, goodguy);
			me->SetNewSpeed(4);

			if (me->seq != ANIM_MOVE)
				me->StartAnimMove(128);

			if (me->CheckRoll(64,ROLL_MOVE) == 0)
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if (me->mind == 1)	// random wandering
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(512) && me->CheckRoll(32,ROLL_ATTACK) == 0)
			{
				// spit at him
				me->StartNewAnimation(ANIM_A1,64,ACTION_BUSY,0,0,0,SND_SKELSHOOT);
				FaceGoodguy(me, goodguy);
				return;
			}
		}

		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(4);
		me->StartAnimMove(128);
	}
}

void AI_Bat(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_BATOUCH, SND_BATDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,2) && goodguy)
		{
			if (me->AttackCheck(16, (me->GetSpaceInFrontX(16)) >> FIXSHIFT, (me->GetSpaceInFrontY(16)) >> FIXSHIFT, goodguy))
				goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 2, map, world);
			me->reload = 2;

		}
		if (me->seq == ANIM_A1 && me->frm > 1 && goodguy && me->hp > 0)
		{
			// diving attack hit check
			if (me->AttackCheck(16, (me->x) >> FIXSHIFT, (me->y) >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(Cosine(me->facing * 32) * 8, Sine(me->facing * 32) * 8, 6, map, world);
				me->StartNewAnimation(ANIM_A2,0,me->action);
				me->dx = -me->dx / 4;
				me->dy = -me->dy / 4;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)		// when mind=0, singlemindedly zip towards Bouapha
	{
		if (goodguy)
		{
			if (!me->CheckTargetWithinReach(64) && me->CheckRoll(32,ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,me->GetFacingX()*12,me->GetFacingY()*12,0,SND_BATDIVE);
				return;
			}
			if (me->CheckTargetWithinReach(48) && me->CheckRoll(4,ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_BATEYES);
				return;
			}

			FaceGoodguy(me, goodguy);

			me->SetNewSpeed(8);
			me->StartAnimMove(128);

			if (me->CheckRoll(16,ROLL_MOVE))
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if (me->mind == 1)	// random wandering
	{
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(6);
		me->StartAnimMove(96);
	}
}

void AI_Spider(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SPD1OUCH, SND_SPD1DIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,2) && goodguy)
		{
			if (me->AttackCheck(16, (me->GetSpaceInFrontX(8)) >> FIXSHIFT, (me->GetSpaceInFrontY(8)) >> FIXSHIFT, goodguy))
				goodguy->GetShot(Cosine(me->facing * 32) * 2, Sine(me->facing * 32) * 2, 1, map, world);
			me->reload = 2;

		}
		return;	// can't do nothin' right now
	}

	if (goodguy)
	{
		if (me->CheckTargetWithinReach(32) && me->CheckRoll(4,ROLL_ATTACK))
		{
			FaceGoodguy(me, goodguy);
			me->StartNewAnimation(ANIM_ATTACK,255,ACTION_BUSY,0,0,0,SND_SPD1GNAW);
			return;
		}
	}
	me->TryGetNewDirection(&me->mind,40,goodguy,false);
	me->SetNewSpeed(8);

	me->StartAnimMove(128);
}

void AI_BigSpider(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SPD2OUCH, SND_SPD2DIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,6))
			me->DoFireBullet(BLT_ACID,40,8,SND_SPD2SPIT);
		return;	// can't do nothin' right now
	}

	if (goodguy)
	{
		if (me->CheckTargetWithinReach(256) && me->CheckRoll(32,ROLL_ATTACK))
		{
			FaceGoodguy(me, goodguy);
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0);
			return;
		}
	}

	if (!--me->mind)	// time to get a new direction
		me->PickRandomDirection(&me->mind,Random(120)+1);

	me->SetNewSpeed(4);
	me->StartAnimMove(128);
}

void AI_Zombie(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_ZOMBIEOUCH, SND_ZOMBIEDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,5,false))
			me->SetNewSpeed(8);

		if (me->seq == ANIM_ATTACK && me->frm > 6)
			me->DampenSpeed(2);

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

	if (me->mind == 0) // when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(128) && me->CheckRoll(32,ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_ATTACK,64,ACTION_BUSY,0,0,0,SND_ZOMBIELEAP); // get him!
				return;
			}
			FaceGoodguy(me, goodguy);
			me->SetNewSpeed(1);
			if (me->seq != ANIM_MOVE)
				me->StartAnimMove(64);
			if (me->CheckRoll(64,ROLL_RANDWALK))
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if (me->mind == 1)	// random wandering
	{
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(1);
		me->StartAnimMove(64);
	}
}

void AI_EggSac(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_EGGOUCH, SND_BUSHDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,18))
			me->TryAddBaby(map, world, MONS_SPIDER, me->x, me->y + FIXAMT*2, 200);
		return;
	}

	if (me->CheckRoll(500, ROLL_ATTACK) && !me->reload)
		me->StartNewAnimation(ANIM_ATTACK, 64, ACTION_BUSY, 0, 0, 0, SND_EGGSACBIRTH);
}

void AI_MamaSpider(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;
	byte f;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SPD3OUCH,SND_SPD3DIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			if (Random(2))
			{
				AddBaby(me->x + ((-8 + Random(17)) << FIXSHIFT),
					me->y + ((-8 + Random(17)) << FIXSHIFT), 0, MONS_SPIDER, me);
			}
		}
		if (me->seq == ANIM_ATTACK && me->frm > 4 && me->frm < 8 && me->reload == 0)
		{
			// only actually fire if the range is reasonable to avoid creating excess bullets
			if (me->CheckTargetWithinReach(700) && GuyCheckTargetWithinSight(me,goodguy,map))
			{
				for (i = 0;i < 3;i++)
				{
					// spit acid glob
					x = me->x + Cosine(me->facing * 32) * 20 - FIXAMT * 4 + (Random(9) << FIXSHIFT);
					y = me->y + Sine(me->facing * 32) * 20 - FIXAMT * 4 + (Random(9) << FIXSHIFT);
					f = me->facing * 32 - 16 + Random(33);
					FireBullet(x, y, f, BLT_ACID, me->friendly);
				}
			}
			me->reload = 0;
		}
		if (me->seq == ANIM_A1 && me->frm == 2 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 32;
			y = me->y + Sine(me->facing * 32) * 32;
			if (me->AttackCheck(32, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(Cosine(me->facing * 32) * 12, Sine(me->facing * 32) * 12, 20, map, world);
				me->reload = 2;
			}
		}
		return;	// can't do nothin' right now
	}

	if (goodguy)
	{
		x = RangeToTarget(me, goodguy);
		if (x < (512 * FIXAMT) && x>128 * FIXAMT && me->CheckRoll(32,ROLL_ATTACK) && me->reload == 0)
		{
			FaceGoodguy(me, goodguy);
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,1,SND_SPD3SPEW);
			return;
		}
		else if (x < (128 * FIXAMT) && me->CheckRoll(16,ROLL_ATTACK) == 0 && me->reload == 0)
		{
			FaceGoodguy(me, goodguy);
			me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,0,0,1,SND_SPD3CHOMP);
			return;
		}
	}

	if (!--me->mind)	// time to get a new direction
		me->PickRandomDirection(&me->mind,Random(120)+1);
	me->SetNewSpeed(3);
	me->StartAnimMove(128);
}

void AI_Pygmy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_PYGMYOUCH,SND_PYGMYDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,6) && goodguy)
		{
			if (me->AttackCheck(16, (me->GetSpaceInFrontX(16)) >> FIXSHIFT, (me->GetSpaceInFrontY(16)) >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(me->GetSpaceInFrontX(6), me->GetSpaceInFrontY(6), 8, map, world);
				me->reload = 10;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// not currently aware of goodguy
	{
		//me->frmAdvance = 32;	// overcome the auto-128 frmAdvance of guy.cpp
		me->StartAnimIdle(32);

		if (me->mind1 < 101)
			me->mind1++;

		if (me->mind1 > 100 && Random(200) == 0)
		{
			switch (Random(3))
			{
			case 0:
				me->StartNewAnimation(ANIM_A1,64,ACTION_BUSY,0,0,0,SND_PYGMYSPIN);
				break;
			case 1:
				me->StartNewAnimation(ANIM_A2,64,ACTION_BUSY,0,0,0,SND_PYGMYYAWN);
				break;
			case 2:
				me->facing = (me->facing + (-1 + Random(3))) & 7;	//change facing
				break;
			}
			me->mind1=0;
		}
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(256) || me->ouch > 0)
			{
				// if the goodguy is near, or he shot me
				me->mind = 1;	// start hunting
				me->mind1 = (!me->ouch) ? 60 : 120;	// for 2 seconds minimum OR 4 seconds, because they HATE getting shot
				FaceGoodguy(me, goodguy);
				me->StartNewAnimation(ANIM_A3,255,ACTION_BUSY,0,0,0,SND_PYGMYANGRY);
			}
		}
		me->SetNewSpeed(0);
	}
	else
	{
		FaceGoodguy(me, goodguy);
		if (!goodguy)
		{
			me->mind = 0;	// go back to being bored, nothing to see here
			me->mind1 = 0;
			return;
		}
		if (me->CheckTargetWithinReach(72) && me->CheckRoll(16,ROLL_ATTACK) && me->reload == 0)
		{
			me->StartNewAnimation(ANIM_ATTACK,255,ACTION_BUSY,0,0,1,SND_PYGMYSTAB); // spear him
			return;
		}
		if (me->ouch && me->mind1 < 240)
			me->mind1 += 10;
		if (!me->mind1--)
		{
			if (!me->CheckTargetWithinReach(256))
				me->mind = 0;	// get bored again
			else
				me->mind1 = 20;	// stay on trail a little longer
			return;
		}
		me->SetNewSpeed(4);
		me->StartAnimMove(128);
	}
}

void AI_Serpent(Guy* me, Map* map, world_t* world, Guy* goodguy)
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

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,6) && goodguy)
		{
			me->DoFireBulletAngled(BLT_ENERGY,4,8);
			me->DoFireBullet(BLT_ENERGY,15,8);
			me->DoFireBulletAngled(BLT_ENERGY,-4,8);
			me->mind = 0;
		}
		return;	// can't do nothin' right now
	}

	if (goodguy)
	{
		if (me->CheckTargetWithinReach(512) && me->CheckRoll(16,ROLL_ATTACK) && me->reload == 0)
		{
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_SERPENTSPIT);
			FaceGoodguy(me, goodguy);
			return;
		}
	}

	if (!--me->mind)	// time to get a new direction
		me->PickRandomDirection(&me->mind, Random(60)+1);
	me->SetNewSpeed(2);
	me->StartAnimMove(128);
}

void AI_MattieBrain(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte tempface;
	static const byte faceTable[8]={5,4,0,1,2,3,0,6};
	int x, y;

	if (me->ouch == 4 && me->aiType == MONS_MATBRAIN)	// skull and head have their own ouch noises
		me->HandleOuchNoises(SND_MATTIEOUCH,0); // no death sound

	// use facing variable to determine which way for eyes to look
	if (me->action != ACTION_BUSY)
	{
		tempface = me->facing;
		FaceGoodguy(me, goodguy);

		if (me->facing >= 5)	// can't look straight back, so deal with it
		{
			if (tempface < 2)
				me->facing = 0;
			else
				me->facing = 4;
		}
		me->facing = faceTable[me->facing];
		if (me->aiType != MONS_MATBRAIN && me->facing!=0)
			me->facing += 4;
	}
	else
		me->facing = 0;	// always look ahead when doing things

	if (me->seq == ANIM_DIE)
	{
		x = me->x >> FIXSHIFT;
		y = me->y >> FIXSHIFT;
		if (me->aiType == MONS_MATBRAIN)
		{
			BlowUpGuy(x + me->rectx - 40, y + me->recty - 200, x + me->rectx2 + 40, y + me->recty2 + 10, me->z, 2);
			BlowSmoke((x + me->rectx - 40 + Random(me->rectx2 - me->rectx + 80)) << FIXSHIFT,
				(y + me->recty - 200 + Random(me->recty2 - me->recty + 210)) << FIXSHIFT,
				me->z, FIXAMT);
			BlowSmoke((x + me->rectx - 40 + Random(me->rectx2 - me->rectx + 80)) << FIXSHIFT,
				(y + me->recty - 200 + Random(me->recty2 - me->recty + 210)) << FIXSHIFT,
				me->z, FIXAMT);
			ShakeScreen(2);
		}
	}
	// use the claws to cover your face when scared
	if (me->ouch && me->mind1 < 64)
		me->mind1 += 8;	// mind1 is fear, when it gets high, the claws cover her face

	if (me->mind1)
		me->mind1--;
}

void AI_MattieSkullOrHead(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_MATTIEOUCH, SND_MATTIEDIE); // no death sound

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_DIE,5,false))
		{
			if (me->aiType == MONS_MATHEAD)
				me->DoTransform(MONS_MATSKULL);
			else
				me->DoTransform(MONS_MATBRAIN);
			me->StartNewAnimation(ANIM_IDLE,128,ACTION_IDLE,me->dx,me->dy,100);
			return;
		}
		if (me->seq == ANIM_DIE && me->frmTimer < 128)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
				me->z, FIXAMT);
		}
		if (me->CheckSequenceFrame(ANIM_ATTACK,3))
		{
			// shoot laser
			x = me->x;
			y = me->y + FIXAMT * 8;
			LaunchMegabeam(x, y, me->ID);
			me->reload = 0;
			me->mind = 0;
			me->reload = 100;
		}
		me->facing = 0;
		return;
	}

	if ((!me->reload) && me->CheckRoll(100,ROLL_ATTACK) == 0)
		me->StartNewAnimation(ANIM_ATTACK,32,ACTION_BUSY,0,0,0,0);

	// do everything the brain does
	AI_MattieBrain(me, map, world, goodguy);
}

void AI_MattieClaw(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload > 0)
		me->reload--;

	if (!me->parent || me->parent->type == MONS_NONE)
	{
		me->type = MONS_NONE;	// can't exist alone
		return;
	}

	if (me->aiType == MONS_MATCLAW1)
	{
		me->x = me->parent->x - 45 * FIXAMT;
		me->y = me->parent->y + 12 * FIXAMT;
		me->z = me->parent->z - 12 * FIXAMT;
	}
	else	// MONS_MATCLAW2
	{
		me->x = me->parent->x + 45 * FIXAMT;
		me->y = me->parent->y + 12 * FIXAMT;
		me->z = me->parent->z - 12 * FIXAMT;
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
				me->z, FIXAMT);
		}
		if (me->seq == ANIM_A1 && me->frm == 2 && me->parent->mind1 > 32)
			me->frmTimer = 0;	// hold them up until she calms down
		if (me->seq == ANIM_ATTACK && me->frm > 1)
		{
			if (me->frm < 5)
			{
				if (me->aiType == MONS_MATCLAW1)
					x = me->x - FIXAMT * 118;
				else
					x = me->x + FIXAMT * 118;
				y = me->y + FIXAMT * 89;
			}
			else
			{
				if (me->aiType == MONS_MATCLAW1)
					x = me->x - FIXAMT * 69;
				else
					x = me->x + FIXAMT * 69;
				y = me->y + FIXAMT * 141;
			}

			FindVictim(x >> FIXSHIFT, y >> FIXSHIFT, 50, 0, FIXAMT * 8, 8, map, world, me->friendly);
		}
		return;
	}

	if (me->reload == 0)
	{
		me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,Random(180)+20,SND_MATTIECLAW);
		return;
	}

	// make their idle weird and wiggly
	me->frmAdvance = Random(128) + 1;

	if (me->parent->mind1 > 32)	// she is scared, block!!
		me->StartNewAnimation(ANIM_A1,48,ACTION_BUSY,0,0);
}

void AI_MattieTail(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte b;
	int x, y;
	static const short shootXTable[9]={-134,-124,-101,-57,0,67,113,134,143};
	static const short shootYTable[9]={-51,-43,-24,-7,-11,-15,-29,-41,-46};

	if (!me->parent || me->parent->type == MONS_NONE)
	{
		me->type = MONS_NONE;	// can't exist alone
		return;
	}

	me->x=me->parent->x;
	me->y=me->parent->y-104*FIXAMT;
	me->z=me->parent->z;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK)
		{
			b = (32 - me->facing) * 4;
			// shoot some bullets
			x = me->x + (shootXTable[(me->facing >> 2)] << FIXSHIFT);
			y = me->y + (shootYTable[(me->facing >> 2)] << FIXSHIFT);

			b += 16 - Random(33);
			FireExactBullet(x, y, me->z + 32 * FIXAMT, Cosine(b) * 6, Sine(b) * 6, -FIXAMT / 2,
				0, 60, b, BLT_ENERGY, me->friendly);
			if (me->parent->aiType == MONS_MATBRAIN)
			{
				b += 16 - Random(33);
				FireExactBullet(x, y, me->z + 32 * FIXAMT, Cosine(b) * 6, Sine(b) * 6, -FIXAMT / 2,
					0, 120, b, BLT_ENERGY, me->friendly);
			}
		}
		return;
	}

	if (me->mind)
		me->mind--;

	if (me->mind < 3)
	{
		me->StartNewAnimation(ANIM_ATTACK,255,ACTION_BUSY,0,0,0,SND_MATTIESHOOT);
		if (!me->mind)
			me->mind = (me->parent->aiType != MONS_MATBRAIN) ? 80 : 30;
	}

	if (goodguy)
	{
		// point at Bouapha
		if (goodguy->x < me->x - FIXAMT * 128)
		{
			if (goodguy->y < me->y + FIXAMT * 64)
				b = 0;
			else
				b = 8;
		}
		else if (goodguy->x > me->x + FIXAMT * 128)
		{
			if (goodguy->y < me->y + FIXAMT * 64)
				b = 32;
			else
				b = 24;
		}
		else if (goodguy->y > me->y + FIXAMT * 64)
		{
			b = 16;
		}
		else
		{
			// can't point at him when he is behind, but you can try
			if (goodguy->x < me->x)
				b = 0;
			else
				b = 32;
		}
	}
	else
		b = 16;

	// b contains the desired facing
	if (me->mind1 < b)
		me->mind1++;
	else if (me->mind1 > b)
		me->mind1--;
	me->facing = me->mind1 & (~3);	// facing is locked in multiples of 4
}

void AI_MattieBody(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte b;

	if (!me->parent || me->parent->type == MONS_NONE)
	{
		me->type = MONS_NONE;	// can't exist alone
		return;
	}

	// unlike other components, this one tells the head itself where to be
	me->parent->x = me->x;
	me->parent->y = me->y + 40 * FIXAMT;
	me->parent->z = me->z + 32 * FIXAMT;

	if (goodguy)
	{
		if (goodguy->x < me->x)
			me->dx -= FIXAMT / 16;	// slow acceleration
		if (goodguy->x > me->x)
			me->dx += FIXAMT / 16;
		if (goodguy->y < me->y)	//CRUSH him if he gets behind you
			me->dy -= FIXAMT;
		else
		{
			if (me->mind > 128)
			{
				me->mind--;
				me->dy += FIXAMT / 16;
			}
			else if (me->mind < 128)
			{
				me->mind++;
				me->dy -= FIXAMT / 16;
			}
		}
	}
	b = (byte)Random(128);
	if (b < 10)
		me->mind = 128 - Random(64);
	else if (b < 40)
		me->mind = 128 + Random(64);
	else if (b < 50)
		me->mind = 128;

	me->DampenSpeed(32);
	me->ClampSpeed(1);

	if (me->seq != ANIM_MOVE)
	{
		MakeSound(SND_MATTIESTOMP, me->x, me->y, SND_CUTOFF, 1200);
		me->StartAnimMove(64);
	}
}

void AI_Ginger(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_GINGEROUCH,SND_GINGERDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && (me->frm == 2 || me->frm == 4 || me->frm == 11) && me->reload == 0 && goodguy)
		{
			if (me->AttackCheck(32, (me->GetSpaceInFrontX(32)) >> FIXSHIFT, (me->GetSpaceInFrontY(32)) >> FIXSHIFT, goodguy))
				goodguy->GetShot(me->GetSpaceInFrontX(4), me->GetSpaceInFrontY(4), 6, map, world);
			if (me->frm == 11)
			{
				me->frmAdvance = 64;
				me->dx = 0;
				me->dy = 0;
			}
			me->reload = 3;
		}
		if (me->seq == ANIM_A1 && goodguy)
		{
			if (me->frm < 2)
				me->SetNewSpeed(0);
			else
			{
				me->SetNewSpeed(10);
				if (FindVictim((me->GetSpaceInFrontX(16)) >> FIXSHIFT, (me->GetSpaceInFrontY(16)) >> FIXSHIFT, 16, me->GetFacingX()*12, me->GetFacingY()*12, 12, map, world, me->friendly))
				{
					me->seq = ANIM_A2;
					me->frm = 0;
					me->frmTimer = 0;
					me->frmAdvance = 32;
					me->SetNewSpeed(-4);
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)		// when mind=0, singlemindedly stalk Bouapha
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(64) && me->CheckRoll(8,ROLL_ATTACK))
			{
				// get him!
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_GINGERCOMBO);
				me->SetNewSpeed(1);
				return;
			}
			FaceGoodguy(me, goodguy);
			me->SetNewSpeed(2);
			me->StartAnimMove(128);
			if (me->CheckRoll(64,ROLL_MOVE))
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if (me->mind == 1)	// random wandering
	{
		if (me->CheckTargetWithinReach(512) && me->CheckRoll(128,ROLL_MOVE) && GuyCheckTargetWithinSight(me, goodguy, map))
		{
			me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,me->dx,me->dy,0,SND_GINGERJKICK);
			FaceGoodguy(me, goodguy);
			me->dz = FIXAMT * 6;
			return;
		}

		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(2);
		me->StartAnimMove(64);
	}
}

void AI_Pumpkin(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(0,SND_PUMPKINDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->reload == 0)
		{
			if (me->aiType == MONS_BOOMKIN && me->frm == 0)
				FireBullet(me->x, me->y, 0, BLT_BOOM, me->friendly);
			ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 8);
			me->reload = 2;
		}
		return;	// can't do nothin' right now
	}

	if (me->CheckSequenceFrame(ANIM_MOVE,2) && goodguy)	// hits on this frame
	{
		if (me->aiType == MONS_PUMPKIN)
			FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 24, Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 2, map, world, me->friendly);
		else
			if (FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 32, 0, 0, 0, map, world, me->friendly))
			{
				me->SelfDestruct(map, world);
				return;
			}
	}

	if (me->mind == 0)	// not currently aware of goodguy
	{
		me->StartAnimMove(32);
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(256) || me->ouch > 0)
			{
				// if the goodguy is near, or he shot me
				me->mind = 1;	// start hunting
				if (me->ouch == 0)
					me->mind1 = 60;	// for 2 seconds minimum
				else
					me->mind1 = 120;	// 4 seconds, because they HATE getting shot
				FaceGoodguy(me, goodguy);
			}
		}
		me->SetNewSpeed(0);
	}
	else
	{
		FaceGoodguy(me, goodguy);
		if (!goodguy)
		{
			me->mind = 0;	// go back to being bored, nothing to see here
			me->mind1 = 0;
			return;
		}

		if (!--me->mind1)
		{
			if (!me->CheckTargetWithinReach(255))
				me->mind = 0;	// get bored again
			else
				me->mind1 = 20;	// stay on trail a little longer
			return;
		}

		me->SetNewSpeed(me->aiType == MONS_BOOMKIN ? 6 : 2); // boomkins go FAST, pumpkins go SLOW.
		me->StartAnimMove(128);
	}
}

void AI_BabyThing(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(0,SND_PUMPKINDIE);

	if (me->parent)
	{
		// sitting on The Thing's tongue
		switch (me->parent->frm)
		{
		case 4:
		case 5:
			me->y += 10 * FIXAMT;
			break;
		case 6:
		case 7:
			me->y += 15 * FIXAMT;
			break;
		case 8:
			me->parent = NULL;	// I am free!
			if (me->hp > 0)
			{
				me->StartAnimMove(128);
				me->mind1 = 200;
				me->mind = 1;	// pop out in hunt mode
			}
			if (!me->CanWalk(me->x, me->y, map, world))
				me->type = MONS_NONE;	// vanish if this spot is occupied
			break;
		}
		return;
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,5))
			me->DoFireBullet(BLT_ENERGY,5,16);
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// not currently aware of goodguy
	{
		if (me->seq == ANIM_MOVE)
			me->StartNewAnimation(ANIM_IDLE,32,me->action,0,0);

		if (me->mind1 < 31)
			me->mind1++;

		if (me->mind1 > 30 && me->CheckRoll(50, ROLL_MOVE))
		{
			switch (Random(16))
			{
			case 0:
				me->StartNewAnimation(ANIM_A1,32,ACTION_BUSY,0,0,0,SND_BABYSNORE); // take a nap
				break;
			case 1:
			case 2:
			case 3:
				me->StartNewAnimation(ANIM_A3,64,ACTION_BUSY,0,0); // look left
				break;
			case 4:
			case 5:
			case 6:
				me->StartNewAnimation(ANIM_A4,64,ACTION_BUSY,0,0); // look right
				break;
			case 7:
			case 8:
				me->mind	= 2;	// go into wander mode
				me->mind1	= Random(40) + 1;
				break;
			default:	// blink is very common
				me->StartNewAnimation(ANIM_A2,128,ACTION_BUSY,0,0); // look right
				break;
			}
		}
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(128) || me->ouch > 0)
			{
				// if the goodguy is near, or he shot me
				me->mind	= 1;	// start hunting
				me->mind1	= 255;
				FaceGoodguy(me, goodguy);
			}
		}
		me->SetNewSpeed(0);
	}
	else if (me->mind == 1)	// hunting mode
	{
		FaceGoodguy(me, goodguy);
		if (!goodguy)
		{
			me->mind = 0;	// go back to being bored, nothing to see here
			me->mind1 = 0;
			return;
		}
		if (me->CheckTargetWithinReach(512) && me->CheckRoll(32,ROLL_ATTACK))
		{
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,1);
			return;
		}

		if (me->ouch && me->mind1 < 240)
			me->mind1 += 10;
		if (me->mind1)
			me->mind1--;
		else
		{
			if (!me->CheckTargetWithinReach(257))
				me->mind = 0;	// get bored again
			else
				me->mind1 = 20;	// stay on trail a little longer
			return;
		}
		if (me->CheckRoll(80,ROLL_MOVE))
		{
			me->mind1 = 20;
			me->mind = 2;	// wander
		}
		me->SetNewSpeed(2);
		me->StartAnimMove(128);
	}
	else	// random wander mode
	{
		if (!(me->mind1--))	// time to get a new direction
		{
			switch (Random(3))
			{
			case 0:
				if (goodguy)
					me->mind = 1;	// hunt him
				break;
			case 1:
				me->mind = 0;	// stop here and rest
				me->mind1 = 0;
				break;
			case 2:	// just wander a different way
				me->PickRandomDirection(&me->mind1,Random(20+1));
				break;
			}
		}
		me->SetNewSpeed(2);
		me->StartAnimMove(128);
	}
	// this is needed because his idle frame doesn't fit into his walking animation, so this skips it
	if (me->seq == ANIM_MOVE && me->frm == 10 && me->frmTimer > 127)
	{
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 128;
	}
}

// Used by moss enemies to create more moss
void MakeMoss(Guy *me, int dx, int dy, int type=MONS_MOSS)
{
	if (Guy* baby = AddBaby(me->x, me->y, 0, type, me))
	{
		baby->action	= ACTION_BUSY;
		baby->seq		= ANIM_ATTACK;
		baby->frm			= 0;
		baby->frmTimer		= 0;
		baby->frmAdvance	= 64;
		baby->dx		= dx;
		baby->dy		= dy;
	}
}

// check the moss
void DoMossMaking(Guy* me, Map* map, world_t* world, byte n=0)
{
	int x, y;
	x = (me->x >> FIXSHIFT) / TILE_WIDTH;
	y = (me->y >> FIXSHIFT) / TILE_HEIGHT;
	switch (n)
	{
	case 0:
		// left
		if (x > 0 && map->GetTile(x - 1, y)->wall == 0 &&
			(GetTerrain(world, map->GetTile(x - 1, y)->floor)->flags & (TF_WATER | TF_LAVA | TF_SOLID)) == 0 &&
			!(GetItem(map->GetTile(x - 1, y)->item)->flags & (IF_SOLID | IF_BULLETPROOF))
			&& (!MossCheck(x - 1, y)))
			MakeMoss(me, -(32 * FIXAMT) / 20, 0);
		break;
	case 1:
		// right
		if (x < map->width - 1 && map->GetTile(x + 1, y)->wall == 0 &&
			(GetTerrain(world, map->GetTile(x + 1, y)->floor)->flags & (TF_WATER | TF_LAVA | TF_SOLID)) == 0 &&
			!(GetItem(map->GetTile(x + 1, y)->item)->flags & (IF_SOLID | IF_BULLETPROOF))
			&& (!MossCheck(x + 1, y)))
			MakeMoss(me, (32 * FIXAMT) / 20, 0);
		break;
	case 2:
		// up
		if (y > 0 && map->GetTile(x, y - 1)->wall == 0 &&
			(GetTerrain(world, map->GetTile(x, y - 1)->floor)->flags & (TF_WATER | TF_LAVA | TF_SOLID)) == 0 &&
			!(GetItem(map->GetTile(x, y - 1)->item)->flags & (IF_SOLID | IF_BULLETPROOF))
			&& (!MossCheck(x, y - 1)))
			MakeMoss(me, 0, -(24 * FIXAMT) / 20);
		break;
	case 3:
		// down
		if (y < map->height - 1 && map->GetTile(x, y + 1)->wall == 0 &&
			(GetTerrain(world, map->GetTile(x, y + 1)->floor)->flags & (TF_WATER | TF_LAVA | TF_SOLID)) == 0 &&
			!(GetItem(map->GetTile(x, y + 1)->item)->flags & (IF_SOLID | IF_BULLETPROOF))
			&& (!MossCheck(x, y + 1)))
			MakeMoss(me, 0, (24 * FIXAMT) / 20);
		break;
	}
}

void AI_Moss(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *baby;

	if(me->reload)
		me->reload--;

	if(me->mind)
		me->mind--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq!=ANIM_DIE && me->frm==4)
		{
			x=((me->x>>FIXSHIFT)/TILE_WIDTH);
			y=((me->y>>FIXSHIFT)/TILE_HEIGHT);
			me->x=(x*TILE_WIDTH+TILE_WIDTH/2)<<FIXSHIFT;
			me->y=(y*TILE_HEIGHT+TILE_HEIGHT/2)<<FIXSHIFT;
			me->dx=0;
			me->dy=0;	// you are at your destination
			me->reload = (me->aiType == MONS_MOSS2) ? (me->CheckRoll(60,ROLL_ATTACK)+10) : (me->CheckRoll(200, ROLL_ATTACK) + 55);

			// this prevents overlapping ones, in cases of ones getting born near-simultaneously
			me->mind3=me->aiType;
			me->aiType=MONS_NONE;	// so it doesn't get counted in the moss check
			if(MossCheck(x,y))
			{
				if(!me->friendly)
					player.totalEnemies--;
				return;	// with type=MONS_NONE, so it is dead
			}
			me->aiType=me->mind3;
		}
		return;
	}

	// if goodguy steps on you, hurt him
	if(goodguy && !me->mind && me->CheckTargetWithinReach(20))
	{
		goodguy->GetShot(0,0,4,map,world);
		me->mind=5;	// so as not to hurt him too often
	}
	if(!me->reload)
	{
		me->reload = (me->aiType == MONS_MOSS2) ? (me->CheckRoll(60,ROLL_ATTACK)+10) : (me->CheckRoll(200, ROLL_ATTACK) + 55);
		DoMossMaking(me, map, world, Random(4)); // spawn the moss
	}
}

void AI_MossGrande(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *baby;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_ATTACK,11,false) && me->mind1==0)
		{
			for(x=0;x<16;x++)
				FireBullet(me->x,me->y,x*16,BLT_ENERGY,me->friendly);	// ring of fire
			me->mind1=60;
		}
		return;
	}

	if(me->seq==ANIM_IDLE)
		me->frmAdvance=Random(200)+1;	// animate idle at random speeds

	if(!me->mind1 && me->CheckRoll(100,ROLL_ATTACK))
	{
		me->mind1=0;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
	}
	if(me->CheckRoll(200,ROLL_ATTACK))
	{
		me->reload=255;
		// spawn in all four directions at once
		for(i=0;i<4;i++)
			DoMossMaking(me,map,world,i);
	}
}

//me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,me->reload);
void AI_Magmazoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SERPENTOUCH,SND_SERPENTDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrames(ANIM_ATTACK,6,13) && goodguy)
		{
			// spit stuff
			me->DoFireBulletAngled(BLT_FLAME2,(me->facing+me->frm-9)*2,16);
			me->mind = 0;
		}
		if (me->CheckSequenceFrame(ANIM_ATTACK,13))
			me->reload = 20;
		return;	// can't do nothin' right now
	}
	// attack if close enough (and targer is within los)
	if (goodguy)
	{
		if (me->CheckTargetWithinReach(512) && me->CheckRoll(16,ROLL_ATTACK))
		{
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0);
			FaceGoodguy(me, goodguy);
			return;
		}
	}
	if (!--me->mind) // time to get a new direction
		me->PickRandomDirection(&me->mind,Random(60)+1);
	me->SetNewSpeed(2);
	me->StartAnimMove(128);
}

void AI_Shroom(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_BUSHOUCH,SND_BUSHDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,5) && goodguy)
		{
			for (i = 0;i < 256;i += 16)
				FireBullet(me->x, me->y, i, BLT_SPORE, me->friendly);
			me->reload = 5;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(72) && me->CheckRoll(8,ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_SKELKICK);
				return;
			}
			FaceGoodguy(me, goodguy);
			me->SetNewSpeed(1);
			me->StartAnimMove(128);

			if (me->CheckRoll(64,ROLL_MOVE))
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if (me->mind == 1)	// random wandering
	{
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(1);
		me->StartAnimMove(128);
	}
}

void AI_Mush(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i, x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_MUSHOUCH,SND_MUSHDIE);

	if (me->action == ACTION_BUSY)
	{
		if (goodguy && me->CheckSequenceFrames(ANIM_A1,1,15))
		{
			x = me->x + Cosine(me->facing * 32) * 72;
			y = me->y + Sine(me->facing * 32) * 64;
			i = (me->facing * 32 - 64) & 255;
			x += Cosine(i) * 48;
			y += Sine(i) * 32;
			i = (me->facing * 32 - 16 + Random(33)) & 255;
			FireExactBullet(x, y, FIXAMT * 64, Cosine(i) * 12, Sine(i) * 12, 0, 0, 16, i, BLT_SPORE, me->friendly);
			MakeSound(SND_MUSHSPORES, me->x, me->y, SND_CUTOFF, 600);
			me->reload = 1;
		}
		if (me->CheckSequenceFrame(ANIM_A1,16))
			me->frmAdvance = 64;	// slow the animation for the recovery part
		if (me->seq == ANIM_ATTACK && me->frm == 1 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 72;
			y = me->y + Sine(me->facing * 32) * 64;
			i = (me->facing * 32 + 64) & 255;
			x += Cosine(i) * 48;
			y += Sine(i) * 32;
			FireBullet(x, y, me->facing, BLT_SHROOM, me->friendly);
			MakeSound(SND_MUSHMISSILE, me->x, me->y, SND_CUTOFF, 1000);
			me->reload = 5;
		}
		if (me->seq == ANIM_DIE)
		{
			me->facing = 2;	// die only works from forward (memory saver)
			if (me->frm >= 3)
			{
				x = me->x >> FIXSHIFT;
				y = me->y >> FIXSHIFT;
				BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
				BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
					(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
					me->z, FIXAMT);
			}
		}

		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// this mode is only used prior to him seeing Bouapha the first time
	{
		if (goodguy)
		{
			if ((me->CheckTargetWithinReach(256) && GuyCheckTargetWithinSight(me,goodguy,map))|| me->ouch)
			{
				me->StartNewAnimation(ANIM_A2,128,ACTION_BUSY,0,0,0,SND_MUSHMAD); // get MAD!
				me->mind = 2;		// destroy bouapha
				me->facing = 2;	// angry animation ONLY works from forward (memory saver)
			}
		}
	}
	else if (me->mind == 2)		// when mind=2, hold still and go off on Bouapha
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(256) && me->CheckRoll(32,ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0); // fire spore cannon
				return;
			}
			if (me->CheckTargetWithinReach(256) && me->CheckRoll(28,ROLL_ATTACK))
			{
				// get him! (fire sporegun)
				me->StartNewAnimation(ANIM_A1,256,ACTION_BUSY,0,0,0); // fire shroom cannon
				return;
			}
			FaceGoodguy2(me, goodguy);
			me->SetNewSpeed(0);
			me->StartAnimIdle();
			if (me->CheckTargetWithinReach(256) && GuyCheckTargetWithinSight(me,goodguy,map))
				me->mind = 1;
		}
		else
		{
			// just sit there
		}
	}
	else if (me->mind == 1)	// chase down Bouapha
	{
		if (goodguy)
		{
			FaceGoodguy2(me, goodguy);
			me->SetNewSpeed(8);
			me->StartAnimMove(128);
			if (me->CheckTargetWithinReach(200))
				me->mind = 2;	// in range, start killin'
		}
		else
			me->mind = 0;
	}
}

void AI_TheThing(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	Guy* g;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_MUSHOUCH,SND_MUSHDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			if (me->frm == 0)
				KillKids(me);	// destroy the tentacles
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 2);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
				me->z, FIXAMT);
			me->x += ((1 - Random(3)) << FIXSHIFT);
			me->y += ((1 - Random(3)) << FIXSHIFT);
		}
		if (me->seq == ANIM_ATTACK && me->frm == 3 && me->frmTimer < 32)
		{
			if (g = AddBaby(me->x, me->y, 0, MONS_BABYTHING, me))
			{
				g->parent = me;
				g->StartNewAnimation(ANIM_A1,32);
				g->action = ACTION_BUSY;
			}
		}
		return;	// can't do nothin' right now
	}
	if (me->CheckRoll(128,ROLL_MOVE))	// blink
	{
		g->StartNewAnimation(ANIM_A1, 32);
		me->action = ACTION_BUSY;
	}
	if (!me->reload && me->CheckRoll(128,ROLL_ATTACK))
	{
		MakeSound(SND_MUSHMISSILE, me->x, me->y, SND_CUTOFF, 1500);
		FireBullet(me->x - 48 * FIXAMT, me->y - 10 * FIXAMT, Random(88) + 20, BLT_GRENADE, me->friendly);
		FireBullet(me->x + 48 * FIXAMT, me->y - 10 * FIXAMT, Random(88) + 20, BLT_GRENADE, me->friendly);
		me->reload = 30;
	}
	if (me->CheckRoll(128,ROLL_ATTACK))	// tongue
	{
		g->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,0,0,0,SND_THINGVOMIT);
	}
}

void AI_ThingTentacle(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	Guy* g;

	if (me->reload)
		me->reload--;

	if (me->mind1 < 128)
		me->mind1++;
	if (me->mind1 > 128)
		me->mind1--;

	if ((!me->parent) || me->parent->type == MONS_NONE)
	{
		if (me->hp > 0 && !me->friendly)
			player.enemiesSlain++;
		me->type = MONS_NONE;
		return;
	}

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_ARMORHIT,0);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
				me->z, FIXAMT);
			me->x += ((1 - Random(3)) << FIXSHIFT);
			me->y += ((1 - Random(3)) << FIXSHIFT);
			me->facing = (me->facing + 1 - Random(3)) & 15;
			FlailLock(me);
			if (me->frm == 4 && me->parent && me->parent->aiType == MONS_THINGTENT && me->parent->hp > 0)	// kill parent (unless it's The Thing)
			{
				me->parent->hp = 1;
				me->parent->GetShot(0, 0, 10, map, world);
			}
			g = me->parent;
			while (g && g->aiType == MONS_THINGTENT)
			{
				// make the whole thing flail
				g->facing = (g->facing + 1 - Random(3)) & 15;
				FlailLock(g);
				g = g->parent;
			}
			if (me->parent && me->parent->aiType == MONS_THINGTENT)
			{
				me->x = me->parent->x + Cosine(me->parent->facing * 16) * 64;
				me->y = me->parent->y + Sine(me->parent->facing * 16) * 44;
			}
		}

		return;	// can't do nothin' right now
	}

	if (me->parent->aiType == MONS_THINGTENT)
	{
		// send damage up and down the list
		if (me->hp > me->parent->hp)
		{
			me->hp = me->parent->hp;
			me->ouch = 4;
		}
		else if (me->hp < me->parent->hp)
		{
			me->parent->hp = me->hp;
			me->parent->ouch = 4;
		}
		if (me->parent->hp == 0 && me->seq != ANIM_DIE)
		{
			me->SelfDestruct(map,world);
		}
	}

	FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 32, 0, 0, 2, map, world, me->friendly);

	if (me->parent->aiType == MONS_THINGTENT)
		me->mind1 = me->parent->mind1;
	else
	{
		if (me->mind1 == 128)
			me->mind1 = 128 - 20 + Random(41);
	}
	if (Random(64) == 0)
		me->mind1 = 128 - 20 + Random(41);
	else
	{
		if (me->mind1 < 128)
			me->facing = (me->facing + 1) & 15;
		else if (me->mind1 > 128)
			me->facing = (me->facing - 1) & 15;
	}

	// keep tentacles within allowable angles
	FlailLock(me);

	if (me->parent->aiType == MONS_THINGTENT)
	{
		me->x = me->parent->x + Cosine(me->parent->facing * 16) * 64;
		me->y = me->parent->y + Sine(me->parent->facing * 16) * 44;
	}
	else if (me->parent->aiType != MONS_THING)
	{
		switch (me->mind)
		{
		case 0:
			me->x = me->parent->x - 40 * FIXAMT;
			me->y = me->parent->y - 1 * FIXAMT;
			break;
		case 1:
			me->x = me->parent->x + 40 * FIXAMT;
			me->y = me->parent->y - 1 * FIXAMT;
			break;
		case 2:
			me->x = me->parent->x + 40 * FIXAMT;
			me->y = me->parent->y - 16 * FIXAMT;
			break;
		case 3:
			me->x = me->parent->x - 40 * FIXAMT;
			me->y = me->parent->y - 16 * FIXAMT;
			break;
		}
	}
}

void AI_SuperZombie(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i, x, y;

	if (me->reload)
		me->reload--;

	if (me->mind1)
		me->mind1--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SZOUCH,SND_SZDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_A1)	// leaping attack
		{
			if (me->frm < 3)
			{
				me->SetNewSpeed(0);
				me->dz = 0;
			}
			else if (me->frm == 3)
			{
				me->SetNewSpeed(12);
				me->dz = 10 * FIXAMT;
			}
			else if (me->frm < 7)
			{
				// stay the course
				Dampen(&me->dx, FIXAMT / 8);
				Dampen(&me->dy, FIXAMT / 8);
			}
			else if (me->frm == 12 && me->frmTimer < 32)
			{
				FireBullet(me->x, me->y - FIXAMT, 0, BLT_SHOCKWAVE, me->friendly);
				MakeSound(SND_BOMBBOOM, me->x, me->y, SND_CUTOFF, 1500);
				ShakeScreen(10);
			}
			else
				me->DampenSpeed(2);
		}
		if (me->seq == ANIM_A2 && me->frm == 1 && me->reload == 0 && goodguy)
		{
			// can only do the pickup move on Bouapha
			if (me->CheckTargetWithinReach(70) && goodguy->aiType == MONS_BOUAPHA && goodguy->hp > 0 && !PlayerShield() && !PlayerUsingMechWeapon())
			{
				// got him
				MakeSound(SND_SZGRAB, me->x, me->y, SND_CUTOFF, 1500);
				me->StartNewAnimation(ANIM_A3,64,ACTION_BUSY,0,0,100,SND_SZGRAB); // begin lifting, cooldown to prevent multigrabs
				goodguy->parent = me;
				goodguy->facing = (me->facing + 4) & 7;
				pickupX = goodguy->x;
				pickupY = goodguy->y;
				goodguy->StartNewAnimation(ANIM_MOVE,512,ACTION_BUSY,0,0); // wriggling out
				goodguy->x = me->x + me->GetSpaceInFrontX(60);
				goodguy->y = me->y + me->GetSpaceInFrontY(60);
				goodguy->z = 0;
				goodguy->dz = 0;
				return;
			}
			me->reload = 4;
		}
		if (me->seq == ANIM_A3 && goodguy)
		{
			if (PlayerUsingMechWeapon())
				me->StartAnimIdle();
			else
			{
				if ((me->frm == 5 || me->frm == 8 || me->frm == 11 || me->frm == 14 || me->frm == 17) && me->frmTimer < 32)
				{
					MakeSound(SND_MISSILEBOOM, me->x, me->y, SND_CUTOFF, 1500);
					goodguy->GetShot(0, 0, 8, map, world);
				}

				if (me->frm == 21)
				{
					goodguy->x = pickupX;
					goodguy->y = pickupY;
				}
			}
		}
		if (me->CheckSequenceFrame(ANIM_ATTACK,5))
		{
			x = (me->x + Cosine(me->facing * 32)*40) >> FIXSHIFT;
			y = (me->y + Sine(me->facing * 32)*40) >> FIXSHIFT;
			if (me->AttackCheck(30, x, y, goodguy))
				goodguy->GetShot(Cosine(me->facing * 32)*16, Sine(me->facing * 32)*16, 15, map, world);
			me->reload = 4;
		}
		if (me->seq == ANIM_DIE)
		{
			if (me->frm >= 8)
			{
				x = me->x >> FIXSHIFT;
				y = me->y >> FIXSHIFT;
				BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
				BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
					(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
					me->z, FIXAMT);
			}
		}

		return;	// can't do nothin' right now
	}

	if (me->mind == 0)
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(300))
				me->mind = 1;
		}
		if (me->ouch)
			me->mind = 1;
	}
	else if (me->mind == 2)		// when mind=2, hold still and go off on Bouapha
	{
		if (goodguy)
		{
			i = RangeToTarget(me, goodguy)/FIXAMT;
			if (!me->reload && i<96 && me->CheckRoll(10, ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0); // get him!! (punch)
				return;
			}
			if (!me->reload && i<96 && me->CheckRoll(10,ROLL_ATTACK) && goodguy->aiType == MONS_BOUAPHA) // only do this for the player
			{
				me->StartNewAnimation(ANIM_A2,128,ACTION_BUSY,0,0,0); // get him!! (grab)
				return;
			}
			FaceGoodguy2(me, goodguy);
			me->DampenSpeed(2);
			me->StartAnimIdle();
			if (i>80 && !me->mind1)
			{
				// chase him down!
				me->mind = 1;
				me->mind1 = 8;
			}
		}
		else
		{
			// just sit there
		}
	}
	else if (me->mind == 1)	// chase down Bouapha
	{
		if (goodguy)
		{
			FaceGoodguy2(me, goodguy);
			me->SetNewSpeed(4);
			me->StartAnimMove(128);
			if (!me->mind1 && me->CheckTargetWithinReach(128))
			{
				me->mind = 2;	// in range, start killin'
				me->mind1 = 8;
			}
			else if (Random(64) == 0)
				me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,0,0,SND_SZLEAP); // leap!
		}
		else
			me->mind = 0;
	}
}

void AI_StickMan(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->HandleOuchNoises(SND_HAPPYOUCH,SND_HAPPYDIE);
		if (me->hp > 0)
			me->StartNewAnimation(ANIM_A2,32);
	}

	if (me->CheckSequenceFrame(ANIM_A2,1,false))
		me->StartNewAnimation(ANIM_IDLE,64);
	if (me->action == ACTION_BUSY)
	{
		if (goodguy && me->CheckSequenceFrame(ANIM_ATTACK,1))
		{
			if (me->CheckTargetWithinReach(60))
				goodguy->GetShot(me->GetSpaceInFrontX(16), me->GetSpaceInFrontY(16), 10, map, world);
			me->reload = 4;
		}
		if (me->CheckSequenceFrame(ANIM_ATTACK, 4, false))
			me->StartAnimIdle();
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// waiting for Bouapha
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(300) && GuyCheckTargetWithinSight(me,goodguy,map) || me->ouch)
			{
				me->StartNewAnimation(ANIM_A1, 64, ACTION_BUSY, 0, 0, 0, SND_HAPPYFLEX);
				me->mind = 1;
			}
		}
	}
	else if (me->mind == 1)	// chase down Bouapha
	{
		if (goodguy)
		{
			if (!me->reload && me->CheckRoll(4,ROLL_ATTACK) && me->CheckTargetWithinReach(60))
			{
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,me->GetFacingX()*2,me->GetFacingY()*2,SND_HAPPYCHOMP);
				return;
			}

			FaceGoodguy2(me, goodguy);
			me->SetNewSpeed((me->seq != ANIM_A2) ? 6 : 2);

			if (me->seq != ANIM_A2)
				me->StartAnimMove(128);

			if (me->CheckRoll(256,ROLL_ATTACK))
				me->StartNewAnimation(ANIM_A1,64,ACTION_BUSY,0,0,0,SND_HAPPYFLEX);
		}
		else
			me->mind = 0;
	}
}

void AI_BabySeal(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte f;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SEALOUCH,SND_SEALDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frmTimer < 32)
		{
			f = me->facing * 32;
			switch (me->frm)
			{
			case 3:
				f -= 20;
				break;
			case 5:
				f -= 10;
				break;
			case 7:
				f -= 4;
				break;
			case 9:
				f += 4;
				break;
			case 11:
				f += 10;
				break;
			case 13:
				f += 20;
				break;
			default:
				return;	// don't fire on this frame
			}
			f = f - 8 + Random(17);
			x = me->x + Cosine(f) * 16;
			y = me->y + Sine(f) * 16;
			FireBullet(x, y, f, BLT_ENERGY, me->friendly);
			MakeSound(SND_BULLETFIRE, x, y, SND_CUTOFF, 1050);
			me->reload = 20;
		}
		if (me->seq == ANIM_DIE && me->frm >= 4)
		{
			for (f = 0;f < 8;f++)
				SpecialSnow(me->x + ((-32 + Random(65)) << FIXSHIFT),
					me->y + ((-32 + Random(65)) << FIXSHIFT));
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(700) && me->CheckRoll(64,ROLL_ATTACK) && !me->reload)
			{
				me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,0);
				return;
			}
			FaceGoodguy(me, goodguy);
			me->SetNewSpeed(2);
			me->StartAnimMove(128);
			if (me->CheckRoll(128, ROLL_RANDWALK))
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if (me->mind == 1)	// random wandering
	{
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(2);
		me->StartAnimMove(128);
	}
}

void AI_Isozoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SERPENTOUCH, SND_SERPENTDIE);

	if (me->mind == 0 && me->hp > 0)	// hasn't spotted Bouapha yet
	{
		if (me->CheckTargetWithinReach(200) && GuyCheckTargetWithinSight(me,goodguy,map) || (me->ouch))
		{
			me->mind = 1;
			me->StartNewAnimation(ANIM_A1,72);
			me->action = ACTION_BUSY;
			ExplodeParticles2(PART_SNOW2, me->x, me->y, 0, 12, 6);
		}
		else if (me->seq != ANIM_A2)
			me->StartNewAnimation(ANIM_A2,1);
		else if (me->frmTimer > 20)
			me->frmTimer = 0;	// keep him on that frame
		FaceGoodguy3(me, goodguy);
		return;
	}

	if (me->action == ACTION_BUSY)
	{
		if (goodguy && me->CheckSequenceFrame(ANIM_ATTACK,5))
		{
			// spit snowball
			x = me->GetSpaceInFrontX(8);
			y = me->GetSpaceInFrontY(8);
			FireBullet(x, y, (me->facing * 32 - 16 + Random(33)) & 255, BLT_SNOWBALL, me->friendly);
			me->reload = 10;
		}
		return;	// can't do nothin' right now
	}

	if (goodguy)
	{
		if (me->CheckTargetWithinReach(200) && GuyCheckTargetWithinSight(me,goodguy,map) && me->CheckRoll(8,ROLL_ATTACK))
		{
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_SERPENTSPIT);
			return;
		}
	}

	FaceGoodguy3(me, goodguy);
	me->SetNewSpeed(0);
	me->StartAnimIdle(128);
}

void AI_Snowguy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SNOWOUCH,SND_SNOWDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,3) && goodguy)
		{
			x = me->GetSpaceInFrontX(20);
			y = me->GetSpaceInFrontY(20);
			if (me->AttackCheck(20, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
				goodguy->GetShot(me->GetFacingX()*4, me->GetFacingY()*4, 8, map, world);
			me->reload = 5;
		}
		if (me->CheckSequenceFrame(ANIM_A1,11) && goodguy)
		{
			// only fire if the range is good
			if (me->CheckTargetWithinReach(800))
			{
				me->DoFireBullet(BLT_BIGSNOW,10,32);
				FireBullet(x, y, me->facing * 32, BLT_BIGSNOW, me->friendly);
			}
			else
				me->reload = 4;
			me->mind1 = 1;
		}
		if (me->seq == ANIM_DIE)
			me->frmAdvance = 128;
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(64) && me->CheckRoll(8,ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_SKELKICK);
				FaceGoodguy(me, goodguy);
				return;
			}
			FaceGoodguy(me, goodguy);
			me->SetNewSpeed(2);
			me->StartAnimMove(128);
			if (me->CheckRoll(64,ROLL_MOVE))
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if (me->mind == 1)	// random wandering
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(512) && me->CheckRoll(32,ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,0,0,0,SND_SNOWTHROW);
				FaceGoodguy(me, goodguy);
				return;
			}
		}
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(2);
		me->StartAnimMove(128);
	}
}

void AI_Penguin(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_PENGDIE, SND_PENGDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK)
		{
			if (me->frm < 4)
			{
				me->SetNewSpeed(7);
				if (me->AttackCheck(10, me->GetSpaceInFrontX(10) >> FIXSHIFT, me->GetSpaceInFrontY(10) >> FIXSHIFT, goodguy))
				{
					goodguy->GetShot(me->GetFacingX()*4, me->GetFacingY()*4, 4, map, world);
					me->frm = 4;	// bounce back
					me->frmTimer = 0;
				}
			}
			else
				me->SetNewSpeed(-3);
			me->reload = 10;
		}

		if (me->seq == ANIM_DIE)
			me->Speen(128);

		return;	// can't do nothin' right now
	}

	if (me->mind == 2)		// when mind=2, pursue relentlessly
	{
		if (goodguy)
		{
			if (me->CheckTargetWithinReach(128))
				me->mind = 1; // circling mode

			FaceGoodguy2(me, goodguy); // pursue
			me->SetNewSpeed(4);
			me->StartAnimMove(256);
		}
		else
		{
			me->mind = 0;	// if there's no goodguy, get random
			me->mind1 = 1;
		}
	}
	else if (me->mind == 0)	// random wandering
	{
		if (me->CheckTargetWithinReach(128))
			me->mind = 1; // circling mode

		me->TryGetNewDirection(&me->mind1,40,goodguy,false);
		me->SetNewSpeed(4);
		me->StartAnimMove(256);
	}
	else if (me->mind == 1)	// circling
	{
		FaceGoodguy2(me, goodguy);

		// move sideways
		me->dx = Cosine((me->facing * 32 + 64) & 255) * 3;
		me->dy = Sine((me->facing * 32 + 64) & 255) * 3;
		me->StartAnimMove(256);

		if (!me->CheckTargetWithinReach(128))
		{
			me->mind = 2;	// chase him down
		}

		if (!me->reload && me->CheckRoll(32,ROLL_ATTACK))
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_PENGPECK);
	}
}

void AI_Zomboni(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			if (me->frm >= 4)
			{
				x = me->x >> FIXSHIFT;
				y = me->y >> FIXSHIFT;
				BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 2);
				BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT, (y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT, me->z, FIXAMT);
			}
			if (me->frm == 8)
			{
				me->DoTransform(MONS_ZOMBIE);
				me->mind = 0;
				me->mind1 = 0;
				me->StartNewAnimation(ANIM_IDLE, 32, ANIM_IDLE, 0, 0);
				return;
			}
		}
		return;
	}

	if (me->CheckSequenceFrame(ANIM_MOVE,1) && me->frmTimer < 32)
		MakeSound(SND_ZOMBONIZOOM, me->x, me->y, SND_CUTOFF, 1500);

	// Wake up
	if (me->mind == 0)
	{
		if (me->CheckTargetWithinReach(400) || me->ouch)
			me->mind = 1;
	}
	else
	{
		if (goodguy)
		{
			int desired;

			// Find the 8-direction angle to the player.
			if (goodguy->x < me->x - FIXAMT * 16)
			{
				if (goodguy->y < me->y - FIXAMT * 16)
					desired = 5;
				else if (goodguy->y > me->y + FIXAMT * 16)
					desired = 3;
				else
					desired = 4;
			}
			else if (goodguy->x > me->x + FIXAMT * 16)
			{
				if (goodguy->y < me->y - FIXAMT * 16)
					desired = 7;
				else if (goodguy->y > me->y + FIXAMT * 16)
					desired = 1;
				else
					desired = 0;
			}
			else
			{
				if (goodguy->y < me->y - FIXAMT * 16)
					desired = 6;
				else
					desired = 2;
			}

			int diff = abs(desired - me->facing);

			if (diff > 4) // handle wraparound
				diff = 8 - diff;


			// Only adjust direction every few frames.
			if (diff != 0)
			{
				if (!me->mind1--)
				{
					FaceGoodguy2(me, goodguy);
					me->mind1 = 3; // turn delay
				}
			}
			else
				me->mind1 = 0; // perfectly aligned, stay locked!

			me->AccelerateToTargetSpeed(8,8,&me->facing);
			me->StartAnimMove(128);

			// Hit player.
			if (me->frm == 0)
			{
				BlowSmoke(me->x-Cosine(me->facing*32)*32,me->y-Sine(me->facing*32)*32,me->z+FIXAMT*40,FIXAMT);
				if(goodguy && RangeToTarget(me,goodguy)<90*FIXAMT)
				{
					// smash into him
					goodguy->GetShot(Cosine(me->facing*32)*12,Sine(me->facing*32)*12,10,map,world);
				}
			}
		}
		else
		{
			me->mind = 0; // go back to being still
		}
	}
}

void AI_Yeti(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_YETIOUCH,SND_YETIDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			me->facing = 2;	// can only die forward
			if (me->frm >= 4)
			{
				x = me->x >> FIXSHIFT;
				y = me->y >> FIXSHIFT;
				BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 2);
				BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
					(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
					me->z, FIXAMT);
			}
		}
		if (me->CheckSequenceFrame(ANIM_ATTACK,7) && me->frmTimer < 32)
		{
			x = me->x + Cosine(me->facing * 32) * 64;
			y = me->y + Sine(me->facing * 32) * 64;
			x += Cosine(((me->facing + 2) * 32) & 255) * 32;
			y += Sine(((me->facing + 2) * 32) & 255) * 32;
			FireExactBullet(x, y, 40 * FIXAMT, Cosine(me->facing * 32) * 8, Sine(me->facing * 32) * 8, 4 * FIXAMT, 0, 100,
				me->facing * 32, BLT_BIGSNOW, me->friendly);
			me->reload = 10;
		}
		if (me->seq == ANIM_A2 && me->frm > 3)
		{
			ShakeScreen(3);
			// snowballs fall all over
			x = me->x + ((320 - Random(641)) << FIXSHIFT);
			y = me->y + ((240 - Random(481)) << FIXSHIFT);
			if (Random(3) == 0)
				FireExactBullet(x, y, 400 * FIXAMT, 0, 0, 0, 0, 100, me->facing * 32, BLT_BIGSNOW, me->friendly);
			else
				FireExactBullet(x, y, 400 * FIXAMT, 0, 0, 0, 0, 100, me->facing * 32, BLT_SNOWBALL, me->friendly);
		}
		if (me->seq == ANIM_A1 && (me->frm == 5 || me->frm == 6))
		{
			x = me->x + Cosine(me->facing * 32) * 64;
			y = me->y + Sine(me->facing * 32) * 64;
			FireBullet(x, y, me->facing, BLT_ICESPIKE, me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)
	{
		if ((goodguy && RangeToTarget(me, goodguy) < (300 * FIXAMT)) || (me->ouch))
		{
			me->mind = 1;	// start into action
			me->mind1 = 0;;
			me->facing = 2;
			me->StartNewAnimation(ANIM_A3,64,ACTION_BUSY,0,0,0,(me->aiType==MONS_SVEN) ? SND_SVENHI : SND_BJORNHI);
		}
	}
	else
	{
		if (!me->mind1--)
		{
			// rotate
			me->Speen(-1+Random(3));
			me->mind1 = Random(30) + 1;
		}

		if (!me->reload && goodguy && me->CheckRoll(20,ROLL_ATTACK))
		{
			// select an attack method
			x = Random(4);
			if (x < 3) // 3 in 4 chance of primary weapon
			{
				me->StartNewAnimation((me->aiType == MONS_SVEN)?ANIM_ATTACK:ANIM_A1,128,ACTION_BUSY,0,0); // Sven throws snowballs, Bjorn throws spikes.
				FaceGoodguy(me, goodguy);
			}
			else // 1 in 4 chance of yodel
			{
				me->StartNewAnimation(ANIM_A2,64,ACTION_BUSY,0,0,0,SND_YETIYODEL);
				me->facing = 2;
			}
			return;
		}
		me->SetNewSpeed(3);
		me->StartAnimMove(128);
	}
}

void AI_Geozoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SERPENTOUCH,SND_SERPENTDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,6) && goodguy)
			me->DoFireBullet(BLT_ROCK,20,8);
		return;	// can't do nothin' right now
	}

	if (goodguy)
	{
		if (me->CheckTargetWithinReach(512) && GuyCheckTargetWithinSight(me,goodguy,map) && me->CheckRoll(8, ROLL_ATTACK) && me->reload == 0)
		{
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_SERPENTSPIT);
			return;
		}
	}

	FaceGoodguy3(me, goodguy);

	// slowly inch towards the player
	if (me->mind1)
	{
		if (!(--me->mind1))
			me->StartNewAnimation(ANIM_MOVE,64,ACTION_IDLE,me->GetFacingX()/8,me->GetFacingY()/8);
		else
			me->SetNewSpeed(0);
	}
	else
	{
		if (me->seq == ANIM_IDLE)
		{
			me->SetNewSpeed(0);
			me->mind1 = 10 + Random(50);
		}
	}
}

void AI_Mumble(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->mind1)
		me->mind1--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_ZOMBIEOUCH,SND_ZOMBIEDIE);


	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,5) && goodguy) // commence the skull crushing!
		{
			if (me->AttackCheck(24, me->GetSpaceInFrontX(20) >> FIXSHIFT, me->GetSpaceInFrontY(20) >> FIXSHIFT, goodguy))
			{
				x = (me->aiType==MONS_MUMBLE2)?15:10;
				goodguy->GetShot(me->GetFacingX()*x, me->GetFacingY()*x, 12, map, world);
			}
			me->reload = 5;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// not yet aware
	{
		if ((me->aiType == MONS_MUMBLE2 && me->CheckTargetWithinReach(640))
			|| me->aiType == MONS_MUMBLE && me->CheckTargetWithinReach(200))
			me->mind = 1; // begin the hase

		if (me->seq != ANIM_IDLE)
			me->StartNewAnimation(ANIM_IDLE,128,ACTION_IDLE,0,0); // stay PERFECTLY STILL!
	}
	else
	{
		if (me->CheckTargetWithinReach(60) && me->CheckRoll(8,ROLL_ATTACK) && !me->reload)
		{
			me->StartNewAnimation(ANIM_ATTACK,(me->aiType==MONS_MUMBLE2)?255:128,ACTION_BUSY,0,0,0,SND_MUMBLECRUSH); // crush his skull
			return;
		}

		if (!me->mind1)
		{
			FaceGoodguy3(me, goodguy); // turn only once a second (or 10, if manic mumble)
			me->mind1 = (me->aiType==MONS_MUMBLE2)?3:30;
		}
		if (me->seq != ANIM_MOVE)
			me->StartNewAnimation(ANIM_MOVE,(me->aiType==MONS_MUMBLE2)?255:64,ACTION_IDLE,me->dx,me->dy); // on the move
		else
		{
			if(!((me->frm >= 3 && me->frm <= 5) || (me->frm >= 9 && me->frm <= 11)))
				me->SetNewSpeed((me->aiType==MONS_MUMBLE2)?8:1);
			else
				me->SetNewSpeed(0);

			if ((me->frm == 10 || me->frm == 5) && !me->reload)
			{
				MakeSound(SND_BOMBREFLECT, me->x, me->y, SND_CUTOFF, 1200);
				me->reload=2;
			}
		}
	}
}

void AI_Djinni(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->mind1)
		me->mind1--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_DJINNIOUCH,SND_DJINNIDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,5) && goodguy)
		{
			if (me->AttackCheck(20, me->GetSpaceInFrontX(20) >> FIXSHIFT, me->GetSpaceInFrontY(10) >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(me->GetSpaceInFrontX(10), me->GetSpaceInFrontY(10), 6, map, world);
			}
			me->reload = 10;
		}
		return;	// can't do nothin' right now
	}

	if (me->CheckTargetWithinReach(54) && me->CheckRoll(8,ROLL_ATTACK) && !me->reload)
	{
		me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_DJINNIPUNCH); // punch him
		return;
	}
	if (me->CheckRoll(256, ROLL_ATTACK))
	{
		me->StartNewAnimation(ANIM_A1, 128, ACTION_BUSY, 0, 0, 0); // scare him
		MakeSound(SND_DJINNIBOO, me->x, me->y, SND_CUTOFF|SND_ONE, 1300); // i guess only one at a time?
		return;
	}

	if (!me->mind1)
	{
		// turns only every so often
		FaceGoodguy3(me, goodguy);
		me->mind1 = 10;
	}
	if (me->seq != ANIM_MOVE)
	{
		me->StartAnimMove();
		me->SetNewSpeed(2);
	}
}

void AI_MagicLamp(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	Guy* g;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp == 0)
		{
			MakeSound(SND_LAMPDIE, me->x, me->y, SND_CUTOFF, 1200);
			KillKids(me);	// kill off all my children!
		}
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,18))
		{
			g = AddBaby(me->x + FIXAMT * 20, me->y + FIXAMT * 12, 10 * FIXAMT, MONS_DJINNI, me);
			if (g && (!g->CanWalk(g->x, g->y, map, world)))
				RemoveGuy(g);
			else if (g)
			{
				g->StartNewAnimation(ANIM_A2,64,ACTION_BUSY,0,0);
				g->parent = me;
			}
			me->reload = 200;
		}
		else if (me->seq == ANIM_ATTACK && me->frm > 4)
			BlowSmoke(me->x + FIXAMT * 20, me->y + FIXAMT * 12, FIXAMT * 10, 3 * FIXAMT);
		if (me->seq == ANIM_DIE)
			me->frmAdvance = 256;
		return;
	}

	if (!me->reload && me->CheckRoll(200,ROLL_ATTACK))
		me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_LAMPPOOF);
}

void AI_Cactus(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;
	byte b;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp == 0)
			MakeSound(SND_CACTONDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm > 6)
		{
			me->frmAdvance = 256;
		}
		if (me->CheckSequenceFrame(ANIM_ATTACK,3,false))
		{
			// fire lots of spines
			MakeSound(SND_CACTONSHOOT, me->x, me->y, SND_CUTOFF, 600);
			// but only fire if within a few hundred miles
			if (me->CheckTargetWithinReach(800))
			{
				b = (me->facing * 32 - 60);
				if (me->frmTimer > 64)
					b += 5;

				for (i = 0;i < 12;i++)
				{
					//x=me->x+Cosine(b)*32;
					//y=me->y+Sine(b)*32;
					FireBullet(me->x, me->y, b, BLT_SPINE, me->friendly);
					b += 10;
				}
			}

		}
		if (me->seq == ANIM_A1)
		{
			// accelerate
			if (abs(me->dx) < abs(Cosine(me->facing * 32) * 6))
				me->dx += Cosine(me->facing * 32) / 2;
			if (abs(me->dy) < abs(Sine(me->facing * 32) * 6))
				me->dy += Sine(me->facing * 32) / 2;
		}
		if (me->seq == ANIM_A2)
		{
			// decelerate
			if (abs(me->dx) > 0)
				me->dx -= Cosine(me->facing * 32);
			if (abs(me->dy) > 0)
				me->dy -= Sine(me->facing * 32);
		}
		return;
	}

	if (me->mind == 0)	//idling
	{
		if (!me->mind1--)
		{
			me->StartNewAnimation(ANIM_A1,256,ACTION_BUSY,me->dx,me->dy);
			me->mind = 1;	// start rolling somewhere
			me->PickRandomDirection(&me->mind1,Random(30)+30);
			return;
		}
		if (!me->reload)
		{
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,me->dx,me->dy,120);
			if (goodguy)
				FaceGoodguy(me, goodguy);
			return;
		}
		me->SetNewSpeed(0);
		me->frmAdvance = 32;
		me->StartAnimIdle(me->frmAdvance);

	}
	else if (me->mind == 1)
	{
		if (me->mind1)
			me->mind1--;
		else
		{
			me->mind	= 0;	// got to the destination
			me->mind1	= Random(70) + 30;
			me->StartNewAnimation(ANIM_A2,256,ACTION_BUSY,me->GetFacingX(),me->GetFacingY());
			return;
		}
		if (abs(me->dx) < abs(Cosine(me->facing * 32) * 6))
			me->dx += Cosine(me->facing * 32) / 2;
		if (abs(me->dy) < abs(Sine(me->facing * 32) * 6))
			me->dy += Sine(me->facing * 32) / 2;

		if ((me->seq != ANIM_MOVE) || (me->seq == ANIM_MOVE && me->frm == 4 && me->frmTimer > 128))
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
		}
		me->frmAdvance = 128;
	}
}

void AI_Roller(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte f;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_ROCKBOUNCE,0);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			FireBullet(me->x + ((32 - Random(65)) << FIXSHIFT), me->y + ((32 - Random(65)) << FIXSHIFT),
				(byte)Random(8), BLT_ROCK, me->friendly);
			FireBullet(me->x + ((32 - Random(65)) << FIXSHIFT), me->y + ((32 - Random(65)) << FIXSHIFT),
				(byte)Random(8), BLT_ROCK, me->friendly);
			FireBullet(me->x + ((32 - Random(65)) << FIXSHIFT), me->y + ((32 - Random(65)) << FIXSHIFT),
				(byte)Random(8), BLT_ROCK, me->friendly);
		}
		return;
	}

	if (FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 40, 0, 0, 255, map, world, me->friendly))
	{
		MakeSound(SND_ROLYPOLYWALL, me->x, me->y, SND_CUTOFF, 1000);
	}
	if (me->mind1)	// bonked a wall
	{
		me->mind1 = 0;
		if (me->aiType == MONS_ROLLER || me->aiType == MONS_ROLLER3)
			me->facing = (me->facing + 2) & 7;
		else
			me->facing = (me->facing + 6) & 7;

		MakeSound(SND_ROLYPOLYWALL, me->x, me->y, SND_CUTOFF, 800);
	}

	switch (me->facing)
	{
	case 0:
		f = ANIM_A1;	// roll to the right
		break;
	case 2:
		f = ANIM_MOVE; // roll down
		break;
	case 4:
		f = ANIM_A2;	// roll to the left
		break;
	case 6:
	default:
		f = ANIM_ATTACK;	// roll up
		break;
	}

	if (me->seq != f)
		me->StartNewAnimation(f,128,ACTION_IDLE,me->GetSpaceInFrontX(4),me->GetSpaceInFrontY(4));
}

void AI_Lich(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte f;
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_RICHIEOUCH, SND_RICHIEDIE);

	if (me->action == ACTION_BUSY)
	{
		if (goodguy && me->seq == ANIM_ATTACK)
		{

			FaceGoodguy3(me, goodguy);
			me->HoneInOnPoint(goodguy->x,goodguy->y-96*FIXAMT,4);
			me->ClampSpeed(1);
			if (me->CheckFrame(1,false))
				LaunchMegabeam(me->x, me->y+FIXAMT*8, me->ID);
		}

		if (me->seq == ANIM_DIE)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
				me->z, FIXAMT);
		}
		return;
	}

	if (me->mind == 0)
	{
		if ((goodguy && me->CheckTargetWithinReach(400)) || me->ouch)
			me->mind = 1;
	}
	if (me->mind == 1)	// active
	{
		if (goodguy)
		{
			if (Guy* victim = FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 55, 0, 0, 1, map, world, me->friendly))
				SetPoisonFrames(victim, 128);

			FaceGoodguy2(me, goodguy);
			me->HoneInOnPoint(goodguy->x,goodguy->y-96*FIXAMT);
			me->ClampSpeed(3,4);
		}

		if (me->reload == 8)
		{
			// left eye fires
			f = (me->facing * 32 - 32 + me->mind1) & 255;
			// can't fire backwards
			if (f > 128)
				f = (f < 192) ? 128 : 0;
			FireExactBullet(me->x - 32 * FIXAMT, me->y + 32 * FIXAMT, me->z + 80 * FIXAMT,
				Cosine(f) * 8, Sine(f) * 8, -3 * FIXAMT, 0, 30, me->facing, BLT_ENERGY, me->friendly);
		}
		if (!me->reload)
		{
			// right eye fires
			f = (me->facing * 32 + 32 - me->mind1) & 255;
			// can't fire backwards
			if (f > 128)
				f = (f < 192) ? 128 : 0;
			FireExactBullet(me->x + 32 * FIXAMT, me->y + 32 * FIXAMT, me->z + 80 * FIXAMT,
				Cosine(f) * 8, Sine(f) * 8, -3 * FIXAMT, 0, 30, me->facing, BLT_ENERGY, me->friendly);
			me->reload = 16;
			me->mind1 += 8;
			if (me->mind1 >= 64)
				me->mind1 = 0;
		}

		if (me->CheckRoll(120,ROLL_ATTACK))
		{
			me->StartNewAnimation(ANIM_ATTACK, 128, ACTION_BUSY, me->dx, me->dy);
			return;
		}
	}

	if (me->seq != ANIM_IDLE)
		me->StartNewAnimation(ANIM_IDLE,128,ACTION_IDLE,me->dx,me->dy);
}

void AI_DustDevil(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_RICHIEOUCH,SND_RICHIEDIE);

	if (me->mind1 < 255)
		me->mind1++;

	me->frmAdvance = 128;	// always frame speed=128
	if (goodguy)
	{
		if (me->mind == 0)
		{
			if (goodguy->x < me->x)
			{
				goodguy->dx += (me->mind1 * FIXAMT / 128);
				goodguy->dy += (me->mind1 * FIXAMT / 1024);
			}
			else
			{
				goodguy->dx -= (me->mind1 * FIXAMT / 128);
				goodguy->dy -= (me->mind1 * FIXAMT / 1024);
			}
			if (goodguy->y < me->y)
			{
				goodguy->dy += (me->mind1 * FIXAMT / 128);
				goodguy->dx -= (me->mind1 * FIXAMT / 1024);
			}
			else
			{
				goodguy->dy -= (me->mind1 * FIXAMT / 128);
				goodguy->dx += (me->mind1 * FIXAMT / 1024);
			}
		}
		else
		{
			// immediately after hitting him, shove him away
			if (goodguy->x < me->x)
				goodguy->dx -= FIXAMT * 2;
			else
				goodguy->dx += FIXAMT * 2;
			if (goodguy->y < me->y)
				goodguy->dy -= FIXAMT * 2;
			else
				goodguy->dy += FIXAMT * 2;
			if (me->mind1 > 10)
				me->mind = 0;	// go back to sucking
		}
		Clamp(&goodguy->dx, FIXAMT * 6);
		Clamp(&goodguy->dy, FIXAMT * 6);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 5 && me->frmTimer < 64 && (goodguy))
		{
			if (me->AttackCheck(80, me->x >> FIXSHIFT, me->y >> FIXSHIFT, goodguy))
			{
				FaceGoodguy(me, goodguy);
				goodguy->GetShot(Cosine(me->facing * 32) * 12,
					Sine(me->facing * 32) * 12, 128 / 5, map, world);
				me->mind1 = 0;
				me->mind = 1;	// push him away
			}
		}

		if (me->seq == ANIM_DIE)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
				me->z, FIXAMT);
		}
		return;
	}

	if (me->CheckTargetWithinReach(80) && goodguy)
		me->StartNewAnimation(ANIM_ATTACK,64,ACTION_BUSY,0,0);
	else if (me->seq != ANIM_IDLE)
		me->StartNewAnimation(ANIM_IDLE,64,ACTION_BUSY,0,0);
}

void AI_MechaBouapha(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->mind1)
		me->mind1--;

	if (me->ouch == 4)
	{
		me->HandleOuchNoises(SND_ROBOBOUAPHAOUCH, SND_ROBODIE);
		if (me->mind1 < 64 && me->mind == 1)
			me->mind1 += 16;	// ouch
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,4,false) && me->frmTimer < 64)
		{
			// fire a hammer
			me->DoFireBullet(BLT_EVILHAMMER,SND_HAMMERTOSS);
			me->reload = 20+Random(50);
		}
		if (me->CheckSequenceFrame(ANIM_ATTACK, 7, false) && me->frmTimer < 64 && me->CheckRoll(4,ROLL_MOVE))
			me->PickRandomDirection();
		// Explode upon death.
		if (me->seq == ANIM_DIE)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
				me->z, FIXAMT);
		}
		if (me->CheckSequenceFrame(ANIM_A1,7,false))
			me->PickRandomDirection();
		return;
	}

	if (me->mind == 0)	// unaware of Bouapha
	{
		if ((me->CheckTargetWithinReach(256) && GuyCheckTargetWithinSight(me, goodguy, map)) || me->ouch) // activate when target in range+sight OR when hit
			me->StartNewAnimation(ANIM_A3,256,ACTION_BUSY,0,0,5+Random(30),SND_ROBOBOUAPHAON);
		else
			me->StartNewAnimation(ANIM_A3,32,ACTION_IDLE,0,0,0);
	}
	else if (me->mind == 1)	// normal mode
	{
		if (me->reload == 0)	// time to fire
		{
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0);
			if (goodguy)
				FaceGoodguy(me, goodguy);
			return;
		}
		if (me->mind1 > 32)	// time to run!
		{
			me->mind = 2;	// running mode
			me->StartNewAnimation(ANIM_A1,256,ACTION_BUSY,0,0);
			return;
		}
		if (me->seq != ANIM_MOVE)
		{
			me->StartAnimMove(128);
			me->SetNewSpeed(6);
		}
	}
	else // running away mode
	{
		if (me->CheckRoll(32,ROLL_MOVE))
			me->PickRandomDirection();
		me->StartNewAnimation(ANIM_A2,16,me->action,me->dx,me->dy);
		if (me->mind1 == 0)
		{
			me->StartNewAnimation(ANIM_A3,256,ACTION_BUSY,me->dx,me->dy,Random(30)+5);
			me->mind = 1;	// activate
			return;
		}
		// accelerate
		me->AccelerateSpeed(2);
		if (me->z < 16 * FIXAMT)
			me->dz += FIXAMT * 3;
		me->ClampSpeed(12);
	}
}

void AI_SphinxArm(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		// make noise?
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 6 && me->frmTimer < 64 && (goodguy))
		{
			// hurt guy
			MakeSound(SND_SPHINXSTOMP, me->x, me->y, SND_CUTOFF, 600);
			x = me->x >> FIXSHIFT;
			y = (me->y >> FIXSHIFT) + 115;
			if (me->aiType == MONS_SPHXARM1)
				x -= 20;
			else
				x += 20;

			if (Guy* victim = FindVictim(x, y, 60, Cosine(me->facing * 32) * 12,
				Sine(me->facing * 32) * 12, 30, map, world, me->friendly))
			{
				victim->dx = Cosine(me->facing * 32) * 6;
				victim->dx = Sine(me->facing * 32) * 6;
			}

			if (RangeToTarget(me, goodguy) < 1000 * FIXAMT)
				ShakeScreen(6);
		}

		if (me->seq == ANIM_DIE)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2 + 1, y + me->recty2 + 1, me->z, 1);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx + 1)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty + 1)) << FIXSHIFT,
				me->z, FIXAMT);
		}
		return;
	}

	if (!me->reload)
	{
		me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,30+Random(60));
		return;
	}
	me->StartAnimIdle(128); // just sit there when not swiping
}

void AI_Sphinx(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	Guy* g;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_RICHIEOUCH,SND_RICHIEDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,2,false))
			if(g=me->TryAddBaby(map,world,MONS_MUMBLE,0,90,0))
				g->mind = 1; // he going!
		if (me->CheckSequenceFrame(ANIM_A1,12,false))
		{
			// sneeze some rocks out
			for (x = 0;x < 8;x++)
			{
				FireExactBullet(me->x, me->y + FIXAMT * 40, 60 * FIXAMT, (8 * FIXAMT - Random(17 * FIXAMT)),
					Random(8 * FIXAMT), 0, 0, 60, 2, BLT_ROCK, me->friendly);
			}
			// special rocks to deal with people trying to hide in "safe" spots
			FireExactBullet(me->x, me->y + FIXAMT * 40, 60 * FIXAMT, 8 * FIXAMT, 0, 0, 0, 60, 2, BLT_ROCK, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 40, 60 * FIXAMT, -8 * FIXAMT, 0, 0, 0, 60, 2, BLT_ROCK, me->friendly);
		}

		if (me->seq == ANIM_DIE)
		{
			if (me->CheckFrame(0))
				KillKids(me);
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 2);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
				me->z, FIXAMT);
			ShakeScreen(60);
		}
		return;
	}

	if (!me->reload)
	{
		// call upon a mummy
		me->StartNewAnimation(ANIM_ATTACK,64,ACTION_BUSY,0,0,30+me->GetRoll(120,ROLL_ATTACK));
		return;
	}
	if (!me->reload && me->GetRoll(128,ROLL_ATTACK) && me->CheckTargetWithinReach(512)) // randomly check if target is within sight
	{
		me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,0,0,0,SND_SPHINXSNEEZE);
		return;
	}
	me->StartAnimIdle(); // just sit there when not making mummies
}

void AI_Freakazoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SERPENTOUCH,SND_SERPENTDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrames(ANIM_ATTACK,5,8,false) && goodguy)
		{
			// spit stuff
			FireBullet(me->GetSpaceInFrontX(8), me->GetSpaceInFrontY(8), me->facing * 32 - 16 + Random(33), BLT_SPINE, me->friendly);
			me->reload = 15;
			me->mind = 0;
		}
		return;	// can't do nothin' right now
	}

	if (goodguy)
	{
		if (!me->reload && me->GetRoll(16,ROLL_ATTACK) && me->CheckTargetWithinReach(512) && GuyCheckTargetWithinSight(me,goodguy,map)) // randomly check if target is within sight
		{
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_SERPENTSPIT); // spit!
			FaceGoodguy(me, goodguy);
			return;
		}
	}

	me->TryGetNewDirection(&me->mind,me->GetRoll(60,ROLL_RANDWALK)+1,goodguy);
	me->SetNewSpeed(2);
	me->StartAnimMove(128);
}

void AI_CentiBody(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(0,SND_PUMPKINDIE);

	if (!me->parent || (me->parent->aiType != MONS_CENTIBODY && me->parent->aiType != MONS_CENTIHEAD))
	{
		me->DoTransform(MONS_CENTIHEAD);
		return;
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
			ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 3);
		return;	// can't do nothin' right now
	}

	if (goodguy && RangeToTarget(me, goodguy) < 44 * FIXAMT && me->reload == 0)
	{
		goodguy->GetShot(me->dy, -me->dx, 4, map, world);
		me->reload = 5;
	}

	// head towards parent
	me->dx += (me->parent->x > me->x) ? (FIXAMT * 2) : -(FIXAMT * 2);
	me->dy += (me->parent->y > me->y) ? (FIXAMT * 2) : -(FIXAMT * 2);

	if (me->CheckTargetWithinReach(15, me->parent)) // stop if within parent reach
		me->SetNewSpeed(0);

	me->ClampSpeed(6);
	me->FaceMovement(); // select facing direction based on dx/dy
	me->StartAnimMove(128);
}

void AI_CentiHead(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_ZOMBIEOUCH,SND_PUMPKINDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
			ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 3);
		return;	// can't do nothin' right now
	}

	if (!me->reload && me->CheckTargetWithinReach(44)) // contact damage
	{
		goodguy->GetShot(me->dx, me->dy, 6, map, world);
		me->reload = 5;
	}

	if (me->facing == 0 || me->facing == 1 || me->facing == 7)
		me->dx += FIXAMT;
	else if (me->facing == 3 || me->facing == 4 || me->facing == 5)
		me->dx -= FIXAMT;
	if (me->facing == 1 || me->facing == 2 || me->facing == 3)
		me->dy += FIXAMT;
	else if (me->facing == 5 || me->facing == 6 || me->facing == 7)
		me->dy -= FIXAMT;

	if (me->mind1)	// bonked a wall or guy
	{
		me->PickRandomDirection(&me->mind1,0);
		me->SetNewSpeed(0);
	}

	if (me->CheckRoll(8, ROLL_MOVE) && goodguy)
		FaceGoodguy2(me, goodguy);

	me->DampenSpeed(2);
	me->ClampSpeed(5);
	me->StartAnimMove(128);
}

void AI_Wacko(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_WACKOOUCH,SND_WACKODIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frmTimer < 196 && goodguy && (me->frm == 5 || me->frm == 11 || me->frm == 17))
		{
			MakeSound(SND_CRAZYSTAB, me->x, me->y, SND_CUTOFF, 1200);
			if (me->AttackCheck(32, me->GetSpaceInFrontX(16) >> FIXSHIFT, me->GetSpaceInFrontY(16) >> FIXSHIFT, goodguy))
				goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 8, map, world);
			me->reload = 10;
		}

		if (me->CheckSequenceFrame(ANIM_DIE,5))
			me->frmAdvance = 128;
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (me->CheckRoll(8,ROLL_ATTACK) && me->CheckTargetWithinReach(48) && GuyCheckTargetWithinSight(me,goodguy,map)) // don't stab through walls
			{
				me->StartNewAnimation(ANIM_ATTACK,196,ACTION_BUSY,0,0,0); // get him!
				return;
			}
			FaceGoodguy(me, goodguy);
			me->SetNewSpeed(4);
			me->StartAnimMove();
			if (me->CheckRoll(64,ROLL_RANDWALK))
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if (me->mind == 1)	// random wandering
	{
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(3);
		me->StartAnimMove(128);
	}
}

void AI_Boiler(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_BOILERHIT,SND_ROBODIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrames(ANIM_ATTACK,3,10,false))
		{
			FireExactBullet(me->x + FIXAMT * 36, me->y, FIXAMT * 50,
				-FIXAMT * 2, FIXAMT * 12, -FIXAMT * 3,
				0, 24, 2, BLT_FLAME2, me->friendly);
			FireExactBullet(me->x + FIXAMT * 52, me->y, FIXAMT * 50,
				0, FIXAMT * 12, -FIXAMT * 3,
				0, 24, 2, BLT_FLAME2, me->friendly);
			FireExactBullet(me->x + FIXAMT * 68, me->y, FIXAMT * 50,
				FIXAMT * 2, FIXAMT * 12, -FIXAMT * 3,
				0, 24, 2, BLT_FLAME2, me->friendly);
		}

		if (me->seq == ANIM_DIE)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 2);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
				me->z, FIXAMT);
			ShakeScreen(10);
		}
		return;	// can't do nothin' right now
	}

	// lob grenades occasionally
	if (!me->reload && me->CheckRoll(16,ROLL_ATTACK))
	{
		me->mind1 = 25;
		me->reload = 80;
	}
	if (me->mind1 > 0)
	{
		me->mind1--;
		if ((me->mind1 & 7) == 0)
		{
			MakeSound(SND_MUSHMISSILE, me->x, me->y, SND_CUTOFF, 1500);
			FireBullet(me->x - 48 * FIXAMT, me->y + 40 * FIXAMT, (byte)Random(129), BLT_GRENADE, me->friendly);
		}
	}
	// shoot flame occasionally, but only when not grenading
	if (!me->mind1 && me->CheckRoll(64,ROLL_ATTACK))
		me->StartNewAnimation(ANIM_ATTACK,64,ACTION_BUSY,0,0);
}



void AI_GreatPumpkin(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;
	Guy* g;
	byte f;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_GREATPKOUCH,SND_GREATPKDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,6))
		{
			for(i=0;i<5;i++)
			{
				if (g=me->TryAddBaby(map,world,MONS_PUMPKIN,-64+i*32,40,60))
				{
					g->mind1 = 1;
					g->mind1 = 120;
				}
			}
			MakeSound(SND_GREATPKVOMIT, me->x, me->y, SND_CUTOFF, 1100);
		}

		if (me->seq == ANIM_DIE)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 2);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
				(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
				me->z, FIXAMT);
			ShakeScreen(10);
		}
		return;	// can't do nothin' right now
	}

	if (me->mind)
		me->mind--;

		if (goodguy)
		{
			FaceGoodguy(me, goodguy);

		if (me->mind == 8)
		{
			// left eye fires
			f = (me->facing * 32 - 32 + me->mind1) & 255;

			FireExactBullet(me->x - 64 * FIXAMT, me->y + 32 * FIXAMT, me->z + 80 * FIXAMT,
				Cosine(f) * 8, Sine(f) * 8, -3 * FIXAMT, 0, 30, me->facing, BLT_ENERGY, me->friendly);
		}
		if (!me->mind)
		{
			// right eye fires
			f = (me->facing * 32 + 32 - me->mind1) & 255;
			// can't fire backwards
			if (f > 128)
				f = (f<192) ? 128 : 0;
			FireExactBullet(me->x + 64 * FIXAMT, me->y + 32 * FIXAMT, me->z + 80 * FIXAMT,
				Cosine(f) * 8, Sine(f) * 8, -3 * FIXAMT, 0, 30, me->facing, BLT_ENERGY, me->friendly);
			me->mind = 16;
			me->mind1 += 8;
			if (me->mind1 >= 64)
				me->mind1 = 0;
		}
	}

	if (!me->reload && me->CheckRoll(20,ROLL_ATTACK))
		me->StartNewAnimation(ANIM_ATTACK,64,ACTION_BUSY,0,0);
}

void AI_Ultrazoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;

	if (me->reload)
		me->reload--;

	if (me->mind1)
		me->mind1--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SERPENTOUCH,SND_SERPENTDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && (me->frm == 6 || me->frm == 7) && me->frmTimer < 64)
		{
			// dump a wad of zoid projectiles
				// first a pile of energy bullets
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, -FIXAMT * 4, FIXAMT * 6, -FIXAMT * 2, 0, 60, 2, BLT_ENERGY, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, -FIXAMT * 2, FIXAMT * 8, -FIXAMT * 2, 0, 60, 2, BLT_ENERGY, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, 0, FIXAMT * 9, -FIXAMT * 2, 0, 60, 2, BLT_ENERGY, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, FIXAMT * 2, FIXAMT * 8, -FIXAMT * 2, 0, 60, 2, BLT_ENERGY, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, FIXAMT * 4, FIXAMT * 6, -FIXAMT * 2, 0, 60, 2, BLT_ENERGY, me->friendly);
			// now some fire
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, -FIXAMT * 4, FIXAMT * 16, -FIXAMT * 10, 0, 24, 2, BLT_FLAME2, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, 0, FIXAMT * 18, -FIXAMT * 10, 0, 24, 2, BLT_FLAME2, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, FIXAMT * 4, FIXAMT * 16, -FIXAMT * 10, 0, 24, 2, BLT_FLAME2, me->friendly);
			// and some rocks
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, -FIXAMT * 8, FIXAMT * 1, -FIXAMT * 1, 0, 60, 2, BLT_ROCK, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, -FIXAMT * 4, FIXAMT * 4, -FIXAMT * 1, 0, 60, 2, BLT_ROCK, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, FIXAMT * 4, FIXAMT * 4, -FIXAMT * 1, 0, 60, 2, BLT_ROCK, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, FIXAMT * 8, FIXAMT * 1, -FIXAMT * 1, 0, 60, 2, BLT_ROCK, me->friendly);
			// and big snowballs
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, -FIXAMT * 5, FIXAMT * 1, -FIXAMT * 2, 0, 40, 2, BLT_BIGSNOW, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, 0, FIXAMT * 6, -FIXAMT * 2, 0, 40, 2, BLT_BIGSNOW, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, FIXAMT * 5, FIXAMT * 1, -FIXAMT * 2, 0, 40, 2, BLT_BIGSNOW, me->friendly);
			// and cactus spines
			for (i = 108;i > 19;i -= 10)
				FireExactBullet(me->x, me->y + FIXAMT * 30, FIXAMT * 60, Cosine(i) * 8, Sine(i) * 8, -FIXAMT * 2, 0, 40, i / 16, BLT_SPINE, me->friendly);

			MakeSound(SND_SERPENTSPIT, me->x, me->y, SND_CUTOFF, 1100);
			me->mind1 = 2;
		}

		if (me->seq == ANIM_DIE)
		{
			ShakeScreen(10);
		}
		return;	// can't do nothin' right now
	}

	if (!me->reload && me->CheckTargetWithinReach(256) && GuyCheckTargetWithinSight(me,goodguy,map)) // only spam if within sight and range!
		me->StartNewAnimation(ANIM_ATTACK,72,ACTION_BUSY,0,0,90);
}

void AI_DrLunatic(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x,y;
	if (me->ouch == 4)
		me->HandleOuchNoises(SND_DRLOUCH,SND_DRLDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			if(me->CheckFrame(1,false))
			{

				player.enemiesSlain--; // nobody was slain tbh
			}
			else if (me->CheckFrame(15, false))
			{
				// turn into the sdzl
				me->DoTransform(MONS_SDZL);
				me->StartAnimIdle();
				// prepare the sdzl
				me->mind = 0;
				me->mind1 = 0;
				me->facing = 2;
			}
			else if(!me->reload) // smoke?!
			{
				x = me->x >> FIXSHIFT;
				y = me->y >> FIXSHIFT;
				BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
					(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
					me->z, FIXAMT);
				me->reload=2;
			}
			else
				me->reload--;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// if Bouapha gets near, laugh and then run
	{
		if (me->CheckTargetWithinReach(160)) // within reach & sight?
		{
			me->StartNewAnimation(ANIM_ATTACK,64,ACTION_BUSY,0,0,96,SND_DRLLAUGH);
			me->mind = 1;
			return;
		}
		if (me->ouch)
		{
			me->mind=2;
		}
	}
	if (me->mind == 1)	// running away
	{
		if (me->reload)
			me->reload--;
		else
		{
			me->mind = 2;	// stop running
		}

		if (me->seq != ANIM_MOVE)
			me->StartNewAnimation(ANIM_MOVE,256,me->action,0,-FIXAMT*8);
	}
	if (me->mind == 2)	// waiting for Bouapha again
	{
		me->facing = 2;
		me->SetNewSpeed(0);
		if ((me->CheckTargetWithinReach(160) && GuyCheckTargetWithinSight(me, goodguy, map)) || me->ouch)
		{
			MakeNormalSound(SND_DRLFEAR);
			me->mind = 3;	// start running in fear
			me->reload = 1;
		}
	}
	if (me->mind == 3)
	{
		// run around scared
		if (!me->reload--)
		{
			me->facing = (me->facing + 1 - (byte)Random(3)) & 7;
			me->reload = Random(16) + 1;
		}

		if (me->mind1)
		{
			me->PickRandomDirection(&me->reload,32);
			me->facing = (byte)Random(8);
			me->mind1 = 0;
		}

		me->StartAnimMove(256);
		me->SetNewSpeed(8);
	}
}

void AI_SDZL(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i, x, y;

	if (!goodguy)
		return;	// no point in doing anything without an opponent

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SDZLOUCH,SND_SDZLDIE);

	if (me->action == ACTION_BUSY)
	{
		if (me->CheckSequenceFrames(ANIM_ATTACK,9,13,false) && !me->mind1)
		{
			// slash for pain
			x = (me->x >> FIXSHIFT) + 50;
			y = (me->y >> FIXSHIFT) + 10;

			if (FindVictim(x, y, 100, -FIXAMT * 12, FIXAMT * 4, 25, map, world, me->friendly))
			{
				if (!(MonsterFlags(goodguy->type, goodguy->aiType) & MF_NOMOVE))
				{
					goodguy->dx = -FIXAMT * 12;
					goodguy->dy = FIXAMT * 2;
				}
				me->mind1 = 1;	// don't hit him again
			}
		}
		if (me->seq == ANIM_A2 && me->frm == 12 && me->frmTimer < 64)
		{
			// spew projectiles
			FireExactBullet(me->x, me->y + FIXAMT * 20, FIXAMT * 60,
				-FIXAMT * 6, FIXAMT * 2, FIXAMT, 0, 60, 2, BLT_GRENADE, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 20, FIXAMT * 60,
				-FIXAMT * 4, FIXAMT * 4, FIXAMT, 0, 60, 2, BLT_GRENADE, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 20, FIXAMT * 60,
				0, FIXAMT * 5, FIXAMT, 0, 60, 2, BLT_GRENADE, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 20, FIXAMT * 60,
				FIXAMT * 4, FIXAMT * 4, FIXAMT, 0, 60, 2, BLT_GRENADE, me->friendly);
			FireExactBullet(me->x, me->y + FIXAMT * 20, FIXAMT * 60,
				FIXAMT * 6, FIXAMT * 2, FIXAMT, 0, 60, 2, BLT_GRENADE, me->friendly);
		}
		if (me->CheckSequenceFrame(ANIM_A3,7,false))
		{
			ShakeScreen(30);
			MakeSound(SND_SDZLPOUND, me->x, me->y, SND_CUTOFF|SND_RANDOM, 1200);
			for (i = 0;i < 10;i++)
			{
				x = me->x + ((256 - Random(513)) << FIXSHIFT);
				y = me->y + ((256 - Random(513)) << FIXSHIFT);
				FireExactBullet(x, y, FIXAMT * 80 + (Random(40) << FIXSHIFT),
					0, 0, 0, 0, 30, 2, BLT_ROCK, me->friendly);
			}
		}
		return;	// can't do nothin' right now
	}

	me->mind1 = 0;

	FaceGoodguy2(me, goodguy);

	if (!me->reload && me->CheckTargetWithinReach(256) && GuyCheckTargetWithinSight(me,goodguy,map))
	{
		i = Random(1024);
		if (i<180)
		{
			if(i<40)
				me->StartNewAnimation(ANIM_A3,128,ACTION_BUSY,0,0,60,SND_SDZLSLASH);
			else if(i<100)
				me->StartNewAnimation(ANIM_A2,128,ACTION_BUSY,0,0,60,SND_SDZLSPIT);
			else
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,30,SND_SDZLSLASH);
			return;
		}
	}

	i = (me->facing==7||me->facing<3) ? ANIM_MOVE : ANIM_A1; // move to the left or right

	if (me->seq != i)
		me->StartNewAnimation(i,128,ACTION_IDLE,0,0);
}

void AI_Santa(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SANTAOUCH,SND_SANTADIE);

	if (me->action == ACTION_BUSY)
	{
		if (goodguy && me->CheckSequenceFrame(ANIM_ATTACK,8))
		{
			if (me->AttackCheck(32, me->GetSpaceInFrontX(16) >> FIXSHIFT, me->GetSpaceInFrontY(16) >> FIXSHIFT, goodguy))
				goodguy->GetShot(Cosine(me->facing * 32) * 6, Sine(me->facing * 32) * 6, 16, map, world);

			if (me->mind1--)
				me->frm = 3;
			else
				me->reload = 10;
		}
		if (me->seq == ANIM_DIE && me->frm > 5)
			me->frmAdvance = 128;

		return;	// can't do nothin' right now
	}

	if (goodguy)
	{
		FaceGoodguy3(me, goodguy);
		if (RangeToTarget(me, goodguy) < (96 * FIXAMT) && Random(16) == 0 && me->reload == 0)
		{
			me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,1,SND_SANTAHIT);
			me->mind1 = 2;	// tell him to repeat twice
			return;
		}

		me->SetNewSpeed(6);
		me->StartAnimMove(128);
	}
}
