#include "winpch.h"
#include "monster.h"
#include "player.h"
#include "progress.h"
#include "shop.h"
#include "editor.h"
#include "goal.h"

/*
 -MT_GOOD	 -MT_EVIL		 -MT_SPOOKY		-MT_ZOMBIE	 -MT_VAMPIRE	-MT_SPIDER -MT_PYGMY
 -MT_BOSS	 -MT_MINIBOSS	 -MT_WACKY		-MT_PUMPKIN	 -MT_THINGY		-MT_VEGGIE -MT_ARCTIC
 -MT_ZOID	 -MT_DESERT		 -MT_CRAZIES	-MT_VEHICLE	 -MT_GENERATE	-MT_TRAP   -MT_ALIEN
 -MT_ANIMAL	 -MT_HUMAN		 -MT_URBAN		-MT_AQUATIC	 -MT_UNDERSEA
 -MT_FLYING  -MT_HIGHTECH	 -MT_BITS
*/
extern MonsterType monsType[NUM_MONSTERS]; // see monsterlist.cpp

void InitMonsters(void)
{
	int i,j,k;
	byte done;

	for(i=0;i<NUM_MONSTERS;i++)
	{
		monsType[i].spr=NULL;
		for(j=0;j<NUM_ANIMS;j++)
		{
			done=0;
			for(k=0;k<ANIM_LENGTH;k++)
			{
				if(monsType[i].anim[j][k]==255)
				{
					done=1;
				}
				if(done)
					monsType[i].anim[j][k]=255;	// fill the unused space in animations with 255's
			}
			monsType[i].anim[j][ANIM_LENGTH-1]=255;	// and set the last one to 255 just in case
		}
	}
	// just keep bouapha perma-loaded
	monsType[MONS_BOUAPHA].spr=new sprite_set_t(monsType[MONS_BOUAPHA].sprName);
}

void ExitMonsters(void)
{
	int i;

	for(i=1;i<NUM_MONSTERS;i++)
	{
		if(monsType[i].sprName[0]=='!')
			monsType[i].spr=NULL;
		else if(monsType[i].spr)
			delete monsType[i].spr;
		monsType[i].spr=NULL;
	}
}

void ChangeOffColor(dword type,byte from,byte to)
{
	monsType[type].fromCol=from;
	monsType[type].toCol=to;
}

void PurgeMonsterSprites(void)
{
	int i;

	// note this starts at 2, skipping bouapha
	for(i=2;i<NUM_MONSTERS;i++)
	{
		// repeat graphics monsters do not delete their sprites
		if(monsType[i].sprName[0]=='!')
			monsType[i].spr=NULL;
		else if(monsType[i].spr)
			delete monsType[i].spr;
		monsType[i].spr=NULL;
	}
}

byte MonsterSize(dword type)
{
	return monsType[type].size;
}

const byte *MonsterAnim(dword type,byte anim)
{
	return monsType[type].anim[anim];
}

MonsterFlags_ MonsterFlags(dword type,dword aiType)
{
	if(aiType==MONS_BOUAPHA && GetCurrentWeaponType()==WPN_PWRARMOR)
		return monsType[MONS_PWRBOUAPHA].flags;
	if(aiType==MONS_BOUAPHA && GetCurrentWeaponType()==WPN_MINISUB)
		return monsType[MONS_MINISUB].flags;
	return monsType[type].flags;
}

byte MonsterFrames(dword type,dword aiType)
{
	if(aiType==MONS_BOUAPHA && GetCurrentWeaponType()==WPN_PWRARMOR)
		return monsType[MONS_PWRBOUAPHA].framesPerDir;
	if(aiType==MONS_BOUAPHA && GetCurrentWeaponType()==WPN_MINISUB)
		return monsType[MONS_MINISUB].framesPerDir;
	return monsType[type].framesPerDir;
}

word MonsterPoints(dword type)
{
	return monsType[type].points;
}

word MonsterHP(dword type)
{
	return monsType[type].hp;
}

const char *MonsterName(short type)
{
	EntityType realType = (EntityType)type;
	// These special monster IDs used in specials have hardcoded names.
	switch (type)
	{
	case MONS_ANYBODY:
		return "Anybody";
	case MONS_GOODGUY:
		return "Goodguy";
	case MONS_BADGUY:
		return "Badguy";
	case MONS_NONPLAYER:
		return "Non-player";
	case MONS_PLAYER:
		return "Player";
	case MONS_TAGGED:
		return "Tagged";
	}
	// These monsters have different names in the editor.
	if (editing)
	{
		switch (realType)
		{
			case EntityType::BunnyPath:
				return "Buddy Bunny:Determined";
			case EntityType::BunnyFollow:
				return "Buddy Bunny:Follow";
			case EntityType::SharkChest:
				return "Sneaky Shark:Chest";
			case EntityType::NohMannComedy:
				return "Noh-Mann:Comedy";
			case EntityType::NohMannTragedy:
				return "Noh-Mann:Tragedy";
			case EntityType::YerfdogBoss:
				return "Yerfdog:Boss";
		}
	}
	// Otherwise the monster name is in its type definition.
	return monsType[type].name;
}

Monster_AIFunc MonsterAI(dword type)
{
	return monsType[type].AI;
}

int8_t MonsterBrightnessChange(dword type)
{
	return monsType[type].brtChg;
}

void LoadMySprite(dword type)
{
	int v;

	if(type==0 || type>=NUM_MONSTERS)
		return;

	if(monsType[type].spr==NULL)
	{
		if(monsType[type].sprName[0]=='!')
		{
			// it's a repeat of someone else's sprite
			v=atoi(&monsType[type].sprName[1]);
			if(!monsType[v].spr)
				monsType[v].spr=new sprite_set_t(monsType[v].sprName);

			monsType[type].spr=monsType[v].spr;
		}
		else
			monsType[type].spr=new sprite_set_t(monsType[type].sprName);

		if(monsType[type].spr==NULL || monsType[type].spr->GetSprite(0)==NULL)
			FatalError("Out of memory or sprites missing!");
	}
}

byte LoadMySpriteMaybe(byte type)
{
	int v;

	if (type == 0 || type >= NUM_MONSTERS)
		return -1;

	if (monsType[type].spr == NULL)
	{
		if (monsType[type].sprName[0] == '!')
		{
			// it's a repeat of someone else's sprite
			v = atoi(&monsType[type].sprName[1]);
			if (!monsType[v].spr)
				monsType[v].spr = new sprite_set_t(monsType[v].sprName);

			monsType[type].spr = monsType[v].spr;
		}
		else
			monsType[type].spr = new sprite_set_t(monsType[type].sprName);

		if (monsType[type].spr == NULL || monsType[type].spr->GetSprite(0) == NULL)
			return 0;
	}
	return 1;
}

const sprite_t *GetMonsterSprite(dword type,byte seq,byte frm,byte facing)
{
	int v;

	if(type==MONS_BOUAPHA)
	{
		if(GetCurrentWeaponType()==WPN_PWRARMOR)
			type=MONS_PWRBOUAPHA;
		else if(GetCurrentWeaponType()==WPN_MINISUB)
			type=MONS_MINISUB; // is this ok???
		else if(type==MONS_BOUAPHA)
		{
			if(player.playAs==PLAY_LUNATIC)
				type=MONS_DRL;
			else if(player.playAs==PLAY_HAPPY)
				type=MONS_STICKMAN;
			else if(player.playAs==PLAY_MECHA)
				type=MONS_PLAYMECHA;
			else if(player.playAs==PLAY_SHROOM)
				type=MONS_PLAYSHROOM;
			else if(player.playAs==PLAY_LUNACHIK)
				type=MONS_LUNACHICK;
		}
	}

	// load if not loaded
	LoadMySprite(type);

	v=monsType[type].anim[seq][frm];

	if(v==FRAME_INVIS)
		return NULL;	// 254 means no sprite for this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(type==MONS_BOUAPHA)
	{
		if(PlayerHasHammer())
			v+=8*monsType[type].framesPerDir;
	}
	if(type==MONS_EVILCLONE)
		v+=8*monsType[type].framesPerDir;

	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	return monsType[type].spr->GetSprite(v);
}


std::unique_ptr<sprite_set_t>* intFace = GetIntfaceSpriteSet();


void DrawMe(Guy* g, const sprite_set_t* set) {
	const sprite_t* curSpr;
	int v;
	int intfx = 0;
	int intfoffset = 0;
	word type = (word)g->type;
	byte shld;

	if (g->aiType == MONS_BOUAPHA)
	{
		if (player.weapon == WPN_PWRARMOR)
			type = MONS_PWRBOUAPHA;
		else if (player.weapon == WPN_MINISUB)
			type = MONS_PWRBOUAPHA;
		else if (type == MONS_BOUAPHA)
		{
			if (player.playAs == PLAY_LUNATIC)
				type = MONS_DRL;
			else if (player.playAs == PLAY_HAPPY)
				type = MONS_STICKMAN;
			else if (player.playAs == PLAY_MECHA)
				type = MONS_PLAYMECHA;
			else if (player.playAs == PLAY_SHROOM)
				type = MONS_PLAYSHROOM;
			else if (player.playAs == PLAY_LUNACHIK)
				type = MONS_LUNACHICK;
		}
	}

	LoadMySprite(type);

	if (set == NULL)
		set = monsType[type].spr;

	v = monsType[type].anim[g->seq][g->frm];

	if (editing)
	{
		if (g->type >= MONS_SUCKER1 && g->type <= MONS_BLOWER4 && g->seq == ANIM_IDLE && g->frm == 0) // suckers and blowers are visible in editor
			v = 0;
	}

	if (v == FRAME_INVIS || v == FRAME_END) // 255 = end of frame
		return;	// don't draw this frame

	if (!(monsType[g->type].flags & MF_ONEFACE))
		v += g->facing * monsType[type].framesPerDir;


	if ((type == MONS_BOUAPHA && PlayerHasHammer()) || type == MONS_EVILCLONE) // hammer man uses hammer sprites
		v += 8 * monsType[type].framesPerDir;




	if (g->type == MONS_BOUAPHA) // do player stuff
	{
		// energy shield
		shld = PlayerShield();
		if (shld > 0 && (shld > 16 || shld % 2 != 0))
		{
			curSpr = monsType[MONS_BOUAPHA].spr->GetSprite(464 + (shld & 7)); // account for shield
		}
		else
		{
			curSpr = set->GetSprite(v);
		}

		if (curSpr == NULL)
			return;

		if (!(monsType[type].flags & MF_NOSHADOW))
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, 0, 255, 0, curSpr, DISPLAY_DRAWME | DISPLAY_SHADOW);

		if (shld) // energy shield
			SprDraw(g->x >> FIXSHIFT, (g->y >> FIXSHIFT) + 1, 1 + (g->z >> FIXSHIFT), 255, g->bright, curSpr, DISPLAY_DRAWME | DISPLAY_GLOW);

		// invisibility
		shld = player.invisibility;
		if (shld > 0 && (shld > 16 || shld%2!=0))
		{
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 255, g->bright, curSpr, DISPLAY_DRAWME | DISPLAY_GLOW);
			return;
		}
	}

	if (monsType[type].flags&MF_FACECMD) // used for one-directional monsters
		v += g->facing;

	curSpr = set->GetSprite(v);

	if (!curSpr)
		return;

	if (!(monsType[type].flags & MF_NOSHADOW))
		SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, 0, 255, 0, curSpr, DISPLAY_DRAWME | DISPLAY_SHADOW);

	byte bright		= monsType[type].brtChg;

	if (!g->ouch)
	{
		if (g->frozen)
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 7, g->bright+bright+4, curSpr, DISPLAY_DRAWME);
		else if (g->slow)
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 0, g->bright+bright+4, curSpr, DISPLAY_DRAWME);
		else if (g->ignite)
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 5, g->bright+bright+4, curSpr, DISPLAY_DRAWME);
		else if (g->poison)
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 1, g->bright+bright+4, curSpr, DISPLAY_DRAWME);
		else if (!(monsType[type].flags & (MF_GHOST | MF_GLOW)))
		{
			if (g->fromColor != 255)
			{
				SprDrawOff(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, g->fromColor, g->toColor, g->bright + bright, curSpr, DISPLAY_DRAWME);
			}
			else if (monsType[type].fromCol != 255)
			{
				SprDrawOff(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, monsType[type].fromCol, monsType[type].toCol, g->bright + bright, curSpr, DISPLAY_DRAWME);
			}
			else
			{
				SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 255, g->bright, curSpr, DISPLAY_DRAWME);
			}
		}
		else if (monsType[type].flags & MF_GHOST)
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 255, g->bright + bright, curSpr, DISPLAY_DRAWME | DISPLAY_GHOST);
		else if (monsType[type].flags & MF_GLOW)
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 255, g->bright + bright, curSpr, DISPLAY_DRAWME | DISPLAY_GLOW);
	}
	else
	{
		if (!(monsType[type].flags & (MF_GHOST | MF_GLOW)))
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 4, g->bright, curSpr, DISPLAY_DRAWME);
		else if (monsType[type].flags & MF_GHOST)
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 4, g->bright + bright, curSpr, DISPLAY_DRAWME | DISPLAY_GHOST);
		else if (monsType[type].flags & MF_GLOW)
			SprDraw(g->x >> FIXSHIFT, g->y >> FIXSHIFT, g->z >> FIXSHIFT, 4, g->bright + bright, curSpr, DISPLAY_DRAWME | DISPLAY_GLOW);
	}

	int iconSpacing = 10; // pixels between icons
	int baseZ = g->z + ((monsType[g->type].height + 8) * FIXAMT);
	int imgs = 0;
	int baseX = (g->x >> FIXSHIFT) - (imgs * 16);

	auto DrawStatusIcon = [&](bool active, int sprite)
	{
		if (!active)
			return;
		SprDraw(
			baseX + imgs * 16,
			g->y >> FIXSHIFT,
			baseZ >> FIXSHIFT,
			255,
			0,
			intFace->get()->GetSprite(sprite),
			DISPLAY_DRAWME
		);
		imgs++;
	};

	DrawStatusIcon(g->poison, 121);
	DrawStatusIcon(g->frozen, 122);
	DrawStatusIcon(g->ignite, 123);
	DrawStatusIcon(g->slow, 124);
	DrawStatusIcon(g->weaken, 125);
	DrawStatusIcon(g->speed, 126);
	DrawStatusIcon(g->strength, 127);
}

void MonsterDraw(
	int x, int y, int z,
	dword type, bool isBouapha,
	byte seq, byte frm, byte facing,
	char bright,
	byte ouch, byte poison, byte frozen,
	byte fromCol, byte toCol, uint8_t brtChg,
	const sprite_set_t* set,
	byte flags
)
{
	const sprite_t *curSpr;
	int v;
	int intfx=0;
	int intfoffset=0;
	byte shld;

	// load if not loaded
	LoadMySprite(type);

	if (set == NULL)
		set = monsType[type].spr;

	v=monsType[type].anim[seq][frm];

	// Suckers and Blowers are visible in the editor.
	if (type >= MONS_SUCKER1 && type <= MONS_BLOWER4 && editing && seq == ANIM_IDLE && frm == 0)
		v = 0;

	if(v==FRAME_INVIS || v==FRAME_END)
		return;	// don't draw this frame

	if(!(monsType[type].flags&MF_ONEFACE))
		v+=facing*monsType[type].framesPerDir;

	if(isBouapha)
	{
		if(type==MONS_BOUAPHA && PlayerHasHammer())
			v+=8*monsType[type].framesPerDir;
		shld=PlayerShield();
		if((shld<16) && (shld&2))	// it blinks when there is 1/2 second left
			shld=0;
		curSpr=monsType[MONS_BOUAPHA].spr->GetSprite(464+(shld&7));
		if(curSpr==NULL)
			return;
		if(shld)
			SprDraw(x>>FIXSHIFT,(y>>FIXSHIFT)+1,1+(z>>FIXSHIFT),255,bright,curSpr,DISPLAY_DRAWME|DISPLAY_GLOW);
		if(player.invisibility)
		{
			curSpr=set->GetSprite(v);
			if(!curSpr)
				return;
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright,curSpr,DISPLAY_DRAWME|DISPLAY_GLOW);
			return;
		}
	}

	if(type==MONS_EVILCLONE)
		v+=8*monsType[type].framesPerDir;

	if(monsType[type].flags&MF_FACECMD)
		v+=facing;

	curSpr=set->GetSprite(v);
	if(!curSpr)
		return;

	if(!(monsType[type].flags&MF_NOSHADOW))
		SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,0,255,0,curSpr,DISPLAY_DRAWME|DISPLAY_SHADOW);

	if(ouch==0)
	{
		if(frozen)
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,7,bright+4,curSpr,DISPLAY_DRAWME);
		else if(poison)
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,1,bright,curSpr,DISPLAY_DRAWME);
		else if(!(monsType[type].flags&(MF_GHOST|MF_GLOW)))
		{
			if (fromCol == 255)
			{
				fromCol = monsType[type].fromCol;
				toCol = monsType[type].toCol;
			}
			if (fromCol == 255)
				SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright+brtChg,curSpr,DISPLAY_DRAWME);
			else
			{
				SprDrawOff(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,fromCol,toCol,
					bright+brtChg,curSpr,DISPLAY_DRAWME);
			}
		}
		else if(monsType[type].flags&MF_GHOST)
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright+brtChg,curSpr,DISPLAY_DRAWME|DISPLAY_GHOST);
		else if(monsType[type].flags&MF_GLOW)
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,255,bright+brtChg,curSpr,DISPLAY_DRAWME|DISPLAY_GLOW);
	}
	else
	{
		if(poison)
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,5,bright,curSpr,DISPLAY_DRAWME);
		else if(frozen)
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,6,bright+8,curSpr,DISPLAY_DRAWME);
		else
			SprDraw(x>>FIXSHIFT,y>>FIXSHIFT,z>>FIXSHIFT,4,bright,curSpr,DISPLAY_DRAWME);

	}
	if (flags)
	{

	}
}

void InstaRenderMonster(int x,int y,dword type,char bright,MGLDraw *mgl)
{
	sprite_t *curSpr;
	int v;

	// load if not loaded
	if (!LoadMySpriteMaybe(type))
		return;

	v=monsType[type].anim[ANIM_IDLE][0];
	if(!(monsType[type].flags&MF_ONEFACE))
		v+=2*monsType[type].framesPerDir;

	// Suckers and Blowers are visible in the editor.
	if (type >= MONS_SUCKER1 && type <= MONS_BLOWER4 && editing /* seq == ANIM_IDLE && frame == 0 are implied */)
		v = 0;

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return;

	if(monsType[type].fromCol==255)
		curSpr->DrawBright(x,y,mgl,bright+monsType[type].brtChg);
	else
		curSpr->DrawOffColor(x,y,mgl,monsType[type].fromCol,monsType[type].toCol,
				bright+monsType[type].brtChg);
}

int InstaRenderScannedMonster(int x,int y,dword type,char bright,MGLDraw *mgl)
{
	sprite_t *curSpr;
	int v;

	// load if not loaded
	if (!LoadMySpriteMaybe(type))
	{
		CenterPrint(x, y - 16, "No graphics!", 0, 1);
		return 0;
	}

	v=monsType[type].anim[ANIM_IDLE][0];
	if(!(monsType[type].flags&MF_ONEFACE))
		v+=2*monsType[type].framesPerDir;

	curSpr=monsType[type].spr->GetSprite(v);
	if(!curSpr)
		return 10;

	curSpr->DrawColored(x,y,mgl,1,bright);

	return curSpr->height;
}

MonsterThemes MonsterTheme(dword type)
{
	return monsType[type].theme;
}

// AI auxiliary functions to make it simple
//---------------------------------------------

void FaceGoodguy(Guy *me,Guy *goodguy)
{
	if(goodguy->x<me->x-FIXAMT*16)
	{
		if(goodguy->y<me->y-FIXAMT*16)
			me->facing=5;
		else if(goodguy->y>me->y+FIXAMT*16)
			me->facing=3;
		else
			me->facing=4;
	}
	else if(goodguy->x>me->x+FIXAMT*16)
	{
		if(goodguy->y<me->y-FIXAMT*16)
			me->facing=7;
		else if(goodguy->y>me->y+FIXAMT*16)
			me->facing=1;
		else
			me->facing=0;
	}
	else
	{
		if(goodguy->y<me->y-FIXAMT*16)
			me->facing=6;
		else if(goodguy->y>me->y+FIXAMT*16)
			me->facing=2;
	}
}

void FacePoint(Guy* me, int x, int y)
{
	if (x < me->x - FIXAMT * 16)
	{
		if (y < me->y - FIXAMT * 16)
			me->facing = 5;
		else if (y > me->y + FIXAMT * 16)
			me->facing = 3;
		else
			me->facing = 4;
	}
	else if (x > me->x + FIXAMT * 16)
	{
		if (y < me->y - FIXAMT * 16)
			me->facing = 7;
		else if (y > me->y + FIXAMT * 16)
			me->facing = 1;
		else
			me->facing = 0;
	}
	else
	{
		if (y < me->y - FIXAMT * 16)
			me->facing = 6;
		else if (y > me->y + FIXAMT * 16)
			me->facing = 2;
	}
}

int RangeToTarget(Guy *me,Guy *goodguy)
{
	return abs(me->x-goodguy->x)+abs(me->y-goodguy->y);
}

// this version doesn't insta-face, it rotates toward the right facing, and it has much
// more leeway than the 16 pixels of the other (it's for bigger creatures)
void FaceGoodguy2(Guy *me,Guy *goodguy)
{
	int desired;
	int diff,dir;

	if(goodguy->x<me->x-FIXAMT*32)
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=5;
		else if(goodguy->y>me->y+FIXAMT*32)
			desired=3;
		else
			desired=4;
	}
	else if(goodguy->x>me->x+FIXAMT*32)
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=7;
		else if(goodguy->y>me->y+FIXAMT*32)
			desired=1;
		else
			desired=0;
	}
	else
	{
		if(goodguy->y<me->y-FIXAMT*32)
			desired=6;
		else
			desired=2;
	}

	if(desired==me->facing)
		return;

	if(desired>me->facing)
	{
		diff=desired-me->facing;
		if(diff>4)
		{
			dir=-1;
		}
		else
			dir=1;
	}
	else
	{
		diff=me->facing-desired;
		if(diff>4)
		{
			dir=1;
		}
		else
			dir=-1;
	}
	me->facing=(me->facing+dir)&7;
}

void FaceGoodguy3(Guy *me,Guy *goodguy)
{
	int desired;
	int diff,dir;

	if(abs(me->x-goodguy->x)+abs(me->y-goodguy->y)>FIXAMT*72)
	{
		FaceGoodguy2(me,goodguy);
		return;
	}

	if(goodguy->x<me->x-FIXAMT*16)
	{
		if(goodguy->y<me->y-FIXAMT*16)
			desired=5;
		else if(goodguy->y>me->y+FIXAMT*16)
			desired=3;
		else
			desired=4;
	}
	else if(goodguy->x>me->x+FIXAMT*16)
	{
		if(goodguy->y<me->y-FIXAMT*16)
			desired=7;
		else if(goodguy->y>me->y+FIXAMT*16)
			desired=1;
		else
			desired=0;
	}
	else
	{
		if(goodguy->y<me->y-FIXAMT*16)
			desired=6;
		else
			desired=2;
	}

	if(desired==me->facing)
		return;

	if(desired>me->facing)
	{
		diff=desired-me->facing;
		if(diff>4)
		{
			dir=-1;
		}
		else
			dir=1;
	}
	else
	{
		diff=me->facing-desired;
		if(diff>4)
		{
			dir=1;
		}
		else
			dir=-1;
	}
	me->facing=(me->facing+dir)&7;
}

// this is only used for The Thing's tentacles, to keep their flailing within quadrants
void FlailLock(Guy *me)
{
	byte parentangle;
	byte diff;
	char dir;

	if(!me->parent || me->parent->type==MONS_NONE)
		return;	// no good

	if(me->parent->aiType!=MONS_THINGTENT)
	{
		switch(me->mind)	// which quadrant of the tentacles is he?
		{
			case 0:	// lower left
				parentangle=6;
				break;
			case 1:	// lower right
				parentangle=2;
				break;
			case 2:	// upper right
				parentangle=14;
				break;
			case 3:	// upper left
			default:
				parentangle=10;
				break;
		}
	}
	else
	{
		parentangle=me->parent->facing;
	}

	if(parentangle>me->facing)
	{
		diff=parentangle-me->facing;
		if(diff>8)
		{
			dir=1;
			diff=16-diff;
		}
		else
			dir=-1;
	}
	else
	{
		diff=me->facing-parentangle;
		if(diff>8)
		{
			dir=-1;
			diff=16-diff;
		}
		else
			dir=1;
	}
	if(diff>2)
	{
		me->facing=(parentangle+dir*2)&15;
	}
}
