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
#include "water.h"

// AI for Zombie.
void AI_Zombie(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_ZOMBIEOUCH, SND_ZOMBIEDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK) // zombie leap attack
		{
			if (me->frm == 5) // lunge
				me->SetSpeed(8);
			else if (me->frm > 5) // post-lunge
				me->DampenSpeed(2);

			if (goodguy && me->frm > 6 && me->frm < 13 && !me->reload) // try and attack the target (player)
			{
				if (me->AttackCheck(8, me->ShiftDirCos(16) >> FIXSHIFT, me->ShiftDirSin(16) >> FIXSHIFT, goodguy))
				{
					goodguy->GetShot(me->DirCos(4), me->DirSin(4), 1, map, world);
					me->reload = 2; // check every 2 frames, to avoid too much damage
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0) // pursuit mode - slowly lumber towards the player
	{
		if (!goodguy)
		{
			me->mind = 1;
			me->mind1 = 1;
			return;
		}

		me->FaceGuy(goodguy);

		if (me->CanSeeGuy(goodguy, 15, map) && me->GuyWithinRange(goodguy, 128) && !Random(32)) // only leap if guy is seen
		{
			MakeSound(SND_ZOMBIELEAP, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_ATTACK, 64);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		me->TryWander(me->mind1, 64, 1);
		me->SetSpeed(1);
		me->ForceSequence(ANIM_MOVE, 64);
	}
	else if (me->mind == 1)	// wander around randomly
	{
		if (!Random(16) && me->CanSeeGuy(goodguy, 15, map)) // more likely to pursue if target is within LOS
		{
			me->mind = 0;
			me->mind1 = 0;
			return;
		}

		me->WanderMode(me->mind1, 40, 3, 0);
		me->SetSpeed(1);
		me->ForceSequence(ANIM_MOVE, 64);
	}
	if (me->seq == ANIM_MOVE && me->frm > 5) // hobble like a zombie
	{
		me->DampenSpeed(4);
	}
}

// AI for Bonehead, Redbone and Crazybone.
void AI_Bonehead(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_SKELOUCH, SND_SKELDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK) // kick attack
		{
			if (me->frm == 3 && !me->reload) // check for kicking
			{
				if (me->AttackCheck(16, me->ShiftDirCos(16) >> FIXSHIFT, me->ShiftDirSin(16) >> FIXSHIFT, goodguy))
				{
					goodguy->GetShot(me->DirCos(4), me->DirSin(4), 4, map, world);
					me->reload = 3;
				}
			}
		}
		if (me->seq == ANIM_A1 && me->frm == 3 && me->reload == 0 && goodguy) // ranged attack
		{
			if (me->frm == 3 && !me->reload) // fire the bullet!
			{
				me->SimpleFireBullet(16, me->HasGuyAI(ENT_CRAZYBONE) ? BLT_ENERGY : BLT_BIGSHELL);
				me->reload = 3;
				me->mind1 = 1;
			}
		}
		if (me->seq == ANIM_DIE) // death animation
		{
			// crazybone has a 1 in 6 chance to revive upon death if their death spot free
			if (!(me->HasGuyAI(ENT_CRAZYBONE) && me->frm == 7 && !Random(6)) || !me->CanWalk(me->x,me->y,map,world))
				return;

			me->Revive();
			me->ForceSequence(ANIM_A3, 128, 2);
			MakeSound(SND_SKELALIVE, me->x, me->y, SND_CUTOFF, 1200);
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0) // pursuit mode
	{

		me->FaceGuy(goodguy);

		if (!goodguy) // no goodguy = wander!
		{
			me->mind = 1;
			me->mind1 = 1;
			return;
		}

		me->FaceGuy(goodguy); // face guy no matter what

		if (me->GuyWithinRange(goodguy, 128) && !Random(100)) // point and taunt
		{
			me->ForceSequence(ANIM_A2, me->HasGuyAI(ENT_CRAZYBONE) ? 128 : 64);
			me->SetSpeed(0);
			return;
		}

		if (!me->HasGuyAI(ENT_REDBONE) && me->GuyWithinRange(goodguy, 48) && !Random(8)) // kick
		{
			MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			me->SetSpeed(0);
			me->ForceSequence(ANIM_ATTACK, 64);
			me->reload = 0;
			return;
		}

		if (!me->HasGuyAI(ENT_BONEHEAD) && me->GuyWithinRange(goodguy, 256) && !Random(32)) // ranged attack
		{
			MakeSound(SND_SKELSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			me->SetSpeed(0);
			me->ForceSequence(ANIM_A1, me->HasGuyAI(ENT_CRAZYBONE) ? 128 : 64);
			me->reload = 0;
			return;
		}

		if (!me->HasGuyAI(ENT_CRAZYBONE))
		{
			x = 4;
			y = 64;
		}
		else
		{
			x = 6;
			y = 128;
		}

		me->TryWander(me->mind1, 64, 1);
		me->SetSpeed(x);
		me->ForceSequence(ANIM_MOVE, y);
	}
	else if (me->mind == 1)	// random wandering
	{
		if (!me->HasGuyAI(ENT_BONEHEAD) && me->GuyWithinRange(goodguy, 256) && !Random(32)) // ranged attack
		{
			MakeSound(SND_SKELSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			me->SetSpeed(0);
			me->ForceSequence(ANIM_A1, me->HasGuyAI(ENT_CRAZYBONE) ? 128 : 64);
			me->reload = 0;
			return;
		}

		if (me->CanSeeGuy(goodguy, 15, map) && me->GuyWithinRange(goodguy, 128) && !Random(32)) // maybe retturn if player is visible?
		{
			me->mind = 0;
			return;
		}

		if (!me->HasGuyAI(ENT_CRAZYBONE))
		{
			x = 4;
			y = 64;
		}
		else
		{
			x = 6;
			y = 128;
		}

		me->WanderMode(me->mind1, 64, 3, 0);
		me->SetSpeed(x);
		me->ForceSequence(ANIM_MOVE, y);
	}
}

// AI for Lantern & Kerosene Lamp.
void AI_Lantern(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_LANTERNOUCH, SND_LANTERNDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK) // zombie leap attack
		{
			if (me->frm == 5) // spit stuff
			{
				// todo: make kerosene lamp spit something else
				if(!me->HasGuyAI(ENT_KEROSENELAMP))
					FireBulletZ(me->ShiftDirCos(12), me->ShiftDirSin(12), FIXAMT * 12, me->facing, BLT_LANTERN, me->friendly);
				else
					FireBulletZ(me->ShiftDirCos(12), me->ShiftDirSin(12), FIXAMT * 12, me->facing, BLT_FLAME, me->friendly);
				me->reload = 15;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->HasGuyAI(ENT_KEROSENELAMP)) // this guy is extra annoying!
	{
		if (!Random(8))
		{
			map->TempTorch(me->mapx, me->mapy, 16);
		}
	}

	if (me->mind == 0) // pursuit mode
	{
		if (!goodguy) // no goodguy = wander!
		{
			me->mind = 1;
			me->mind1 = 1;
			return;
		}

		me->FaceGuy(goodguy);

		if (me->CanSeeGuy(goodguy, 15, map) && me->GuyWithinRange(goodguy, 128) && !Random(32)) // only spit if guy is seen
		{
			MakeSound(SND_LANTERNSPIT, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		me->TryWander(me->mind1, 64, 1);
		me->SetSpeed(3);
		me->ForceSequence(ANIM_MOVE, 64);
	}
	else if (me->mind == 1)	// random wandering
	{
		if (me->CanSeeGuy(goodguy, 15, map) && me->GuyWithinRange(goodguy, 128) && !Random(32)) // only spit if guy is seen
		{
			MakeSound(SND_LANTERNSPIT, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		if (!Random(16) && me->CanSeeGuy(goodguy, 15, map)) // more likely to pursue if target is within LOS
		{
			me->mind = 0;
			me->mind1 = 0;
			return;
		}

		me->WanderMode(me->mind1, 40, 3, 0);
		me->SetSpeed(3);
		me->ForceSequence(ANIM_MOVE, 64);
	}
}

// ai for hardhat enemy
void AI_Hardhat(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_HARDHATOUCH, SND_HARDHATDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 5 && me->frmTimer<32)
		{
			me->SimpleFireBullet(16, BLT_CLAWHAMMER);
			me->reload = 20;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if (!goodguy) // no goodguy = wander!
		{
			me->mind = 1;
			me->mind1 = 1;
			return;
		}

		x = me->RangeToGuy(goodguy)/FIXAMT; // get range once

		if (x < 128 && !Random(100)) // point and taunt
		{
			me->ForceSequence(ANIM_A1, 96);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		if ((x < 96 && !Random(8)) || (x < 256 && !Random(32))) // throw hammer
		{
			MakeSound(SND_HARDHATSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		me->TryWander(me->mind1, 64, 1);
		me->SetSpeed(2);
		me->ForceSequence(ANIM_MOVE, 64);
	}
	else if (me->mind == 1)	// random wandering
	{

		if ((me->GuyWithinRange(goodguy,256) && !Random(32))) // throw hammer
		{
			MakeSound(SND_HARDHATSHOOT, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		if (!Random(16) && me->CanSeeGuy(goodguy, 15, map)) // more likely to pursue if target is within LOS
		{
			me->mind = 0;
			me->mind1 = 0;
			return;
		}

		me->WanderMode(me->mind1, 40, 3, 0);
		me->SetSpeed(1);
		me->ForceSequence(ANIM_MOVE, 64);
	}
}

void AI_Pelican(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_PELICANOUCH, SND_PELICANDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK) // spit water
		{
			if (me->frm > 3 && me->frm < 8 && me->frmTimer<32)
			{
				x = me->x + Cosine(me->facing * 32) * 16;
				y = me->y + Sine(me->facing * 32) * 16;
				FireBulletZ(x, y, FIXAMT * 12, me->facing * 32 - 16 + (byte)Random(33), BLT_SHARK, me->friendly);
			}
			if (me->frm == 8)
				me->reload = 10;
		}
		if (me->seq == ANIM_A1 && me->frm > 1 && goodguy && me->hp > 0) // dive at the player
		{
			// diving attack hit check
			x = me->x;
			y = me->y;
			if (me->AttackCheck(16, me->ShiftDirCos(8) >> FIXSHIFT, me->ShiftDirSin(8) >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(me->DirCos(8), me->DirSin(8), 6, map, world);
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

	if (me->mind == 0) // pursuit mode
	{
		if (!goodguy) // no goodguy = wander!
		{
			me->mind = 1;
			me->mind1 = 1;
			return;
		}

		me->FaceGuyBroad(goodguy);

		i = me->RangeToGuy(goodguy)/FIXAMT;

		if (i > 128 && i < 512 && !Random(32)) // within diving distance, but not circling distance
		{
			MakeSound(SND_BATDIVE, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_A1, 128);
			me->SetSpeed(8);
			return;
		}

		if (i < 128) // within circling distance... start circling!
		{
			me->mind = 2;
			me->mind1 = 20;
			return;
		}

		me->TryWander(me->mind1, 64, 1);
		me->ForceSequence(ANIM_MOVE, 192);
		me->SetSpeed(3);
	}
	else if (me->mind == 1)	// random wandering
	{
		if (me->GuyWithinRange(goodguy, 128))
		{
			me->mind = 2;
			me->mind1 = 20;
			return;
		}

		if (!Random(16) && me->CanSeeGuy(goodguy, 15, map)) // more likely to pursue if target is within LOS
		{
			me->mind = 0;
			me->mind1 = 0;
			return;
		}

		me->WanderMode(me->mind1, 40, 0, 0);
		me->SetSpeed(3);
		me->ForceSequence(ANIM_MOVE, 128);
	}
	else if (me->mind == 2)	// circling
	{
		me->FaceGuyBroad(goodguy);

		// move sideways
		me->dx = Cosine((me->facing * 32 + 64) & 255) * 3;
		me->dy = Sine((me->facing * 32 + 64) & 255) * 3;

		if (!me->GuyWithinRange(goodguy, 128))
		{
			if (!--me->mind1) // offer a lil wiggle room
			{
				me->mind = 0;	// enter pursuit mode
				return;
			}
		}
		else if (me->mind1 < 20)
			me->mind1 = 20;

		if (!me->reload && !Random(32))
		{
			MakeSound(SND_PELICANSPEW, me->x, me->y, SND_CUTOFF, 1000);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			return;
		}
		me->ForceSequence(ANIM_MOVE, 192);
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

	//mind2 = 0 (has water), 1 (no water)

	int x, y, i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_SNOWOUCH, SND_SNOWDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK) // sword attack
		{
			if (me->frm == 4 && me->frmTimer < 32)
			{
				MakeSound(SND_SLASH, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);

				if (!me->HasGuyAI(ENT_MADKAP)) // kaptana
				{
					me->SetSpeed(8);
					me->frmAdvance = 96;

				}
				else // madkap
				{
					me->SetSpeed(12);
					me->frmAdvance = 128;
				}
			}
			if (me->frm > 4)
			{
				me->DampenSpeed(4);
				if (me->frm < 9 && !me->reload)
				{
					if (me->AttackCheck(16, me->ShiftDirCos(16) >> FIXSHIFT, me->ShiftDirSin(16) >> FIXSHIFT, goodguy))
					{
						goodguy->GetShot(me->DirCos(4), me->DirSin(4), 4, map, world);
						me->reload = 3;
					}
				}
			}
		}

		if (me->seq == ANIM_A1) // water attack
		{
			if (me->frm == 3 && me->frmTimer < 32)
			{
				if (!me->mind2) // water is gone
				{
					MakeSound(SND_MUSHMISSILE, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
					me->SimpleFireBullet(16, BLT_GRENADE); // TODO: waterball
					me->mind2 = 90;
					ExplodeParticles(PART_WATER, me->x, me->y, FIXAMT * 20, 4);
				}
				else // water is back!
				{
					MakeSound(SND_BATSTARE, me->x, me->y, SND_CUTOFF, 1200);
					me->mind2 = 0;
					ExplodeParticles(PART_WATER, me->x, me->y, FIXAMT * 20, 2);
				}
			}
		}

		return;
	}

	if (me->mind == 0) // pursuit mode
	{
		if (me->mind2) // manage water timer
		{
			if (me->mind2 > 1)
				me->mind2--;
			else
			{
				me->ForceSequence(ANIM_A1, 96); // get ready to replenish!
				MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1200);
				return;
			}
		}

		if (!goodguy) // no goodguy = wander!
		{
			me->mind = 1;
			me->mind1 = 1;
			return;
		}

		me->FaceGuy(goodguy);

		if (!Random(60) && me->CanSeeGuy(goodguy, 15, map) && !me->GuyWithinRange(goodguy,96))
		{
			MakeSound(SND_SNOWTHROW, me->x, me->y, SND_CUTOFF, 1200);
			me->FaceGuy(goodguy);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			me->reload = 20;
			return;
		}

		if (!me->mind2 && !Random(60) && me->CanSeeGuy(goodguy,15,map) && me->GuyWithinRange(goodguy, 96))
		{
			MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1200); // do the water bowl thing
			me->FaceGuy(goodguy);
			me->ForceSequence(ANIM_A1, 96);
			me->SetSpeed(0);
			me->reload = 20;
			return;
		}

		me->TryWander(me->mind1, 64, 1);
		me->ForceSequence(ANIM_MOVE, 64);
		me->SetSpeed(3);
	}
	else if (me->mind == 1)	// random wandering
	{
		if (me->mind2) // manage water timer
		{
			if (me->mind2 > 1)
				me->mind2--;
			else
			{
				me->ForceSequence(ANIM_A1, 96); // get ready to replenish!
				MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1200);
				return;
			}
		}

		if (!Random(16) && me->CanSeeGuy(goodguy, 15, map)) // more likely to pursue if target is within LOS
		{
			me->mind = 0;
			me->mind1 = 0;
			return;
		}

		me->WanderMode(me->mind1, 40, 0, 0);
		me->ForceSequence(ANIM_MOVE, 64);
		me->SetSpeed(3);
	}
}

void AI_Tsuchizoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;
	byte b;

	// mind2 = annoyance meter (60 is max!!)

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_SERPENTOUCH, SND_SERPENTDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK) // spew bullets
		{
			if (me->frm == 5 && me->frmTimer < 32) // hit the guy
			{
				for (i = 0; i < 2; i++) // shoot bullets
				{
					byte b = (me->facing * 32 + i * 16 - 8) & 255;
					x = me->x + Cosine(b) * 8;
					y = me->y + Sine(b) * 8;
					FireBullet(x, y, b, BLT_ENERGY, me->friendly);
				}
				me->reload = 30;
			}
		}
		if (me->seq == ANIM_A1) // SNAP!
		{
			if(me->frm == 5 && me->frmTimer < 32)
				if (Guy* g = FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 72, 0, 0, 10, map, world, me->friendly))
				{
					SetPoisonFrames(g, g->poison + 30 * 10);
					me->mind = 0;
					me->mind1 = Random(2) ? 1 : -1;
					me->mind2 = 0;
					MakeSound(SND_LIGHTNING, me->x, me->y, SND_CUTOFF, 1000);
				}
		}
		return;
	}

	if (me->mind == 0) // "pursuit" (stalk, maintain distance from the player)
	{
		if (!goodguy) // no goodguy = wander!
		{
			me->mind = 1;
			me->mind1 = 1;
			return;
		}

		// pursuit mode uses me->mind1 to dictate movement

		i = me->RangeToGuy(goodguy)/FIXAMT;

		if (i < 192)
		{
			me->mind2 += 2;

			me->FaceGuy(goodguy);
			me->facing = (me->facing + 4) & 7; // spinning mad
			me->SetSpeed(5);

			if (me->mind2 % 8 == 0) // indicates
				BlowSmoke(me->x, me->y, me->z + 8 * FIXAMT, FIXAMT);

			if (me->mind2 > 30 * 2) // 2 seconds of annoyance -> chase mode
			{
				me->mind = 2;
				me->mind1 = 0;
				me->mind2 = 0;
				return;
			}
		}
		else if (i < 320)
		{
			me->FaceGuy(goodguy);

			if (me->mind2 > 0) // annoy meter cooldown
				me->mind2--;

			// complex movement (might delete later idk)
			if (me->mind1 != 1 && me->mind1 != -1) // need to set mind1
				me->mind1 = Random(2) ? 1 : -1;

			if (me->mind1 > 0)
				b = (me->facing + 2) & 7;
			else
				b = (me->facing + 6) & 7;

			me->dx = Cosine(b * 32) * 4;
			me->dy = Sine(b * 32) * 4;

			// not annoyed? flip i guess idk what this does
			if (!me->mind2 && !Random(90))
			{
				me->mind1 = -me->mind1;
				me->mind2 = Random(30) + 20;
			}

			if (!me->reload && !Random(30))
			{
				MakeSound(SND_SERPENTSPIT, me->x, me->y, SND_CUTOFF, 1200);
				me->ForceSequence(ANIM_ATTACK, 96);
				me->SetSpeed(0);
				me->FaceGuy(goodguy);
				me->reload = 0;
				return;
			}

			me->TryWander(me->mind2, 64, 1);
			me->ForceSequence(ANIM_MOVE, 128);
		}
	}
	else if (me->mind == 1) // wander mode
	{
		if (me->GuyWithinRange(goodguy, 320) && !Random(32) && me->CanSeeGuy(goodguy, 15, map)) // attack
		{
			MakeSound(SND_SERPENTSPIT, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_ATTACK, 96);
			me->FaceGuy(goodguy);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		if (me->GuyWithinRange(goodguy, 240) && !Random(8) && me->CanSeeGuy(goodguy, 15, map)) // go back to pursuit
		{
			me->mind = 0;
			me->mind1 = Random(2) ? 1 : -1;
			me->mind2 = 0;
			return;
		}

		me->WanderMode(me->mind2, 64, 0, 0);
		me->SetSpeed(4);
		me->ForceSequence(ANIM_MOVE, y);
	}
	else if (me->mind == 2) // ANGRY pursuit!
	{
		if (!goodguy) // no goodguy = wander!
		{
			me->mind = 1;
			me->mind1 = 1;
			me->mind2 = Random(40) + 20;
			return;
		}

		if (!me->GuyWithinRange(goodguy, 72)) // too far for quick rotation
			me->FaceGuyBroad(goodguy);
		else // use quick rotation
			me->FaceGuy(goodguy);

		me->SetSpeed(7);

		if (me->frm % 2 == 0)
			BlowSmoke(me->x, me->y, me->z, FIXAMT);


		if (me->GuyWithinRange(goodguy,72) && !Random(8)) // target close by? CHOMP!
		{
			MakeSound(SND_HAPPYCHOMP, me->x, me->y, SND_CUTOFF, 1500);
			me->FaceGuy(goodguy);
			me->ForceSequence(ANIM_A1, 192);
			me->SetSpeed(0);
			me->reload = 0;
			me->mind1 = 0;
			me->mind2 = 0;
			return;
		}

		if (!me->GuyWithinRange(goodguy, 255)) // target out of range? cool down!
		{
			me->mind2++;
			if (me->mind2 > 30 * 2)
			{
				me->mind = 0;
				me->mind1 = Random(2) ? 1 : -1;
				me->mind2 = 0;
				return;
			}
		}
		else if (me->mind2)
		{
			me->mind2--;
		}

		me->ForceSequence(ANIM_MOVE, 256);
	}
}

void DoBunnyLogic(Guy *me, Guy * target)
{
	int dist = me->RangeToGuy(target);
	me->DampenSpeed(4);
	if (!me->GuyWithinRange(target,32))
	{
		me->dx += (target->x - me->x) / 32;
		me->dy += (target->y - me->y) / 32;
	}
	me->dx += Random(FIXAMT / 4) - FIXAMT / 8;
	me->dy += Random(FIXAMT / 4) - FIXAMT / 8;
}

void AI_Jimbo(Guy* me, Map* map, world_t* world, Guy* goodguy)
{

	int x, y;
	byte b;
	Guy* bouapha; // specifically looks for player entity

	bouapha = GetGuyOfAIType(MONS_BOUAPHA);

	if (!bouapha)
		return;

	if (me->reload)
		me->reload--;

	if (me->mind2)
		me->mind2--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_JIMBOOUCH, SND_JIMBODIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK) // vertical charge
		{
			if (me->frm == 4 && me->frmTimer < 32) // start charge
			{
				MakeSound(SND_JIMBOATTACK, me->x, me->y, SND_CUTOFF, 1200);
				me->SetSpeed(10);
			}
			if (me->frm > 4)
			{
				me->DampenSpeed(4);
				if (me->frm < 9 && !me->reload)
				{
					if (me->AttackCheck(16, me->ShiftDirCos(16) >> FIXSHIFT, me->ShiftDirSin(16) >> FIXSHIFT, goodguy))
					{
						goodguy->GetShot(me->DirCos(4), me->DirSin(4), 4, map, world);
						me->reload = 3;
					}
				}
			}
		}
		if (me->seq == ANIM_A1) // sword bullet
		{
			if (me->frm == 5 && me->frmTimer < 32)
			{
				me->SimpleFireBullet(12, BLT_BIGSHELL);
				me->reload = 5;
			}
		}
		if (me->seq == ANIM_DIE) // snow upon death
		{
			for (x = 0;x < 4;x++)
				SpecialSnow(me->x + ((-16 + Random(33)) << FIXSHIFT), me->y + ((-16 + Random(33)) << FIXSHIFT));
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
		DoBunnyLogic(me, bouapha); // bunny logic!

		if (me->GuyWithinRange(bouapha, 48)) // too close to bouapha!
		{
			me->mind = 2;
			me->facing = Random(8);
			me->dx = Cosine(me->facing * 32) * 2;
			me->dy = Sine(me->facing * 32) * 2;
			me->mind1 = Random(20) + 10;
		}
	}
	else if (me->mind == 2) // taking a break!
	{
		me->mind1--;
		if (!me->mind1)
			me->mind = 1;
	}
	else if (me->mind == 3) // hunting for fresh meat
	{
		if (!me->GuyWithinRange(bouapha, 320) || !me->CanSeeGuy(bouapha, 15, map)) // too far from bouapha
		{
			me->mind = 1;
			return;
		}

		DoBunnyLogic(me, bouapha); // bunny logic!

		if (!Random(60) && me->CanSeeGuy(goodguy, 15, map) && me->GuyWithinRange(goodguy, 96)) // in this case, "goodguy" is target.
		{
			me->ForceSequence(ANIM_ATTACK, 128);
			me->FaceGuy(goodguy);
			me->SetSpeed(0);
			return;
		}

		if (!Random(60) && me->CanSeeGuy(goodguy, 15, map) && !me->GuyWithinRange(goodguy, 96)) // too far? get close!
		{
			me->ForceSequence(ANIM_A1, 128);
			me->FaceGuy(goodguy);
			me->SetSpeed(0);
			return;
		}
	}

	// if following Bouapha, and an enemy comes within sight, and you aren't already far from Bouapha
	if ((me->mind == 1 || me->mind == 2) && me->GuyWithinRange(bouapha,320) && me->GuyWithinRange(goodguy,128)
		&& me->CanSeeGuy(goodguy, 15, map) && me->CanSeeGuy(bouapha, 15, map))
	{
		me->mind = 3;	// enter assault mode,
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

byte IsGuyLooking(Guy* me, Guy* g)
{
	if (!me || !g)
		return 0;

	byte angle = AngleFrom(g->x, g->y, me->x, me->y);
	byte dir = ((angle + 16) / 32) % 8;

	byte diff = (dir - g->facing + 8) % 8;

	return (diff <= 1 || diff >= 7);
}

void AI_Umbrello(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_UMBRELLOOUCH, SND_UMBRELLODIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK) // claw attack!
		{
			if (me->frm == 2 && me->frmTimer < 32 && goodguy)
			{
				if (me->AttackCheck(16, me->ShiftDirCos(16) >> FIXSHIFT, me->ShiftDirSin(16) >> FIXSHIFT, goodguy))
				{
					goodguy->GetShot(me->DirCos(4), me->DirSin(4), 2, map, world);
					me->reload = 2; // check every 2 frames, to avoid too much damage
				}
			}
		}
		if (me->seq == ANIM_A2) // lowering down
		{
			if (me->frm == 3) // prepping
				me->mind = 0;

			if (me->frm > 5 && me->z == 0 && !me->reload) // no idea when he actually lands
			{
				MakeSound(SND_BOMBREFLECT, me->x, me->y, SND_CUTOFF, 1200);
				for (x = 0; x < 8; x++)
				{
					FireBulletZ(x, y, FIXAMT * 20, (x * 32), BLT_ENERGY, me->friendly);
				}
				me->reload = 20;
			}
		}
		return;
	}

	if (me->mind == 0) // tired. yawn.
	{
		me->FaceGuy(goodguy);
		if (!IsGuyLooking(me, goodguy) && me->GuyWithinRange(goodguy,256)) // activates if you are not looking!!
		{
			if (me->mind1++ > 30)
			{
				MakeSound(SND_UMBRELLOTAUNT, me->x, me->y, SND_CUTOFF, 1200);
				me->ForceSequence(ANIM_A1, 64);
			}
		}
		else if (me->mind1)
			me->mind1 = 0;
	}
	else if (me->mind == 1) // chasing target down
	{
		if (!IsGuyLooking(me, goodguy)) // if the player isn't looking at me, chase him down
		{
			if (me->mind1)
				me->mind1 = 0;

			me->FaceGuy(goodguy);

			if(!Random(8)) // random scary fumes
			{
				StinkySteam(me->x - FIXAMT * 20 + Random(FIXAMT * 40),
					me->y - FIXAMT * 20 + Random(FIXAMT * 40),
					me->z + FIXAMT * 40, FIXAMT * 2);
			}

			if (me->GuyWithinRange(goodguy,96) && !Random(32)) // get ready to attack!
			{
				MakeSound(SND_UMBRELLOSPEW, me->x, me->y, SND_CUTOFF, 1200);
				me->ForceSequence(ANIM_ATTACK, 128);
				me->SetSpeed(0);
				me->reload = 0;
				return;
			}
			me->SetSpeed(4);
		}
		else
		{
			if (me->mind1++ > 30) // cooldown meter going up?
			{
				MakeSound(SND_UMBRELLOSPEW, me->x, me->y, SND_CUTOFF, 1200);
				me->ForceSequence(ANIM_A2, 64);
				me->mind = 0;
				me->mind1 = 0;
				return;
			}
			me->DampenSpeed(8);
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
			if (me->aiType == ENT_NOHMANNCOMEDY)
			{

			}
			else if (me->aiType == ENT_NOHMANNTRAGEDY)
			{

			}
		}
		if (me->seq == ANIM_A1 && me->frm == 6 && me->frmTimer < 32)
		{
			if (me->aiType == ENT_NOHMANNCOMEDY)
			{

			}
			else if (me->aiType == ENT_NOHMANNTRAGEDY)
			{

			}
		}
		if (me->seq == ANIM_A2 && me->frm == 7 && me->frmTimer < 32)
		{
			if (me->type == ENT_NOHMANNCOMEDY)
				me->type = ENT_NOHMANNTRAGEDY;
			else if(me->type == ENT_NOHMANNTRAGEDY)
				me->type = ENT_NOHMANNCOMEDY;

			if (me->aiType == ENT_NOHMANNCOMEDY)
				me->aiType = ENT_NOHMANNTRAGEDY;
			else if (me->aiType == ENT_NOHMANNTRAGEDY)
				me->aiType = ENT_NOHMANNCOMEDY;
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

// deprecated
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
		me->SimpleOuchNoises(SND_RICHIEOUCH, SND_RICHIEDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK || me->seq == ANIM_A1 || me->seq == ANIM_A2) // throw the bullets!!
		{
			if (me->frm == 4 && me->frmTimer < 32 && goodguy)
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
			me->ForceSequence(ANIM_MOVE, 0, 1);
		}

		return;
	}

	if (me->mind == 0) // asleep
	{
		if (me->GuyWithinRange(goodguy, 320) && me->CanSeeGuy(goodguy, 15, map) || me->ouch) // guy visible and within range
		{
			MakeSound(SND_GHOSTTPORT, me->x, me->y, SND_CUTOFF, 1200);
			me->mind = 1;
			me->ForceSequence(ANIM_A3, 64);
			return;
		}
		// sleeping...
	}
	else if (me->mind == 1) // awake!
	{
		if (!goodguy)
		{
			me->mind = 0; // nevermind, i want to be a flower
			return;
		}

		if (goodguy->x < me->x - 45 * FIXAMT)
			me->facing = 3;
		else if (goodguy->x > me->x + 45 * FIXAMT)
			me->facing = 1;
		else
			me->facing = 2;

		if (goodguy->y > me->y && me->GuyWithinRange(goodguy,320) && me->CanSeeGuy(goodguy,15,map))
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

		me->ForceSequence(ANIM_MOVE, 0, 1);
	}
}


// AI for Bombie
void AI_Bombie(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x,y,i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_ZOMBIEOUCH, SND_ZOMBIEDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{

		if (me->seq == ANIM_DIE && !me->reload) // likes to spew
		{
			ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 8);
			me->reload = 1;
		}

		if (me->seq == ANIM_A1)
		{
			if (me->frm == 10)
			{
				me->SetSpeed(12);
				me->dz=20*FIXAMT;
			}
			if (me->frm > 10)
			{
				if (!me->reload && goodguy && (me->AttackCheck(8, me->ShiftDirCos(16) >> FIXSHIFT, me->ShiftDirSin(16) >> FIXSHIFT, goodguy)))
				{
					goodguy->GetShot(Cosine(me->facing*32)*4, Sine(me->facing*32)*4, 3, map, world);
					me->reload = 2;
				}
				if (me->z <= 0) // hit the ground (BOOM!!!) 
				{
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
			if (me->frm > 16) // stay like this until death
			{
				me->frm = 17;
				me->frmAdvance = 0;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0) // when mind=0, singlemindedly lumber towards Bouapha
	{
		if (!goodguy)
		{
			me->mind = 1;	// if there's no goodguy, get random
			me->mind1 = 1;
			return;
		}

		if (me->GuyWithinRange(goodguy, 128) && !Random(32) && me->CanSeeGuy(goodguy, 15, map))
		{
			me->ForceSequence(ANIM_A1, 192);
			me->SetSpeed(0);
			MakeSound(SND_BOMBIELEAP, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200); // random for the funnies
			return;
		}

		me->FaceGuy(goodguy);
		me->TryWander(me->mind1, 64, 1);
		me->SetSpeed(2);
		me->ForceSequence(ANIM_MOVE, 96);
	}
	else if (me->mind == 1)	// wander around randomly
	{
		if (!Random(16) && me->CanSeeGuy(goodguy, 15, map)) // more likely to pursue if target is within LOS
		{
			me->mind = 0;
			me->mind1 = 0;
			return;
		}

		me->WanderMode(me->mind1, 40, 3, 0);
		me->SetSpeed(1);
		me->ForceSequence(ANIM_MOVE, 64);
	}
	if (me->seq == ANIM_MOVE && me->frm > 5) // hobble like a zombie
	{
		me->DampenSpeed(2);
	}
}

void AI_Shroom(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_SKELOUCH, SND_PUMPKINDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 5 && !me->reload) // spores!!
		{
			for (i = 0;i < 256;i += 16)
				FireBullet(me->x, me->y, i, BLT_SPORE, me->friendly);
			me->reload = 5;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0) // pursuit mode
	{
		if (!goodguy)
		{
			me->mind = 1;	// if there's no goodguy, get random
			me->mind1 = 1;
			return;
		}
		if (me->GuyWithinRange(goodguy,72) && !Random(8)) // shoot spores
		{
			MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_ATTACK,128);
			me->SetSpeed(0);
			return;
		}

		me->FaceGuy(goodguy);
		me->SetSpeed(1);
		me->ForceSequence(ANIM_MOVE,128);
		me->TryWander(me->mind1, 64, 1);
	}
	else if (me->mind == 1)	// wander mode
	{
		me->WanderMode(me->mind1, 64, 3, 0);
		me->SetSpeed(1);
		me->ForceSequence(ANIM_MOVE, 128);
	}
}

void AI_Deathcap(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_SKELOUCH, SND_PUMPKINDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 5 && !me->reload)
		{
			MakeSound(SND_MISSILELAUNCH, me->x, me->y, SND_CUTOFF, 1200);
			for (i = 0;i < 4;i++)
				FireBullet(me->x, me->y, i*2 + me->mind2, BLT_ROCKET, me->friendly);
			me->mind2 = me->mind2==0; // flip!
			me->reload = 40;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0) // pursuit mode
	{
		if (goodguy) // no goodguy = wander
		{
			me->mind = 1;
			me->mind1 = 1;
			return;
		}

		if (me->GuyWithinRange(goodguy, 160) && !Random(16) && me->CanSeeGuy(goodguy, 20, map)) // shoot ROCKETS!
		{
			MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			return;
		}

		me->FaceGuy(goodguy);
		me->SetSpeed(2);
		me->ForceSequence(ANIM_MOVE, 128);
		me->TryWander(me->mind1, 64, 1);
	}
	else if (me->mind == 1)	// random wandering
	{

		if (me->GuyWithinRange(goodguy, 160) && !Random(64)) // shoot ROCKETS!
		{
			MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			return;
		}

		me->WanderMode(me->mind1, 64, 3, 0);
		me->SetSpeed(2);
		me->ForceSequence(ANIM_MOVE, 128);
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

static void FacePathMarker(Guy* me, marker_t* marker)
{
	if (!marker)
		return;

	int targetX = marker->x1 * FIXAMT + FIXAMT / 2;
	int targetY = marker->y1 * FIXAMT + FIXAMT / 2;

	int dx = targetX - me->x;
	int dy = targetY - me->y;

	float angle = atan2f((float)dy, (float)dx);
	int angle256 = (int)(angle * (128.0f / 3.14159265f));

	if (angle256 < 0)
		angle256 += 256;

	me->facing = ((angle256 + 16) / 32) & 7;
}

// AI for Easter Egg
void AI_EasterEgg(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	byte b;
	int x, y;

	// mind2 = current path priority
	// mind3 = current marker index + 1
	// mind1 = reserved

	// More damaged = faster.
	x = me->maxHP - me->hp + 5;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_ZOMBIEOUCH, SND_ZOMBIEDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frmTimer < 63)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty,
				x + me->rectx2, y + me->recty2, me->z, 1);
		}

		if (me->seq == ANIM_ATTACK && me->frmTimer < 128 && goodguy && (me->frm == 6 || me->frm == 10))
		{
			MakeSound(SND_MUSHMISSILE, me->x, me->y, SND_CUTOFF | SND_RANDOM, 1200);
			b = (me->facing * 32 + (Random(17) - 8)) & 255;
			FireExactBullet(me->ShiftDirCos(16), me->ShiftDirSin(16), FIXAMT * 10,
				Cosine(b) * 4, Sine(b) * 4, FIXAMT * 16, 0, 60, b, BLT_GRENADE, me->friendly);
			if (me->frm == 10)
				me->reload = 30;
		}
		return;
	}

	// set the marker
	if (!me->mind3)
	{
		word path = GetPathMarker(me, 1, me->mind2);
		if (path == 65535)
		{
			me->DampenSpeed(4);
			return;
		}
		me->mind3 = path + 1;
	}

	word path = me->mind3 - 1;
	marker_t* node = GetMarker(path);

	if (!node || node->x1 == 255 || node->type != MRK_PATH)
	{
		me->mind3 = 0;
		me->dx = 0;
		me->dy = 0;
		return;
	}

	int targetX = (node->x1 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
	int targetY = (node->y1 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;

	// arrived at the marker
	if (RangeToPoint(me, targetX, targetY) <= 8 * FIXAMT)
	{
		me->mind2 = node->value2;
		me->mind3 = 0;
		me->SetSpeed(0);
		return;
	}

	me->ForceSequence(ANIM_MOVE, 96 + (x - 5) * 96 / me->maxHP);
	x = 6 + (x - 5) * 6 / me->maxHP;

	if (me->frm < 4 && (me->z + me->dz) < 1 * FIXAMT)
	{
		if (me->mind1)
		{
			MakeSound(SND_SPIKECLANK, me->x, me->y,
				SND_CUTOFF, 1200);

			me->mind1 = 0;
		}

		Dampen(&me->dx, FIXAMT * 2);
		Dampen(&me->dy, FIXAMT * 2);
	}
	else if (me->frm == 4 && me->frmTimer < me->frmAdvance)
	{
		byte angle = AngleFrom(me->x, me->y, targetX, targetY);

		me->facing = ((angle + 16) / 32) & 7;

		MakeSound(SND_HAMMERTOSS, me->x, me->y,
			SND_CUTOFF, 2);

		me->dz = x * FIXAMT;
		me->mind1 = 1;

		me->dx = Cosine(angle) * x;
		me->dy = Sine(angle) * x;
	}
	else if (me->frm > 4)
	{
		me->DampenSpeed(4);
	}

	if (goodguy && !me->reload && abs(me->dx + me->dy) > 1 * FIXAMT && RangeToTarget(me, goodguy) < 44 * FIXAMT)
	{
		goodguy->GetShot(me->dx, me->dy, 6, map, world);
		me->reload = 2;
	}

	if (me->GuyWithinRange(goodguy,128) && !Random(32) && !me->reload)
	{
		me->ForceSequence(ANIM_ATTACK, 128 + (x - 5) * 128 / me->maxHP);
		me->SetSpeed(0);
		me->FaceGuy(goodguy);
		return;
	}

	// face the marker
	byte angle = AngleFrom(me->x, me->y, targetX, targetY);
	me->facing = ((angle + 16) / 32) & 7;
}


void AI_SpikeBall(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_ZOMBIEOUCH, SND_PUMPKINDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && !me->reload) // spew sharpnel
		{
			ExplodeParticles(PART_SHRAPNEL, me->x, me->y, me->z, 8);
			me->reload = 2;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind2)
		me->mind2--;
	else
	{
		me->mind2 = 3;
		me->FaceGuyBroad(goodguy);
	}
	me->DampenSpeed(8);		// very gradual
	me->Accelerate(0.5);	// accelerate until clamped

	if (!me->reload && me->GuyWithinRange(goodguy,64))
	{
		if (me->AttackCheck(35, me->ShiftDirCos(16) >> FIXSHIFT, me->ShiftDirSin(16) >> FIXSHIFT, goodguy))
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
		MakeSound(SND_SPIKEBOUNCE, me->x, me->y, SND_CUTOFF, 200);
		if(me->facing%2==0)
			me->facing = (me->facing+4)&7;
		else if (!Random(2))
			me->facing = (me->facing+2)&7;
		else
			me->facing = (me->facing-2)&7;
	}

	me->ClampSpeed(8);
	me->ForceSequence(ANIM_MOVE, (abs(me->dx) + abs(me->dy)) / FIXAMT * 32);
}

byte IsTargetVisible(Guy* me, Map* map, Guy* goodguy)
{
	if (!goodguy || goodguy->type == ENT_NOBODY)
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
	byte b;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_STOPSIGNOUCH, SND_STOPSIGNDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 3 && !me->reload)
		{
			if (FindVictim(me->ShiftDirCos(24) >> FIXSHIFT, me->ShiftDirSin(24) >> FIXSHIFT, 40, 0, 0, 16, map, world, me->friendly))
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

		if (goodguy && abs(goodguy->dx + goodguy->dy) > FIXAMT && !me->reload) // player is moving too much
		{
			me->mind2++;
			if (me->mind2 >= 15 && !Random(100-me->mind2)) // had enough moving!
			{
				MakeSound(SND_STOPSIGNHEY, me->x, me->y, SND_CUTOFF, 1200);
				me->mind = 1;
				me->mind2 = 0;
				me->ForceSequence(ANIM_A1, 96);
				me->SetSpeed(0);
				me->FaceGuy(goodguy);
				me->reload = 0;
				return;
			}
		}
		else if (me->mind2 > 0)  // reset move timer
		{
			me->mind2 = 0;
		}

		me->SetSpeed(2);
		me->ForceSequence(ANIM_MOVE,96);

	}
	if (me->mind == 1) // awake, mad at you
	{
		me->FaceGuyBroad(goodguy);

		if (!goodguy || goodguy->hp < 1) // guy is dead/nonexistant, no need to chase.
		{
			me->mind	= 0;
			me->mind2	= 0;
			return;
		}

		if (abs(goodguy->dx + goodguy->dy) < FIXAMT) // cooled down!
		{
			me->mind2++;
			if (me->mind2 >= 60)
			{
				me->mind	= 0;
				me->mind2	= 0;
				return;
			}
		}

		if (me->GuyWithinRange(goodguy,48) && !Random(8)) // goodguy within smashing range!
		{
			MakeSound(SND_SKELKICK, me->x, me->y, SND_CUTOFF, 1200);
			me->FaceGuy(goodguy);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		me->SetSpeed(6);
		me->ForceSequence(ANIM_A2, 128);
		me->action = ANIM_IDLE;
	}
}

void AI_Kabocha(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_BUSHOUCH, SND_PUMPKINDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{

	}

	if (me->seq == ANIM_MOVE && me->frm == 2 && goodguy)	// hits on this frame
	{
		FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 24, Cosine(me->facing * 32) * 4, Sine(me->facing * 32) * 4, 2, map, world, me->friendly);
	}

	if (me->mind == 0)	// bored
	{
		me->ForceSequence(ANIM_IDLE, 32);

		if ((me->GuyWithinRange(goodguy, 256) && me->CanSeeGuy(goodguy, 15, map)) || me->ouch) // if the goodguy is near, or he shot me
		{
			me->mind = 1;			// start hunting...
			if (!me->ouch)
				me->mind1 = 60;		// for 2 seconds minimum
			else
				me->mind1 = 120;	// 4 seconds, because they HATE getting shot
			me->FaceGuy(goodguy);
		}
		me->DampenSpeed(4);
	}
	else // woke pumpkin
	{
		if (!goodguy)
		{
			me->mind = 0;	// go back to being bored, nothing to see here
			me->mind1 = 0;
			return;
		}

		me->FaceGuy(goodguy);

		if (me->mind1)
			me->mind1--;
		else
		{
			if (me->GuyWithinRange(goodguy, 256) && me->CanSeeGuy(goodguy, 15, map))
				me->mind = 0;	// get bored again
			else
				me->mind1 = 20;	// stay on trail a little longer
			return;
		}

		me->SetSpeed(2);
		me->ForceSequence(ANIM_MOVE, 128);
	}
}

void AI_Pumpkin(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_BUSHOUCH, SND_PUMPKINDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && !me->reload)
		{
			me->frmAdvance = 64;
			if (IsEntity(me,ENT_BOOMKIN) && me->frm == 3)
				FireBullet(me->x, me->y, 0, BLT_BOOM, me->friendly);
			ExplodeParticles(PART_HAMMER, me->x, me->y, me->z, 8);
			me->reload = 2;
		}
		return;	// can't do nothin' right now
	}

	if (me->seq == ANIM_MOVE)	// hits on this frame
	{
		if (me->frm == 1) // hit the player
		{
			if (me->HasGuyAI(ENT_BOOMKIN))
			{
				// explode
				if (FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 32, 0, 0, 0, map, world, me->friendly))
				{
					me->hp = 1;
					me->GetShot(0, 0, 1, map, world);
					return;
				}
			}
			else
			{
				FindVictim(me->x >> FIXSHIFT, me->y >> FIXSHIFT, 24, me->DirCos(4), me->DirSin(4), 2, map, world, me->friendly);
			}
		}
	}

	if (me->mind == 0)	// bored
	{
		me->ForceSequence(ANIM_IDLE, 32);

		if ((me->GuyWithinRange(goodguy, 256) && me->CanSeeGuy(goodguy, 15, map)) || me->ouch) // if the goodguy is near, or he shot me
		{
			me->mind = 1;			// start hunting...
			if (!me->ouch)
				me->mind1 = 60;		// for 2 seconds minimum
			else
				me->mind1 = 120;	// 4 seconds, because they HATE getting shot
			me->FaceGuy(goodguy);
		}
		me->DampenSpeed(4);
	}
	else // woke pumpkin
	{
		if (!goodguy)
		{
			me->mind = 0;	// go back to being bored, nothing to see here
			me->mind1 = 0;
			return;
		}

		me->FaceGuy(goodguy);

		if (me->mind1)
			me->mind1--;
		else
		{
			if (me->GuyWithinRange(goodguy, 256) && me->CanSeeGuy(goodguy, 15, map))
				me->mind = 0;	// get bored again
			else
				me->mind1 = 20;	// stay on trail a little longer
			return;
		}

		if (!me->HasGuyAI(ENT_BOOMKIN)) // pumpkin
		{
			me->SetSpeed(2);
			me->ForceSequence(ANIM_MOVE, 128);
		}
		else // boomkin
		{
			me->SetSpeed(6);
			me->ForceSequence(ANIM_MOVE, 192);
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
		me->SimpleOuchNoises(SND_BUSHOUCH, SND_PATCHDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 18)
		{
			me->seq = ANIM_A1;
			me->frm = 0;
		}
		if (me->seq == ANIM_A1 && me->frm == 7 && !me->reload)
		{
			if (g = AddBaby(me->x, me->y + FIXAMT * 2, 0, ENT_PUMPKIN, me))
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

	if (me->GuyWithinRange(goodguy, 320) && !me->reload && !Random(50) && me->CanSeeGuy(goodguy,15,map))
	{
		MakeSound(SND_EGGOPEN, me->x, me->y, SND_CUTOFF, 1200);
		me->ForceSequence(ANIM_ATTACK, 128);
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
		me->SimpleOuchNoises(SND_ROBOOUCH, SND_ROBODIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 8 && !me->reload)
		{
			if (g = AddBaby(me->x, me->y + FIXAMT * 64, 0, ENT_NINJABREAD, me))
			{
				if (!g->CanWalk(g->x, g->y, map, world))
				{
					RemoveGuy(g);
					me->reload = 5;
				}
				else
				{
					MakeSound(SND_GENERATE, me->x, me->y, SND_CUTOFF | SND_ONE, 1200); // TODO: make this an oven timer.
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

	if (me->GuyWithinRange(goodguy,512) && !me->reload && !Random(50) && me->CanSeeGuy(goodguy,15,map))
	{
		//MakeSound(SND_MISSILELAUNCH, me->x, me->y, SND_CUTOFF, 1200);
		me->ForceSequence(ANIM_ATTACK, 64);
		return;
	}

	me->ForceSequence(ANIM_MOVE, 64);
	if (me->frm % 4 == 0 && !Random(8))
	{
		BlowSmoke(me->x - 40 * FIXAMT + Random(80 * FIXAMT), me->y + + Random(15 * FIXAMT),
			20 * FIXAMT, Random(8) * FIXAMT + FIXAMT);
	}
}

void AI_DullDrum(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte ang;

	// mind1	= exact facing for shooting
	// mind		= timer to wake up

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_SKELOUCH, SND_SKELDIE, 1); // todo: make new voice liens?
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm >= 2 && me->frm <= 5 && !me->reload)
		{
			ang = (me->facing * 32 - 40 + (20 * (me->frm - 2))) & 255;
			x = me->x + Cosine(ang) * 16;
			y = me->y + Sine(ang) * 16;
			FireBullet(x, y, ang, BLT_ACID, me->friendly);
			me->reload = 2;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0) // asleep
	{
		if ((me->GuyWithinRange(goodguy, 256) && !Random(8) && me->CanSeeGuy(goodguy, 15, map)) || me->ouch)
		{
			MakeSound(SND_SKELALIVE, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_A1, 96);
			me->reload = 0;
			me->mind = 2;
			return;
		}
	}
	else if (me->mind == 1) // awake, but dormant.
	{
		if (me->GuyWithinRange(goodguy, 256) && !Random(128) && me->CanSeeGuy(goodguy, 15, map))
		{
			MakeSound(SND_SKELALIVE, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_A1, 96);
			me->mind = 2;
			return;
		}
	}
	else if (me->mind == 2) // awake, mad at you
	{
		if ((!me->GuyWithinRange(goodguy, 256) || !me->CanSeeGuy(goodguy, 15, map)) && !Random(128))
		{
			me->ForceSequence(ANIM_A2, 96);
			me->reload = 0;
			me->mind = 1;
			return;	// go back into your shell, you can't reach him
		}

		if (me->GuyWithinRange(goodguy, 256) && !Random(32))
		{
			me->FaceGuy(goodguy);
			me->ForceSequence(ANIM_ATTACK, 128);
			MakeSound(SND_SKELSHOOT, me->x, me->y, SND_CUTOFF, 1200);
			me->reload = 0;
			return;
		}

		me->ForceSequence(ANIM_MOVE, 128);
	}
}

void AI_Bodzha(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, ang;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE && me->frm == 3)
			me->frmAdvance = 256;

		if (me->seq == ANIM_ATTACK && me->frm == 2 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			// ???
			me->reload = 30;
		}

		if (me->seq == ANIM_A1)	// burrowing
		{
			if (me->frm == 7)
			{
				me->ForceSequence(ANIM_A3, 16);
				me->mind2 = 1;
				return;
			}
			else if (me->frm >= 2)
				me->facing = (me->facing + 1) & 7;
		}

		if (me->seq == ANIM_A2)	// popping up
		{
			if (me->frm == 7)
			{
				me->ForceSequence(ANIM_MOVE, 16);
				me->mind2 = 0;
				return;
			}
			else if (me->frm < 8) // spinning
				me->facing = (me->facing + 1) & 7;

			if (me->frm == 2 && !me->reload) // setting up reload?
			{
				me->reload = 30;
			}
		}
		return;
	}

	// he is underground
	if (me->mind2 == 1)
	{
		ExplodeParticles2(PART_DIRT, me->x, me->y, 0, 5, 6);
		me->ForceSequence(ANIM_A3, 16);

		if (me->CanSeeGuy(goodguy, 3, map) || !Random(200) || me->ouch);
		{
			me->ForceSequence(ANIM_A2, 128);
			MakeSound(SND_CRAZYSPIN, me->x, me->y, SND_CUTOFF, 1200);
		}
		return;
	}

	if (me->mind == 0) // pursuit mode
	{
		me->FaceGuy(goodguy);

		if (me->GuyWithinRange(goodguy, 256) && !Random(32) && me->CanSeeGuy(goodguy,3,map));
		{
			MakeSound(SND_HAMMERTOSS, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		me->TryWander(me->mind1, 64, 1);
		me->SetSpeed(5);
		me->ForceSequence(ANIM_MOVE, 128);
	}
	else if (me->mind == 1)	// random wandering
	{
		if (me->GuyWithinRange(goodguy, 256) && !Random(32) && me->CanSeeGuy(goodguy, 3, map));
		{
			MakeSound(SND_HAMMERTOSS, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		if (me->GuyWithinRange(goodguy, 64) && !Random(8) && me->CanSeeGuy(goodguy, 3, map));
		{
			// spit at him
			MakeSound(SND_HAMMERTOSS, me->x, me->y, SND_CUTOFF, 1200);
			me->FaceGuy(goodguy);
			me->ForceSequence(ANIM_ATTACK, 128);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		if (!me->GuyWithinRange(goodguy, 128) && !Random(24) && me->CanSeeGuy(goodguy, 3, map));
		{
			// spit at him
			MakeSound(SND_HAMMERTOSS, me->x, me->y, SND_CUTOFF, 1200);
			me->FaceGuy(goodguy);
			me->ForceSequence(ANIM_A1, 128);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		me->WanderMode(me->mind1, 40, 3, 0);
		me->SetSpeed(5);
		me->ForceSequence(ANIM_MOVE, 64);
	}
}

void AI_DecoyBouapha(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->ouch == 4)
	{
		// silent but deadly
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			me->facing = (me->facing + 1) & 7;
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,me->z, FIXAMT);
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)
	{
		me->mind = 1;
		me->mind1 = 0;
		me->mind2 = 0;
	}
	else if (me->mind == 1)
	{
		if (me->mind1++ > 30)
		{
			me->mind2+=1;
			me->mind1=0;
		}
		if (me->mind2 == 10)
		{
			me->hp = 1;
			me->GetShot(0, 0, 10, map, world);
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
			BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT, (y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT, me->z, FIXAMT);
		}
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			if (me->mind2 < 5)
				me->frmAdvance = 64;
			else if (me->mind2 < 8)
				me->frmAdvance = 128;
			else
				me->frmAdvance = 256;
		}
	}
}

// AI for Cacton
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
		if (me->seq == ANIM_ATTACK && me->frm == 3)
		{
			// fire lots of spines
			MakeSound(SND_CACTONSHOOT, me->x, me->y, SND_CUTOFF, 600);

			// but only fire if within a few hundred miles
			if (me->GuyWithinRange(goodguy, 512))
			{
				b = (me->facing * 32 - 60);
				if (me->frmTimer > 64)
					b += 5;

				for (i = 0;i < 12;i++)
				{
					FireBullet(me->ShiftDirCos(32), me->ShiftDirSin(32), b, BLT_SPINE, me->friendly);
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
				me->dx -= Cosine(me->facing * 32)/2;
			if (abs(me->dy) > 0)
				me->dy -= Sine(me->facing * 32)/2;
		}
		return;
	}

	if (me->mind == 0)	//idling
	{
		if (me->mind1)
			me->mind1--;
		else
		{
			me->mind	= 1;	// start rolling somewhere
			me->facing	= (byte)Random(8);
			me->mind1	= Random(30) + 30;
			me->ForceSequence(ANIM_A1, 256);
			return;
		}
		if (!me->reload)
		{
			me->reload	= 120;
			me->FaceGuy(goodguy);
			me->ForceSequence(ANIM_ATTACK, 128);
			return;
		}
		me->DampenSpeed(2);
		me->ForceSequence(ANIM_IDLE,32);

	}
	else if (me->mind == 1)
	{
		if (!--me->mind1)
		{
			me->mind = 0;	// got to the destination
			me->mind1 = Random(70) + 30;
			me->ForceSequence(ANIM_A2, 256);
			me->SetSpeed(1);
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

void AI_MutantZombie(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_ZOMBIEOUCH, SND_ZOMBIEDIE, 1);
	}

	// handle the hp regeneration
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
			me->SetSpeed(12);
		}
		if (me->seq == ANIM_ATTACK && me->frm > 6)
		{
			me->DampenSpeed(2);
			if (me->frm == 13 && me->frmTimer < 64)
			{
				for (x = 0;x < 256;x += 8)
				{
					FireExactBullet(me->x, me->y, FIXAMT * 10, Cosine(x) * 12, Sine(x) * 12, 0, 0, 16, x, BLT_SPORE, me->friendly);
					FireExactBullet(me->x, me->y, FIXAMT * 10, Cosine(x) * 6, Sine(x) * 6, 0, 0, 16, x, BLT_SPORE, me->friendly);
				}
			}
		}
		if (me->seq == ANIM_ATTACK && me->frm >= 7 && me->frm < 13 && !me->reload && goodguy)
		{
			if (me->AttackCheck(8, me->ShiftDirCos(16) >> FIXSHIFT, me->ShiftDirSin(16) >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(me->DirCos(4), me->DirSin(4), 1, map, world);
				me->reload = 2;
			}
		}
		return;	// can't do nothin' right now
	}

	if (!Random(10))
		FireBullet(me->x - FIXAMT * 20 + Random(FIXAMT * 40), me->y - FIXAMT * 20 + Random(FIXAMT * 40), Random(256), BLT_SPORE, me->friendly);

	if (me->mind == 0)	// pursuit mode
	{
		if (!goodguy)
		{
			me->mind = 1;
			me->mind1 = 1;
			return;
		}

		if (me->CanSeeGuy(goodguy, 15, map) && me->GuyWithinRange(goodguy, 128) && !Random(32)) // only leap if guy is seen
		{
			MakeSound(SND_ZOMBIELEAP, me->x, me->y, SND_CUTOFF, 1200);
			me->ForceSequence(ANIM_ATTACK, 64);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

		me->FaceGuy(goodguy);

		if (me->frm % 6 < 3)
			me->SetSpeed(2);
		else
			me->DampenSpeed(2);

		me->TryWander(me->mind1, 64, 1);
		me->ForceSequence(ANIM_MOVE, 128);
	
	}
	else if (me->mind == 1)	// random wandering
	{
		if (!Random(16) && me->CanSeeGuy(goodguy, 15, map)) // more likely to pursue if target is within LOS
		{
			me->mind = 0;
			me->mind1 = 0;
			return;
		}

		me->WanderMode(me->mind1, 40, 3, 0);

		if (me->frm % 6 < 3)
			me->SetSpeed(2);
		else
			me->DampenSpeed(2);

		me->ForceSequence(ANIM_MOVE, 64);
	}
}

void AI_Serpent(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_SERPENTOUCH, SND_SERPENTDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 6 && !me->reload == 0)
		{
			// spit stuff
			x = me->x + Cosine(me->facing * 32) * 8;
			y = me->y + Sine(me->facing * 32) * 8;
			FireBullet(x, y, me->facing * 32, BLT_ENERGY, me->friendly);
			FireBullet(x, y, (me->facing * 32 + 4) & 255, BLT_ENERGY, me->friendly);
			FireBullet(x, y, (me->facing * 32 - 4) & 255, BLT_ENERGY, me->friendly);
			me->reload = 15;
			me->mind = 0;
		}
		return;	// can't do nothin' right now
	}

	if (me->GuyWithinRange(goodguy,512)) // if a player doesn't see you ripple, do you really ripple?
		WaterRipple((me->x + me->dx * 2) / FIXAMT, (me->y + me->dy * 2) / FIXAMT, 8 * Random(32));

	if (me->mind == 0) // wander mode?
	{
		if (me->GuyWithinRange(goodguy, 512) && !Random(16) && !me->reload)
		{
			me->ForceSequence(ANIM_ATTACK, 128);
			me->FaceGuy(goodguy);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

	}

	me->WanderMode(me->mind1, 60, 0, me->mind);
	me->SetSpeed(2);
	me->ForceSequence(ANIM_MOVE, 128);
}

void AI_Radozoid(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_SERPENTOUCH, SND_SERPENTDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && (me->frm >= 5 && me->frm <= 8) && goodguy)
		{
			i = (me->facing * 32 + 256 - 16 + Random(33)) & 255;
			FireExactBullet(me->x, me->y, FIXAMT * 30, Cosine(i) * 12, Sine(i) * 12, 0, 0, 16, i, BLT_ENERGY, me->friendly);
			me->reload = 15;
			me->mind = 0;
		}
		return;	// can't do nothin' right now
	}

	if (me->GuyWithinRange(goodguy, 512)) // if a player doesn't see you ripple, do you really ripple?
		WaterRipple((me->x + me->dx * 2) / FIXAMT, (me->y + me->dy * 2) / FIXAMT, 8 * Random(32));

	if (me->mind == 0) // wander mode?
	{
		if (me->GuyWithinRange(goodguy, 512) && !Random(16) && !me->reload)
		{
			me->ForceSequence(ANIM_ATTACK, 128);
			me->FaceGuy(goodguy);
			me->SetSpeed(0);
			me->reload = 0;
			return;
		}

	}

	me->WanderMode(me->mind1, 60, 0, me->mind);
	me->SetSpeed(2);
	me->ForceSequence(ANIM_MOVE, 128);
}

// used for mechatruck
void AI_Zomboni(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			if (me->seq == ANIM_DIE && me->reload == 0)
			{
				FireBullet(me->x, me->y, 0, BLT_BOOM, me->friendly);
				BlowSmoke(me->x - 32 * FIXAMT + Random(64 * FIXAMT),
					me->y - 32 * FIXAMT + Random(64 * FIXAMT),
					Random(64) << FIXSHIFT, Random(FIXAMT * 4));
				me->reload = 5;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->seq == ANIM_MOVE && me->frm % 4 == 1 && me->frmTimer < 32)
		MakeSound(SND_ZOMBONIZOOM, me->x, me->y, SND_CUTOFF, 1500);

	if (me->mind == 0) // asleep
	{
		if ((goodguy && RangeToTarget(me, goodguy) < (400 * FIXAMT)) || (me->ouch))
		{
			me->mind = 1;	// start chasing him down
		}
	}
	else // awake and ANGRY!!!
	{
		if (!me->reload)
		{
			me->FaceGuyBroad(goodguy);
			me->reload = 8;
		}

		me->SetSpeed(6);
		me->ForceSequence(ANIM_MOVE, 256);

		if(me->GuyWithinRange(goodguy,128) && me->AttackCheck(64, me->ShiftDirCos(32) >> FIXSHIFT, me->ShiftDirSin(32) >> FIXSHIFT, goodguy))
		{
			goodguy->GetShot(me->DirCos(16), me->DirSin(16), 10, map, world);
		}
	}
}

byte TargetInSight(Guy* me, Map* map, Guy* goodguy)
{
	if (!goodguy || goodguy->type == ENT_NOBODY)
		return 0;

	switch (me->mind2)
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

void AI_Patrol(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;
	byte a;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_HARDHATOUCH, SND_HARDHATDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 5 && me->frmTimer < 32)
		{
			a = me->facing * 32 - 16 + Random(32);
			FireExactBullet(me->ShiftDirCos(20), me->ShiftDirSin(20), FIXAMT * 20, Cosine(a) * 14, Sine(a) * 14, FIXAMT * 10, 0, 60, 0, BLT_CLAWHAMMER, me->friendly);
			me->reload = 20;
		}
		return;	// can't do nothin' right now
	}

	if (abs(me->mind2 - me->facing) == 4)	// max distance
	{
		if (Random(2))
			me->facing++;
		else
			me->facing--;
		me->facing &= 7;
	}
	else if (me->mind2 == me->facing)
	{
		// idk man
	}
	else	// rotate towards true facing
	{
		int dir, diff;

		if (me->mind2 > me->facing)
		{
			diff = me->mind2 - me->facing;
			if (diff > 4)
			{
				dir = -1;
			}
			else
				dir = 1;
		}
		else
		{
			diff = me->facing - me->mind2;
			if (diff > 4)
			{
				dir = 1;
			}
			else
				dir = -1;
		}
		me->facing = (me->facing + dir) & 7;
	}

	if (me->mind == 0)	// newborn
	{
		me->facing = Random(8);
		if (IsEntity(me,ENT_PATROLLERH))
			me->mind2 = 0;	// desired facing/also direction of movement
		else
			me->mind2 = 2;	// desired facing/also direction of movement

		me->mind = 1;
	}
	else if (me->mind == 1)	// pacing
	{
		// if lighting is dark, turn on flashlight
		mapTile_t* mt = &map->map[me->mapx + me->mapy * map->width];
		if (mt->templight < -5) // un-stealth it!
		{
			map->BrightTorch(me->mapx, me->mapy, 8, 8);
		}

		if (me->mind1)	// bonked a wall
		{
			me->mind1 = 0;
			if (me->mind2 < 4)
				me->mind2 += 4;
			else
				me->mind2 -= 4;
		}
		if (me->mind2 != me->facing)
		{
			me->SetSpeed(0);
		}
		else
		{
			me->dx = Cosine(me->mind2 * 32)*2;
			me->dy = Sine(me->mind2 * 32)*2;
		}

		me->ForceSequence(ANIM_MOVE, 64);


		if (me->mind2 == me->facing && TargetInSight(me, map, goodguy))
		{
			MakeSound(SND_HARDHATTAUNT, me->x, me->y, SND_CUTOFF, 1000);
			me->ForceSequence(ANIM_A1, 256);
			me->SetSpeed(0);
			me->reload = 0;
			if (goodguy->HasGuyAI(ENT_BOUAPHA)) // light the player "on fire"
				player.spotted = 30 * 8;
			me->mind = 2;	// start clapping!
		}
	}
	else if (me->mind == 2)	// staring at goodguy
	{
		if (TargetInSight(me, map, goodguy))
		{
			if (goodguy->HasGuyAI(ENT_BOUAPHA)) // restart the spotted timer
				player.spotted = 30 * 8;
			MakeSound(SND_HARDHATSHOOT, me->x, me->y, SND_CUTOFF, 1000);
			me->ForceSequence(ANIM_ATTACK, 256);
			me->SetSpeed(0);
			me->reload = 0;
		}
		else
			me->mind = 1;
	}
}

void AI_Shamisen(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_SHAMOUCH, SND_SHAMDIE, 0); // does not do random noises!
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && (me->frm == 5) && me->reload == 0 && goodguy)
		{
			me->SimpleFireBullet(12, BLT_PAPER);
			me->reload = 2;
		}
		return;	// can't do nothin' right now
	}

	if (me->mind==0)
	{
		me->mind = 1;
		x = (byte)((Random(4)+1)*2-1);
		me->dx = Cosine(x*32)*3;
		me->dy = Sine(x*32)*3;
	}
	else if (me->mind == 1)
	{
		// bump logic
		if (me->mind1)
		{
			MakeSound(SND_SHAMBUMP, me->x, me->y, SND_CUTOFF, 256);
			if (me->mind1 & 1)
				me->dx = -me->dx;
			if (me->mind1 & 2)
				me->dy = -me->dy;
			me->mind1 = 0; // reset!
		}

		me->FaceGuy(goodguy);

		if (me->GuyWithinRange(goodguy, 128) && !me->reload && !Random(16) && me->CanSeeGuy(goodguy, 5, map)) // guy is nearby and visible
		{
			me->FaceGuy(goodguy);
			MakeSound(SND_SHAMATTACK, me->x, me->y, SND_CUTOFF, 1000);
			me->ForceSequence(ANIM_ATTACK, 192);
			return;
		}

		me->ForceSequence(ANIM_MOVE, 128);
	}
}

void AI_Shopkeeper(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;
	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		// dies silently.
	}

	if (me->action == ACTION_BUSY)
	{
		FaceGoodguy(me, goodguy);
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// idling
	{
		// face the player if he's close enough
		if (me->GuyWithinRange(goodguy,192))
		{
			me->FaceGuy(goodguy);
			i = 256;
			if (goodguy->dx == 0 && goodguy->dy == 0)
				i = 64;
			if(!Random(i))
			{
				i=Random(100);
				if (i < 50)
					MakeSound(SND_SHOPKEEPER1, me->x, me->y, SND_ONE|SND_RANDOM, 1000);
				else if (i < 80)
					MakeSound(SND_SHOPKEEPER2, me->x, me->y, SND_ONE|SND_RANDOM, 1000);
				else
					MakeSound(SND_SHOPKEEPER3, me->x, me->y, SND_ONE|SND_RANDOM, 1000);

				me->ForceSequence(ANIM_ATTACK, 128);
				return;
			}
		}
		me->ForceSequence(ANIM_IDLE, 128);
	}
	else if (me->mind == 1)
	{
		me->ForceSequence(ANIM_A1, 64);
		me->mind = 0;
	}
}

void AI_Spotlight(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (!me->mind4)
		me->mind4 = 1;

	if (me->mind1)
	{
		if(me->mind1>26)
			map->BrightTorch(me->mapx, me->mapy, 16, 16);
		else
			map->BrightTorch(me->mapx, me->mapy, 4, 8);
		me->mind1--;
	}
	else
	{
		me->mind1 = 30;
		MakeSound(SND_CRABCLICK, me->x, me->y, SND_CUTOFF, 100);
	}

	// mind2 = current path priority
	// mind3 = current marker index + 1 (0 means no marker selected)

	if (!me->mind3) // find the next waypoint.
	{
		word path = GetPathMarker(me, me->mind4, me->mind2);

		if (path == 65535)
		{
			me->SetSpeed(0);
			return;
		}
		me->mind3 = path + 1; // marker index found!
	}

	word path = me->mind3 - 1;
	marker_t* node = GetMarker(path);

	// no valid node.
	if (!node || node->x1 == 255 || node->type != MRK_PATH)
	{
		me->mind3 = 0; // reset marker index.
		me->SetSpeed(0);
		return;
	}

	int targetX = (node->x1 * TILE_WIDTH + TILE_WIDTH / 2) * FIXAMT;
	int targetY = (node->y1 * TILE_HEIGHT + TILE_HEIGHT / 2) * FIXAMT;

	if (RangeToPoint(me, targetX, targetY) <= 8 * FIXAMT) // waypoint reached.
	{
		me->mind2 = node->value2;
		me->mind3 = 0;
		me->SetSpeed(0);
		return;
	}

	// move directly towards waypoint
	byte angle = AngleFrom(me->x, me->y, targetX, targetY);
	me->facing = ((angle + 16) / 32) & 7;
	me->SetSpeed(angle, 6);
	me->ForceSequence(ANIM_MOVE, 128); // in case you give this AI to another guy...
}

void AI_Edison(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->ouch == 4)
	{
		OuchKids(me);
		if (me->hp > 0)
			MakeSound(SND_RICHIEOUCH, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->reload)
		me->reload--;

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 4 && !me->reload)
		{
			MakeSound(SND_EDISONCARNAGE, me->x, me->y, SND_CUTOFF, 1200);
			me->reload = 20;
		}
		if (me->seq == ANIM_A1 && me->frm == 4 && !me->reload)
		{
			MakeSound(SND_EDISONCARNAGE, me->x, me->y, SND_CUTOFF, 1200);
			me->reload = 20;
		}
		if (me->seq == ANIM_DIE)
		{

			if (me->frm == 4)
			{
				if (me->aiType == ENT_EDISONBOSS) // turn into edison brain
				{
					me->RegenerateInto(ENT_EDISONBRAIN);
					me->ForceSequence(ANIM_IDLE, 128, me->frm);
					me->reload = 100;
					me->facing = 0;
				}
				else // you're done
				{
					KillKids(me);
				}
				return;
			}
			else
			{
				x = me->x >> FIXSHIFT;
				y = me->y >> FIXSHIFT;
				BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 2);
				BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
					(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
					me->z, FIXAMT);
				ShakeScreen(10);
			}
		}
		return;	// can't do nothin' right now
	}
	if (me->mind == 1) // fire grenades mode
	{
		// lob grenades occasionally
		if (me->reload == 0 && !Random(16))
		{
			me->mind1 = 25;
			me->reload = 80;
		}
		if (me->mind1 > 0)
		{
			me->FaceGuyBroad(goodguy);
			me->mind1--;
			if ((me->mind1 % 10) == 0)
			{
				MakeSound(SND_ARMORSHOOT, me->x, me->y, SND_CUTOFF, 1500);
				FireExactBullet(me->x, me->y + FIXAMT * 20, me->z, FIXAMT * Random(7) - 4, FIXAMT * Random(4) + 3, FIXAMT * 2, 0, 90, 2, BLT_GRENADE, me->friendly);
			}
		}
		if (!Random(128)) // attack 1
		{
			me->ForceSequence(ANIM_ATTACK, 96);
			return;
		}
		if (!Random(128)) // attack 2
		{
			me->seq = ANIM_A1;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 96;
			return;
		}
	}
}

void AI_EdisonBody(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	if (me->parent)
	{
		me->x = me->parent->x;
		me->y = me->parent->y - 8 * FIXAMT;
		me->z = me->parent->z - 96 * FIXAMT;
	}
}

void AI_EdisonTreads(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int i;
	if (me->parent)
	{
		me->parent->x = me->x;
		me->parent->y = me->y + 16 * FIXAMT;
		me->parent->z = me->z + 196 * FIXAMT;
	}

	int x, y;

	if (me->ouch == 4)
	{
		if (!me->parent && me->hp > 0)
			MakeSound(SND_ARMORHIT, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_DIE)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)
	{
		if (RangeToTarget(me, goodguy) < 300 * FIXAMT || me->ouch > 0)
		{
			if(me->parent)
			{
				MakeSound(SND_EDISONFUTURE, me->x, me->y, SND_CUTOFF, 1200);
				me->parent->mind = 1; // wake edison up!!
			}
			me->mind = 1;
		}
	}
	else if (me->mind == 1)
	{
		if (!Random(128) && me->z == 0)
		{
			me->mind = 2;
			me->dz = 12 * FIXAMT;
		}

		me->SetSpeed(3);
		me->FaceGuyBroad(goodguy);
		me->ForceSequence(ANIM_MOVE, 96);
	}
	else if (me->mind == 2)
	{
		if (me->mind1 < 12)
		{
			me->dz += FIXAMT / 4;
			me->mind1++;
		}
		else if (me->mind1 == 12)
		{
			ShakeScreen(120);
			MakeSound(SND_BOMBBOOM, me->x, me->y, SND_CUTOFF, 1200);
			FireBullet(me->x, me->y - FIXAMT, 0, BLT_SHOCKWAVE, me->friendly);
			for (i = 0;i < 10;i++)
			{
				x = me->x + ((256 - Random(513)) << FIXSHIFT);
				y = me->y + ((256 - Random(513)) << FIXSHIFT);
				FireExactBullet(x, y, FIXAMT * 80 + (Random(40) << FIXSHIFT),
					0, 0, 0, 0, 30, 2, BLT_ROCK, me->friendly);
			}
			me->reload = 5;
			me->mind1 = 1;
			me->mind = 1;
		}
	}
}

void AI_EdisonArm(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;

	if (me->reload)
		me->reload--;

	if (me->parent)
	{
		if (me->aiType == ENT_EDISONARMRIGHT) // goes on the left
			me->x = me->parent->x + 64 * FIXAMT;
		else // goes on the right
			me->x = me->parent->x - 64 * FIXAMT;
	}
	me->y = me->parent->y - 8 * FIXAMT;
	me->z = me->parent->z - 64 * FIXAMT;

	if (me->ouch == 4 && !me->parent->ouch)
	{
		if (me->hp > 0)
			MakeSound(SND_ARMORHIT, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_A1 && me->frm == 6 && !me->reload)
		{
			if (me->aiType == ENT_EDISONARMRIGHT)
				i = (Random(16)-9)&255;
			else
				i = (Random(16)-9+128) & 255;
			x = me->x + Cosine(i) * 48;
			y = me->y + Sine(i) * 48;
			FireExactBullet(x, y, FIXAMT * 256, 0, 0, -FIXAMT * 12, 0, 64, 0, BLT_EDISONSTRIKE, me->friendly);
			me->reload = 20;
			MakeSound(SND_BALLLIGHTNING, x, y, SND_CUTOFF | SND_RANDOM, 1200);
		}
		if (me->seq == ANIM_ATTACK && me->frm == 6 && !me->reload)
		{
			if (me->aiType == ENT_EDISONARMRIGHT)
				i = (Random(16) - 9 + 64) & 255;
			else
				i = (Random(16) - 9 + 160) & 255;
			x = me->x + Cosine(i) * 48;
			y = me->y + Sine(i) * 48;
			FireExactBullet(x, y, FIXAMT * 256, 0, 0, -FIXAMT * 12, 0, 64, 0, BLT_EDISONSTRIKE, me->friendly);
			me->reload = 20;
			MakeSound(SND_BALLLIGHTNING, x, y, SND_CUTOFF | SND_RANDOM, 1200);
		}
		if (me->seq == ANIM_DIE)
		{
			x = me->x >> FIXSHIFT;
			y = me->y >> FIXSHIFT;
			BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 1);
		}
		return;	// can't do nothin' right now
	}

	if (me->parent->mind == 0)
		return;

	if (!Random(128))
	{
		me->ForceSequence(ANIM_MOVE, 128);
		return;
	}

	if (!Random(128))
	{
		me->ForceSequence(ANIM_ATTACK, 96);
		return;
	}

	if (!Random(128))
	{
		me->ForceSequence(ANIM_ATTACK, 96);
		return;
	}
}




// unused for now
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

void AI_CrazyBush(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i, j;
	if (me->reload)
		me->reload--;
	if (me->mind2)
		me->mind2--;

	if (me->ouch == 4)
	{
		me->SimpleOuchNoises(SND_BUSHOUCH, SND_BUSHDIE, 1);
	}

	if (me->action == ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if (!goodguy)
		return;

	if (me->GuyWithinRange(goodguy,32))
	{
		goodguy->GetShot(me->dx/2,me->dy/2,1,map,world);
	}

	me->FaceGuy(goodguy);
	me->SetSpeed(5);
	me->ForceSequence(ANIM_MOVE, 128);
}


void AI_PunkBunny(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y, i;
	Guy* bouapha;

	bouapha = GetGuyOfAIType(ENT_BOUAPHA);
	if (!bouapha)
		return;

	if (me->ouch == 4)
	{
		MakeSound(SND_BUNNYDIE, me->x, me->y, SND_CUTOFF, 800);
	}

	if (me->action == ACTION_BUSY)
	{

		if (me->seq == ANIM_A1 && (me->frm == 3 || me->frm == 6 || me->frm == 9) && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 20;
			y = me->y + Sine(me->facing * 32) * 20;

			FireBullet(x, y, (me->facing * 32 - 8 + Random(17)) & 255, BLT_ENERGY, me->friendly);
			MakeSound(SND_BULLETFIRE, x, y, SND_CUTOFF, 100);
			me->reload = 3;
		}
		if (me->seq == ANIM_A2 && me->frm == 2 && me->reload == 0)
		{
			x = me->x + Cosine(me->facing * 32) * 10;
			y = me->y + Sine(me->facing * 32) * 10;

			FireBullet(x, y, me->facing * 32, BLT_BOMB, me->friendly);
			me->reload = 3;
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
		if (RangeToTarget(me, bouapha) < 80 * FIXAMT)
		{
			me->mind = 1;
			MakeNormalSound(SND_FOLLOWHI);
			me->seq = ANIM_ATTACK;
			me->frm = 0;
			me->frmTimer = 0;
			me->action = ACTION_BUSY;
			me->frmAdvance = 128;
			return;
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
		if (goodguy == NULL || goodguy->type == ENT_NONE || RangeToTarget(me, bouapha) > 320 * FIXAMT)	// got too far from papa!!
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

			if (RangeToTarget(me, goodguy) < 280 * FIXAMT && !Random(10))
			{
				me->seq = ANIM_A1;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 256;
				me->dx = 0;
				me->dy = 0;
				me->action = ACTION_BUSY;
				FaceGoodguy(me, goodguy);
				return;
			}
			if (Random(50) == 0)
			{
				me->seq = ANIM_A2;
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

// used for gnome
void AI_Pygmy(Guy* me, Map* map, world_t* world, Guy* goodguy)
{
	int x, y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_PYGMYOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_PYGMYDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm == 6 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32) * 16;
			y = me->y + Sine(me->facing * 32) * 16;
			if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(Cosine(me->facing * 32) * 6, Sine(me->facing * 32) * 6, 8, map, world);
				me->reload = 10;
			}
		}
		return;	// can't do nothin' right now
	}

	if (me->mind == 0)	// not currently aware of goodguy
	{
		me->frmAdvance = 32;	// overcome the auto-128 frmAdvance of guy.cpp

		if (me->seq == ANIM_MOVE)
		{
			me->seq = ANIM_IDLE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 32;
		}
		if (me->mind1 < 101)
			me->mind1++;

		if (me->mind1 > 100 && Random(200) == 0)
		{
			switch (Random(3))
			{
			case 0:
				MakeSound(SND_PYGMYSPIN, me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_A1;	// spin the spear
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
				me->mind1 = 0;
				me->action = ACTION_BUSY;
				break;
			case 1:
				MakeSound(SND_PYGMYYAWN, me->x, me->y, SND_CUTOFF, 1200);
				me->seq = ANIM_A2;	// yawn
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 64;
				me->mind1 = 0;
				me->action = ACTION_BUSY;
				break;
			case 2:
				me->facing = (me->facing + (-1 + Random(3))) & 7;	//change facing
				me->mind1 = 0;
				break;
			}
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

				// do angry animation
				me->action = ACTION_BUSY;
				me->seq = ANIM_A3;
				me->frm = 0;
				me->frmTimer = 0;
				me->frmAdvance = 255;	// very fast
				MakeSound(SND_PYGMYANGRY, me->x, me->y, SND_CUTOFF, 1200);
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
		if (RangeToTarget(me, goodguy) < (72 * FIXAMT) && Random(16) == 0 && me->reload == 0)
		{
			// spear him!
			MakeSound(SND_PYGMYSTAB, me->x, me->y, SND_CUTOFF, 1200);
			me->seq = ANIM_ATTACK;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 255;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 1;
			return;
		}

		if (me->ouch && me->mind1 < 240)
			me->mind1 += 10;
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
