#include "winpch.h"
#include "monsterai.h"
#include "guy.h"
#include "sound.h"
#include "bullet.h"
#include "player.h"

void AI_GoodTurret(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_ROBOOUCH, SND_ROBODIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frmTimer<63)
		{
			FireBullet(me->x-FIXAMT*30+Random(FIXAMT*60),me->y-FIXAMT*22+Random(FIXAMT*44),
				0,BLT_BOOM,me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if(me->CheckTargetWithinReach(500))
		{
			me->mind=1;
			me->reload=(byte)Random(20);
		}
	}
	else
	{
		if(GuyCheckTargetWithinReachAndSight(me,goodguy,map,500))
			me->mind=0;

		if(me->reload==0)
		{
			MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me->x+Cosine(me->facing*32)*20,me->y+Sine(me->facing*32)*17,
				me->facing*32,BLT_GREEN,me->friendly);
			me->reload=10;
		}
		FaceGoodguy3(me,goodguy);
	}
}


void AI_Mattie2Brain(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte tempface;
	static const byte faceTable[8]={5,4,0,1,2,3,0,6};
	int x,y;
	

	if (me->ouch == 4 && me->aiType == MONS_MATBRAIN)	// skull and head have their own ouch noises
		me->HandleOuchNoises(SND_MATTIEOUCH,0); // no death sound

	// use facing variable to determine which way for eyes to look
	if(me->action!=ACTION_BUSY)
	{
		tempface=me->facing;
		FaceGoodguy(me,goodguy);

		if(me->facing>=5)	// can't look straight back, so deal with it
		{
			if(tempface<2)
				me->facing=0;
			else
				me->facing=4;
		}
		me->facing=faceTable[me->facing];
		if(me->aiType!=MONS_MAT2BRAIN && me->facing!=0)
			me->facing+=4;
	}
	else
		me->facing=0;	// always look ahead when doing things

	if(me->seq==ANIM_DIE)
	{
		x=me->x>>FIXSHIFT;
		y=me->y>>FIXSHIFT;
		if(me->aiType==MONS_MAT2BRAIN)
		{
			BlowUpGuy(x+me->rectx-40,y+me->recty-200,x+me->rectx2+40,y+me->recty2+10,me->z,2);
			BlowSmoke((x+me->rectx-40+Random(me->rectx2-me->rectx+80))<<FIXSHIFT,
					  (y+me->recty-200+Random(me->recty2-me->recty+210))<<FIXSHIFT,
					  me->z,FIXAMT);
			BlowSmoke((x+me->rectx-40+Random(me->rectx2-me->rectx+80))<<FIXSHIFT,
					  (y+me->recty-200+Random(me->recty2-me->recty+210))<<FIXSHIFT,
					  me->z,FIXAMT);
			ShakeScreen(2);
		}
	}
	// use the claws to cover your face when scared
	if(me->ouch && me->mind1<64)
		me->mind1+=8;	// mind1 is fear, when it gets high, the claws cover her face

	if(me->mind1)
		me->mind1--;
}

void AI_Mattie2SkullOrHead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_MATTIEOUCH, SND_MATTIEDIE); // no death sound

	if(me->action==ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_DIE,5,false))
		{
			if (me->aiType == MONS_MAT2HEAD)
				me->DoTransform(MONS_MAT2SKULL);
			else
				me->DoTransform(MONS_MAT2BRAIN);
			me->StartNewAnimation(ANIM_IDLE,128,ACTION_IDLE,me->dx,me->dy,100);
			return;
		}
		if(me->seq==ANIM_DIE && me->frmTimer<128)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			// shoot laser
			x=me->x;
			y=me->y+FIXAMT*8;
			LaunchMegabeam(x,y,me->ID);
			me->reload=0;
			me->mind=0;
			me->reload=100;
		}
		me->facing=0;
		return;
	}

	if ((!me->reload) && me->CheckRoll(100,ROLL_ATTACK) == 0)
		me->StartNewAnimation(ANIM_ATTACK,32,ACTION_BUSY,0,0,0,0);

	// do everything the brain does
	AI_Mattie2Brain(me,map,world,goodguy);
}

void AI_Mattie2Tail(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte b;
	int x,y;
	static const short shootXTable[9]={-134,-124,-101,-57,0,67,113,134,143};
	static const short shootYTable[9]={-51,-43,-24,-7,-11,-15,-29,-41,-46};

	if(me->reload)
		me->reload--;

	if(!me->parent || me->parent->type==MONS_NONE)
	{
		me->type=MONS_NONE;	// can't exist alone
		return;
	}

	me->x=me->parent->x;
	me->y=me->parent->y-104*FIXAMT;
	me->z=me->parent->z;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->reload==0)
		{
			b=(32-me->facing)*4;
			// shoot some bullets
			x=me->x+(shootXTable[(me->facing>>2)]<<FIXSHIFT);
			y=me->y+(shootYTable[(me->facing>>2)]<<FIXSHIFT);

			b+=20-Random(41);
			FireExactBullet(x,y,me->z+32*FIXAMT,Cosine(b)*10,Sine(b)*10,FIXAMT*12,
				0,60,b,BLT_GRENADE,me->friendly);
			if(me->parent->aiType==MONS_MAT2BRAIN)
			{
				b+=20-Random(41);
				FireExactBullet(x,y,me->z+32*FIXAMT,Cosine(b)*10,Sine(b)*10,FIXAMT*12,
					0,120,b,BLT_GRENADE,me->friendly);
			}
			me->reload=2;
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

	if(goodguy)
	{
		// point at Bouapha
		if(goodguy->x<me->x-FIXAMT*128)
		{
			if(goodguy->y<me->y+FIXAMT*64)
				b=0;
			else
				b=8;
		}
		else if(goodguy->x>me->x+FIXAMT*128)
		{
			if(goodguy->y<me->y+FIXAMT*64)
				b=32;
			else
				b=24;
		}
		else if(goodguy->y>me->y+FIXAMT*64)
		{
			b=16;
		}
		else
		{
			// can't point at him when he is behind, but you can try
			if(goodguy->x<me->x)
				b=0;
			else
				b=32;
		}
	}
	else
		b=16;

	// b contains the desired facing
	if(me->mind1<b)
		me->mind1++;
	else if(me->mind1>b)
		me->mind1--;
	me->facing=me->mind1&(~3);	// facing is locked in multiples of 4
}

void AI_Mattie2Body(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte b;

	if(!me->parent || me->parent->type==MONS_NONE)
	{
		me->type=MONS_NONE;	// can't exist alone
		return;
	}

	// unlike other components, this one tells the head itself where to be
	me->parent->x=me->x;
	me->parent->y=me->y+40*FIXAMT;
	me->parent->z=me->z+32*FIXAMT;

	if(goodguy)
	{
		if(goodguy->x<me->x)
			me->dx-=FIXAMT/16;	// slow acceleration
		if(goodguy->x>me->x)
			me->dx+=FIXAMT/16;
		if(goodguy->y<me->y)	//CRUSH him if he gets behind you
			me->dy-=FIXAMT;
		else
		{
			if(me->mind>128)
			{
				me->mind--;
				me->dy+=FIXAMT/8;
			}
			else if(me->mind<128)
			{
				me->mind++;
				me->dy-=FIXAMT/16;
			}
		}
	}
	b=(byte)Random(128);
	if(b<10)
		me->mind=128-Random(64);
	else if(b<40)
		me->mind=128+Random(64);
	else if(b<50)
		me->mind=128;

	me->DampenSpeed(32);
	me->ClampSpeed(2);

	if (me->seq != ANIM_MOVE)
	{
		MakeSound(SND_MATTIESTOMP, me->x, me->y, SND_CUTOFF, 1200);
		me->StartAnimMove(64);
	}
}

static byte IsBunnyAble(const mapTile_t *m, world_t *world)
{
	if(!(GetTerrain(world,m->floor)->flags&TF_BUNNY))
		return 0;

	if(GetItem(m->item)->flags&IF_SOLID)
		return 0;

	return 1;
}

void AI_BuddyBunny(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte ok[4];
	int diff,dir;

	if(me->ouch==4)
	{
		MakeSound(SND_BUNNYDIE, me->x, me->y, SND_CUTOFF | SND_RANDOM, 800);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			for(x=0;x<8;x++)
				SpecialSnow(me->x+((-16+Random(33))<<FIXSHIFT),
							me->y+((-16+Random(33))<<FIXSHIFT));
		}
		return;
	}

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
		if(diff>8)
			diff=8;
	}

	if(me->mind1)
	{
		me->mind1=0;
		if(me->aiType==MONS_FRIENDLY)
			me->mind3=Random(4)*64;
		me->mind=30;	// wait 1 seconds before trying again
		me->dx=0;
		me->dy=0;
		me->frm=0;
		me->seq=ANIM_IDLE;
		me->frmTimer=0;
		me->frmAdvance=128;
		return;
	}

	if(me->mind>0)
	{
		// countdown to try for a path again, in case the map has changed under you
		me->mind--;
		if(me->mind==0)
		{
			me->oldx=me->x+1;
		}
		return;
	}
	me->mind2=(me->mind2+diff*dir)&255;

	// if you've reached the center of a tile, time to decide where to go
	x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	if((me->x<=x && me->oldx>x) || (me->x>=x && me->oldx<x) ||
	   (me->y<=y && me->oldy>y) || (me->y>=y && me->oldy<y))
	{
	//	me->x=x;
	//	me->y=y;
		// figure out which directions are valid bunny paths
		if(me->mapx<map->width-1 && IsBunnyAble(map->GetTile(me->mapx+1, me->mapy),world))
			ok[0]=1;
		else
			ok[0]=0;
		if(me->mapy<map->height-1 && IsBunnyAble(map->GetTile(me->mapx, me->mapy+1),world))
			ok[1]=1;
		else
			ok[1]=0;
		if(me->mapx>0 && IsBunnyAble(map->GetTile(me->mapx-1, me->mapy),world))
			ok[2]=1;
		else
			ok[2]=0;
		if(me->mapy>0 && IsBunnyAble(map->GetTile(me->mapx, me->mapy-1),world))
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
			if(me->aiType==MONS_FRIENDLY)
				me->mind3=Random(4)*64;
			me->mind=30;	// wait 1 second before trying again
			me->StartNewAnimation(ANIM_IDLE,128,ACTION_IDLE,0,0);
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
				if(!ok[me->mind3/64])
					// if you can continue ahead, then go on and enjoy
				{
					// in the case of multiple directions, we must pick randomly
					x=Random(4);
					if(ok[x])	// the direction you're facing is ok
						me->mind3=x*64;
					else if(ok[(x+1)&3])	// direction to your left
						me->mind3=((x+1)&3)*64;
					else if(ok[(x+5)&3])	// direction to your right (same as x-1)
						me->mind3=((x+5)&3)*64;
					else
						me->mind3=((x+2)&3)*64;	// behind you as a last resort
				}
			}
		}
	}

	// move forward at that pace
	me->SetNewSpeed(2,&me->mind3);
	if(me->seq!=ANIM_MOVE)
		me->StartNewAnimation(ANIM_MOVE,128);
	else
		me->frmAdvance=128;
}

void AI_PygmyQueen(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_SHAMANOUCH, SND_SHAMANDIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckFrame(ANIM_ATTACK,2) && goodguy)
		{
			x = me->GetSpaceInFrontX(8);
			y = me->GetSpaceInFrontY(8);
			// summon babies!
			for(i=0;i<5;i++)
			{
				x = me->x + (-200 + Random(401)) * FIXAMT;
				y = me->y + (-170 + Random(341)) * FIXAMT;
				if(g=me->TryAddBaby(map,world,MONS_MINIPYGMY,x-me->x,y-me->y,10))
					LightningBolt(me->x,me->y,x,y);
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		me->frmAdvance=32;	// overcome the auto-128 frmAdvance of guy.cpp

		if(me->seq==ANIM_MOVE)
			me->StartNewAnimation(ANIM_IDLE,32,ACTION_IDLE);
		if(me->mind1<101)
			me->mind1++;
		if(me->mind1>100 && Random(200)==0)
		{
			if(Random(2)==0)
				me->StartNewAnimation(ANIM_A1, 64, ACTION_BUSY, 0, 0, 0, SND_SHAMANDANCE);
			else
				me->facing=(me->facing+(-1+Random(3)))&7;	//change facing
			me->mind1=0;
		}
		if(goodguy)
		{
			if ((me->CheckTargetWithinReach(700) && GuyCheckTargetWithinSight(me,goodguy,map)||me->ouch))
			{
				// if the goodguy is near, or he shot me
				me->mind = 1;	// start hunting
				me->mind1 = (!me->ouch) ? 60 : 120;	// for 2 seconds minimum OR 4 seconds, because they HATE getting shot
				FaceGoodguy(me, goodguy);
				me->StartNewAnimation(ANIM_A3,255,ACTION_BUSY,0,0,0,SND_PYGMYANGRY);
			}
		}
		me->dx=0;
		me->dy=0;
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
		if(!me->reload && me->CheckRoll(64,ROLL_ATTACK) && me->CheckTargetWithinReach(500))
		{
			me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,1,SND_SHAMANZAP); // zap
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(!me->mind1--)
		{
			if(RangeToTarget(me,goodguy)>400*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		me->SetNewSpeed(2);
		me->StartAnimMove(128);
	}
}

void AI_Jalapeno(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			if(me->frm==2 && me->reload==0)
			{
				me->reload=10;
				FireBullet(me->x,me->y,0,BLT_BOOM,me->friendly);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm>=2 && me->frm<=4 && me->reload==0)
		{
			FireBullet(me->x+Cosine(me->facing*32)*16,me->y+Sine(me->facing*32)*16,me->facing,
				BLT_FLAME2,me->friendly);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3==0)
	{
		BlowSmoke(me->x,me->y,FIXAMT*10,Random(4)*FIXAMT);
		me->mind3=(byte)Random(10);
	}
	else
		me->mind3--;

	if(me->mind==0)	// not currently aware of goodguy
	{
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}

		if(goodguy)
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

		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>=512*FIXAMT)
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

void AI_Generator(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;
	}

	// countdown subseconds
	if(!me->mind1--)
	{
		// a second has elapsed
		me->mind2--;
		me->mind1=30;
		if(!me->mind2)
		{
			// summon!
			g=AddBaby(me->x,me->y,10*FIXAMT,me->mind,me);
			if(g && !g->CanWalk(g->x,g->y,map,world))
			{
				RemoveGuy(g);
			}
			else
			{
				map->BrightTorch(me->x/(TILE_WIDTH*FIXAMT),me->y/(TILE_HEIGHT*FIXAMT),20,10);
				for(x=0;x<15;x++)
					BlowSmoke(me->x-40*FIXAMT+Random(80*FIXAMT),me->y-30*FIXAMT+Random(60*FIXAMT),
						0,Random(8)*FIXAMT+FIXAMT);
				MakeSound(SND_GENERATE,me->x,me->y,SND_CUTOFF|SND_ONE,1200);
			}

			switch(me->type)
			{
				case MONS_GENERATOR1:
					me->mind2=1;
					break;
				case MONS_GENERATOR2:
					me->mind2=5;
					break;
				case MONS_GENERATOR3:
					me->mind2=15;
					break;
				case MONS_GENERATOR4:
					me->mind2=30;
					break;
			}
		}
	}

	switch(me->type)
	{
		case MONS_GENERATOR1:
			me->facing=9-((int)((me->mind2-1)*30)+(int)me->mind1)*10/30;
			if(me->facing>9)
				me->facing=0;
			break;
		case MONS_GENERATOR2:
			me->facing=9-((int)((me->mind2-1)*30)+(int)me->mind1)*10/(30*5);
			if(me->facing>9)
				me->facing=0;
			break;
		case MONS_GENERATOR3:
			me->facing=9-((int)((me->mind2-1)*30)+(int)me->mind1)*10/(30*15);
			if(me->facing>9)
				me->facing=0;
			break;
		case MONS_GENERATOR4:
			me->facing=9-((int)((me->mind2-1)*30)+(int)me->mind1)*10/(30*30);
			if(me->facing>9)
				me->facing=0;
			break;
	}
}

void AI_Shark(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_SHARKDIE,SND_SHARKOUCH);

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_A1,2))
		{
			me->reload=5;
			FireBullet(me->x,me->y,0,BLT_SHOCKWAVE,me->friendly);
		}
		if(me->CheckSequenceFrames(ANIM_ATTACK,2,7,false))
		{
			FaceGoodguy(me,goodguy);
			FireBulletZ(me->x,me->y,FIXAMT*4,me->facing*32-16+(byte)Random(32),BLT_SHARK,me->friendly);
		}
		return;
	}

	if(me->mind==0)	// hiding
	{
		me->frmAdvance=32;
		me->SetNewSpeed(0);
		if(me->CheckTargetWithinReach(64) || me->ouch)
		{
			me->mind=1;
			me->mind1=40+(byte)Random(60);
			ExplodeParticles2(PART_WATER,me->x,me->y,0,16,6);
			MakeSound(SND_SHARKATTACK,me->x,me->y,SND_CUTOFF,1200);
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			return;
		}
	}
	else
	{
		me->StartAnimMove(2);
		FaceGoodguy(me,goodguy);
		me->AccelerateSpeed(1/2);
		me->ClampSpeed(6);

		if(!me->mind1--)
		{
			ExplodeParticles2(PART_WATER,me->x,me->y,0,16,6);
			me->mind1=40+(byte)Random(60);
			MakeSound(SND_SHARKATTACK,me->x,me->y,SND_CUTOFF,1200);
			me->StartNewAnimation(ANIM_ATTACK,128);
			return;
		}
	}
}

void AI_MadBug(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_SPD1OUCH, SND_SPD1DIE);

	if(me->action==ACTION_BUSY)
	{
		if (me->CheckSequenceFrame(ANIM_ATTACK,2) && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*4;
			y=me->y+Sine(me->facing*32)*4;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*2,Sine(me->facing*32)*2,10,map,world);
			me->reload=2;

		}
		if (me->CheckSequenceFrame(ANIM_A1,2) && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*4;
			y=me->y+Sine(me->facing*32)*4;
			FireBulletZ(x,y,FIXAMT*2,me->facing*32,BLT_GRENADE,me->friendly);
			me->reload=2;

		}
		return;	// can't do nothin' right now
	}

	if(me->CheckTargetWithinReach(32) && me->CheckRoll(4,ROLL_ATTACK))
	{
		FaceGoodguy(me,goodguy);
		me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,0,SND_SPD1GNAW); // get him!!
		return;
	}
	if(me->CheckTargetWithinReach(256) && me->CheckRoll(16,ROLL_ATTACK) && GuyCheckTargetWithinSight(me,goodguy,map))
	{
		FaceGoodguy(me,goodguy);
		me->StartNewAnimation(ANIM_A1,256,ACTION_BUSY,0,0,0,SND_SPD3SPEW); // get him!!
		return;
	}

	me->TryGetNewDirection(&me->mind,40,goodguy,true);
	me->SetNewSpeed(4);
	me->StartAnimMove(128);
}

void AI_Wizard(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		me->HandleOuchNoises(SND_WIZOUCH,SND_WIZDIE);
		if(me->hp>0 && me->CheckRoll(10,ROLL_ATTACK))
			FireBullet(me->x,me->y,0,BLT_REFLECT,me->friendly);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_ATTACK,8) && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;

			FindVictim(me->GetSpaceInFrontX(16) >> FIXSHIFT, me->GetSpaceInFrontY(16) >> FIXSHIFT, 32, me->GetSpaceInFrontX(6), me->GetSpaceInFrontY(6), 8, map, world, me->friendly);

			if(me->mind1)
			{
				me->frm=3;
				me->mind1--;
			}
			else
				me->reload=10;
		}
		if(me->CheckSequenceFrames(ANIM_A1,1,9))
		{
			me->reload=3;
			MakeSound(SND_ZAP,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine((me->facing*32+64)&255)*8;
			y=me->y+Sine((me->facing*32+64)&255)*8;
			x+=Cosine(me->facing*32)*12;
			y+=Sine(me->facing*32)*12;
			FireBullet(x,y,(me->facing*32-4)&255,BLT_BIGSHELL,me->friendly);
			x=me->x+Cosine((me->facing*32-64)&255)*8;
			y=me->y+Sine((me->facing*32-64)&255)*8;
			x+=Cosine(me->facing*32)*12;
			y+=Sine(me->facing*32)*12;
			FireBullet(x,y,(me->facing*32+4)&255,BLT_BIGSHELL,me->friendly);

		}

		if(me->seq==ANIM_DIE && me->frm>5)
			me->frmAdvance=128;

		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		FaceGoodguy3(me,goodguy);
		if(!me->reload && me->CheckTargetWithinReach(96) && me->CheckRoll(16,ROLL_ATTACK) && GuyCheckTargetWithinSight(me,goodguy,map))
		{
			me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,1,SND_WIZHIT); // smack him!
			me->mind1=2;	// tell him to repeat twice
			return;
		}
		if(me->reload && me->CheckTargetWithinReach(500) && me->CheckRoll(50,ROLL_ATTACK))
		{
			me->StartNewAnimation(ANIM_A1,256,ACTION_BUSY,0,0,1);
			return;
		}
		me->SetNewSpeed(4);
		me->StartAnimMove(128);
	}
}

void AI_EvilClone(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_CLONEOUCH,SND_CLONEDIE);

	if(me->action==ACTION_BUSY)
	{
		if(goodguy && me->CheckSequenceFrame(ANIM_ATTACK,3))
		{
			me->DoFireBullet(BLT_EVILHAMMER,5,16);
			me->mind1=1;
		}
		if (me->seq == ANIM_DIE)
			me->Speen();
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		FaceGoodguy(me, goodguy);
		if(me->CheckTargetWithinReach(512) && me->CheckRoll(32,ROLL_ATTACK) && GuyCheckTargetWithinSight(me,goodguy,map))
		{
			me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,0,SND_HAMMERTOSS); // spit at him
			return;
		}
		me->SetNewSpeed(5);
		me->StartAnimMove(128);
		if (me->CheckRoll(64,ROLL_MOVE))
			me->WalkAround();
	}
	else if(me->mind==1)	// random wandering
	{
		if(me->CheckTargetWithinReach(512) && me->CheckRoll(32,ROLL_ATTACK) && GuyCheckTargetWithinSight(me,goodguy,map))
		{
			FaceGoodguy(me, goodguy);
			me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,0,SND_HAMMERTOSS); // spit at him
			return;
		}
		me->TryGetNewDirection(&me->mind1, 40, goodguy, true);
		me->SetNewSpeed(6);
		me->StartAnimMove(128);
	}
}

void AI_Bob(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if (me->ouch == 4)
		me->HandleOuchNoises(SND_GINGEROUCH, SND_GINGERDIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && (me->frm==2 || me->frm==4 || me->frm==11) && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if (me->AttackCheck(32, (me->GetSpaceInFrontX(32)) >> FIXSHIFT, (me->GetSpaceInFrontY(32)) >> FIXSHIFT, goodguy))
				goodguy->GetShot(me->GetSpaceInFrontX(4), me->GetSpaceInFrontY(4), 12, map, world);
			me->DoFireBullet(BLT_SHOCKWAVE,4,0);
			if(me->frm==11)
			{
				me->frmAdvance=64;
				me->SetNewSpeed(0);
			}
		}
		if(me->CheckSequenceFrame(ANIM_A3,4))
		{
			me->reload=6;
			for(i=0;i<4;i++)
				AddNinja(map,world,me->friendly);
		}
		if(me->seq==ANIM_A1 && goodguy)
		{
			if(me->frm<2)
				me->DampenSpeed(2);
			else
			{
				me->SetNewSpeed(14);
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				if(me->AttackCheck(16,(me->GetSpaceInFrontX(16)) >> FIXSHIFT, (me->GetSpaceInFrontY(16)) >> FIXSHIFT, goodguy))
				{
					goodguy->GetShot(Cosine(me->facing*32)*12,Sine(me->facing*32)*12,24,map,world);
					me->StartNewAnimation(ANIM_A2,32);
					me->SetNewSpeed(4);
					me->FlipSpeed();
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly stalk Bouapha
	{
		if(goodguy)
		{
			if (me->CheckTargetWithinReach(64) && me->CheckRoll(8,ROLL_ATTACK))
			{
				// get him!
				MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=Cosine(me->facing*32);
				me->dy=Sine(me->facing*32);
				return;
			}
		if (me->CheckTargetWithinReach(512) && me->CheckRoll(128,ROLL_MOVE) && GuyCheckTargetWithinSight(me,goodguy,map))
			{
				me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,me->dx,me->dy,0,SND_GINGERJKICK);
				FaceGoodguy(me, goodguy);
				me->dz = FIXAMT * 6;
				return;
			}
			FaceGoodguy(me, goodguy);
			me->SetNewSpeed(5);
			me->StartAnimMove(128);
			if (me->CheckRoll(64,ROLL_MOVE))
				me->WalkAround();
		}
		else
			me->WalkAround();
	}
	else if(me->mind==1)	// random wandering
	{
		if (me->CheckTargetWithinReach(1024) && me->CheckRoll(256,ROLL_MOVE))
		{
			
			me->StartNewAnimation(ANIM_A3,128,ACTION_BUSY,me->dx,me->dy,0,SND_GINGERJKICK); // yell for help
			me->seq=ANIM_A3;
			me->facing=0;
			return;
		}
		if (me->CheckTargetWithinReach(512) && me->CheckRoll(32,ROLL_MOVE) && GuyCheckTargetWithinSight(me, goodguy, map))
		{
			
			me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,me->dx,me->dy,0,SND_GINGERJKICK); // jumpkick him
			FaceGoodguy(me,goodguy);
			me->dz=FIXAMT*6;
			return;
		}

		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(3);
		me->StartAnimMove(64);
	}
}

void AI_MultiMoss(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;
	byte tries;

	if(me->reload)
		me->reload--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==2 && me->reload==0)
		{
			// divide in two!
			me->reload=100;
			for(i=0;i<2;i++)
			{
				tries=255;
				while((tries--))
				{
					if(g=me->TryAddBaby(map,world,MONS_MULTIMOSS,FIXAMT*48+Random(FIXAMT*96),FIXAMT*32+Random(FIXAMT*64),0))
					{
						g->seq=ANIM_A1;
						break;
					}
				}
			}
		}

		return;	// can't do nothin' right now
	}

	me->TryGetNewDirection(&me->mind1,40,goodguy,true);
	me->SetNewSpeed(1/2);

	if(me->CheckTargetWithinReach(40))
		goodguy->GetShot(me->dx*4,me->dy*4,1,map,world);
}

void AI_Snowball(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ACIDSPLAT, 0);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			if(me->frm==0 && !me->reload)
			{
				MakeSound(SND_LAMPPOOF,me->x,me->y,SND_CUTOFF,1000);
				me->reload=10;
			}

			BlowSmoke(me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				Random(48)*FIXAMT,Random(5)*FIXAMT);
			BlowSmoke(me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				Random(48)*FIXAMT,Random(5)*FIXAMT);
			BlowSmoke(me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				Random(48)*FIXAMT,Random(5)*FIXAMT);
			FireBullet(me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				(byte)Random(256),BLT_BIGSNOW,me->friendly);
			FireBullet(me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				(byte)Random(256),BLT_BIGSNOW,me->friendly);
		}
		return;
	}

	if(GetTerrain(world,map->map[me->mapx+me->mapy*map->width].floor)->flags&(TF_WATER|TF_LAVA))
	{
		me->SelfDestruct(map,world);
		return;
	}

	if(FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,40,0,0,255,map,world,me->friendly))
	{
		MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850); // kill him
	}
	if(me->mind1)	// bonked a wall
	{
		me->mind1=0;
		if(me->aiType==MONS_SNOWBALL)
			me->facing=(me->facing+2)&7;
		else
			me->facing=(me->facing+6)&7;

		MakeSound(SND_ACIDSPLAT,me->x,me->y,SND_CUTOFF,850);
	}

	switch(me->facing)
	{
		case 0:
		case 1:
			f=ANIM_A1;	// roll to the right
			break;
		case 2:
		case 3:
			f=ANIM_MOVE; // roll down
			break;
		case 4:
		case 5:
			f=ANIM_A2;	// roll to the left
			break;
		case 6:
		default:
			f=ANIM_ATTACK;	// roll up
			break;
	}
	if (me->seq != f)
		me->StartNewAnimation(f,128,ACTION_IDLE,me->GetSpaceInFrontX(4),me->GetSpaceInFrontY(4));
}

void AI_Snowblower(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte ang;

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
			ang=128-8+(byte)Random(17);
			switch(me->mind2)
			{
				case 0:
					if(!map->GetTile(me->mapx+1,me->mapy)->wall)
						FireBullet(me->x+FIXAMT*TILE_WIDTH/2,me->y,0+ang,BLT_SNOWBALL,me->friendly);
					break;
				case 1:
					if(!map->GetTile(me->mapx,me->mapy+1)->wall)
						FireBullet(me->x,me->y+FIXAMT*TILE_HEIGHT/2,2*32+ang,BLT_SNOWBALL,me->friendly);
					break;
				case 2:
					if(!map->GetTile(me->mapx-1,me->mapy)->wall)
						FireBullet(me->x-FIXAMT*TILE_WIDTH/2,me->y,4*32+ang,BLT_SNOWBALL,me->friendly);
					break;
				case 3:
					if(!map->GetTile(me->mapx,me->mapy-1)->wall)
						FireBullet(me->x,me->y-FIXAMT*TILE_HEIGHT/2,6*32+ang,BLT_SNOWBALL,me->friendly);
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

void AI_XenoMama(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->mind3)
		me->mind3--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ALIENOUCH, SND_ALIENDIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->CheckSequenceFrame(ANIM_ATTACK,2) && goodguy)
		{
			x=me->GetSpaceInFrontX(16);
			y=me->GetSpaceInFrontY(16);
			if(g=me->TryAddBaby(map,world,MONS_ALIENEGG,x-me->x,y-me->y,2))
				MakeSound(SND_LAYEGG,x,y,SND_CUTOFF,1200);

			me->reload=10;
		}
		if(me->seq==ANIM_A1 && me->frm==5 && me->reload==0)
		{
			x=me->GetSpaceInFrontX(20);
			y=me->GetSpaceInFrontY(20);
			SpitAcid(x,y,me->facing*32,BLT_ACID,me->friendly);	// same as FireBullet, just does it up a little (z axis)
													// because alien is taller than other acid-spitters
			SpitAcid(x,y,(me->facing*32+256-4)&255,BLT_ACID,me->friendly);
			SpitAcid(x,y,(me->facing*32+4)&255,BLT_ACID,me->friendly);
			me->reload=5;
			me->mind1=1;
		}
		if(me->seq==ANIM_DIE)
		{
			for(x=0;x<8;x++)
				FireBullet(me->x,me->y,(byte)Random(256),BLT_ACID,me->friendly);
			ExplodeParticles2(PART_SLIME,me->x,me->y,FIXAMT*20,10,10);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(!me->mind3 && me->CheckTargetWithinReach(800) && me->CheckRoll(32,ROLL_ATTACK) && GuyCheckTargetWithinSight(me, goodguy, map))
		{
			me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,0,SND_SKELKICK); // get him!!
			me->mind3=60;
			return;
		}
		FaceGoodguy(me,goodguy);
		me->SetNewSpeed(4);
		me->StartAnimMove(128);
		if (me->CheckRoll(32,ROLL_MOVE) == 0)
			me->WalkAround();
	}
	else if(me->mind==1)	// random wandering
	{
		if(me->CheckTargetWithinReach(512) && me->CheckRoll(16,ROLL_ATTACK) && GuyCheckTargetWithinSight(me, goodguy, map))
		{
			me->StartNewAnimation(ANIM_A1,128,ACTION_BUSY,0,0,0); // get him!!
			FaceGoodguy(me,goodguy);
			return;
		}
		if(!me->mind3 && me->CheckTargetWithinReach(800) && me->CheckRoll(32,ROLL_ATTACK) && GuyCheckTargetWithinSight(me, goodguy, map))
		{
			me->StartNewAnimation(ANIM_ATTACK,256,ACTION_BUSY,0,0,0,SND_SKELKICK); // get him!!
			me->mind3=60;
			return;
		}
		me->TryGetNewDirection(&me->mind1,40,goodguy,true);
		me->SetNewSpeed(4);
		me->StartAnimMove(128);
	}
}

void AI_Gnome(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
		me->HandleOuchNoises(SND_ULTRAPYGMYOUCH,SND_ULTRAPYGMYDIE);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0 && goodguy)
		{
			me->SetNewSpeed(8);
			if(me->AttackCheck(12,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				if(goodguy->aiType==MONS_BOUAPHA && !me->mind3)
				{
					if(me->mind3=StealWeapon())
						MakeSound(SND_GNOMELAUGH,me->x,me->y,SND_CUTOFF,1200);
				}
				goodguy->GetShot(Cosine(me->facing*32)*3,Sine(me->facing*32)*3,3,map,world);
				me->reload=5;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3!=0)
	{
		// have stolen something, must run away!
		FaceGoodguy(me,goodguy);
		me->facing=(me->facing+4)&7;
		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
		return;
	}
	if(me->mind==0)	// not currently aware of goodguy
	{
		me->frmAdvance=32;	// overcome the auto-128 frmAdvance of guy.cpp

		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
		if(me->mind1<101)
			me->mind1++;

		if(me->mind1>100 && Random(200)==0)
		{
			switch(Random(3))
			{
				case 0:
					MakeSound(SND_PYGMYSPIN,me->x,me->y,SND_CUTOFF,1200);
					me->seq=ANIM_A1;	// spin the spear
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 1:
					MakeSound(SND_ULTRAPYGMYYAWN,me->x,me->y,SND_CUTOFF,1200);
					me->seq=ANIM_A2;	// yawn
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 2:
					me->facing=(me->facing+(-1+Random(3)))&7;	//change facing
					me->mind1=0;
					break;
			}
		}
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<300*FIXAMT || me->ouch>0)
			{
				// if the goodguy is near, or he shot me
				me->mind=1;	// start hunting
				if(me->ouch==0)
					me->mind1=60;	// for 2 seconds minimum
				else
					me->mind1=120;	// 4 seconds, because they HATE getting shot
				FaceGoodguy(me,goodguy);

				// do angry animation
				me->action=ACTION_BUSY;
				me->seq=ANIM_A3;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=255;	// very fast
				MakeSound(SND_GNOMESEE,me->x,me->y,SND_CUTOFF,1200);
			}
		}
		me->dx=0;
		me->dy=0;
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
		if(RangeToTarget(me,goodguy)<(72*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			// spear him!
			MakeSound(SND_ULTRAPYGMYSTAB,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
	}
}

void AI_Nobody(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->mind)
		me->mind--;
	else
	{
		switch(Random(4))
		{
			case 0:	// above the world
				me->x=(Random(map->width*2)-map->width/2)*TILE_WIDTH*FIXAMT;
				me->y=-FIXAMT*TILE_HEIGHT*100;
				me->mind=Random(60)+2;
				break;
			case 1:	// below the world
				me->x=(Random(map->width*2)-map->width/2)*TILE_WIDTH*FIXAMT;
				me->y=FIXAMT*TILE_HEIGHT*(100+map->height);
				me->mind=Random(60)+2;
				break;
			case 2:	// right of the world
				me->x=FIXAMT*TILE_WIDTH*(100+map->width);
				me->y=(Random(map->height*2)-map->height/2)*TILE_HEIGHT*FIXAMT;
				me->mind=Random(60)+2;
				break;
			case 3:	// left of the world
				me->x=-FIXAMT*TILE_WIDTH*100;
				me->y=(Random(map->height*2)-map->height/2)*TILE_HEIGHT*FIXAMT;
				me->mind=Random(60)+2;
				break;
		}

	}
}

void AI_BigPumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,16);
			me->reload=2;
			x=me->x-FIXAMT*32+Random(FIXAMT*64);
			y=me->y-FIXAMT*24+Random(FIXAMT*48);
			g=AddBaby(x,y,0,MONS_PUMPKIN,me);
			if(g && !g->CanWalk(g->x,g->y,map,world))
				RemoveGuy(g);
		}
		return;	// can't do nothin' right now
	}

	if(me->seq==ANIM_MOVE && me->frm==2 && goodguy)	// hits on this frame
	{
		FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,36,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world,me->friendly);
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}

		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<300*FIXAMT || me->ouch>0)
			{
				// if the goodguy is near, or he shot me
				me->mind=1;	// start hunting
				if(me->ouch==0)
					me->mind1=60;	// for 2 seconds minimum
				else
					me->mind1=120;	// 4 seconds, because they HATE getting shot
				FaceGoodguy(me,goodguy);
			}
		}
		me->dx=0;
		me->dy=0;
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

		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>300*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		me->dx=Cosine(me->facing*32);
		me->dy=Sine(me->facing*32);
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_RockTrooper(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ROBOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ROBODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=3)
		{
			if(me->frm<7)
			{
				me->dx=Cosine(me->facing*32)*10;
				me->dy=Sine(me->facing*32)*10;
				if(me->mind1==0)
				{
					FindVictim(me->x>>FIXSHIFT,me->y>>FIXSHIFT,70,Cosine(me->facing*32)*12,Sine(me->facing*32)*12,10,map,world,me->friendly);
					me->mind1=1;
				}
			}
			else
			{
				// slow down
				Dampen(&me->dx,FIXAMT*2);
				Dampen(&me->dy,FIXAMT*2);
			}
		}
		if(me->seq==ANIM_A1 && (me->frm>=3 && me->frm<=5) && me->reload==0 && goodguy)
		{
			MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(((me->facing+2)&7)*32)*48;
			y=me->y+Sine(((me->facing+2)&7)*32)*32;
			FireExactBullet(x,y,FIXAMT*100,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,FIXAMT*8,
							0,60,me->facing,BLT_ROCK,me->friendly);
			me->reload=2;
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
	if(RangeToTarget(me,goodguy)<(300*FIXAMT) && Random(20)==0)
	{
		// get him!
		MakeSound(SND_INVIDCHARGE,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		me->mind1=0;
		return;
	}
	if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0 && me->reload==0)
	{
		// shoot at him
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		return;
	}
	FaceGoodguy2(me,goodguy);

	me->dx=Cosine(me->facing*32)*3;
	me->dy=Sine(me->facing*32)*3;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_CrazyBone(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireBullet(x,y,me->facing*32,BLT_BIGSHELL,me->friendly);
			me->reload=5;
			me->mind1=1;
		}
		if(me->seq==ANIM_DIE && me->frm==7 && Random(6)==0)
		{
			// come back!
			if(me->friendly==0)
				player.enemiesSlain--;
			me->seq=ANIM_A3;
			me->hp=MonsterHP(me->type);
			if(!me->CanWalk(me->x,me->y,map,world))
			{
				// nevermind if there's no room
				me->hp=0;
				me->seq=ANIM_DIE;
				return;
			}
			MakeSound(SND_SKELALIVE,me->x,me->y,SND_CUTOFF,1200);
			me->frm=2;
			me->frmTimer=0;
			me->frmAdvance=128;
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
				me->frmAdvance=256;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0)
			{
				// spit at him
				MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				FaceGoodguy(me,goodguy);
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*5;
			me->dy=Sine(me->facing*32)*5;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
			}
			if(Random(128)==0)
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
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0)
		{
			// spit at him
			MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(10)+1;
		}

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

void AI_Creepazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && (me->frm>=5 && me->frm<=8) && goodguy)
		{
			// spit stuff
			//x=me->x+Cosine(me->facing*32)*8;
			//y=me->y+Sine(me->facing*32)*8;
			i=(me->facing*32+256-16+Random(33))&255;
			FireExactBullet(me->x,me->y,FIXAMT*30,Cosine(i)*12,Sine(i)*12,0,0,16,i,BLT_SPORE,me->friendly);
			me->reload=15;
			me->mind=0;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// spit on him!
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
	}

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(60)+1;
	}

	me->dx=Cosine(me->facing*32)*3;
	me->dy=Sine(me->facing*32)*3;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}
