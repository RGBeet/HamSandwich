#include "winpch.h"
#include "monsterai.h"
#include "guy.h"
#include "world.h"
#include "player.h"
#include "editor.h"

void AI_MineCart(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte ok[4];
	int diff,dir;
	static byte noiseLoop=0;

	goodguy=GetGuyOfAIType(MONS_BOUAPHA);
	if(!goodguy)
		return;

	me->facing=me->mind2/32;	// mind2 holds the 'true' visual facing 0-255

	// mind2 is the visual facing, mind3 is the actual facing,
	// so rotate mind2 towards mind3
	if(me->mind3>me->mind2)
	{
		diff=me->mind3-me->mind2;
		if(diff>128)
		{
			dir=-1;
			diff=256-diff;
		}
		else
			dir=1;
	}
	else if(me->mind3<me->mind2)
	{
		diff=me->mind2-me->mind3;
		if(diff>128)
		{
			dir=1;
			diff=256-diff;
		}
		else
			dir=-1;
	}
	else
	{
		diff=0;
		dir=0;
	}

	if(me->mind==0)
	{
		// turn quickly if sitting still
		if(diff>32)
			diff=32;
	}
	else
	{
		if(diff>me->mind1/4)
			diff=me->mind1/4;
	}

	me->mind2=(me->mind2+diff*dir)&255;

	if(me->mind==0)	// waiting to be ridden
	{
		// figure out which directions are valid minecart paths
		if(me->mapx<map->width-1 && GetTerrain(world,map->map[me->mapx+1+me->mapy*map->width].floor)->flags&TF_MINECART)
			ok[0]=1;
		else
			ok[0]=0;
		if(me->mapy<map->height-1 && GetTerrain(world,map->map[me->mapx+(me->mapy+1)*map->width].floor)->flags&TF_MINECART)
			ok[1]=1;
		else
			ok[1]=0;
		if(me->mapx>0 && GetTerrain(world,map->map[me->mapx-1+me->mapy*map->width].floor)->flags&TF_MINECART)
			ok[2]=1;
		else
			ok[2]=0;
		if(me->mapy>0 && GetTerrain(world,map->map[me->mapx+(me->mapy-1)*map->width].floor)->flags&TF_MINECART)
			ok[3]=1;
		else
			ok[3]=0;

		// pick one to face
		for(x=0;x<4;x++)
			if(ok[x])
				me->mind3=x*64;

		if(me->mind1==1)	// wait for goodguy to go away before re-allowing him to board
		{
			if(!me->CheckTargetWithinReach(33))
				me->mind1=0;
		}
		else if(me->CheckTargetWithinReach(32) && player.vehicle == 0)
		{
			MakeSound(SND_MINECART,goodguy->x,goodguy->y,SND_CUTOFF,1200);
			noiseLoop=0;
			me->mind=1;
			player.vehicle=VE_MINECART;
			goodguy->parent=me;
			goodguy->x=me->x;
			goodguy->y=me->y+1;
			goodguy->z=FIXAMT*8;
			me->mind1=0;	// acceleration
		}
	}
	else	// being ridden
	{
		if(!editing && !player.cheated && verified)
			profile.progress.cartDistance+=abs(me->dx/FIXAMT)+abs(me->dy/FIXAMT);
		noiseLoop++;
		if(noiseLoop>=28)
		{
			noiseLoop=0;
			MakeSound(SND_MINECART,goodguy->x,goodguy->y,SND_CUTOFF,1200);
		}
		// if you've reached the center of a tile, time to decide where to go
		x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		if((me->x<=x && me->oldx>x) || (me->x>=x && me->oldx<x) ||
		   (me->y<=y && me->oldy>y) || (me->y>=y && me->oldy<y))
		{
			me->x=x;
			me->y=y;
			// figure out which directions are valid minecart paths
			if(me->mapx<map->width-1 && GetTerrain(world,map->map[me->mapx+1+me->mapy*map->width].floor)->flags&TF_MINECART)
				ok[0]=1;
			else
				ok[0]=0;
			if(me->mapy<map->height-1 && GetTerrain(world,map->map[me->mapx+(me->mapy+1)*map->width].floor)->flags&TF_MINECART)
				ok[1]=1;
			else
				ok[1]=0;
			if(me->mapx>0 && GetTerrain(world,map->map[me->mapx-1+me->mapy*map->width].floor)->flags&TF_MINECART)
				ok[2]=1;
			else
				ok[2]=0;
			if(me->mapy>0 && GetTerrain(world,map->map[me->mapx+(me->mapy-1)*map->width].floor)->flags&TF_MINECART)
				ok[3]=1;
			else
				ok[3]=0;

			ok[(((me->mind3/32)+4)&7)/2]=0;

			// count how many directions are available
			y=0;
			for(x=0;x<4;x++)
				if(ok[x])
					y++;

			if(y==0)	// no directions to go!
			{
				me->mind1=1;
				me->mind=0;
				me->SetNewSpeed(0);
				goodguy->SetNewSpeed(0);
				player.vehicle=0;
				goodguy->parent=NULL;
				goodguy->x=me->x;
				goodguy->y=me->y;
				me->frm=0;
				me->seq=ANIM_IDLE;
				me->frmTimer=0;
				me->frmAdvance=128;
				return;
			}
			else
			{
				if(y==1)
				{
					// face the only valid direction
					for(x=0;x<4;x++)
						if(ok[x])
							me->mind3=x*64;
				}
				else
				{
					// if you can go straight, then that's what you do
					if(ok[me->facing/2])
					{
						me->mind3=(me->facing/2)*64;
					}
					else
					{
						// in the case of multiple directions, we must decide on one
						// based on the player's facing
						x=goodguy->facing/2;
						if(ok[x])	// the direction you're facing is ok
							me->mind3=x*64;
						else if(ok[(x+1)&3])	// direction to your left
							me->mind3=((x+1)&3)*64;
						else if(ok[(x+3)&3])	// direction to your right (same as x-1)
							me->mind3=((x+3)&3)*64;
						else
							me->mind3=((x+2)&3)*64;	// behind you as a last resort
					}
				}
			}
		}

		// move forward at that pace
		me->dx=Cosine(me->mind3)*(me->mind1/8);
		me->dy=Sine(me->mind3)*(me->mind1/8);

		// confine the player to the cart
		goodguy->x=me->x;
		goodguy->y=me->y+1;
		goodguy->z=FIXAMT*8;
		goodguy->SetNewSpeed(0);
		goodguy->dz=0;

		if(me->mind1<96)
			me->mind1++;

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=me->mind1;
	}
}

void AI_Raft(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte tries;

	goodguy=GetGuyOfAIType(MONS_BOUAPHA);
	if(!goodguy)
		return;

	if(me->mind==0)	// waiting to be ridden
	{
		// chill
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=32;
	}
	else	// being ridden
	{
		if(!editing && !player.cheated && verified)
			profile.progress.raftDistance+=abs(me->dx/FIXAMT)+abs(me->dy/FIXAMT);
		// if you've reached the center of a tile, time to decide where to go
		x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		if((me->x<=x && me->oldx>x) || (me->x>=x && me->oldx<x) ||
		   (me->y<=y && me->oldy>y) || (me->y>=y && me->oldy<y) || me->mind1==0)
		{
			tries=0;
			do
			{
				x=me->mapx;
				y=me->mapy;
				switch(me->facing)
				{
					case 0:
						x++;
						break;
					case 1:
						y++;
						break;
					case 2:
						x--;
						break;
					case 3:
						y--;
						break;
				}
				if (mapTile_t *tile = map->TryGetTile(x, y); tile &&
					tile->wall == 0 &&
					!(GetItem(tile->item)->flags & IF_SOLID))
				{
					tries=10;	// we're okay!
				}
				else
				{
					me->x-=me->dx;
					me->y-=me->dy;
					me->facing=(me->facing+2)&3;
					tries++;
				}
			}
			while(tries<3);
			if(tries<10)	// it failed and would've been in an infinite loop
			{
				for(tries=0;tries<4;tries++)
				{
					x=me->mapx;
					y=me->mapy;
					switch(tries)
					{
						case 0:
							x++;
							break;
						case 1:
							y++;
							break;
						case 2:
							x--;
							break;
						case 3:
							y--;
							break;
					}
					if (mapTile_t *tile = map->TryGetTile(x, y); tile &&
						tile->wall==0 &&
						!(GetItem(tile->item)->flags&IF_SOLID) &&
						!(GetTerrain(world,tile->floor)->flags&(TF_SOLID|TF_WATER|TF_LAVA)))
					{
						me->facing=tries;
						me->x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
						me->y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
						me->mind1=0;
						me->mind=0;
						goodguy->dx=0;
						goodguy->dy=0;
						me->dx=0;
						me->dy=0;
						goodguy->x=me->x+Cosine(me->facing*64)*TILE_WIDTH;
						goodguy->y=me->y+Sine(me->facing*64)*TILE_HEIGHT;
						player.vehicle=0;
						return;
					}
				}
				if(tries==4)	// there was no spot to put the player
				{
					NewMessage("Your raft broke!",60,0);
					me->type=MONS_NONE;
					player.vehicle=0;
					return;
				}

			}
			// otherwise if it's not water, we're done
			if(!(GetTerrain(world,map->GetTile(x, y)->floor)->flags&(TF_WATER|TF_LAVA)))
			{
				me->x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				me->y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
				me->mind1=0;
				me->mind=0;
				goodguy->dx=0;
				goodguy->dy=0;
				me->dx=0;
				me->dy=0;
				goodguy->x=me->x+Cosine(me->facing*64)*TILE_WIDTH;
				goodguy->y=me->y+Sine(me->facing*64)*TILE_HEIGHT;
				player.vehicle=0;
				return;
			}
		}
		// move forward at the right speed otherwise
		me->dx=Cosine(me->facing*64)*(me->mind1/4);
		me->dy=Sine(me->facing*64)*(me->mind1/4);

		// confine the player to the raft
		goodguy->x=me->x+me->dx;
		goodguy->y=me->y+me->dy+1;
		goodguy->z=FIXAMT*8;
		goodguy->dx=0;
		goodguy->dy=0;
		goodguy->dz=0;

		if(me->mind1<16)
			me->mind1++;

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=32;
	}
}

void AI_Vampire(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->aiType==MONS_DARKVAMP)
	{
		// get burned by the light!
		if(map->GetTile(me->mapx,me->mapy)->light>0 && me->ouch==0 && me->hp>0)
		{
			me->GetShot(0,0,map->GetTile(me->mapx,me->mapy)->light,map,world, /* bypassInvincible */ true);
			BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
			BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
			BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
			BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
			BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
		}
	}

	if(me->ouch==4)
		me->HandleOuchNoises(SND_VAMPOUCH,SND_VAMPDIE);

	if(me->action==ACTION_BUSY)
	{
		if(goodguy && me->CheckSequenceFrame(ANIM_ATTACK,3))
		{
			if(me->AttackCheck(20, me->GetSpaceInFrontX(16) >> FIXSHIFT, me->GetSpaceInFrontY(16) >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,12,map,world);
				if(me->aiType==MONS_DARKVAMP)
					SetPoisonFrames(goodguy, goodguy->poison+20);
			}
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly drift towards Bouapha
	{
		if(player.garlic && goodguy && me->CheckTargetWithinReach(500) && GuyCheckTargetWithinSight(me,goodguy,map))
		{
			MakeSound(SND_VAMPFEAR,me->x,me->y,SND_CUTOFF,1200);
			me->mind=2;	// enter fear mode
			return;
		}

		if(goodguy)
		{
			if(me->CheckRoll(8,ROLL_ATTACK) && me->CheckTargetWithinReach(72) && GuyCheckTargetWithinSight(me,goodguy,map))
			{
				me->StartNewAnimation(ANIM_ATTACK,320,ACTION_BUSY,0,0,0,SND_VAMPATTK);
				return;
			}
			FaceGoodguy(me,goodguy);

			me->SetNewSpeed(9);
			me->StartAnimMove(64);
			if(me->mind1 && me->CheckTargetWithinReach(64) && GuyCheckTargetWithinSight(me,goodguy,map)) // bump a wall
			{
				me->mind=1;	// bumped a wall, so randomly maneuver
				me->facing=(byte)Random(8);
				me->PickRandomDirection(&me->mind2,10);
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->PickRandomDirection();
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind2--))	// time to get a new direction
			me->mind=0;	// get back on track
		me->SetNewSpeed(6);
		me->StartAnimMove(64);
	}
	else
	{
		if(player.garlic && goodguy && me->CheckTargetWithinReach(52) && GuyCheckTargetWithinSight(me, goodguy, map)) // runs away
		{
			// face away from the target! they STINK!!
			FaceGoodguy(me,goodguy);
			me->FlipFacing();

			me->SetNewSpeed(8);
			me->StartAnimMove(64);
		}
		else
			me->mind=0;
		return;
	}
	me->mind1=0;
}

void AI_Coffin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->ouch==4)
		me->HandleOuchNoises(SND_VAMPOUCH,me->mind?0:SND_VAMPDIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_ATTACK,15) && me->frmTimer>64)
		{
			AddBaby(me->x+FIXAMT*20,me->y+FIXAMT*10,0,(me->aiType==MONS_COFFIN) ? MONS_VAMPIRE : MONS_DARKVAMP,me);
			me->SelfDestruct(map,world); // DIE!
			me->mind=1;
			me->ouch=0;
		}
		return;	// can't do nothin' right now
	}

	if ((goodguy && me->CheckTargetWithinReach(180) && GuyCheckTargetWithinSight(me, goodguy, map)) || me->ouch) // open the coffin
		me->StartNewAnimation(ANIM_ATTACK, 127, ACTION_BUSY, 0, 0, 0, SND_COFFIN);
}

void AI_Ghost(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		me->HandleOuchNoises(SND_GHOSTOUCH,SND_GHOSTDIE);
		if(me->action==ACTION_IDLE)
		{
			me->StartNewAnimation(ANIM_A1, 128, ACTION_BUSY, 0, 0);
			return;
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_ATTACK,2))
		{
			me->reload=10;
			if(goodguy && me->CheckTargetWithinReach(120))
				goodguy->GetShot(0,0,6,map,world);
		}
		if(me->CheckSequenceFrame(ANIM_A1,2) && me->frmTimer>64)
		{
			// teleport
			MakeSound(SND_GHOSTTPORT,me->x,me->y,SND_CUTOFF,1200);
			me->x+=-FIXAMT*320+Random(FIXAMT*640);
			me->y+=-FIXAMT*240+Random(FIXAMT*480);
			if(me->x<TILE_WIDTH*2*FIXAMT)
				me->x=TILE_WIDTH*2*FIXAMT;
			else if(me->x>(map->width-2)*TILE_WIDTH*FIXAMT)
				me->x=(map->width-2)*TILE_WIDTH*FIXAMT;
			if(me->y<TILE_HEIGHT*2*FIXAMT)
				me->y=TILE_HEIGHT*2*FIXAMT;
			else if(me->y>(map->height-2)*TILE_HEIGHT*FIXAMT)
				me->y=(map->height-2)*TILE_HEIGHT*FIXAMT;
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0 && me->hp>0)	// hasn't spotted Bouapha yet
	{
		if((goodguy && me->CheckTargetWithinReach(300)) || (me->ouch>0))
			me->StartNewAnimation(ANIM_A3, 64, ACTION_BUSY, 0, 0, 0, SND_GHOSTTPORT);
		else if(me->seq!=ANIM_A2)
			me->StartNewAnimation(ANIM_A2, 1);
		else if(me->frmTimer>20)
			me->frmTimer=0;	// keep him on that frame
		FaceGoodguy3(me,goodguy);
		return;
	}
	else if(me->mind==1)	// has spotted him, give chase
	{
		if(me->CheckRoll(500,ROLL_MOVE))
		{
			me->StartNewAnimation(ANIM_A1, 128, ACTION_BUSY, 0, 0);
			return;
		}
		if(goodguy)
		{
			FaceGoodguy3(me,goodguy);
			me->SetNewSpeed(3);
			if(me->CheckRoll(20,ROLL_ATTACK) && me->CheckTargetWithinReach(140))
			{
				me->StartNewAnimation(ANIM_ATTACK, 72, ACTION_BUSY, 0, 0, 0, SND_GHOSTYELL);
				return;
			}
		}
	}
}

void AI_Burner(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->mind==0)
	{
		if(me->reload)
			me->reload--;
		else
		{
			me->mind=1;
			me->mind1=10;
		}
	}
	else
	{
		if(me->mind1)
		{
			me->mind1--;
			switch(me->mind2)
			{
				case 0:
					if(me->mapx<map->width-1 && !map->map[me->mapx+1+me->mapy*map->width].wall)
						FireBullet(me->x+FIXAMT*TILE_WIDTH/2,me->y,0,BLT_FLAME2,me->friendly);
					break;
				case 1:
					if(me->mapy<map->height-1 && !map->map[me->mapx+0+(me->mapy+1)*map->width].wall)
						FireBullet(me->x,me->y+FIXAMT*TILE_HEIGHT/2,2,BLT_FLAME2,me->friendly);
					break;
				case 2:
					if(me->mapx>0 && !map->map[me->mapx-1+(me->mapy+0)*map->width].wall)
						FireBullet(me->x-FIXAMT*TILE_WIDTH/2,me->y,4,BLT_FLAME2,me->friendly);
					break;
				case 3:
					if(me->mapy>0 && !map->map[me->mapx+(me->mapy-1)*map->width].wall)
						FireBullet(me->x,me->y-FIXAMT*TILE_HEIGHT/2,6,BLT_FLAME2,me->friendly);
					break;
			}
		}
		else
		{
			me->mind2++;
			if(me->mind2>3)
				me->mind2=0;
			me->mind=0;
			me->reload=15;
		}
	}
}

void AI_Lefty(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_LEFTYOUCH,SND_LEFTYDIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
			me->frmAdvance=72;
		if(goodguy && me->CheckSequenceFrame(ANIM_ATTACK,5))
		{
			if(me->AttackCheck(16,me->GetSpaceInFrontX(16)>>FIXSHIFT,me->GetSpaceInFrontY(16)>>FIXSHIFT,goodguy))
				goodguy->GetShot(me->GetSpaceInFrontX(4),me->GetSpaceInFrontY(4),4,map,world);
			me->reload=10;
		}
		return;	// can't do nothin' right now
	}

	// randomly decide to wave at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	if(!me->mind && me->CheckRoll(100,ROLL_ATTACK))
	{
		me->StartNewAnimation(ANIM_A1, 128, ACTION_BUSY, 0, 0);
		return;
	}

	if(me->mind==0)		// when mind=0, singlemindedly skitter towards Bouapha
	{
		if(goodguy)
		{
			if(me->CheckRoll(48,ROLL_ATTACK) && me->CheckTargetWithinReach(48))
			{
				me->StartNewAnimation(ANIM_ATTACK,140,ACTION_BUSY,0,0,0,SND_SKELKICK);
				return;
			}
			FaceGoodguy(me,goodguy);
			me->SetNewSpeed(6);
			me->StartAnimMove(200);

			if (me->CheckRoll(64,ROLL_MOVE) == 0)
				me->WalkAround();
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(6);
		me->StartAnimMove(128);
	}
}

void AI_Pygmy2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_PYGMYOUCH, SND_PYGMYDIE);

	if(me->action==ACTION_BUSY)
	{
		if(goodguy && me->CheckSequenceFrame(ANIM_ATTACK,4))
			me->DoFireBullet(BLT_BADSPEAR,10,8,SND_THROWSMTH);
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
		FaceGoodguy(me,goodguy);
		if(!goodguy)
		{
			me->mind=0;	// go back to being bored, nothing to see here
			me->mind1=0;
			return;
		}
		if (me->CheckTargetWithinReach(72) && me->CheckRoll(16,ROLL_ATTACK) && me->reload == 0)
		{
			me->StartNewAnimation(ANIM_ATTACK,255,ACTION_BUSY,0,0,1,SND_PYGMYSTAB); // spear him
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if (!me->CheckTargetWithinReach(256))
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		me->SetNewSpeed(3);
		me->StartAnimMove(128);
	}
}

void AI_Pygmy3(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SHAMANOUCH, SND_SHAMANDIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_ATTACK,2))
		{
			x=me->GetSpaceInFrontX(8);
			y=me->GetSpaceInFrontY(8);
			// poison zap
			if(me->CheckTargetWithinReach(200))
			{
				LightningBolt(x,y-FIXAMT*20,goodguy->x,goodguy->y-FIXAMT*20);
				LightningBolt(x,y-FIXAMT*20,goodguy->x,goodguy->y-FIXAMT*20);
				LightningBolt(x,y-FIXAMT*20,goodguy->x,goodguy->y-FIXAMT*20);
				goodguy->GetShot(0,0,1,map,world);
				SetPoisonFrames(goodguy, goodguy->poison+30);
			}
			me->reload=10;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		me->frmAdvance=32;	// overcome the auto-128 frmAdvance of guy.cpp

		if(me->seq==ANIM_MOVE)
			me->StartAnimIdle(32);
		if(me->mind1<101)
			me->mind1++;

		if(me->mind1>100 && me->CheckRoll(200,ROLL_MOVE))
		{
			if(Random(2)==0)
				me->StartNewAnimation(ANIM_A1, 64, ACTION_BUSY, 0, 0, 0, SND_SHAMANDANCE);
			else
				me->facing=(me->facing+(-1+Random(3)))&7;	//change facing
			me->mind1=0;
		}
		if (goodguy)
		{
			if ((me->CheckTargetWithinReach(300) && GuyCheckTargetWithinSight(me,goodguy,map)||me->ouch))
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
		FaceGoodguy(me,goodguy);
		if(!goodguy)
		{
			me->mind=0;	// go back to being bored, nothing to see here
			me->mind1=0;
			return;
		}
		if(!me->reload && me->CheckRoll(100,ROLL_ATTACK) && me->CheckTargetWithinReach(200))
		{
			me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,1,SND_SHAMANZAP); // zap
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(!me->mind1--)
		{
			me->mind=me->CheckTargetWithinReach(256) ? 20 : 0;	// get bored again
			return;
		}
		me->SetNewSpeed(3);
		me->StartAnimMove(128);
	}
}

void AI_Pumpkinstein(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ZOMBIEOUCH,SND_ZOMBIEDIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm>4)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);

		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		// walking
		if(goodguy && me->CheckTargetWithinReach(64))
		{
			goodguy->GetShot(-4*FIXAMT+Random(8*FIXAMT),-4*FIXAMT+Random(8*FIXAMT),10,map,world);
			me->mind1=1;
		}
		if(me->mind1)
		{
			// bumped a wall
			MakeSound(SND_PKSTEINBUMP,me->x,me->y,SND_CUTOFF,1200);
			// pick a new direction
			if(Random(2)==0)
			{
				me->mind2=(me->facing+4+Random(3))&7;
				me->mind3=0;
			}
			else
			{
				me->mind2=(me->facing+4-Random(3))&7;
				me->mind3=2;
			}
			me->mind1=0;
			me->SetNewSpeed(0);
			me->mind=1;	// switch to turning mode
			me->reload=2;
			me->StartAnimIdle(128);
		}
		else
		{
			if(me->dx==0 && me->dy==0)
				me->SetNewSpeed(Random(6)+4);
			me->StartAnimMove();
		}
	}
	else
	{
		// turning
		if(!me->reload)
		{
			me->facing=(me->facing+me->mind3-1)&7;
			if(me->facing==me->mind2)	// done turning
				me->mind=0;
			me->reload=2;
		}
	}
}

void AI_Knight(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		me->HandleOuchNoises(SND_GLASSHIT,SND_GLASSDIE);
		if(me->hp>0 && me->action!=ACTION_BUSY && Random(2)==0)
		{
			me->StartNewAnimation(ANIM_A2,128);
			me->action=ACTION_BUSY;
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_ATTACK,5))
		{
			// hack at him with axe
			x=me->x-FIXAMT*20;
			y=me->y+FIXAMT*60;
			FindVictim(x>>FIXSHIFT,y>>FIXSHIFT,60,0,FIXAMT*8,20,map,world,me->friendly);
			me->reload=10;

		}
		if(me->CheckSequenceFrame(ANIM_A1,13))
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			GlassShatter(x-40,y-100,x+40,y+20,me->z,50);
			MakeSound(SND_GLASSBREAK,me->x,me->y,SND_CUTOFF,1200);
			me->reload=3;
		}
		if(me->CheckSequenceFrame(ANIM_DIE,12))
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			GlassShatter(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,100);
			MakeSound(SND_GLASSBREAK,me->x,me->y,SND_CUTOFF,1200);
			me->reload=3;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		// unaware, sitting in glass
		me->StartNewAnimation(ANIM_A1,64);

		if ((me->CheckTargetWithinReach(160) && GuyCheckTargetWithinSight(me,goodguy,map)||me->ouch))
		{
			me->mind=1;
			me->action=ACTION_BUSY;	// continue the arrival animation
		}
	}
	else
	{
		if(goodguy)
		{
			me->dx=0;
			me->dy=0;
			if(goodguy->y<me->y+FIXAMT*30)
				me->dy=-FIXAMT*6;
			else if(goodguy->y>me->y+FIXAMT*128)
				me->dy=FIXAMT*4;
			if(goodguy->x<me->x-FIXAMT*40)
				me->dx=-FIXAMT*4;
			if(goodguy->x>me->x+FIXAMT*40)
				me->dx=FIXAMT*4;

			if(me->dx==0 && me->dy==0)
			{
				if(RangeToTarget(me,goodguy)>72*FIXAMT)
				{
					if(goodguy->x>me->x+FIXAMT*2)
						me->dx=FIXAMT*2;
					else if(goodguy->x<me->x-FIXAMT*2)
						me->dx=-FIXAMT*2;
					if(goodguy->y>me->y+FIXAMT*2)
						me->dy=FIXAMT*2;
					else if(goodguy->y<me->y-FIXAMT*2)
						me->dy=-FIXAMT*2;
				}
			}
			if(me->CheckTargetWithinReach(120) && goodguy->y>me->y && me->CheckRoll(10,ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_SKELKICK);
				return;
			}
		}
		if(me->dx!=0 || me->dy!=0)
			me->StartAnimMove(128);
		else
			me->StartAnimIdle(128);
	}
}

void AI_Triceroid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_TRICEROUCH,SND_TRICERDIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm>5)
			me->Speen();
		return;	// can't do nothin' right now
	}

	x=me->maxHP-me->hp+5;	// more damaged=faster

	if(me->mind==0)
	{
		// walking
		if(me->CheckTargetWithinReach(64))
		{
			goodguy->GetShot(-4*FIXAMT+Random(8*FIXAMT),-4*FIXAMT+Random(8*FIXAMT),10,map,world);
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,2,SND_TRICERAROAR);
			me->mind=1;	// switch to turning mode
			// pick a new direction
			if(Random(2)==0)
			{
				me->mind2=(me->facing+4+Random(3))&7;
				me->mind3=0;
			}
			else
			{
				me->mind2=(me->facing+4-Random(3))&7;
				me->mind3=2;
			}
			return;
		}
		if(me->mind1)
		{
			// bumped a wall
			MakeSound(SND_TRICERBUMP,me->x,me->y,SND_CUTOFF,1200);
			// pick a new direction
			if(Random(2)==0)
			{
				me->mind2=(me->facing+4+Random(3))&7;
				me->mind3=0;
			}
			else
			{
				me->mind2=(me->facing+4-Random(3))&7;
				me->mind3=2;
			}
			me->mind1=0;
			me->mind=1;	// switch to turning mode
			me->reload=2;
			me->SetNewSpeed(0);
			me->StartAnimIdle(128);
		}
		else
		{
			if((me->dx==0 && me->dy==0) || (me->ouch))
				me->SetNewSpeed(x/5);
			me->StartAnimMove(x/5*30+40);
		}
	}
	else
	{
		// turning
		if(!me->reload)
		{
			me->facing=(me->facing+me->mind3-1)&7;
			if(me->facing==me->mind2)	// done turning
			{
				me->mind=0;
			}
			me->reload=2;
		}
	}
}

void AI_Countess(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	int x,y,i;
	int xpos[]={-FIXAMT*50,0,FIXAMT*50};
	int ypos[]={0,FIXAMT*50,0};

	if(me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_COUNTOUCH,SND_COUNTDIE);
	{
		if(me->hp>0)
			MakeSound(SND_COUNTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_COUNTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrames(ANIM_ATTACK,5,9))
		{
			if(g=me->TryAddBaby(map,world,MONS_BAT,40,20,2))
				g->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,g->GetSpaceInFrontX(12)-FIXAMT*2+Random(FIXAMT*4),g->GetSpaceInFrontY(12),0,SND_BATDIVE);
		}
		if(me->CheckSequenceFrame(ANIM_A1,2))
		{
			me->reload=5;
			for(i=0;i<3;i++)
			{
				if(g=me->TryAddBaby(map,world,MONS_BAT,40,20,2))
					g->StartNewAnimation(ANIM_A3,128,ACTION_BUSY,0,0);
			}
		}

		if(me->seq==ANIM_DIE && me->frm>3)
		{
			me->frmAdvance=128;
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z-Random(FIXAMT*60),FIXAMT);
		}
		if(me->CheckSequenceFrame(ANIM_A2,2)) // do the DASH!!
			me->SetNewSpeed(14);

		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;

	if(!me->mind)	// waiting for Bouapha
	{
		if ((me->CheckTargetWithinReach(160) && GuyCheckTargetWithinSight(me,goodguy,map)||me->ouch))
		{
			MakeSound(SND_COUNTHELLO,me->x,me->y,SND_CUTOFF,1200);
			me->mind=1;
		}
		me->mind1=60;
	}
	if(me->mind==1)	// wandering mode
	{
		// calculate desired location (want to be above Bouapha)
		x=goodguy->x;
		y=goodguy->y-96*FIXAMT;

		FaceGoodguy3(me,goodguy);
		me->HoneInOnPoint(goodguy->x,goodguy->y-96*FIXAMT);
		me->ClampSpeed(3,4);

		if(me->CheckTargetWithinReach(400) && me->CheckRoll(10,ROLL_ATTACK))
		{
			FaceGoodguy(me, goodguy);
			me->StartNewAnimation(ANIM_A2,256,ACTION_BUSY,0,0,0,SND_COUNTCHARGE);
			me->mind=2;
			me->mind1=10+Random(10)+1;
			me->facing=((me->facing*32)-16+Random(33));
		}
		if(me->mind1)
			me->mind1--;
		else
		{
			x=Random(4);
			switch(x)
			{
				case 0:	// shoot bats
					if(me->y>goodguy->y && me->CheckTargetWithinReach(100))
						me->mind1=10;	// don't fire bats if below player
					else
					{
						me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_COUNTSHOOT);
						me->mind1=60;
					}
					break;
				case 1:	// summon boneheads
					if(me->y<goodguy->y && me->CheckTargetWithinReach(160))
					{
						MakeSound(SND_COUNTSUMMON,me->x,me->y,SND_CUTOFF,1200);
						me->action=ACTION_BUSY;
						me->seq=ANIM_A1;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=128;
						me->mind1=60;
						me->dx=0;
						me->dy=0;
					}
					else
						me->mind1=10;	// don't summon if close, unless under goodguy
					break;
				case 2: // hyper-dash
					if (GuyCheckTargetWithinSight(me, goodguy, map))
					{
						FaceGoodguy(me, goodguy);
						me->facing = ((me->facing * 32) - 16 + Random(33));
						me->StartNewAnimation(ANIM_A2,256,ACTION_BUSY,0,0,0,SND_COUNTCHARGE);
						me->mind=2;
						me->mind1=10+Random(10);
					}
					break;
				case 3:
					me->mind1=10;
					break;
			}
		}
	}
	else if(me->mind==2)
	{
		if(me->CheckTargetWithinReach(72))
			goodguy->GetShot(0,0,4,map,world);
		CountessGlow(me->x,me->y);
		if(!me->mind1--)
		{
			me->mind=1;
			me->mind1=10;
		}
	}
}

void AI_AlienEgg(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;

	if(me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		me->HandleOuchNoises(SND_EGGOUCH, SND_EGGDIE);
		me->mind1=1;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_ATTACK,10))
		{
			if(g = me->TryAddBaby(map,world,MONS_BABYALIEN,0,20,me->aiType==MONS_ALIENEGG2 ? 10 : 90))
			{
				g->PickRandomDirection();
				g->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,me->GetSpaceInFrontX(12),me->GetSpaceInFrontY(12));
				g->frm=2; // lil quick
				g->dz=3*FIXAMT; // lil jump
			}
		}
		if(me->seq==ANIM_DIE)
		{
			FireBullet(me->x,me->y,(byte)Random(256),BLT_ACID,me->friendly);
			ExplodeParticles2(PART_SLIME,me->x,me->y,FIXAMT*20,4,10);
		}
		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;

	if(GuyCheckTargetWithinReachAndSight(me,goodguy,map,300)|| me->mind1)
	{
		if(!me->reload && me->CheckRoll(150,ROLL_ATTACK) || me->aiType == MONS_ALIENEGG2)
			me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,0,SND_EGGOPEN);
	}
}

void AI_BabyAlien(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_SPD2OUCH,SND_SPD2DIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>2)
		{
			me->SetNewSpeed(14);
			if(me->CheckTargetWithinReach(44) && me->mind2)
			{
				goodguy->GetShot(me->dx,me->dy,4,map,world);
				me->mind2=0;	// don't allow multiple hits
			}
		}
		if(me->seq==ANIM_DIE)
		{
			ExplodeParticles2(PART_SLIME,me->x,me->y,FIXAMT*20,4,10);
		}
		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;

	if(me->mind1)
	{
		me->mind1--;
		me->StartAnimMove(128);
		if(GuyCheckTargetWithinReachAndSight(me,goodguy,map,160) && me->CheckRoll(160,ROLL_ATTACK))
		{
			me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_SPD2SPIT);
			FaceGoodguy(me, goodguy);
			me->mind1=0;
			me->mind2=1;
		}
	}
	else
	{
		me->PickRandomDirection(&me->mind1,10+Random(20));
		me->SetNewSpeed(10);
	}
}

void AI_Alien(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ALIENOUCH,SND_ALIENDIE);


	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_ATTACK,2) && goodguy)
		{
			x=me->GetSpaceInFrontX(16);
			y=me->GetSpaceInFrontY(16);
			if(me->AttackCheck(20,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,8,map,world);
			me->reload=5;
		}
		if(me->CheckSequenceFrame(ANIM_A1,5) && goodguy)
		{
			x=me->GetSpaceInFrontX(20);
			y=me->GetSpaceInFrontY(20);
			SpitAcid(x,y,me->facing*32,BLT_ACID,me->friendly);	// same as FireBullet, just does it up a little (z axis)
													// because alien is taller than other acid-spitters
			me->reload=5;
			me->mind1=1;
		}
		if(me->seq==ANIM_DIE && me->frm==5)
		{
			for(x=0;x<8;x++)
				FireBullet(me->x,me->y,(byte)Random(256),BLT_ACID,me->friendly);
			ExplodeParticles2(PART_SLIME,me->x,me->y,FIXAMT*20,10,10);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(me->CheckTargetWithinReach(78) && me->CheckRoll(8,ROLL_ATTACK))
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
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
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
		me->SetNewSpeed(6);
		me->StartAnimMove(128);
	}
}

void AI_Robopumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ROBOOUCH,SND_ROBODIE);

	if(me->action==ACTION_BUSY)
	{
		if(goodguy && me->CheckSequenceFrame(ANIM_ATTACK,2))
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(20,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,20,map,world);
			me->reload=5;
		}
		if(goodguy && me->CheckSequenceFrame(ANIM_A1,me->frm))
		{
			x=me->x+Cosine(me->facing*32)*40;
			x+=Cosine(((me->facing+2)&7)*32)*20;
			y=me->y+Sine(me->facing*32)*40;
			y+=Sine(((me->facing+2)&7)*32)*20;
			FireBulletZ(x,y,FIXAMT*50,me->facing*32-16+Random(33),BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,950);
			me->reload=1;
			me->mind1=1;
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(GuyCheckTargetWithinReachAndSight(me,goodguy,map,96) && me->CheckRoll(8,ROLL_ATTACK))
			{
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0); // get him!!!
				return;
			}
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0)
			{
				me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,0,0,0); // shoot at him
				FaceGoodguy(me,goodguy);
				return;
			}
			FaceGoodguy2(me,goodguy);
			me->SetNewSpeed(4);
			if (me->CheckRoll(64,ROLL_RANDWALK))
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0)
			{
				me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,0,0,0); // shoot at him!
				FaceGoodguy(me,goodguy);
				return;
			}
		}
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(4);
	}
}

void AI_ShockTrooper(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ROBOOUCH, SND_ROBODIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=3)
		{
			if(me->frm<7)
			{
				me->SetNewSpeed(12);
				if(me->mind1==0)
				{
					FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,100,Cosine(me->facing*32)*12,Sine(me->facing*32)*12,20,map,world,me->friendly);
					me->mind1=1;
				}
			}
			else
				me->DampenSpeed(2);
		}
		if(me->seq==ANIM_A1 && (me->frm==3 || me->frm==4) && me->reload==0 && goodguy)
		{
			MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(((me->facing+2)&7)*32)*48;
			y=me->y+Sine(((me->facing+2)&7)*32)*32;
			FireBulletZ(x,y,FIXAMT*100,me->facing*32-16+Random(33),BLT_GRENADE,me->friendly);
			if(me->frm==4)
				me->reload=30;
		}
		if(me->seq==ANIM_DIE && me->frm>6)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;
	if(GuyCheckTargetWithinReachAndSight(me,goodguy,map,300) && me->CheckRoll(20,ROLL_ATTACK))
	{
		// get him!
		me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_INVIDCHARGE); // charge him
		MakeSound(SND_INVIDCHARGE,me->x,me->y,SND_CUTOFF,1200);
		me->mind1=0;
		return;
	}
	if(!me->reload && GuyCheckTargetWithinReachAndSight(me,goodguy,map,512) && me->CheckRoll(32,ROLL_ATTACK))
	{
		me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,0,0,0); // shoot at him
		return;
	}
	FaceGoodguy2(me,goodguy);
	me->SetNewSpeed(4);
	me->StartAnimMove(128);
}

void AI_Minibot(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ROBOOUCH,SND_ROBODIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_DIE,4,false))
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly roll towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy3(me,goodguy);
			if(!me->reload && me->CheckRoll(32, ROLL_ATTACK) && GuyCheckTargetWithinReachAndSight(me,goodguy,map,512))
			{
				me->DoFireBullet(BLT_ENERGY,10,8,SND_ROBOSHOOT); // shoot at him
				return;
			}
			me->SetNewSpeed(5);
			me->StartAnimMove(128);
			if (me->CheckRoll(48,ROLL_RANDWALK)) // wanders a little more than others!
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
		{
			if(!me->reload && me->CheckRoll(256, ROLL_ATTACK) && me->CheckTargetWithinReach(512))
			{
				me->DoFireBullet(BLT_ENERGY, 10, 8, SND_ROBOSHOOT); // shoot at him
				return;
			}
		}
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(5);
		me->StartAnimMove(128);
	}
}

void AI_Meaniebot(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ROBOOUCH,SND_ROBODIE);


	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrames(ANIM_ATTACK,2,4))
		{
			MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(((me->facing+2)&7)*32)*24;
			y=me->y+Sine(((me->facing+2)&7)*32)*20;
			FireBullet(x,y,me->facing*32-16+Random(33),BLT_ENERGY,me->friendly);
			x=me->x+Cosine(((me->facing-2)&7)*32)*24;
			y=me->y+Sine(((me->facing-2)&7)*32)*20;
			FireBullet(x,y,me->facing*32-16+Random(33),BLT_ENERGY,me->friendly);
			me->reload=2;
		}
		if(me->seq==ANIM_DIE && me->frm>3 && me->frmTimer<63)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			if(me->frm==5 && me->frmTimer<63)
			{
				if(me->aiType==MONS_GOODROBOT2)
				{
					g=AddBaby(me->x-FIXAMT*20,me->y,0,MONS_GOODROBOT,me);
					if(g && (!g->CanWalk(g->x,g->y,map,world)))
						RemoveGuy(g);
					g=AddBaby(me->x+FIXAMT*20,me->y,0,MONS_GOODROBOT,me);
					if(g && (!g->CanWalk(g->x,g->y,map,world)))
						RemoveGuy(g);
				}
				else
				{
					g=AddBaby(me->x-FIXAMT*20,me->y,0,MONS_ROBOT1,me);
					if(g && (!g->CanWalk(g->x,g->y,map,world)))
						RemoveGuy(g);
					g=AddBaby(me->x+FIXAMT*20,me->y,0,MONS_ROBOT1,me);
					if(g && (!g->CanWalk(g->x,g->y,map,world)))
						RemoveGuy(g);
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly roll towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy3(me,goodguy);
			if(!me->reload && me->CheckRoll(32, ROLL_ATTACK) && GuyCheckTargetWithinReachAndSight(me,goodguy,map,512))
			{
				me->DoFireBullet(BLT_ENERGY,10,8,SND_ROBOSHOOT); // shoot at him
				return;
			}
			me->SetNewSpeed(5);
			me->StartAnimMove(128);
			if (me->CheckRoll(48,ROLL_RANDWALK)) // wanders a little more than others!
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy)
		{
			if(!me->reload && me->CheckRoll(256, ROLL_ATTACK) && me->CheckTargetWithinReach(512))
			{
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0); // shoot at him
				FaceGoodguy(me,goodguy);
				return;
			}
		}
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(3);
		me->StartAnimMove(128);
	}
}

void AI_Robofactory(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_ROBOOUCH, SND_ROBODIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckFrame(ANIM_ATTACK,15) && me->frmTimer>128)
			me->StartNewAnimation(ANIM_A1,me->frmAdvance); // the animation was too long to fit on one sequence
		if(me->CheckFrame(ANIM_A1,10))
		{
			me->TryAddBaby(map, world, MONS_ROBOT1, me->x, me->y + FIXAMT*2, 200);
			me->reload=30;
			me->mind1=8;
		}
		if(me->seq==ANIM_DIE && me->frmTimer<63)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if(GuyCheckTargetWithinReachAndSight(me,goodguy,map,800))
		{
			me->mind=1;
			me->mind1=Random(15)+1;
			me->reload=30;
		}
	}
	else
	{
		if(!me->reload)
		{
			if(!me->mind1--)
				me->StartNewAnimation(ANIM_ATTACK,128,ACTION_BUSY,0,0,0,SND_FACTORYGO); // get him!
			else
				me->reload=30;
		}
	}
}

void AI_Turret(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,a;

	if(me->reload)
		me->reload--;

	if(me->ouch == 4)
		me->HandleOuchNoises(SND_ROBOOUCH, SND_ROBODIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frmTimer<63)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if(me->CheckTargetWithinReach(800))
		{
			me->mind=1;
			if(me->aiType!=MONS_DEATHTURRET)
				me->reload=(byte)Random(60);
			else
				me->reload=5;
		}
	}
	else
	{
		if(me->reload==0)
		{
			x=me->GetSpaceInFrontX(12);
			y=me->GetSpaceInFrontY(12);
			switch(me->aiType)
			{
				case MONS_TURRET:
					MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
					FireExactBullet(x,y,FIXAMT*4,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,0,0,30,me->facing*32,BLT_ENERGY,me->friendly);
					me->reload=30;
					break;
				case MONS_MISLTURRET:
					FireBullet(x,y,me->facing,BLT_MISSILE,me->friendly);
					me->reload=60;
					break;
				case MONS_DEATHTURRET:
					if(GuyCheckTargetWithinReachAndSight(me,goodguy,map,400))
					{
						MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
						a=(me->facing*32-16+Random(33))&255;
						FireExactBullet(x,y,FIXAMT*4,Cosine(a)*4,Sine(a)*4,0,0,60,a/16,BLT_MISSILE,me->friendly);
						me->reload=5;
					}
					break;
			}
		}
		FaceGoodguy3(me,goodguy);
	}
}

// random bullet choices to fire (12 choices)
byte bunnyBulletTypes[] = { BLT_ENERGY,BLT_FLAME2,BLT_SPORE,BLT_GRENADE,BLT_SHOCKWAVE,BLT_SNOWBALL,
	BLT_BIGSNOW,BLT_ICESPIKE,BLT_ROCK,BLT_SPINE,BLT_EVILHAMMER,BLT_BADSPEAR };
// whether each of those takes 256 facings or 8
byte bunnyBulletFacing[] = { 1,0,1,1,0,1,1,1,0,1,0,0 };

void AI_Bunny(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		MakeSound(SND_BUNNYDIE,me->x,me->y,SND_CUTOFF|SND_RANDOM,800);
		// go nuts shooting
		y=10+Random(40);
		for(x=0;x<y;x++)
		{
			i=Random(12);
			if(bunnyBulletFacing[i]==0)
				FireBullet(me->x,me->y,(byte)Random(8),bunnyBulletTypes[i],me->friendly);
			else
				FireBullet(me->x,me->y,(byte)Random(256),bunnyBulletTypes[i],me->friendly);
		}
	}

	if(me->action==ACTION_BUSY)
	{
		// go nuts shooting continuously while you die
		y=2+Random(10);
		for(x=0;x<y;x++)
		{
			i=Random(12);
			if(bunnyBulletFacing[i]==0)
				FireBullet(me->x,me->y,(byte)Random(8),bunnyBulletTypes[i],me->friendly);
			else
				FireBullet(me->x,me->y,(byte)Random(256),bunnyBulletTypes[i],me->friendly);
		}

		return;	// can't do nothin' right now
	}

	if(me->mind1)
	{
		me->mind=0;
		me->mind1=0;
	}
	if(!me->mind--)	// time to get a new direction
		me->facing = me->PickRandomDirectionNear(&me->mind,Random(20)+2);

	me->SetNewSpeed(3);
	me->StartAnimMove(128);
}

void AI_Kongor(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		me->HandleOuchNoises(SND_KONGOUCH,SND_KONGDIE);
		me->mind2++;
	}

	if(me->action==ACTION_BUSY)
	{
		// left pound
		if(me->CheckSequenceFrame(ANIM_ATTACK,7))
		{
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me->x-FIXAMT*96,me->y+FIXAMT*64,0,BLT_SHOCKWAVE,me->friendly);
			ShakeScreen(5);
		}
		// right pound
		if(me->CheckSequenceFrame(ANIM_A2, 7))
		{
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me->x+FIXAMT*96,me->y+FIXAMT*64,0,BLT_SHOCKWAVE,me->friendly);
			ShakeScreen(5);
		}
		// chest pound (go to chest pound second half)
		if(me->seq==ANIM_A3)
		{
			if(me->frm>=5 && (me->frm-5)%5==0)
			{
				ShakeScreen(3);
				if(me->mindControl)
				{
					AddPygmy(map,world,0);
					AddPygmy(map,world,0);
				}
				else
				{
					AddPygmy(map,world,me->friendly);
					AddPygmy(map,world,me->friendly);
				}
				MakeSound(SND_CHESTPOUND,me->x,me->y,SND_CUTOFF,1200);
			}

			if(me->frm==17 && me->frmTimer>64)
				me->StartNewAnimation(ANIM_A4,me->frmAdvance);
		}
		if(me->CheckSequenceFrame(ANIM_A4,2,false))
		{
			ShakeScreen(3);
			if(me->mindControl)
			{
				AddPygmy(map,world,0);
				AddPygmy(map,world,0);
			}
			else
			{
				AddPygmy(map,world,me->friendly);
				AddPygmy(map,world,me->friendly);
			}
			MakeSound(SND_CHESTPOUND,me->x,me->y,SND_CUTOFF,1200);
		}

		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;

	// waiting for goodguy to arrive
	if(me->mind==0)
	{
		if(me->ouch || GuyCheckTargetWithinReachAndSight(me,goodguy,map,350))
			me->StartNewAnimation(ANIM_A3,128,ACTION_BUSY,0,0,0,SND_KONGHELLO); //pound chest
	}
	else
	{
		if(goodguy->x>me->x-FIXAMT*225 && goodguy->x<me->x+FIXAMT*225 && goodguy->y<me->y+FIXAMT*190)
		{
			me->SetNewSpeed(0);
			if(goodguy->x<me->x)
			{
				me->StartNewAnimation(ANIM_ATTACK,128);
				return;
			}
			else
			{
				me->StartNewAnimation(ANIM_A2, 128);
				return;
			}
		}
		else
		{
			if(goodguy->x<me->x-FIXAMT*100)
			{
				me->StartAnimMove(128);
				me->dx=-FIXAMT*3;
			}
			else if(goodguy->x>me->x+FIXAMT*100)
			{
				me->StartNewAnimation(ANIM_A1, 128);
				me->dx=FIXAMT*3;
			}
			else
			{
				me->StartAnimIdle(128);
				me->dx=0;
			}
		}
		if(me->CheckRoll(70,ROLL_ATTACK) && me->mind2>5)
		{
			me->mind2=0;
			me->dx=0;
			me->StartNewAnimation(ANIM_A3,128); // pound chest
		}
	}
}

void AI_Squash(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ZOMBIEOUCH,SND_PUMPKINDIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && !me->reload)
		{
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,8);
			me->reload=2;
		}
		if(me->CheckSequenceFrame(ANIM_ATTACK,3))
		{
			me->reload=4;
			if(me->AttackCheck(24,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		if(me->seq==ANIM_MOVE)
			me->StartNewAnimation(ANIM_IDLE,32);
		if(goodguy && GuyCheckTargetWithinSight(me,goodguy,map) && (me->CheckTargetWithinReach(512)||me->ouch))
		{
			// if the goodguy is near, or he shot me
			me->mind=1;	// start hunting
			if(me->ouch==0)
				me->mind1=240;
			FaceGoodguy(me,goodguy);
		}
		me->SetNewSpeed(0);
	}
	else
	{
		FaceGoodguy(me,goodguy);
		if(!goodguy)
		{
			me->mind=0;	// go back to being bored, nothing to see here
			me->mind1=0;
			return;
		}
		if(!me->mind1--)
		{
			if(GuyCheckTargetWithinReachAndSight(me,goodguy,map,512))
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		if(me->CheckTargetWithinReach(40))
		{
			me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,0);
			return;
		}
		me->SetNewSpeed(6);
		me->StartAnimMove(256);
	}
}

void AI_UltraPygmy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ULTRAPYGMYOUCH,SND_ULTRAPYGMYDIE);

	if(me->action==ACTION_BUSY)
	{
		if(goodguy && me->CheckSequenceFrame(ANIM_ATTACK,6))
		{
			if(me->AttackCheck(12, me->GetSpaceInFrontX(8) >> FIXSHIFT, me->GetSpaceInFrontY(8) >> FIXSHIFT, goodguy))
			{
				goodguy->GetShot(Cosine(me->facing*32)*3,Sine(me->facing*32)*3,6,map,world);
				me->reload=5;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		me->frmAdvance=32;	// overcome the auto-128 frmAdvance of guy.cpp
		me->StartAnimIdle(64);
		if(me->mind1<101)
			me->mind1++;

		if(me->mind1>100 && me->CheckRoll(200,ROLL_MOVE))
		{
			switch(Random(3))
			{
				case 0:
					me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,0,0,0,SND_PYGMYSPIN);
					break;
				case 1:
					me->StartNewAnimation(ANIM_A2,128,ACTION_BUSY,0,0,0,SND_ULTRAPYGMYYAWN);
					break;
				case 2:
					me->facing=(me->facing+(-1+Random(3)))&7;	//change facing
					me->mind1=0;
					break;
			}
		}
		if(goodguy)
		{
			if(goodguy && GuyCheckTargetWithinSight(me,goodguy,map) && (me->CheckTargetWithinReach(512)||me->ouch))
			{
				// if the goodguy is near, or he shot me
				me->mind = 1;	// start hunting
				me->mind1 = (!me->ouch) ? 60 : 120;	// for 2 seconds minimum OR 4 seconds, because they HATE getting shot
				FaceGoodguy(me, goodguy);
				me->StartNewAnimation(ANIM_A3,255,ACTION_BUSY,0,0,0,SND_ULTRAPYGMYANGRY);
			}
		}
		me->SetNewSpeed(0);
	}
	else
	{
		FaceGoodguy(me,goodguy);
		if(!goodguy)
		{
			me->mind=0;	// go back to being bored, nothing to see here
			me->mind1=0;
			return;
		}
		if (me->CheckTargetWithinReach(72) && me->CheckRoll(16,ROLL_ATTACK) && me->reload == 0)
		{
			me->StartNewAnimation(ANIM_ATTACK,255,ACTION_BUSY,0,0,1,SND_ULTRAPYGMYSTAB); // spear him
			return;
		}
		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(!me->mind1--)
		{
			if (!me->CheckTargetWithinReach(256))
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		me->SetNewSpeed(6);
		me->StartAnimMove(256);
	}
}

void AI_LoonyBot(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(!goodguy)
		return;

	if(!me->parent)
	{
		me->type=MONS_NONE;
		return;
	}

	if(me->reload)
		me->reload--;

	if(me->parent->ouch>0)
		me->ouch=me->parent->ouch;

	if(me->parent->mind1==0)
	{
		me->parent->x=me->x;
		me->parent->y=me->y;
	}
	else
	{
		me->parent->x=me->x;
		me->parent->y=me->y+FIXAMT*80;
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if(me->CheckFrame(2,false))
				me->parent->mind1 = 1;
			if(me->CheckFrame(20,false))
			{
				me->parent->mind1=0;
				me->frmAdvance=64;
			}
			if(me->CheckFrame(10))
			{
				LaunchMegabeam(me->x,me->y+96*FIXAMT,me->parent->ID);
				me->frmAdvance=32;
				me->reload=10;
			}
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			if(me->frm>4)
			{
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
			if(me->frm==7 && me->frmTimer<64)
			{
				AddBaby(me->x,me->y-FIXAMT*96,0,MONS_LOONYSHIP,me);
			}
		}
		return;
	}
	if(me->mind==0)	// waiting for player
	{
		if(goodguy && GuyCheckTargetWithinSight(me,goodguy,map) && (me->CheckTargetWithinReach(400)||me->ouch))
			me->mind=1;
	}
	else
	{
		if(me->seq!=ANIM_MOVE)
		{
			if(me->mind1==6)
			{
				me->StartNewAnimation(ANIM_ATTACK,64,ACTION_BUSY,0,0); // time to fire
				return;
			}
			else
			{
				// lob grenades
				MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1500);
				FireBullet(me->x-87*FIXAMT,me->y-30*FIXAMT,Random(88)+20,BLT_GRENADE,me->friendly);
				FireBullet(me->x+92*FIXAMT,me->y-30*FIXAMT,Random(88)+20,BLT_GRENADE,me->friendly);

				// move towards him
				FaceGoodguy(me,goodguy);
				me->SetNewSpeed(2);
				if(goodguy->y<me->y+FIXAMT*200)
					me->dy=-3*FIXAMT;
				else if(goodguy->y>me->y+FIXAMT*300)
					me->dy=2*FIXAMT;
				else
					me->dy=0;
				if(goodguy->x<me->x-FIXAMT*100)
					me->dx=-2*FIXAMT;
				else if(goodguy->x>me->x+FIXAMT*100)
					me->dx=2*FIXAMT;
				else
					me->dx=0;
			}
			me->StartNewAnimation(ANIM_MOVE,128);
			me->mind1++;
			MakeSound(SND_ROBOSTOMP,me->x,me->y,SND_CUTOFF,500);
		}
		else
		{
			if(me->frm==5)
				MakeSound(SND_ROBOSTOMP,me->x,me->y,SND_CUTOFF,500);
		}
	}
}

void AI_LoonyCore(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	// take them with me
	if(me->hp==0)
		KillKids(me);
}

void AI_LoonyGun(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte diff;
	char dir;
	int x,y;

	if(me->action==ACTION_BUSY)
	{
		x=me->x>>FIXSHIFT;
		y=me->y>>FIXSHIFT;
		BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
				  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
				  me->z,FIXAMT);
		return;
	}
	if(!me->parent || me->parent->mind==0)
		return;

	me->recty+=60;
	me->recty2+=60;

	me->z=FIXAMT*48;
	me->dz=0;
	if(me->mind3==0)
	{
		me->x=me->parent->x-87*FIXAMT;
		me->y=me->parent->y;
	}
	else
	{
		me->x=me->parent->x+92*FIXAMT;
		me->y=me->parent->y;
	}

	if(!goodguy)
		return;

	if(me->mind)
		me->mind--;
	else if(me->mind1<6)
	{
		// rotate towards guy, with 16 facings
		me->mind=me->facing;
		FaceGoodguy(me,goodguy);

		me->facing*=2;

		if(me->mind3==0)	// left gun can't rotate to upper right quadrant
		{
			if(me->facing>10)
			{
				if(me->mind<6)
					me->facing=0;
				else
					me->facing=10;
			}
		}
		else	// right gun can't rotate to upper left quadrant
		{
			if(me->facing>8 && me->facing<14)
			{
				if(me->mind>13 || me->mind<3)
					me->facing=14;
				else
					me->facing=8;
			}
		}
		if(me->facing>me->mind)
		{
			diff=me->facing-me->mind;
			dir = (diff>8) ? -1 : 1;
		}
		else if(me->facing<me->mind)
		{
			diff=me->mind-me->facing;
			dir = (diff>8) ? 1 : -1;
		}
		else
		{
			dir=0;
		}
		me->facing=(me->mind+dir)&15;
		me->mind=2;	// rotate counter
	}

	switch(me->mind1)
	{
		case 0:	// waiting to unleash a burst
		case 2:
		case 4:
			if(me->mind2<60)
				me->mind2++;
			else
			{
				me->mind1++;	// next mode
				me->mind2=10;
			}
			break;
		case 1: // firing a burst
		case 3:
		case 5:
			diff=me->facing*16-10+Random(20);
			FireExactBullet(me->GetSpaceInFrontX(40), me->GetSpaceInFrontY(32), me->z + 12 * FIXAMT, Cosine(diff)*6, Sine(diff)*6, -FIXAMT/2,
				0,60,diff,BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,1000);
			if(me->mind2)
				me->mind2--;
			else
				me->mind1++;	// next mode
			break;
		case 6:	// waiting to unleash the doom barrage
			if(me->mind2<60)
				me->mind2++;
			else
				me->mind1=7;
			if((me->mind2&1)==0)
			{
				if(me->mind3==0)	// left gun must rotate up to 10
				{
					if(me->facing<10)
						me->facing=(me->facing+1)&15;
				}
				else	// right gun rotates 'down' to 14
				{
					if(me->facing!=14)
						me->facing=(me->facing-1)&15;
				}
			}
			break;
		case 7:	// the doom barrage
			diff=me->facing*16-10+Random(20);
			FireExactBullet(me->GetSpaceInFrontX(40), me->GetSpaceInFrontY(32), me->z + 12 * FIXAMT, Cosine(diff)*6, Sine(diff)*6, -FIXAMT/2,
				0,60,diff,BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,1000);
			me->mind2--;
			if((me->mind2&1)==0)
			{
				if(me->mind3==0)
				{
					me->facing=(me->facing-1)&15;
					if(me->facing==15)	// overshot
					{
						me->facing=0;
						me->mind1=0;
						me->mind2=0;
					}
				}
				else
				{
					me->facing=(me->facing+1)&15;
					if(me->facing==9)	// overshot
					{
						me->facing=8;
						me->mind1=0;
						me->mind2=0;
					}
				}
			}
			break;
	}
}

void AI_LoonyShip(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;

	if(me->reload)
		me->reload--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;
	}

	if(!goodguy)
		return;

	FaceGoodguy2(me,goodguy);

	if(!me->mind2--)
	{
		me->mind2=10;
		me->mind=(me->mind+1-Random(3))&7; // turn randomly
	}

	me->AccelerateToTargetSpeed(7,6,8,&me->mind);

	if(!me->reload)
	{
		me->reload=20;
		me->mind3=8;
	}

	if(me->mind3)
	{
		if((me->mind3&1)==0)
		{
			f=me->facing*32-5+Random(10);
			FireExactBullet(me->GetSpaceInFrontX(40), me->GetSpaceInFrontY(32), me->z + 12 * FIXAMT, Cosine(f) * 6, Sine(f) * 6, -FIXAMT / 2,
				0,60,f,BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,1000);
		}
		me->mind3--;
	}

	// bounce off walls
	if(me->mind1&1)
	{
		me->dx=-me->dx/2;
		me->mind2=10;
		me->mind=(byte)Random(8);
	}
	if(me->mind1&2)
	{
		me->dy=-me->dy/2;
		me->mind2=10;
		me->mind=(byte)Random(8);
	}
	me->mind1=0;
}
