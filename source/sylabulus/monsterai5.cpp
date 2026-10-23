#include "winpch.h"
#include "monsterai.h"
#include "guy.h"
#include "sound.h"
#include "bullet.h"
#include "player.h"
#include "editor.h"
#include "shop.h"
#include "goal.h"
#include "map.h"

// Bonehead, Redbone, Crazybone
void AI_Bonehead(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_SKELOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_SKELDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		// melee attack
		if (me->seq == ANIM_ATTACK && me->frm == 3 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
				goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 4, map, world);
			me->reload = 5;
		}
		// ranged attack
		if (me->seq == ANIM_A1 && me->frm == 3 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if(!IsEntity(me,EntityType::Crazybone))
				FireBullet(x, y, me->facing * 32, BLT_ENERGY, me->friendly);
			else
				FireBullet(x, y, me->facing * 32, BLT_BIGSHELL, me->friendly);
			me->reload = 5;
			me->mind1 = 1;
		}
		// crazybone: 1 in 6 revival chance
		if (me->seq == ANIM_DIE && IsEntity(me, EntityType::Crazybone) && me->frm == 7 && !Random(6))
		{
			// come back!
			if (me->friendly != goodguy->friendly)
				player.enemiesSlain--;
			me->seq = ANIM_A3;
			me->hp = MonsterHP(me->type);
			if (!me->CanWalk(me->x, me->y, map, world))
			{
				// nevermind if there's no room
				me->hp = 0;
				me->seq = ANIM_DIE;
				return;
			}
			MakeSound(SND_SKELALIVE, me->x, me->y, SND_CUTOFF, 1200);
			me->frm = 2;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
		return;	// can't do nothin' right now
	}

	// randomly decide to point at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	if (me->mind == 0 && RangeToTarget(me, goodguy) > (128 * FIXAMT) && !Random(100))
	{
		me->seq = ANIM_A2;
		me->frm = 0;
		me->frmTimer = 0;
		if (IsEntity(me, EntityType::Crazybone))
			me->frmAdvance = 128;
		else
			me->frmAdvance = 96;
		me->action = ACTION_BUSY;
		me->dx = 0;
		me->dy = 0;
		return;
	}

	if (me->mind == 0) // when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			switch ((EntityType) me->aiType)
			{
				case EntityType::Bonehead:
					if (RangeToTarget(me, goodguy) < (48 * FIXAMT) && !Random(8))
					{
						// get him!
						MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1200);
						me->seq = ANIM_ATTACK;
						me->frm = 0;
						me->frmTimer = 0;
						me->frmAdvance = 96;
						me->action = ACTION_BUSY;
						me->dx = 0;
						me->dy = 0;
						me->reload = 0;
						return;
					}
					break;
				case EntityType::Redbone:
				case EntityType::Crazybone:
					if (RangeToTarget(me, goodguy) < (256 * FIXAMT) && !Random(32))
					{
						// spit at him
						MakeSound(SND_SKELSHOOT, me->x, me->y, SND_CUTOFF, 1200);
						me->seq = ANIM_A1;
						me->frm = 0;
						me->frmTimer = 0;
						if (!IsEntity(me, EntityType::Crazybone))
							me->frmAdvance = 96;
						else
							me->frmAdvance = 128;
						me->action = ACTION_BUSY;
						me->dx = 0;
						me->dy = 0;
						me->reload = 0;
						FaceGoodguy(me, goodguy);
						return;
					}
					break;
			}

			FaceGoodguy(me, goodguy);

			switch ((EntityType)me->aiType)
			{
				default:
					me->dx = Cosine(me->facing * 32) * 4;
					me->dy = Sine(me->facing * 32) * 4;
					break;
				case EntityType::Crazybone:
					me->dx = Cosine(me->facing * 32) * 5;
					me->dy = Sine(me->facing * 32) * 5;
					break;
			}

			if (me->seq != ANIM_MOVE)
			{
				me->seq = ANIM_MOVE;
				me->frm = 0;
				me->frmTimer = 0;
				if (!IsEntity(me, EntityType::Crazybone))
					me->frmAdvance = 96;
				else
					me->frmAdvance = 128;
			}

			if (!Random(64))
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
		switch ((EntityType)me->aiType)
		{
			case EntityType::Redbone:
			case EntityType::Crazybone:
				if (RangeToTarget(me, goodguy) < (256 * FIXAMT) && !Random(32))
				{
					// spit at him
					MakeSound(SND_SKELSHOOT, me->x, me->y, SND_CUTOFF, 1200);
					me->seq = ANIM_A1;
					me->frm = 0;
					me->frmTimer = 0;
					me->frmAdvance = 64;
					me->action = ACTION_BUSY;
					me->dx = 0;
					me->dy = 0;
					me->reload = 0;
					FaceGoodguy(me, goodguy);
					return;
				}
				break;
		}
		if (!(me->mind1--))	// time to get a new direction
		{
			if (goodguy && RangeToTarget(me, goodguy) < (256 * FIXAMT) && !Random(3))
				me->mind = 0;	// get back on track
			else
				me->facing = (byte)Random(8);
			me->mind1 = Random(40) + 1;
		}

		switch ((EntityType)me->aiType)
		{
			default:
				me->dx = Cosine(me->facing * 32) * 4;
				me->dy = Sine(me->facing * 32) * 4;
				break;
			case EntityType::Crazybone:
				me->dx = Cosine(me->facing * 32) * 6;
				me->dy = Sine(me->facing * 32) * 6;
				break;
		}

		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			if (!IsEntity(me, EntityType::Crazybone))
				me->frmAdvance = 96;
			else
				me->frmAdvance = 128;
		}
	}
}

// Mumble, Manic Mumble, Tut Uncommon
void AI_Mumble(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->mind1)
		me->mind1--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_ZOMBIEOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_ZOMBIEDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		// mumble, manic mumble - melee attack
		if (me->seq == ANIM_ATTACK && me->frm == 5 && !me->reload)
		{
			// crush skull
			x = me->x + Cosine(me->facing * 32) * 20;
			y = me->y + Sine(me->facing * 32) * 20;
			if (me->AttackCheck(24, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(Cosine(me->facing * 32) * 10, Sine(me->facing * 32) * 10, 12, map, world);
			}
			me->reload = 5;
		}

		// toot uncommon - ranged attack
		if (me->seq == ANIM_A1 && me->frm > 1 && me->frm < 9 && !me->reload)
		{
			MakeSound(SND_ZAP, me->x, me->y, SND_CUTOFF, 1200);
			x = me->x + Cosine((me->facing * 32 + 64) & 255) * 8;
			y = me->y + Sine((me->facing * 32 + 64) & 255) * 8;
			x += Cosine(me->facing * 32) * 12;
			y += Sine(me->facing * 32) * 12;
			FireBullet(x, y, (me->facing * 32 - 4) & 255, BLT_BIGSHELL, me->friendly);
			x = me->x + Cosine((me->facing * 32 - 64) & 255) * 8;
			y = me->y + Sine((me->facing * 32 - 64) & 255) * 8;
			x += Cosine(me->facing * 32) * 12;
			y += Sine(me->facing * 32) * 12;
			FireBullet(x, y, (me->facing * 32 + 4) & 255, BLT_BIGSHELL, me->friendly);
			me->reload = 5;
		}

		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// not yet aware
	{
		if (!IsEntity(me,EntityType::Mumble))
			me->mind = 1;

		if (goodguy && RangeToTarget(me, goodguy) < 200 * FIXAMT)
		{
			me->mind = 1;	// begin chasing.  Slowly.
		}
		if (me->seq != ANIM_IDLE)
		{
			me->seq = ANIM_IDLE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
	}
	else
	{
		if (!IsEntity(me, EntityType::TutUncommon) && RangeToTarget(me, goodguy) < (60 * FIXAMT) && !Random(8) && !me->reload)
		{
			// crush his skull
			MakeSound(SND_MUMBLECRUSH, me->x, me->y, SND_CUTOFF, 1000);
			me->seq = ANIM_ATTACK;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
			if (IsEntity(me, EntityType::ManicMumble))
				me->frmAdvance = 256;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 0;
			return;
		}
		if (IsEntity(me, EntityType::TutUncommon) && RangeToTarget(me, goodguy) < (256 * FIXAMT) && !Random(32) && !me->reload)
		{
			// crush his skull
			MakeSound(SND_SKELSHOOT, me->x, me->y, SND_CUTOFF, 1000);
			me->seq = ANIM_A1;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 0;
			return;
		}
		if (!me->mind1)
		{
			// turns only once a second
			FaceGoodguy3(me, goodguy);
			switch ((EntityType)me->aiType)
			{
				case EntityType::ManicMumble:
					me->mind1 = 0;
					break;
				case EntityType::TutUncommon:
					me->mind1 = 15;
					break;
				default:
					me->mind1 = 30;
					break;
			}
		}
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			switch ((EntityType)me->aiType)
			{
			default:
				me->frmAdvance = 64;
				break;
			case EntityType::TutUncommon:
				me->frmAdvance = 96;
				break;
			case EntityType::ManicMumble:
				me->frmAdvance = 256;
				break;
			}
		}
		else
		{
			switch ((EntityType)me->aiType)
			{
			case EntityType::ManicMumble:
				me->dx = Cosine(me->facing * 32) * 4;
				me->dy = Sine(me->facing * 32) * 4;
				break;
			case EntityType::TutUncommon:
				if (!((me->frm >= 3 && me->frm <= 5) || (me->frm >= 9 && me->frm <= 11)))
				{
					me->dx = Cosine(me->facing * 32)*2;
					me->dy = Sine(me->facing * 32)*2;
				}
				else
				{
					me->dx = 0;
					me->dy = 0;
				}
				break;
			default:
				if (!((me->frm >= 3 && me->frm <= 5) || (me->frm >= 9 && me->frm <= 11)))
				{
					me->dx = Cosine(me->facing * 32);
					me->dy = Sine(me->facing * 32);
				}
				else
				{
					me->dx = 0;
					me->dy = 0;
				}
				break;
			}
		}
	}
}

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
			MakeSound(SND_LANTERNOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_LANTERNDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && (me->frm == 5) && goodguy)
		{
			// spit stuff
			x = me->x + Cosine(me->facing * 32) * 8;
			y = me->y + Sine(me->facing * 32) * 8;
			FireBulletZ(x, y, FIXAMT * 12, me->facing, BLT_FLAME, me->friendly);
			me->reload = 15;
			me->mind = 0;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 1)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			switch ((EntityType)me->aiType)
			{
				default:
					if (RangeToTarget(me, goodguy) < (48 * FIXAMT) && !Random(8))
					{
						// get him!
						MakeSound(SND_LANTERNSPIT, me->x, me->y, SND_CUTOFF, 1200);
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
					break;
			}

			FaceGoodguy(me, goodguy);

			switch ((EntityType)me->aiType)
			{
			default:
				me->dx = Cosine(me->facing * 32) * 3;
				me->dy = Sine(me->facing * 32) * 3;
				break;
			}

			if (me->seq != ANIM_MOVE)
			{
				me->seq = ANIM_MOVE;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
			}

			if (!Random(64))
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
	else if (me->mind == 0)	// random wandering
	{
		switch ((EntityType)me->aiType)
		{
			default:
			if (RangeToTarget(me, goodguy) < (256 * FIXAMT) && !Random(32))
			{
				// spit at him
				MakeSound(SND_LANTERNSPIT, me->x, me->y, SND_CUTOFF, 1200);
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
			break;
		}
		if (!(me->mind1--))	// time to get a new direction
		{
			if (goodguy && RangeToTarget(me, goodguy) < (256 * FIXAMT) && !Random(3))
				me->mind = 0;	// get back on track
			else
				me->facing = (byte)Random(8);
			me->mind1 = Random(40) + 1;
		}

		switch ((EntityType)me->aiType)
		{
			default:
				me->dx = Cosine(me->facing * 32) * 3;
				me->dy = Sine(me->facing * 32) * 3;
				break;
		}

		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 64;
		}
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
		if (me->seq == ANIM_ATTACK && me->frm == 5 && me->frmTimer<32)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			FireBullet(x, y, me->facing, BLT_CLAWHAMMER, me->friendly);
			me->reload = 20;
		}
		return;	// can't do nothin' right now
	}

	// randomly decide to point at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	if (me->mind == 0 && !Random(100) && RangeToTarget(me, goodguy) < (256 * FIXAMT))
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
			if ((RangeToTarget(me, goodguy) < (96 * FIXAMT) && !Random(8)) || (RangeToTarget(me, goodguy) < (256 * FIXAMT) && !Random(32)))
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

			if (!(me->mind1--))	// time to get a new direction
			{
				FaceGoodguy3(me, goodguy);
				me->mind1 = Random(5) + 1;
			}

			me->dx = Cosine(me->facing * 32) * 3;
			me->dy = Sine(me->facing * 32) * 3;
			if (me->seq != ANIM_MOVE)
			{
				me->seq = ANIM_MOVE;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 96;
			}
			if (!Random(64) || (RangeToTarget(me, goodguy) >= (256 * FIXAMT) && !Random(32)))
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
			if ((RangeToTarget(me, goodguy) < (96 * FIXAMT) && !Random(8)))
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
			me->frmAdvance = 96;
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
		if (me->seq == ANIM_A1 && me->frm > 1 && goodguy && me->hp > 0)
		{
			// diving attack hit check
			x = me->x;
			y = me->y;
			if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(Cosine(me->facing * 32) * 8, Sine(me->facing * 32) * 8, 6, map, world);
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
				me->dx = -me->dx / 4;
				me->dy = -me->dy / 4;
			}
		}
		if (me->seq == ANIM_DIE)
		{
			me->frmAdvance = 64;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 2)		// when mind=2, pursue relentlessly
	{
		if (goodguy)
		{
			i = RangeToTarget(me, goodguy);

			if (i > (128 * FIXAMT) && i < (512 * FIXAMT) && !Random(32))
			{
				MakeSound(SND_BATDIVE, me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_A1;	// diving attack move, to get closer to the player!
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
				me->action = ACTION_BUSY;
				me->dx = Cosine(me->facing * 32) * 8;
				me->dy = Sine(me->facing * 32) * 8;
				return;
			}

			if (i < (128 * FIXAMT) && !Random(8))
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
			me->frmAdvance = 128;
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


bool FindClosestWaterTile(Map* map, int startX, int startY, byte* waterX, byte* waterY)
{
	int maxRadius = 20;

	for (int radius = 0; radius <= maxRadius; radius++)
	{
		// Search the square ring around the starting tile.
		for (int y = startY - radius; y <= startY + radius; y++)
		{
			for (int x = startX - radius; x <= startX + radius; x++)
			{
				// Only check the outer edge of this radius.
				if (radius > 0 &&
					x > startX - radius && x < startX + radius &&
					y > startY - radius && y < startY + radius)
					continue;

				if (x < 0 || y < 0 ||
					x >= map->width ||
					y >= map->height)
					continue;

				if (map->GetTile(x, y))
				{
					*waterX = x;
					*waterY = y;
					return true;
				}
			}
		}
	}

	return false;
}

void AI_Kappa(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_UMBRELLOOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_UMBRELLODIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK)
		{
			if (me->frm == 4 && me->frmTimer < 64)
			{
				me->dx = Cosine(me->facing * 32) * 8;
				me->dy = Sine(me->facing * 32) * 8;
				MakeSound(SND_CLANG, me->x, me->y, SND_CUTOFF, 1200);
			}
			if (me->frm > 4 && me->frm < 10 && me->reload == 0 && goodguy)
			{
				printf("Kappa attack check\n");
				x = me->x + Cosine(me->facing * 32) * 16;
				y = me->y + Sine(me->facing * 32) * 16;
				if (me->AttackCheck(24, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
					goodguy->GetShot(Cosine(me->facing * 32) * 8, Sine(me->facing * 32) * 8, 4, map, world);
				me->reload = 5;
			}
			if (me->frm > 4)
			{
				Dampen(&me->dx, FIXAMT / 2);
				Dampen(&me->dy, FIXAMT / 2);
			}
		}

		if (me->seq == ANIM_A1)
		{
			if (me->frm == 8 && me->frmTimer < 64)
			{
				me->mind2 = me->mind2 ? 0 : 1; // toggle
				me->mind1= Random(40) + 1;
				me->reload = 100;
				return;
			}

			if (!me->mind2 &&
				me->frm > 3 &&
				me->frm < 9 &&
				me->frmTimer < 64 &&
				goodguy)
			{
				MakeSound(SND_CRAZYSTAB, me->x, me->y,
					SND_CUTOFF, 1200);

				x = me->x + Cosine(me->facing * 32) * 16;
				y = me->y + Sine(me->facing * 32) * 16;

				for (i = 0; i < 2; i++)
				{
					FireBulletZ(
						x,
						y,
						FIXAMT * 12,
						me->facing * 32 - 16 + (byte)Random(33),
						BLT_SHARK,
						me->friendly);
				}
			}
		}

		return;
	}

	if (me->mind2 && !me->reload)
	{
		MakeSound(SND_FOOD, me->x, me->y, SND_CUTOFF, 1200);
		me->seq = ANIM_A1;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 128;
		me->action = ACTION_BUSY;
		me->dx = 0;
		me->dy = 0;
		return;
	}

	if (me->mind == 1)
	{
		if (!(me->mind1--))
		{
			if (goodguy && Random(3) == 0)
			{
				me->mind = 0;
			}
			else
			{
				me->facing = (byte)Random(8);
				me->mind1 = Random(40) + 1;
			}
		}

		me->dx = Cosine(me->facing * 32) * 2;
		me->dy = Sine(me->facing * 32) * 2;

		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 64;
		}

		return;
	}

	if (me->mind == 0)
	{
		if (goodguy)
		{
			if (!(me->mind3--) && RangeToTarget(me, goodguy) < (128 * FIXAMT) && !Random(32))
			{
				MakeSound(SND_UMBRELLOSPEW,me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_A1;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
				me->action = ACTION_BUSY;
				me->dx = 0;
				me->dy = 0;
				me->reload = 100;
				return;
			}

			if (RangeToTarget(me, goodguy) < (192 * FIXAMT) && !Random(32) && !me->reload)
			{
				MakeSound(SND_UMBRELLOTAUNT,me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_ATTACK;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 96;
				me->action = ACTION_BUSY;
				me->dx = 0;
				me->dy = 0;
				me->reload = 100;

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
				me->frmAdvance = 64;
			}

			if (Random(64) == 0)
			{
				me->mind = 1;
				me->mind1 = 1;
			}
		}
		else
		{
			me->mind = 1;
			me->mind1 = 1;
		}
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
		if (me->seq == ANIM_ATTACK && me->frm == 5 && me->reload == 0 && goodguy)
		{
			for (i = 0; i < 2; i++)
			{
				byte b = (me->facing * 32 + i * 16 - 8) & 255;
				x = me->x + Cosine(b) * 8;
				y = me->y + Sine(b) * 8;
				FireBullet(x, y, b, BLT_ENERGY, me->friendly);
			}

			me->reload = 15;

			if (RangeToTarget(me, goodguy) < (512 * FIXAMT))
			{
				me->mind = 0;
				me->mind1 = Random(2) ? 1 : -1;
				me->mind2 = 0;
			}
		}

		if (me->seq == ANIM_A1 && me->frm == 5 && goodguy)
		{
			if (Guy* g = FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT,
				72, 0, 0, 10, map, world, me->friendly))
			{
				SetPoisonFrames(g, g->poison + 30 * 10);

				me->mind = 0;
				me->mind1 = Random(2) ? 1 : -1;
				me->mind2 = 0;

				MakeSound(SND_LIGHTNING, me->x, me->y,
					SND_CUTOFF, 1000);
			}
		}

		return;
	}


	if (me->seq != ANIM_MOVE)
	{
		me->seq = ANIM_MOVE;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 128;
	}

	if (!goodguy) // wander
	{
		me->mind = 1;
		me->mind1 = Random(8);
		me->mind2 = Random(40) + 1;
	}

	if (me->mind == 0) // stalk, maintain distance from the player
	{
		if (goodguy)
		{
			int range = RangeToTarget(me, goodguy);

			if (range < (192 * FIXAMT)) // TOO CLOSE!!! get mad!!!
			{
				me->mind2 += 2;

				FaceGoodguy(me, goodguy);
				me->facing = (me->facing + 4) & 7;

				me->dx = Cosine(me->facing * 32) * 5;
				me->dy = Sine(me->facing * 32) * 5;

				if (me->mind2 % 8 == 0)
					BlowSmoke(me->x, me->y, me->z + 8*FIXAMT, FIXAMT);

				if (me->mind2 > 30 * 2)
				{
					me->mind = 2;
					me->mind1 = 0;
					me->mind2 = 0;
					return;
				}
			}
			else if (range < (320 * FIXAMT))
			{
				
				if (me->mind1 != 1 && me->mind1 != -1)
					me->mind1 = Random(2) ? 1 : -1;

				if (me->mind2 > 0)
					me->mind2--;

				FaceGoodguy(me, goodguy);

				byte orbit;

				if (me->mind1 > 0)
					orbit = (me->facing + 2) & 7;
				else
					orbit = (me->facing + 6) & 7;

				me->dx = Cosine(orbit * 32) * 4;
				me->dy = Sine(orbit * 32) * 4;

				if (!me->mind2 && !Random(90))
				{
					me->mind1 = -me->mind1;
					me->mind2 = Random(30) + 20;
				}

				if (!me->reload && !Random(30))
				{
					MakeSound(SND_SERPENTSPIT, me->x, me->y,
						SND_CUTOFF, 1200);

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
			else
			{
				me->mind1 = Random(2) ? 1 : -1;
				me->mind2 = 0;

				FaceGoodguy(me, goodguy);

				me->dx = Cosine(me->facing * 32) * 4;
				me->dy = Sine(me->facing * 32) * 4;
			}

			if (me->seq != ANIM_MOVE)
			{
				me->seq = ANIM_MOVE;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
			}
		}
	}
	// random wandering
	else if (me->mind == 1)
	{
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < (320 * FIXAMT) && !Random(32))
			{
				MakeSound(SND_SERPENTSPIT, me->x, me->y,
					SND_CUTOFF, 1200);

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

			// stalk a bit
			if (RangeToTarget(me, goodguy) < (240 * FIXAMT))
			{
				me->mind = 0;
				me->mind1 = Random(2) ? 1 : -1;
				me->mind2 = 0;
				return;
			}
		}

		if (me->mind2 <= 0) // mind2 = wandering timer
		{
			me->facing = (byte)Random(8);
			me->mind2 = Random(40) + 20;
		}
		else
		{
			me->mind2--;
		}

		me->dx = Cosine(me->facing * 32) * 4;
		me->dy = Sine(me->facing * 32) * 4;
	}

	// chase the player angrily!!!
	else if (me->mind == 2)
	{
		if (!goodguy)
		{
			me->mind = 1;
			me->mind1 = Random(8);
			me->mind2 = Random(40) + 20;
			return;
		}

		FaceGoodguy3(me, goodguy);

		me->dx = Cosine(me->facing * 32) * 7;
		me->dy = Sine(me->facing * 32) * 7;

		if (me->frm % 2 == 0)
			BlowSmoke(me->x, me->y, me->z, FIXAMT);

		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 256;
		}

		// chomp!
		if (RangeToTarget(me, goodguy) < (72 * FIXAMT))
		{
			FaceGoodguy(me, goodguy);

			MakeSound(SND_HAPPYCHOMP, me->x, me->y,
				SND_CUTOFF, 1500);

			me->seq = ANIM_A1;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 192;
			me->action = ACTION_BUSY;

			me->dx = 0;
			me->dy = 0;
			me->reload = 0;

			me->mind1 = 0;
			me->mind2 = 0;

			return;
		}

		// escape
		if (RangeToTarget(me, goodguy) > (256 * FIXAMT))
		{
			me->mind2++;

			if (me->mind2 > 30 * 2)
			{
				me->mind = 0;
				me->mind1 = Random(2) ? 1 : -1;
				me->mind2 = 0;
			}
		}
		else
		{
			me->mind2 = 0;
		}
	}
}

void AI_Jimbo(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

	int x, y;
	byte b;
	Guy* bouapha;

	bouapha = GetGuyOfAIType(MONS_BOUAPHA);
	if (!bouapha)
		return;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_JIMBOOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_JIMBODIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK) // vertical charge
		{
			if (me->frm == 5) // start charge
			{
				MakeSound(SND_JIMBOATTACK, me->x, me->y, SND_CUTOFF, 1200);
				me->dx = Cosine(me->facing * 32) * 10;
				me->dy = Sine(me->facing * 32) * 10;
			}
			else if (me->frm > 5)
			{
				Dampen(&me->dx, FIXAMT/2);
				Dampen(&me->dy, FIXAMT/2);
				x = me->x + Cosine(me->facing * 32) * 12;
				y = me->y + Sine(me->facing * 32) * 12;
				if (!me->reload && me->AttackCheck(8, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
				{
					goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 1, map, world);
					me->reload = 2;
				}
			}
		}
		if (me->seq == ANIM_A1 && me->frm == 5 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 12;
			y = me->y + Sine(me->facing * 32) * 12;
			FireBullet(x, y, me->facing * 32, BLT_BIGSHELL, me->friendly);
			me->reload = 5;
		}
		if (me->seq == ANIM_DIE)
		{
			for (x = 0;x < 8;x++)
				SpecialSnow(me->x + ((-16 + Random(33)) << FIXSHIFT),
					me->y + ((-16 + Random(33)) << FIXSHIFT));
		}
		return;
	}

	if (me->mind == 0)	// not activated yet
	{
		if (RangeToTarget(me, bouapha) < 72 * FIXAMT)
		{
			me->mind = 1;
			MakeNormalSound(SND_JIMBOWAKE);
		}
	}
	else if (me->mind == 1)	// following Bouapha
	{
		Dampen(&me->dx, FIXAMT / 8);
		Dampen(&me->dy, FIXAMT / 8);

		if (me->x < bouapha->x)
			me->dx += FIXAMT;
		else
			me->dx -= FIXAMT;
		if (me->y < bouapha->y)
			me->dy += FIXAMT;
		else
			me->dy -= FIXAMT;

		me->dx += -FIXAMT / 2 + Random(FIXAMT);
		me->dy += -FIXAMT / 2 + Random(FIXAMT);

		if (RangeToTarget(me, bouapha) < 48 * FIXAMT)	// got too close
		{
			me->mind = 2;
			me->facing = Random(8);
			me->dx = Cosine(me->facing * 32) * 2;
			me->dy = Sine(me->facing * 32) * 2;
			me->mind1 = Random(20) + 10;
		}
	}
	else if (me->mind == 2) // not following, got too close
	{
		me->mind1--;
		if (!me->mind1)
			me->mind = 1;
	}
	else	// hunting for fresh meat
	{
		if (goodguy == NULL || IsEntity(goodguy,EntityType::Nobody) || RangeToTarget(me, bouapha) > 320 * FIXAMT)	// got too far from papa!!
		{
			me->mind = 1;
			return;
		}
		else
		{
			Dampen(&me->dx, FIXAMT / 8);
			Dampen(&me->dy, FIXAMT / 8);

			if (me->x < goodguy->x)
				me->dx += FIXAMT;
			else
				me->dx -= FIXAMT;
			if (me->y < goodguy->y)
				me->dy += FIXAMT;
			else
				me->dy -= FIXAMT;

			me->dx += -FIXAMT / 2 + Random(FIXAMT);
			me->dy += -FIXAMT / 2 + Random(FIXAMT);

			if (RangeToTarget(me, goodguy) < 100 * FIXAMT && !Random(32))
			{
				me->seq = ANIM_ATTACK;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
				me->dx = 0;
				me->dy = 0;
				me->action = ACTION_BUSY;
				FaceGoodguy(me, goodguy);
				return;
			}
			if (RangeToTarget(me, goodguy) >= 100 * FIXAMT && !Random(32))
			{
				me->seq = ANIM_A1;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
				me->dx = 0;
				me->dy = 0;
				me->action = ACTION_BUSY;
				FaceGoodguy(me, goodguy);
				return;
			}
		}
	}

	// if following Bouapha, and an enemy comes within sight, and you aren't already far from Bouapha
	if ((me->mind == 1 || me->mind == 2) && RangeToTarget(me, goodguy) < 320 * FIXAMT &&
		RangeToTarget(me, bouapha) < 100 * FIXAMT)
	{
		me->mind = 3;	// enter assault mode
	}

	Clamp(&me->dx, FIXAMT * 4);
	Clamp(&me->dy, FIXAMT * 4);

	if (me->dx > FIXAMT)
	{
		if (me->dy > FIXAMT)
			me->facing = 1;
		else if (me->dy < -FIXAMT)
			me->facing = 7;
		else
			me->facing = 0;
	}
	else if (me->dx < -FIXAMT)
	{
		if (me->dy > FIXAMT)
			me->facing = 3;
		else if (me->dy < -FIXAMT)
			me->facing = 5;
		else
			me->facing = 4;
	}
	else
	{
		if (me->dy > FIXAMT)
			me->facing = 2;
		else if (me->dy < -FIXAMT)
			me->facing = 6;
	}

	if (me->mind != 0)
	{
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
	}
}

void AI_Umbrello(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp == 0)
			MakeSound(SND_BATOUCH, me->x, me->y, SND_CUTOFF, 1200);
	}
	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
				goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 2, map, world);
			me->reload = 2;

		}
		if (me->seq == ANIM_A1 && me->frm > 1 && goodguy && me->hp > 0)
		{
			// diving attack hit check
			x = me->x;
			y = me->y;
			if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(Cosine(me->facing * 32) * 8, Sine(me->facing * 32) * 8, 6, map, world);
				me->seq = ANIM_A2;	// bounce off
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
				me->dx = -me->dx / 4;
				me->dy = -me->dy / 4;
			}
		}
		if (me->seq == ANIM_A2)
		{
			if (me->frm == 3)
				me->mind = 0;
			if (me->frm > 5 && me->z == 0 && !me->reload)
			{
				MakeSound(SND_BOMBREFLECT, me->x, me->y, SND_CUTOFF, 1200);
				me->reload = 20;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)		// when mind=0, singlemindedly zip towards Bouapha
	{
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < 300 * FIXAMT || me->ouch)
			{
				MakeSound(SND_UMBRELLOTAUNT, me->x, me->y, SND_CUTOFF, 1200);
				me->action = ACTION_BUSY;
				me->seq = ANIM_A1;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
				me->mind = 1;
				AddSpottedTimer(30*8);
			}
		}
	}
	else if (me->mind == 1)		// when mind=1, singlemindedly zip towards Bouapha
	{
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < (64 * FIXAMT) && !Random(8))
			{
				// go back down
				me->seq = ANIM_A2;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
				me->action = ACTION_BUSY;
				me->dx = 0;
				me->dy = 0;
				me->reload = 0;
				return;
			}

			if (RangeToTarget(me, goodguy) < (96 * FIXAMT) && !Random(32))
			{
				// get him!
				MakeSound(SND_UMBRELLOSPEW, me->x, me->y, SND_CUTOFF, 1200);
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
			if (Random(16) == 0)
			{
				me->mind = 2;		// occasionally wander
				me->mind1 = 1;
			}
		}
		else
		{
			me->mind = 2;	// if there's no goodguy, get random
			me->mind1 = 1;
		}
	}
	else if (me->mind == 2)	// random wandering
	{
		if (RangeToTarget(me, goodguy) < (64 * FIXAMT) && !Random(8))
		{
			// go back down
			me->seq = ANIM_A2;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 0;
			return;
		}

		if (!(me->mind1--))	// time to get a new direction
		{
			if ((goodguy) && !Random(3))
				me->mind = 1;	// get back on track
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
			me->frmAdvance = 110;
		}
	}
}

void AI_NohMannComedy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_MATTIEOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_MATTIEDIE, me->x, me->y, SND_CUTOFF, 1200);

	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 6 && me->frmTimer < 32)
		{
			if ((EntityType)me->aiType == EntityType::NohMannComedy)
			{

			}
			else if ((EntityType)me->aiType == EntityType::NohMannTragedy)
			{

			}
		}
		if (me->seq == ANIM_A1 && me->frm == 6 && me->frmTimer < 32)
		{
			if ((EntityType)me->aiType == EntityType::NohMannComedy)
			{

			}
			else if ((EntityType)me->aiType == EntityType::NohMannTragedy)
			{

			}
		}
		if (me->seq == ANIM_A2 && me->frm == 7 && me->frmTimer < 32)
		{
			if ((EntityType)me->type == EntityType::NohMannComedy)
				me->type = (short)EntityType::NohMannTragedy;
			else if((EntityType)me->type == EntityType::NohMannTragedy)
				me->type = (short)EntityType::NohMannComedy;

			if ((EntityType)me->aiType == EntityType::NohMannComedy)
				me->aiType = (short)EntityType::NohMannTragedy;
			else if ((EntityType)me->aiType == EntityType::NohMannTragedy)
				me->aiType = (short)EntityType::NohMannComedy;
		}
		if (me->seq == ANIM_DIE)
		{
			ExplodeParticles2(PART_SLIME, me->x, me->y, FIXAMT * 20, 4, 10);
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// waiting for Bouapha
	{
		if (RangeToTarget(me, goodguy) < 300 * FIXAMT || me->ouch > 0)
		{
			MakeSound(SND_COUNTHELLO, me->x, me->y, SND_CUTOFF, 1200);
			me->mind = 1;
		}
		me->mind1 = 60;
	}
	else if (me->mind == 1)	// chasing
	{
		// calculate desired location (want to be above Bouapha)
		switch (me->mind2)
		{
			case 0:
				x = goodguy->x - 96 * FIXAMT;
				y = goodguy->y;
				break;
			case 1:
				x = goodguy->x + 96 * FIXAMT;
				y = goodguy->y;
				break;
		}

		if (!Random(256))
		{
			me->mind2 = (me->mind2 + 1) & 1;
		}

		FaceGoodguy3(me, goodguy);
		if (me->x < x)
			me->dx += FIXAMT / 4;
		if (me->y < y)
			me->dy += FIXAMT / 4;
		if (me->x > x)
			me->dx -= FIXAMT / 4;
		if (me->y > y)
			me->dy -= FIXAMT / 4;

		Clamp(&me->dx, FIXAMT * 8);
		Clamp(&me->dy, FIXAMT * 6);

		if (me->reload == 0 && RangeToTarget(me, goodguy) < 256 * FIXAMT)
		{
			i = Random(1024);


			if (i < 20)
			{
				MakeSound(SND_COUNTDIE, me->x, me->y, SND_CUTOFF, 1200);
				me->action = ACTION_BUSY;
				me->seq = ANIM_A2;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
				return;
			}
			else if (i < 80)
			{
				MakeSound(SND_ALIENSPIT, me->x, me->y, SND_CUTOFF, 1200);
				me->action = ACTION_BUSY;
				me->seq = ANIM_ATTACK;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
				return;
			}
			else if (i < 140)
			{
				MakeSound(SND_ALIENSPIT, me->x, me->y, SND_CUTOFF, 1200);
				me->action = ACTION_BUSY;
				me->seq = ANIM_A1;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
				return;
			}

			if (me->seq != ANIM_MOVE)
			{
				me->seq = ANIM_MOVE;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 128;
			}
		}
	}
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
	int x, y, i;
	byte b;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_RICHIEOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_RICHIEDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK || me->seq == ANIM_A1 || me->seq == ANIM_A2)
		{
			if (me->frm == 4 && me->reload == 0 && goodguy)
			{
				MakeSound(SND_BOMBTHROW, me->x, me->y, SND_CUTOFF, 1200);
				for (i = 0;i < 3;i++)
				{
					b = (me->facing * 32 + Random(17) - 8) & 255;
					x = me->x + Cosine(b) * 16;
					y = me->y + Sine(b) * 16;
					FireBullet(x, y, b, BLT_BIGSHELL, me->friendly);
				}
				me->reload = 3;
			}
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
		else if (me->frm > 8)
		{
			me->action = ACTION_IDLE;
			me->seq = ANIM_MOVE;
			me->frm = 1;
			me->frmTimer = 0;
			me->frmAdvance = 0;
		}

		return;
	}

	if (me->mind == 0 && me->hp > 0)
	{
		if ((goodguy && RangeToTarget(me, goodguy) < 300 * FIXAMT) || me->ouch > 0)
		{
			MakeSound(SND_GHOSTTPORT, me->x, me->y, SND_CUTOFF, 1200);
			me->mind = 1;
			me->action = ACTION_BUSY;
			me->seq = ANIM_A3;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 64;
			return;
		}
	}
	else if (me->mind == 1 && goodguy)
	{
		if (goodguy->x < me->x - 45 * FIXAMT)
			me->facing = 3;
		else if (goodguy->x > me->x + 45 * FIXAMT)
			me->facing = 1;
		else
			me->facing = 2;

		if (goodguy->y > me->y &&
			RangeToTarget(me, goodguy) < 300 * FIXAMT &&
			!Random(32))
		{
			switch (me->facing)
			{
			case 1:
				me->seq = ANIM_A2;
				break;
			case 3:
				me->seq = ANIM_A1;
				break;
			default:
				me->seq = ANIM_ATTACK;
				break;
			}

			me->action = ACTION_BUSY;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
			return;
		}

		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 1;
			me->frmTimer = 0;
			me->frmAdvance = 0;
		}
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
		if (me->seq == ANIM_ATTACK && me->frm == 4 && me->frmTimer < 63)
		{
			x = me->x + Cosine(me->facing * 32) * 12;
			y = me->y + Sine(me->facing * 32) * 12;
			MakeSound(SND_MISSILELAUNCH, me->x, me->y, SND_CUTOFF, 1200);
			FireBullet(x, y, me->facing * 32, BLT_GLUESHOT, me->friendly);
			me->reload = 40;
			return;
		}
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
		if (RangeToTarget(me, goodguy) > 500 * FIXAMT)
			me->mind = 0;

		if (!me->reload && me->seq == ANIM_IDLE)
		{
			// get him!!
			me->seq = ANIM_ATTACK;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 0;
		}
		FaceGoodguy3(me, goodguy);
	}
}

void AI_Gingersnap(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte b;
	int x, y;
	x = me->maxHP - me->hp + 5;	// more damaged=faster

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


		if (me->seq == ANIM_ATTACK && me->frmTimer < 128 && goodguy && (me->frm == 6 || me->frm == 10))
		{
			MakeSound(SND_MUSHMISSILE, me->x, me->y, SND_CUTOFF|SND_RANDOM, 1200);
			x = me->x + Cosine(me->facing * 32)*16;
			y = me->y + Sine(me->facing * 32);
			b = ((me->facing * 32) + (Random(17) - 8)) & 255;
			FireExactBullet(x, y, FIXAMT*10, Cosine(b)*4, Sine(b)*4, FIXAMT*16, 0, 60, b, BLT_GRENADE, me->friendly);
			if (me->frm == 10)
				me->reload = 30;
		}
		return;	// can't do nothin' right now
	}

	if (me->seq != ANIM_MOVE)
	{
		me->seq = ANIM_MOVE;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 96;
	}

	if (me->seq == ANIM_MOVE)
	{
		me->frmAdvance = 96 + (x - 5) * 96 / me->maxHP;
		x = 6 + (x - 5) * 6 / me->maxHP;


		if (me->frm < 4 && (me->z + me->dz) < 1*FIXAMT)
		{
			if (me->mind3)
			{
				MakeSound(SND_SPIKECLANK, me->x, me->y, SND_CUTOFF, 1200);
				me->mind3 = 0;
			}
			Dampen(&me->dx, FIXAMT*2);
			Dampen(&me->dy, FIXAMT*2);
		}
		else if (me->frm == 4 && me->frmTimer < me->frmAdvance)
		{
			FaceGoodguy2(me, goodguy);
			MakeSound(SND_HAMMERTOSS, me->x, me->y, SND_CUTOFF, 2);
			me->dz = x * FIXAMT;
			me->mind3 = 1;

			me->dx = Cosine(me->facing * 32) * x;
			me->dy = Sine(me->facing * 32) * x;
		}
		else if (me->frm > 4)
		{
			Dampen(&me->dx, FIXAMT/4);
			Dampen(&me->dy, FIXAMT/4);
		}
		if (goodguy && !me->reload && abs(me->dx + me->dy) > 1 * FIXAMT && RangeToTarget(me, goodguy) < 44 * FIXAMT)
		{
			goodguy->GetShot(me->dx, me->dy, 6, map, world);
			me->reload = 2;
		}
	}

	if (RangeToTarget(me, goodguy) < (128 * FIXAMT) && Random(32) == 0 && !me->reload)
	{
		// get him!
		me->seq = ANIM_ATTACK;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 128 + (x - 5) * 128 / me->maxHP;
		me->action = ACTION_BUSY;
		me->dx = 0;
		me->dy = 0;
		me->reload = 0;
		return;
	}
	FaceGoodguy(me, goodguy);
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
	if(!Random(100) && me->mind==0 && RangeToTarget(me, goodguy) > (128 * FIXAMT))
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

byte IsTargetVisible(Guy* me, Map* map, Guy* goodguy)
{
	if (!goodguy || goodguy->type == (short)EntityType::Nobody)
		return 0;

	switch (me->facing)
	{
	case 0:
		if (goodguy->mapx > me->mapx && goodguy->mapx - me->mapx < 12)
		{
			// on the right side anyway, and within the range
			if (abs(goodguy->mapy - me->mapy) < 2 + (goodguy->mapx - me->mapx) / (12 / 6))
			{
				// within the cone
				return map->FindGuy(me->mapx, me->mapy, 8, goodguy);
			}
		}
		return 0;
		break;
	case 2:
		if (goodguy->mapy > me->mapy && goodguy->mapy - me->mapy < 12)
		{
			// on the right side anyway, and within the range
			if (abs(goodguy->mapx - me->mapx) < 2 + (goodguy->mapy - me->mapy) / (12 / 6))
			{
				// within the cone
				return map->FindGuy(me->mapx, me->mapy, 8, goodguy);
			}
		}
		return 0;
		break;
	case 4:
		if (goodguy->mapx < me->mapx && -goodguy->mapx + me->mapx < 12)
		{
			// on the right side anyway, and within the range
			if (abs(goodguy->mapy - me->mapy) < 2 + (-goodguy->mapx + me->mapx) / (12 / 6))
			{
				// within the cone
				return map->FindGuy(me->mapx, me->mapy, 8, goodguy);
			}
		}
		return 0;
		break;
	case 6:
		if (goodguy->mapy < me->mapy && -goodguy->mapy + me->mapy < 12)
		{
			// on the right side anyway, and within the range
			if (abs(goodguy->mapx - me->mapx) < 2 + (-goodguy->mapy + me->mapy) / (12 / 6))
			{
				// within the cone
				return map->FindGuy(me->mapx, me->mapy, 8, goodguy);
			}
		}
		return 0;
		break;
	}
	return 0;
}

void AI_Pathfinder(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_SKELOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_SKELDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 3 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
				goodguy->GetShot(Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 4, map, world);
			me->reload = 5;
		}
		if (me->seq == ANIM_A1 && me->frm == 3 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			FireBullet(x, y, me->facing * 32, BLT_ENERGY, me->friendly);
			me->reload = 5;
			me->mind1 = 1;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < (48 * FIXAMT) && Random(8) == 0)
			{
				// get him!
				MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_ATTACK;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
				me->action = ACTION_BUSY;
				me->dx = 0;
				me->dy = 0;
				me->reload = 0;
				return;
			}
			FaceGoodguy(me, goodguy);

			me->dx = Cosine(me->facing * 32) * 4;
			me->dy = Sine(me->facing * 32) * 4;
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
			if (RangeToTarget(me, goodguy) > 256*FIXAMT && Random(2) == 0)
			{
				me->mind = 2; // PATHFINDING MODE!
				printf("Going into pathfinding mode...\n");
				return;
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
				MakeSound(SND_SKELSHOOT, me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_A1;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
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
	else if (me->mind == 2)
	{
		int px = (goodguy->x >> FIXSHIFT) / TILE_WIDTH;
		int py = (goodguy->y >> FIXSHIFT) / TILE_HEIGHT;
		me->UpdatePathfinding(map, world, 4, px, py);

		if (me->GetPathDistance() < 60 * FIXAMT && Random(8) == 0 && map->FindGuy(me->mapx,me->mapy,8,goodguy))
		{
			// back to pursuing!
			me->mind=0;
		}
	}

	if (me->seq != ANIM_MOVE)
	{
		me->seq = ANIM_MOVE;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 128;
	}
}

void AI_StopSign(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_STOPSIGNOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_STOPSIGNDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 3 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;

			byte clang = 0;
			if (FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 40, 0, 0, 16, map, world, me->friendly))
				clang = 1;
			// at least one enemy is hit
			if (clang)
			{
				MakeSound(SND_CLANG, me->x, me->y, SND_CUTOFF, 4000);
				me->reload = 50;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0) // asleep
	{

		if (me->mind1 <= 0) // mind1 = wandering timer
		{
			me->facing = (byte)Random(8);
			me->mind1 = Random(40) + 20;
		}
		else
		{
			me->mind1--;
		}

		if (goodguy && abs(goodguy->dx + goodguy->dy) > FIXAMT && !me->reload)
		{
			me->mind2++;
			if (me->mind2 >= 15 && !Random(100-me->mind2))
			{
				MakeSound(SND_STOPSIGNHEY, me->x, me->y, SND_CUTOFF, 1200);
				me->mind = 1;
				me->mind2 = 0;
				me->seq = ANIM_A1;
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
		else if (me->mind2 > 0)
		{
			me->mind2 = 0;
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
	if (me->mind == 1) // awake, mad at you
	{
		if (goodguy)
		{
			if (goodguy->hp <= 0)
			{
				me->mind = 0;
				me->mind2 = 0;
				return;
			}
			if (abs(goodguy->dx + goodguy->dy) < FIXAMT)
			{
				me->mind2++;
				if (me->mind2 >= 60)
				{
					me->mind = 0;
					me->mind2 = 0;
					return;
				}
			}
		}

		me->dx = Cosine(me->facing * 32) * 6;
		me->dy = Sine(me->facing * 32) * 6;
		FaceGoodguy2(me, goodguy);

		if (me->seq != ANIM_A2)
		{
			me->seq = ANIM_A2;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}

		if (RangeToTarget(me, goodguy) < (48 * FIXAMT) && !Random(4))
		{
			// get him!
			FaceGoodguy(me, goodguy);
			me->seq = ANIM_ATTACK;
			MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1200);
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 0;
			return;
		}
	}
}

void AI_Kabocha(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_BUSHOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_PUMPKINDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

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

	if (me->seq == ANIM_MOVE && me->frm == 2 && goodguy)	// hits on this frame
	{
		FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 24, Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 2, map, world, me->friendly);
	}

	if (me->mind == 0)	// not currently aware of goodguy
	{
		if (me->seq == ANIM_MOVE)
		{
			me->seq = ANIM_IDLE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 32;
		}

		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < 256 * FIXAMT || me->ouch > 0)
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
		me->dx = 0;
		me->dy = 0;
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

		if (me->mind1)
			me->mind1--;
		else
		{
			if (RangeToTarget(me, goodguy) > 256 * FIXAMT)
				me->mind = 0;	// get bored again
			else
				me->mind1 = 20;	// stay on trail a little longer
			return;
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

void AI_Pumpkin(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_BUSHOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_PUMPKINDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && !me->reload)
		{
			me->frmAdvance = 64;
			if (IsEntity(me,EntityType::Boomkin) && me->frm == 3)
				FireBullet(me->x, me->y, 0, BLT_BOOM, me->friendly);
			ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 8);
			me->reload = 2;
		}
		return;	// can't do nothin' right now
	}

	if (me->seq == ANIM_MOVE && me->frm == 2 && goodguy)	// hits on this frame
	{
		if (IsEntity(me, EntityType::Boomkin))
		{
			if (FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 32, 0, 0, 0, map, world, me->friendly))
			{
				me->hp = 1;
				me->GetShot(0, 0, 1, map, world);
				return;
			}
		}
		else
		{
			FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 24, Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 2, map, world, me->friendly);
		}
	}

	if (me->mind == 0)	// not currently aware of goodguy
	{
		if (me->seq == ANIM_MOVE)
		{
			me->seq = ANIM_IDLE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 32;
		}

		if (goodguy)
		{
			if (RangeToTarget(me, goodguy) < 256 * FIXAMT || me->ouch > 0)
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
		me->dx = 0;
		me->dy = 0;
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

		if (me->mind1)
			me->mind1--;
		else
		{
			if (RangeToTarget(me, goodguy) > 256 * FIXAMT)
				me->mind = 0;	// get bored again
			else
				me->mind1 = 20;	// stay on trail a little longer
			return;
		}

		if (IsEntity(me, EntityType::Boomkin))
		{
			me->dx = Cosine(me->facing * 32) * 6;
			me->dy = Sine(me->facing * 32) * 6;
		}
		else
		{
			me->dx = Cosine(me->facing * 32) * 2;
			me->dy = Sine(me->facing * 32) * 2;
		}

		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
	}
}

void AI_PumpkinPatch(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	Guy* g;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_BUSHOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_PATCHDIE, me->x, me->y, SND_CUTOFF, 1200);

		if (me->reload > 0)
		{
			me->reload = 0;
		}
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 18)
		{
			me->seq = ANIM_A1;
			me->frm = 0;
		}
		if (me->seq == ANIM_A1 && me->frm == 7 && me->reload == 0)
		{
			g = AddBaby(me->x, me->y + FIXAMT * 2, 0, (int)EntityType::Pumpkin, me);

			if (g)
			{
				if (!g->CanWalk(g->x, g->y, map, world))
				{
					RemoveGuy(g);
					me->reload = 5;
				}
				else
				{
					MakeSound(SND_PKSTEINBUMP, me->x, me->y, SND_CUTOFF, 1200);
					HealRing(1, g->x, g->y, 0, 4, 1);
					g->mind = 1;
					g->mind1 = 30;
					g->facing = Random(8);
					me->reload = 30;
				}
			}
		}
		return;
	}

	if (RangeToTarget(me, goodguy) < 320 * FIXAMT && !me->reload && !Random(50))
	{
		MakeSound(SND_EGGOPEN, me->x, me->y, SND_CUTOFF, 1200);
		me->seq = ANIM_ATTACK;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 128;
		me->action = ACTION_BUSY;
	}
}

void AI_Oven(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	Guy* g;

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
		if (me->seq == ANIM_ATTACK && me->frm == 8 && me->reload == 0)
		{
			g = AddBaby(me->x, me->y + FIXAMT * 64, 0, (int)EntityType::Ninjabread, me);

			if (g)
			{
				if (!g->CanWalk(g->x, g->y, map, world))
				{
					RemoveGuy(g);
					me->reload = 5;
				}
				else
				{
					MakeSound(SND_GENERATE, me->x, me->y, SND_CUTOFF | SND_ONE, 1200);
					for (x = 0;x < 15;x++)
						BlowSmoke(me->x - 40 * FIXAMT + Random(80 * FIXAMT), me->y + 60 * FIXAMT + Random(15 * FIXAMT),
							0, Random(8) * FIXAMT + FIXAMT);
					g->facing = 2;
					me->reload = 30;
				}
			}
		}
		return;
	}

	if (RangeToTarget(me, goodguy) < 512 * FIXAMT && !me->reload && !Random(50))
	{
		MakeSound(SND_MISSILELAUNCH, me->x, me->y, SND_CUTOFF, 1200);
		me->seq = ANIM_ATTACK;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 64;
		me->action = ACTION_BUSY;
		return;
	}

	if (me->seq != ANIM_MOVE)
	{
		me->seq = ANIM_MOVE;
		me->frm = 0;
		me->frmTimer = 0;
		me->frmAdvance = 64;
	}
	else if (me->frm % 4 == 0 && !Random(8))
	{
		BlowSmoke(me->x - 40 * FIXAMT + Random(80 * FIXAMT), me->y + + Random(15 * FIXAMT),
			20 * FIXAMT, Random(8) * FIXAMT + FIXAMT);
	}
}
