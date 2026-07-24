#include "winpch.h"
#include "special.h"
#include "monster.h"
#include "repair.h"
#include "guy.h"
#include "message.h"
#include "sound.h"
#include "game.h"
#include "dialogbits.h"
#include "vars.h"
#include "editor.h"
#include "world.h"
#include "shop.h"
#include "goal.h"
#include "palettes.h"
#include "chat.h"
#include "world.h"
#include "worldselect.h"
#include "particle.h"
#include "display.h"

static std::span<special_t> spcl;			// Local specials
static std::span<special_t> spclGlobal;		// Global specials
static byte	globalspcluses[128];			// save the # of uses for globals - resets each level

static int numSpecials;  // Specials >= this aren't set.
static int numSpecialsGlobal;

void InitSpecials(std::span<special_t> list)
{
	spcl = list;
	numSpecials = 0;
	for (special_t &me : list)
	{
		me.x = 255;
	}
}

// Fires upon starting up a world, after loading specials.
// Used for storing the # of uses per global special, which
// resets upon the start of each level.
void FillGlobalSpecialUseData(std::span<special_t> list)
{
	int i;
	printf("\n--- FillGlobalSpecialUseData---\n");

	for (i=0;i<128;i++)
	{
		byte b = spclGlobal[i].uses;
		if (spclGlobal[i].x==255) // blank!
			continue;
		globalspcluses[i] = spclGlobal[i].uses;
		printf("SPCL #%03d = %d Uses\n", i, globalspcluses[i]);
		numSpecialsGlobal = i+1;
	}

	printf("# of Global Specials:%d\n", numSpecialsGlobal);

	printf("--- DONE! ---\n");
}

// Initializes blank global special data
void InitGlobalSpecials(std::span<special_t> list)
{
	printf("\n--- InitGlobalSpecials ---\n");
	spclGlobal = list;
	printf("\n--- DONE! ---\n");
}

void GetSpecialsFromMap(std::span<special_t> list)
{
	spcl = list;
	numSpecials = 0;
	for (size_t i = 0; i < spcl.size(); i++)
	{
		special_t &me = spcl[i];
		if(me.x!=255)
		{
			numSpecials=i+1;
			for (effect_t &eff : me.effect)
				if(eff.type==EFF_SUMMON && eff.value2>ITM_RANDOM)
				{
					eff.value2=ITM_NONE;
				}
		}
	}
}

void GetSpecialsFromWorld(std::span<special_t> list)
{

}

void GetSpecialsFromMapAndWorld(std::span<special_t> mapList, std::span<special_t> worldList)
{
	int i;
	printf("\n--- Get Specials Form Map and World ---\n");
	GetSpecialsFromMap(mapList);
	for (i = 0;i < numSpecialsGlobal;i++)
	{
		printf("\nSPCL #%03d: %d -> %d uses.\n",i, spclGlobal[i].uses, globalspcluses[i]);
		spclGlobal[i].uses	= globalspcluses[i];
		spclGlobal[i].x = i;
	}

	printf("--- DONE! ---\n");

	//GetSpecialsFromWorld(worldList);
}

int NewSpecial(byte x,byte y)
{
	for(int i=0;i<(int)spcl.size();i++)
		if(spcl[i].x==255)
		{
			memset(&spcl[i],0,sizeof(special_t));
			spcl[i].x=x;
			spcl[i].y=y;
			spcl[i].uses=1;
			spcl[i].color=0; // set to gray
			if(i>=numSpecials)
				numSpecials=i+1;
			return i;
		}

	return -1;
}

int GetSpecial(byte x,byte y)
{
	int i;

	for(i=0;i<numSpecials;i++)
		if(spcl[i].x==x && spcl[i].y==y)
			return i;

	return -1;
}

special_t *GetSpecial(int i)
{
	return (i >= 0 && i < numSpecials) ? &spcl[i] : NULL;
}

special_t* GetGlobalSpecial(int i)
{
	return (i >= 0 && i < 128) ? &EditorGetWorld()->special[i] : NULL;
}

void DeleteSpecial(int i)
{
	int j;

	if(i>=0 && i<numSpecials)
	{
		spcl[i].x=255;
		for(j=i;j<numSpecials-1;j++)
			spcl[j]=spcl[j+1];
		spcl[numSpecials-1].x=255;
		numSpecials--;
	}
}

void DefaultTrigger(trigger_t *trig,int x,int y)
{
	// clear flags like LESS and MORE, but keep NOT and AND
	trig->flags &= (TF_NOT | TF_AND);
	trig->x=x;
	trig->y=y;
	switch(trig->type)
	{
		case TRG_STEP:
			trig->value=MONS_BOUAPHA;
			trig->value2=0;
			break;
		case TRG_STEPRECT:
			trig->value=MONS_BOUAPHA;
			trig->value2=x+y*256;
			break;
		case TRG_HAVEITEM:
			trig->value=ITM_HAMMERUP;
			trig->value2=1;
			break;
		case TRG_PASSLEVELS:
			trig->value=1;
			break;
		case TRG_PASSLEVEL:
			trig->value=0;
			break;
		case TRG_SHOOT:
			trig->value=0;
			break;
		case TRG_TIMED:
			trig->value=0;
			trig->value2=0;
			break;
		case TRG_DELAY:
			trig->value=0;
			break;
		case TRG_MONSTER:
			trig->value=MONS_ANYBODY;
			trig->value2=0;
			break;
		case TRG_CHAIN:
			break;
		case TRG_KILL:
			trig->value=MONS_ANYBODY;
			break;
		case TRG_VAR:
			trig->value=0;
			trig->value2=0;
			break;
		case TRG_FLOOR:
			trig->value=0;
			break;
		case TRG_RANDOM:
			trig->value=0;
			break;
		case TRG_LIFE:
			trig->value=MONS_BOUAPHA;
			trig->value2=0;
			trig->x=255;
			break;
		case TRG_STEPTILE:
			trig->value=MONS_BOUAPHA;
			trig->value2=0;
			break;
		case TRG_GETITEM:
			trig->value=0;
			break;
		case TRG_ITEM:
			trig->value=0;
			break;
		case TRG_AWAKE:
			trig->value=MONS_BOUAPHA;
			trig->x=255;
			break;
		case TRG_ITEMS:
			trig->value=ITM_HAMMERUP;
			trig->value2=1;
			break;
		case TRG_COMPMAP:
			trig->value2=x+y*256;
			trig->value=0;
			break;
		case TRG_COMPVAR:
			trig->value=0;
			trig->value2=0;
			break;
		case TRG_MONSINRECT:
			trig->value=MONS_BOUAPHA;
			trig->value2=0;
			break;
		case TRG_ITEMRECT:
			trig->value=ITM_HAMMERUP;
			break;
		case TRG_DIFFICULTY:
			trig->value=0;
			break;
		case TRG_KEYPRESS:
			trig->value=CONTROL_UP;
			break;
		case TRG_PLAYAS:
			trig->value=PLAY_BOUAPHA;
			break;
		case TRG_MONSCOLOR:
			trig->value=MONS_BOUAPHA;
			trig->x=255;
			trig->value2=0;
			break;
		case TRG_EQUATION:
		case TRG_EQUVAR:
			trig->value=0;
			break;
		case TRG_BULLETRECT:
			trig->value=BLT_HAMMER;
			trig->value2=0;
			break;
		case TRG_TIMER:
			trig->value = 0;
			break;
		case TRG_HURT:
			trig->value = MONS_BOUAPHA;
			trig->value2 = 0;
			trig->x = 255;
			break;
		case TRG_AFFLICT:
			trig->value = MONS_BOUAPHA;
			trig->value2 = 0;
			trig->x = 255;
			break;
		case TRG_RAGEBAR:
			trig->value = 0;
			break;
		case TRG_PROXIMITY:
			trig->value = MONS_BOUAPHA;
			trig->value2 = 0;
			trig->x = 255;
			break;
		case TRG_LINESIGHT:
			trig->value = MONS_BOUAPHA;
			trig->x = 255;
			break;
		case TRG_CHAINCOLOR:
			trig->value = 0;
			trig->value2 = 0;
			break;
		case TRG_MONSAGE:
			trig->value = MONS_BOUAPHA;
			trig->value2 = 0;
			trig->x = 255;
			break;
	}
}

void DefaultEffect(effect_t *eff,int x,int y,byte savetext)
{
	eff->flags={};
	eff->x=x;
	eff->y=y;
	if(!savetext)
		eff->text[0]='\0';
	switch(eff->type)
	{
		case EFF_SOUND:
			eff->value=1;
			break;
		case EFF_SONG:
			eff->value=0;
			strcpy(eff->text,"");
			break;
		case EFF_WINLEVEL:
		case EFF_GOTOMAP:
			eff->value=0;
			eff->x=255;
			break;
		case EFF_CHANGETILE:
			eff->value=0;
			eff->value2=0;
			break;
		case EFF_SUMMON:
			eff->value=MONS_ANYBODY;
			eff->value2=ITM_RANDOM;
			break;
		case EFF_PICTURE:
			eff->value=TEXTFILE_NORMAL;
			strcpy(eff->text,"");
			break;
		case EFF_LIGHT:
			eff->value=1;
			eff->value2=1;
			break;
		case EFF_ITEM:
			eff->value=1;
			break;
		case EFF_SWAPMAP:
			eff->value=x+y*256;
			eff->value2=x+y*256;
			break;
		case EFF_KILLMONS:
			eff->value=MONS_ANYBODY;
			eff->x=255;
			break;
		case EFF_CHANGEMONS:
			eff->value=MONS_ANYBODY;
			eff->value2=MONS_BAT;
			eff->x=255;
			break;
		case EFF_CHANGETEAM:
			eff->value=MONS_ANYBODY;
			eff->value2=2;
			eff->x=255;
			break;
		case EFF_VAR:
			eff->value=0;
			strcpy(eff->text,"0");
			break;
		case EFF_LEVELFLAG:
			eff->value=0;
			eff->value2=0;
			break;
		case EFF_OLDTOGGLE:
			eff->value=0;
			eff->value2=0;
			break;
		case EFF_LIFE:
			eff->value=MONS_ANYBODY;
			eff->value2=0;
			eff->x=255;
			break;
		case EFF_LIGHTRECT:
			eff->value=x+y*256;
			eff->value2=0;
			break;
		case EFF_WEAPON:
			eff->value=0;
			break;
		case EFF_TAGMONS:
			eff->value=MONS_ANYBODY;
			eff->x=255;
			break;
		case EFF_MONSITEM:
			eff->value=MONS_ANYBODY;
			eff->x=255;
			eff->value2=ITM_RANDOM;
			break;
		case EFF_TILEVAR:
			eff->value=0;
			break;
		case EFF_LIFEAMT:
			eff->value=MONS_ANYBODY;
			eff->value2=0;
			eff->x=255;
			break;
		case EFF_AI:
			eff->value=MONS_ANYBODY;
			eff->value2=MONS_BAT;
			eff->x=255;
			break;
		case EFF_NAME:
		case EFF_MONSGRAPHICS:
			eff->value=MONS_ANYBODY;
			eff->x=255;
			eff->text[0]='\0';
			break;
		case EFF_COLOR:
			eff->value=MONS_ANYBODY;
			eff->value2=0;
			eff->x=255;
			break;
		case EFF_MONSBRIGHT:
			eff->value=MONS_ANYBODY;
			eff->value2=0;
			eff->x=255;
			break;
		case EFF_PLAYAS:
			eff->value=MONS_BOUAPHA;
			break;
		case EFF_VARBAR:
			eff->value=0;
			break;
		case EFF_MAKEBULLET:
			strcpy(eff->text, "0");
			eff->value2=1;
			[[fallthrough]];
		case EFF_CHANGEBULLET:
			eff->value=BLT_NONE;
			eff->value2=BLT_HAMMER;
			eff->x=255;
			break;
		case EFF_CHAT:
			eff->value=TEXTFILE_NORMAL;
			strcpy(eff->text,"");
			break;
		case EFF_TIMER:
			eff->value = 1;
			break;
		case EFF_CAMERAFOCUS:
			eff->x = 0;
			eff->y = 0;
			break;
		case EFF_CAMERAGUY:
			eff->value = MONS_ANYBODY;
			break;
		case EFF_CAMERASCROLL:
			eff->value		= 0;
			eff->value2		= 0;
			break;
		case EFF_MOVESPCL:
			eff->x			= 0;
			eff->y			= 0;
			eff->value		= 1;
			eff->value2		= 1;
			break;
		default:
			break;
	}
}

int SlideCoord(int coord,int delta,int max)
{
	coord+=delta;
	if(coord<0)
		coord+=max;
	if(coord>=max)
		coord-=max;

	return coord;
}

void SlideCombinedCoord(int *combo,int dx,int dy,int width,int height)
{
	int x,y;

	x=(*combo)%256;
	y=(*combo)/256;

	x=SlideCoord(x,dx,width);
	y=SlideCoord(y,dy,height);

	*combo=(x+y*256);
}

void SlideSpecials(Map *map,int dx,int dy)
{
	for (special_t &me : map->special)
	{
		if(me.x!=255)
		{
			int offX=-me.x;
			int offY=-me.y;

			me.x=(byte)SlideCoord(me.x,dx,map->width);
			me.y=(byte)SlideCoord(me.y,dy,map->height);

			offX+=me.x;
			offY+=me.y;

			AdjustSpecialCoords(&me,offX,offY);
			AdjustSpecialEffectCoords(&me,offX,offY);
		}
	}
}

void RepairSpecialToItem(std::span<special_t> list, int n)
{
	for (special_t &me : list)
	{
		if(me.x!=255)
		{
			for (trigger_t &trg : me.trigger)
			{
				if(trg.type==TRG_HAVEITEM || trg.type==TRG_ITEM || trg.type==TRG_ITEMS || trg.type==TRG_ITEMRECT)
				{
					if(trg.value==n)
						trg.value=0;
					else if(trg.value>n)
						trg.value--;
				}
			}
			for (effect_t &eff : me.effect)
			{
				if(eff.type==EFF_ITEM)
				{
					if(eff.value==n)
						eff.value=0;
					else if(eff.value>n)
						eff.value--;
				}
				if(eff.type==EFF_SUMMON)
				{
					if(eff.value2==n)
						eff.value2=0;
					else if(eff.value2>n && eff.value2!=ITM_RANDOM)
						eff.value2--;
				}
				if(eff.type==EFF_MONSITEM)
				{
					if(eff.value2==n)
						eff.value2=0;
					else if(eff.value2>n && eff.value2!=ITM_RANDOM)
						eff.value2--;
				}
			}
		}
	}
}

void RepairSpecialToSound(std::span<special_t> list, int n)
{
	for (special_t &me : list)
	{
		if(me.x!=255)
		{
			for (effect_t &eff : me.effect)
			{
				if(eff.type==EFF_SOUND)
				{
					if(eff.value==n)
						eff.value=0;
					else if(eff.value>n)
						eff.value--;
				}
			}
		}
	}
}

void RepairSpecialToLevel(std::span<special_t> list, const SwapTable &table)
{
	for (special_t &me : list)
	{
		if(me.x!=255)
		{
			for (trigger_t &trg : me.trigger)
			{
				if(trg.type==TRG_PASSLEVEL)
				{
					trg.value=table.GetSwap(trg.value);
				}
			}
			for (effect_t &eff : me.effect)
			{
				if(eff.type==EFF_WINLEVEL || eff.type==EFF_GOTOMAP)
				{
					eff.value=table.GetSwap(eff.value);
				}
			}
		}
	}
}

void RepairSpecialToTile(std::span<special_t> list, const SwapTable &table)
{
	for (special_t &me : list)
	{
		if(me.x!=255)
		{
			for (trigger_t &trg : me.trigger)
			{
				if(trg.type==TRG_FLOOR || trg.type==TRG_FLOORRECT)
				{
					trg.value=table.GetSwap(trg.value);
				}
				else if(trg.type==TRG_STEPTILE)
				{
					trg.value2=table.GetSwap(trg.value2);
				}
			}
			for (effect_t &eff : me.effect)
			{
				if(eff.type==EFF_CHANGETILE)
				{
					eff.value=table.GetSwap(eff.value);
					if(eff.value2!=0)
						eff.value2=table.GetSwap(eff.value2);
				}
				if(eff.type==EFF_OLDTOGGLE)
				{
					eff.value=table.GetSwap(eff.value);
					if(eff.value2!=0)
						eff.value2=table.GetSwap(eff.value2);
				}
			}
		}
	}
}

byte CheckTimer(int seconds, byte flags)
{
	if (player.timer == seconds)
		return 1;
	else if ((flags & TF_LESS) && player.timer < seconds)
		return 1;
	else if ((flags & TF_MORE) && player.timer > seconds)
		return 1;
	else
		return 0;
}


//------------------------------------  GAMEPLAY!
struct SpecialEvent
{
	Guy *victim;
	int  value;	// guy #, bullet #, var #, item #
	int  x,y;
	byte type;
	byte guyType;
	byte guyFriendly;
};

static constexpr int MAX_EVENT = 128;
static SpecialEvent events[MAX_EVENT];
static int nextEvent;
static Guy *victim, *tagged;

Guy *TaggedMonster(void)
{
	return tagged;
}

static byte TeleportGuy(Guy *victim,int x,int y,Map *map,byte noFX, bool sphinxException = false)
{
	if(x<0 || y<0 || x>(map->width*TILE_WIDTH*FIXAMT) || y>(map->height*TILE_HEIGHT*FIXAMT))
		return 0;	// invalid location

	if(victim==NULL)
		return 0;	// nobody to teleport

	if(victim->aiType==MONS_NOBODY)
		return 0;	// NOBODY to teleport

	if(victim->aiType==MONS_BOUAPHA)
	{
		if(GetTportClock())
			return 0;	// can't teleport yet
	}
	if(!noFX)
		MakeSound(SND_TELEPORT,victim->x,victim->y,SND_CUTOFF,1500);
	victim->x=(x*TILE_WIDTH+(TILE_WIDTH/2))<<FIXSHIFT;
	victim->y=(y*TILE_HEIGHT+(TILE_HEIGHT/2))<<FIXSHIFT;
	victim->dx=0;
	victim->dy=0;
	if(victim->aiType==MONS_BOUAPHA)
	{
		PutCamera(victim->x,victim->y);
		UpdateCamera(victim->x>>FIXSHIFT,victim->y>>FIXSHIFT,victim->dx,victim->dy,map);
		SetTportClock(15);
		if(shopping)
			SetTportClock(2);
	}
	if(!noFX)
	{
		map->GetTile(x,y)->templight=34;
		if(x>0)
			map->GetTile(x-1,y)->templight=20;
		if(x<map->width-1)
			map->GetTile(x+1,y)->templight=20;
		if(y>0)
			map->GetTile(x,y-1)->templight=20;
		if(y<map->height-1)
			map->GetTile(x,y+1)->templight=20;
	}

	victim->mapx=x;
	victim->mapy=y;
	Telefrag(victim, sphinxException);
	EventOccur(EVT_STEP,0,x,y,victim);
	return 1;
}

void ClearEvents(void)
{
	memset(events,0,sizeof(events));
	nextEvent=0;
}

void EventOccur(byte type,int value,int x,int y,Guy *victim)
{
	if(nextEvent>=MAX_EVENT)
		return;
	events[nextEvent].type=type;
	events[nextEvent].value=value;
	events[nextEvent].x=x;
	events[nextEvent].y=y;
	if(victim)
	{
		events[nextEvent].guyFriendly=victim->friendly;
		events[nextEvent].guyType=victim->type;
		events[nextEvent].victim=victim;
	}
	else
	{
		events[nextEvent].guyFriendly=0;
		events[nextEvent].guyType=MONS_NONE;
		events[nextEvent].victim=NULL;
	}
	nextEvent++;
}

byte CheckForItem(byte item,int count,byte flags)
{
	int amt;

	amt=0;
	if(GetItem(item)->effectAmt<0)
		return 0;	// not valid to look for anti-items

	switch(GetItem(item)->effect)
	{
		case IE_HEAL:	// health
		case IE_WEIRD:
		case IE_HEALRANDOM:
			amt=player.life;
			break;
		case IE_WEAPON:	// weapons
			if(GetCurrentWeaponType()==GetItem(item)->effectAmt)
				amt=GetCurrentWeaponAmmo();
			else
				amt=0;
			break;
		case IE_HAMMER: // hammers
			amt=player.hammers;
			break;
		case IE_PANTS:	// pants
			amt=(16-player.hamSpeed)/4;
			break;
		case IE_KEY:	// yellow keys
			amt=player.keys[0];
			break;
		case IE_RKEY:	// red keys
			amt=player.keys[1];
			break;
		case IE_GKEY:	// green keys
			amt=player.keys[2];
			break;
		case IE_BKEY:	// blue keys
			amt=player.keys[3];
			break;
		case IE_LOONYKEY:
			if(count<2 && (flags&(TF_LESS|TF_MORE))==0)
			{
				if(player.worldProg->keychains&KC_LOONY)
					amt=1;
				else
					amt=0;
			}
			else
			{
				amt=profile.progress.loonyKeys;
			}
			break;
		case IE_COIN:
			amt=player.coins;
			break;
		case IE_CANDLE:
			amt=player.candles;
			break;
		case IE_OXYGEN:
			amt=player.oxygen/256;
			break;
		case IE_RAGE:
			amt=player.rage/256;
			break;
		case IE_POWERUP:
			switch(GetItem(item)->effectAmt)
			{
				case PU_REVERSE:
					if(player.hammerFlags&HMR_REVERSE)
						amt=1;
					else
						amt=0;
					break;
				case PU_REFLECT:
					if(player.hammerFlags&HMR_REFLECT)
						amt=1;
					else
						amt=0;
					break;
				case PU_SHIELD:
					amt=player.shield;
					break;
				case PU_GARLIC:
					amt=player.garlic;
					break;
				case PU_SPEED:
					amt=player.speed;
					break;
				case PU_INVISO:
					amt=player.invisibility;
					break;
			}
			break;
		case IE_BRAIN:
			amt=player.brains;
			break;
		case IE_KEYCHAIN:
			if(count==1 && !(flags&(TF_LESS|TF_MORE)))	// 1 exactly
			{
				amt=GetItem(item)->effectAmt;
				if(amt==0)
					amt=KC_KEYCH1;
				else if(amt==1)
					amt=KC_KEYCH2;
				else if(amt==2)
					amt=KC_KEYCH3;
				else if(amt==3)
					amt=KC_KEYCH4;

				if(player.worldProg->keychains&amt)
					return 1;
				else
					return 0;
			}
			else
			{
				amt=0;
				if(player.worldProg->keychains&KC_KEYCH1)
					amt++;
				if(player.worldProg->keychains&KC_KEYCH2)
					amt++;
				if(player.worldProg->keychains&KC_KEYCH3)
					amt++;
				if(player.worldProg->keychains&KC_KEYCH4)
					amt++;
			}
			break;
		case IE_ORBITER:
			amt = CountBullets(BLT_ORBITER);
			break;
		case IE_ORBITER2:
			amt = CountBullets(BLT_ORBITER2);
			break;
	}
	if(amt==count)
		return 1;
	if((flags&TF_LESS) && amt<count)
		return 1;
	if((flags&TF_MORE) && amt>count)
		return 1;

	return 0;
}

byte TriggerYes(special_t *me,trigger_t *t,Map *map)
{
	byte answer;
	int i,j;
	byte effNum;

	answer=0;
	switch(t->type)
	{
		case TRG_NONE:
			return 2;	// special code so it isn't counted
			break;
		case TRG_STEP:
			for(i=0;i<nextEvent;i++)
			{
				if(events[i].type==EVT_STEP && abs(events[i].x-t->x)+abs(events[i].y-t->y)<=t->value2)
				{
					switch(t->value)
					{
						case MONS_ANYBODY:
							answer=1;
							victim=GetGuy(events[i].value);
							break;
						case MONS_GOODGUY:
							if(events[i].guyFriendly==1)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_BADGUY:
							if(events[i].guyFriendly==0)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_NONPLAYER:
							if(events[i].guyType!=MONS_BOUAPHA)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_PLAYER:
							if(GetGuy(events[i].value)!=NULL && GetGuy(events[i].value)->aiType==MONS_BOUAPHA)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_TAGGED:
							if(GetGuy(events[i].value)==tagged)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						default:
							if(events[i].guyType==t->value)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
					}
				}
			}
			break;
		case TRG_MONSTER:
			i=CountMonsters(t->value);	// count how many of this type there are
			if(i==t->value2)
				answer=1;
			else
			{
				if((t->flags&TF_LESS) && i<t->value2)
					answer=1;
				else if((t->flags&TF_MORE) && i>t->value2)
					answer=1;
				else
					answer=0;
			}
			break;
		case TRG_PASSLEVELS:
			i=LevelsPassed(player.worldProg);
			if(i==t->value)
				answer=1;
			else
			{
				if((t->flags&TF_LESS) && i<t->value)
					answer=1;
				else if((t->flags&TF_MORE) && i>t->value)
					answer=1;
				else
					answer=0;
			}
			break;
		case TRG_PASSLEVEL:
			answer=LevelIsPassed(player.worldProg,t->value);
			break;
		case TRG_SHOOT:
			for(i=0;i<nextEvent;i++)
			{
				if(events[i].type==EVT_SHOOT && abs(events[i].x-t->x)<=1 && abs(events[i].y-t->y)<=1)
				{
					if(t->flags&TF_LESS)	// good bullets only
					{
						if(events[i].value==1)
						{
							answer=1;
							break;
						}
					}
					else if(t->flags&TF_MORE)
					{
						if(events[i].value==0)
						{
							answer=1;
							break;
						}
					}
					else
					{
						answer=1;
						break;
					}
				}
			}
			break;
		case TRG_HAVEITEM:
			answer=CheckForItem(t->value,t->value2,t->flags);
			break;
		case TRG_CHAIN:
			for(i=0;i<nextEvent;i++)
			{
				if(events[i].type==EVT_SPECIAL)
				{
					if(t->x==255 && t->y==255)	// any neighboring special will work
					{
						if(abs(events[i].x-me->x)<=1 && abs(events[i].y-me->y)<=1)
						{
							answer=1;
							break;
						}
					}
					else if(events[i].x==t->x && events[i].y==t->y)
					{
						answer=1;
						break;
					}
				}
			}
			break;
		case TRG_STEPRECT:
			for(i=0;i<nextEvent;i++)
			{
				if(events[i].type==EVT_STEP && PointInRect(events[i].x,events[i].y,t->x,t->y,(t->value2%256),(t->value2/256)))
				{
					switch(t->value)
					{
						case MONS_ANYBODY:
							answer=1;
							victim=GetGuy(events[i].value);
							break;
						case MONS_GOODGUY:
							if(events[i].guyFriendly==1)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_BADGUY:
							if(events[i].guyFriendly==0)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_NONPLAYER:
							if(events[i].guyType!=MONS_BOUAPHA)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_PLAYER:
							if(GetGuy(events[i].value)!=NULL && GetGuy(events[i].value)->aiType==MONS_BOUAPHA)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_TAGGED:
							if(GetGuy(events[i].value)==tagged)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						default:
							if(events[i].guyType==t->value)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
					}
				}
			}
			break;
		case TRG_KILL:
			for(i=0;i<nextEvent;i++)
			{
				if(events[i].type==EVT_DIE)
				{
					switch(t->value)
					{
						case MONS_ANYBODY:
							answer=1;
							victim=GetGuy(events[i].value);
							break;
						case MONS_GOODGUY:
							if(events[i].guyFriendly==1)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_BADGUY:
							if(events[i].guyFriendly==0)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_NONPLAYER:
							if(events[i].guyType!=MONS_BOUAPHA)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_PLAYER:
							if(GetGuy(events[i].value)!=NULL && GetGuy(events[i].value)->aiType==MONS_BOUAPHA)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_TAGGED:
							if(GetGuy(events[i].value)==tagged)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						default:
							if(events[i].guyType==t->value)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
					}
				}
			}
			break;
		case TRG_FLOOR:
			if(t->x<map->width && t->y<map->height && map->GetTile(t->x,t->y)->floor==t->value)
				answer=1;
			break;
		case TRG_VAR:
			answer=CompareVar(t->value,t->flags,t->value2);
			break;
		case TRG_TIMED:
			if((player.clock>=t->value2) && (t->value==0 || !((player.clock-t->value2)%t->value)))
				answer=1;
			break;
		case TRG_DELAY:
			if(player.clock>=t->value)
				answer=1;
			break;
		case TRG_RANDOM:
			if((int)Random(100*FIXAMT)<t->value)
				answer=1;
			break;
		case TRG_FLOORRECT:
			answer=1;
			for(i=t->x;i<=(t->value2%256);i++)
				for(j=t->y;j<=(t->value2/256);j++)
				{
					if(i<0 || j<0 || i>=map->width || j>=map->height || map->GetTile(i,j)->floor!=t->value)
					{
						answer=0;
						break;
					}
				}
			break;
		case TRG_LIFE:
			answer=CheckMonsterLife(t->x,t->y,t->value,t->value2,t->flags);
			break;
		case TRG_AWAKE:
			answer=CheckMonsterAwake(t->x,t->y,t->value,t->flags);
			break;
		case TRG_STEPTILE:
			for(i=0;i<nextEvent;i++)
			{
				if(events[i].type==EVT_STEP && map->GetTile(events[i].x,events[i].y)->floor==t->value2)
				{
					switch(t->value)
					{
						case MONS_ANYBODY:
							answer=1;
							victim=GetGuy(events[i].value);
							break;
						case MONS_GOODGUY:
							if(events[i].guyFriendly==1)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_BADGUY:
							if(events[i].guyFriendly==0)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_NONPLAYER:
							if(events[i].guyType!=MONS_BOUAPHA)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_PLAYER:
							if(GetGuy(events[i].value)!=NULL && GetGuy(events[i].value)->aiType==MONS_BOUAPHA)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_TAGGED:
							if(GetGuy(events[i].value)==tagged)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						default:
							if(events[i].guyType==t->value)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
					}
				}
			}
			break;
		case TRG_GETITEM:
			for(i=0;i<nextEvent;i++)
			{
				if(events[i].type==EVT_GET && events[i].x==t->x && events[i].y==t->y)
				{
					answer=1;
					break;
				}
			}
			break;
		case TRG_ITEM:
			if(map->GetTile(t->x,t->y)->item==t->value)
				answer=1;
			break;
		case TRG_ITEMS:
			i=map->ItemCount(t->value);
			if(i==t->value2)
				answer=1;
			else if(i>t->value2 && (t->flags&TF_MORE))
				answer=1;
			else if(i<t->value2 && (t->flags&TF_LESS))
				answer=1;
			else
				answer=0;
			break;
		case TRG_COMPMAP:
			answer=map->CompareRegions(t->x,t->y,(t->value2%256),(t->value2/256),(t->value%256),(t->value/256),((t->flags&TF_LESS)==0));
			break;
		case TRG_COMPVAR:
			i=GetVar(t->value);

			if(i==GetVar(t->value2))
				answer=1;
			else if(i>GetVar(t->value2) && (t->flags&TF_MORE))
				answer=1;
			else if(i<GetVar(t->value2) && (t->flags&TF_LESS))
				answer=1;
			else
				answer=0;
			break;
		case TRG_MONSINRECT:
			i=CountMonstersInRect(t->value,t->x,t->y,(t->value2%256),(t->value2/256));	// count how many of this type there are in the rect
			if(i>0)
				answer=1;
			else
				answer=0;
			break;
		case TRG_ITEMRECT:
			i=map->ItemCountInRect(t->value,t->x,t->y,(t->value2%256),(t->value2/256));	// count how many of this item there are in the rect
			if(i>0)
				answer=1;
			else
				answer=0;
			break;
		case TRG_DIFFICULTY:
			if(profile.difficulty==t->value)
				answer=1;
			else if(profile.difficulty<t->value && (t->flags&TF_LESS))
				answer=1;
			else if(profile.difficulty>t->value && (t->flags&TF_MORE))
				answer=1;
			else
				answer=0;
			break;
		case TRG_KEYPRESS:
			i=ControlCheck(t->value);
			if(t->flags&TF_LESS)
			{
				if(i==3)	// tapped
					answer=1;
				else
					answer=0;
			}
			else if(t->flags&TF_MORE)
			{
				if(i==2)	// just released
					answer=1;
				else
					answer=0;
			}
			else	// held
			{
				if(i==3 || i==1)	// tapped or held
					answer=1;
				else
					answer=0;
			}
			break;
		case TRG_PLAYAS:
			answer=(player.playAs==t->value);
			break;
		case TRG_MONSCOLOR:
			answer=CheckMonsterColor(t->x,t->y,t->value,(byte)t->value2);
			break;
		case TRG_EQUATION:
			effNum=0;
			for(i=0;i<NUM_TRIGGERS;i++)
				if(t==&me->trigger[i])
					effNum=i;

			if(!VarMath(255,me->effect[effNum].text))
					PauseGame();	// pause if there's an error in the equation

			if(t->value==GetVar(255))
				answer=1;
			else if(t->value<GetVar(255) && (t->flags&TF_MORE))
				answer=1;
			else if(t->value>GetVar(255) && (t->flags&TF_LESS))
				answer=1;
			else
				answer=0;
			break;
		case TRG_EQUVAR:
			effNum=0;
			for(i=0;i<NUM_TRIGGERS;i++)
				if(t==&me->trigger[i])
					effNum=i;

			i=GetVar(t->value);
			if(!VarMath(255,me->effect[effNum].text))
					PauseGame();	// pause if there's an error in the equation

			if(i==GetVar(255))
				answer=1;
			else if(i<GetVar(255) && (t->flags&TF_MORE))
				answer=1;
			else if(i>GetVar(255) && (t->flags&TF_LESS))
				answer=1;
			else
				answer=0;
			break;
		case TRG_TIMER:
			answer = CheckTimer(t->value, t->flags);
			break;
		case TRG_HURT:
			answer = CheckMonsterOuch(t->x, t->y, t->value, t->flags);
			break;
		case TRG_AFFLICT:
			break;
		case TRG_BULLETRECT:
			i=CountBulletsInRect(t->value,t->x,t->y,(t->value2%256),(t->value2/256));	// count how many of this type there are in the rect
			if(i>0)
				answer=1;
			else
				answer=0;
			break;
		case TRG_RAGEBAR:
			if (i == player.rage)
				answer = 1;
			else if (i < player.rage)
				answer = 1;
			else if (i > player.rage)
				answer = 1;
			else
				answer = 0;
			break;
		case TRG_HASRAGED: // TODO: make it so it only activates at START of rage
			if (GetGameMode() == GAMEMODE_RAGE)
				answer = 1;
			else
				answer = 0;
			break;
		case TRG_PROXIMITY:
			answer = CheckMonsterProximity(t->value, t->value2);
			break;
		case TRG_LINESIGHT:
			answer = CheckMonsterLOS(t->x, t->y, t->value, t->value2);
			break;
		case TRG_CHAINCOLOR:
			for (i = 0;i < nextEvent;i++)
			{
				if (events[i].type == EVT_SPECIAL)
				{
					if (abs(events[i].x - me->x) <= 1
						&& abs(events[i].y - me->y) <= 1 &&
						curMap->special[GetSpecial(events[i].x,events[i].y)].color == (byte)t->value2)
					{
						answer = 1;
					}
				}
			}
			break;
		case TRG_WRLDKEY:
			effNum = 0;
			for (i = 0;i < NUM_TRIGGERS;i++)
				if (t == &me->trigger[i])
					effNum = i;
			if (GetWorldProgress(me->effect[effNum].text)->keychains & (1 << t->value))
				answer = 1;
			else
				answer = 0;
			break;
		case TRG_WRLDPRC:
			effNum = 0;
			for (i = 0;i < NUM_TRIGGERS;i++)
				if (t == &me->trigger[i])
					effNum = i;
			i = (float)GetWorldProgress(me->effect[effNum].text)->percentage;
			if (t->value == i)
				answer = 1;
			else if (t->value < i && (t->flags & TF_MORE))
				answer = 1;
			else if (t->value > i && (t->flags & TF_LESS))
				answer = 1;
			else
				answer = 0;
			break;
		case TRG_MONSAGE:
			answer = CheckMonsterAge(t->x, t->y, t->value, t->value2, t->flags);
			break;
		case TRG_STPVARTILE:
			for(i=0;i<nextEvent;i++)
			{
				if(events[i].type==EVT_STEP && map->GetTile(events[i].x,events[i].y)->floor==GetVar(t->value2))
				{
					switch(t->value)
					{
						case MONS_ANYBODY:
							answer=1;
							victim=GetGuy(events[i].value);
							break;
						case MONS_GOODGUY:
							if(events[i].guyFriendly==1)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_BADGUY:
							if(events[i].guyFriendly==0)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_NONPLAYER:
							if(events[i].guyType!=MONS_BOUAPHA)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_PLAYER:
							if(GetGuy(events[i].value)!=NULL && GetGuy(events[i].value)->aiType==MONS_BOUAPHA)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						case MONS_TAGGED:
							if(GetGuy(events[i].value)==tagged)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
						default:
							if(events[i].guyType==t->value)
							{
								victim=GetGuy(events[i].value);
								answer=1;
							}
							break;
					}
				}
			}
			break;
		case TRG_FORCESTART:
			answer=0;
			break;
	}

	if(t->flags&TF_NOT)
		return (1-answer);
	else
		return answer;
}

byte IsTriggered(byte chain,special_t *me,Map *map)
{
	byte result;
	byte ok[NUM_TRIGGERS];
	int i;

	victim=goodguy;

	// only allow chainers if chain is true, only allow non-chainers otherwise
	result=0;
	for(i=0;i<NUM_TRIGGERS;i++)
		if(me->trigger[i].type==TRG_CHAIN)
			result=1;

	if(result==chain)
	{
		for(i=0;i<NUM_TRIGGERS;i++)
		{
			ok[i]=TriggerYes(me,&me->trigger[i],map);
		}

		if(ok[0]!=2)
			result=ok[0];
		else if(me->trigger[0].flags&TF_AND)
			result=1;
		else
			result=0;
		for(i=1;i<NUM_TRIGGERS;i++)
		{
			if(ok[i]!=2)	// 2 means there was no trigger there
			{
				if(me->trigger[i-1].flags&TF_AND)
					result&=ok[i];
				else
					result|=ok[i];
			}
		}
		return result;
	}
	else
		return 0;
}

void SpecialEffect(special_t *me,Map *map)
{
	int i,v,v2;
	byte fx;

	for(i=0;i<NUM_EFFECTS;i++)
	{
		switch(me->effect[i].type)
		{
			case EFF_MESSAGE:
				if(me->effect[i].text[0]!='/' || me->effect[i].text[1]!='/')
				{
					if(me->effect[i].flags&EF_TOGGLE)
					{
						NewBigMessage(me->effect[i].text,100);
						if (!(me->effect[i].flags&EF_NOFX)) MakeNormalSound(SND_MESSAGE);
					}
					else if(NoRepeatNewMessage(me->effect[i].text,120,90) && !(me->effect[i].flags&EF_NOFX))
						MakeNormalSound(SND_MESSAGE);
				}
				break;
			case EFF_SOUND:
				MakeNormalSound(me->effect[i].value);
				break;
			case EFF_SONG:
				if(me->effect[i].text[0]=='\0')
					StopSong();
				else
					PlaySong(me->effect[i].text);
				break;
			case EFF_WINLEVEL:
				if(PlayerBrains()>=map->numBrains)
				{
					if(map->flags&MAP_HUB)
					{
						MakeNormalSound(SND_GOTOMAP);
						SendMessageToGame(MSG_GOTOMAP,me->effect[i].value);
					}
					else
					{
						SendMessageToGame(MSG_WINLEVEL,me->effect[i].value);
						MakeNormalSound(SND_WINLEVEL);
					}
					if(me->effect[i].x==255)
						SetPlayerStart(-1,-1);
					else
						SetPlayerStart(me->effect[i].x,me->effect[i].y);
				}
				else
					return;	// to avoid doing anything more or using up any uses
				break;
			case EFF_GOTOMAP:
				MakeNormalSound(SND_GOTOMAP);
				if(me->effect[i].x==255)
					SetPlayerStart(-1,-1);
				else
					SetPlayerStart(me->effect[i].x,me->effect[i].y);
				SendMessageToGame(MSG_GOTOMAP,me->effect[i].value);
				break;
			case EFF_TELEPORT:
				if(me->effect[i].flags&EF_PLAYER)
				{
					if(!TeleportGuy(goodguy,me->effect[i].x,me->effect[i].y,map,(me->effect[i].flags&EF_NOFX)))
						return; // to avoid doing anything more or using up any uses
				}
				else if(me->effect[i].flags&EF_TAGGED)
				{
					if(!TeleportGuy(tagged,me->effect[i].x,me->effect[i].y,map,(me->effect[i].flags&EF_NOFX)))
						return; // to avoid doing anything more or using up any uses
				}
				else
					if(!TeleportGuy(victim,me->effect[i].x,me->effect[i].y,map,(me->effect[i].flags&EF_NOFX)))
						return; // to avoid doing anything more or using up any uses
				break;
			case EFF_CHANGETILE:
				v=map->GetTile(me->effect[i].x,me->effect[i].y)->floor;
				v2=map->GetTile(me->effect[i].x,me->effect[i].y)->wall;

				fx=(byte)((me->effect[i].flags&EF_NOFX)==0);
				if(me->effect[i].value2==0 && v2!=0 && !(me->effect[i].flags&EF_NOFX))	// zapwall
				{
					fx=1;
					MakeSound(SND_WALLDOWN,me->effect[i].x*TILE_WIDTH*FIXAMT,me->effect[i].y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,800);
				}
				else if(me->effect[i].value2!=0 && v2==0 && !(me->effect[i].flags&EF_NOFX))	// makewall
				{
					fx=1;
					MakeSound(SND_WALLUP,me->effect[i].x*TILE_WIDTH*FIXAMT,me->effect[i].y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,800);
				}

				if(me->effect[i].flags&EF_CONTIGUOUS)
					map->ContiguousTileChange(me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2,fx);
				else if(me->effect[i].flags&EF_ALL)
					map->AllTileChange(me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2,fx);
				else
					map->TileChange(me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2,fx);
				if(me->effect[i].flags&EF_TOGGLE)
				{
					me->effect[i].value=v;
					me->effect[i].value2=v2;
				}
				break;
			case EFF_SUMMON:
			{
				Guy *g = AddGuy(
					(me->effect[i].x*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,
					(me->effect[i].y*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,
					20*FIXAMT,
					me->effect[i].value,
					2
				);
				if(g)
				{
					if (MonsterFlags(g->type, g->aiType) & MF_FLYING)
						g->z = 0 * FIXAMT;

					g->item=me->effect[i].value2;

					TeleportGuy(g,me->effect[i].x,me->effect[i].y,map,(me->effect[i].flags&EF_NOFX), true);
				}
				break;
			}
			case EFF_LIGHT:
				if(!(me->effect[i].flags&EF_PERMLIGHT))
					map->BrightTorch(me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				else
				{
					if(!(me->effect[i].flags&EF_NOFX))
						MakeSound(SND_LIGHTSON,(me->effect[i].x*TILE_WIDTH)<<FIXSHIFT,
							(me->effect[i].y*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF|SND_ONE,1300);
					map->LightSpecial(me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				}

				break;
			case EFF_PICTURE:
				coro::launch(std::bind(ShowImageOrFlic,me->effect[i].text,(me->effect[i].flags&EF_NOFX),me->effect[i].value));
				break;
			case EFF_ITEM:
				if(me->effect[i].flags&EF_CONTIGUOUS)
					v=map->ContiguousItemChange(me->effect[i].x,me->effect[i].y,me->effect[i].value,!(me->effect[i].flags&EF_NOFX));
				else if(me->effect[i].flags&EF_ALL)
					v=map->AllItemChange(me->effect[i].x,me->effect[i].y,me->effect[i].value,!(me->effect[i].flags&EF_NOFX));
				else
					v=map->ItemChange(me->effect[i].x,me->effect[i].y,me->effect[i].value,!(me->effect[i].flags&EF_NOFX));
				if(me->effect[i].flags&EF_TOGGLE)
				{
					me->effect[i].value=v;
				}
				break;
			case EFF_SWAPMAP:
				if(me->effect[i].flags&EF_TOGGLE)
				{
					map->Swap(me->effect[i].x,me->effect[i].y,((me->effect[i].value%256)-me->effect[i].x)+1,((me->effect[i].value/256)-me->effect[i].y)+1,
						me->effect[i].value2%256,me->effect[i].value2/256);
				}
				else
				{
					map->Copy(me->effect[i].x,me->effect[i].y,((me->effect[i].value%256)-me->effect[i].x)+1,((me->effect[i].value/256)-me->effect[i].y)+1,
						me->effect[i].value2%256,me->effect[i].value2/256);
				}
				break;
			case EFF_KILLMONS:
				KillMonster(me->effect[i].x,me->effect[i].y,me->effect[i].value,(me->effect[i].flags&EF_NOFX));
				break;
			case EFF_CHANGEMONS:
				ChangeMonster(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				break;
			case EFF_CHANGETEAM:
				ChangeTeam(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				break;
			case EFF_DELETESPCL: {
				special_t* other = GetSpecial(GetSpecial(me->effect[i].x,me->effect[i].y));
				if(other)
				{
					other->x=255;
				}
				break;
			}
			case EFF_VAR:
				if(!VarMath(me->effect[i].value,me->effect[i].text))
					PauseGame();	// pause if there's an error in the equation
				break;
			case EFF_LEVELFLAG:
				word w,w2;
				w=1;
				w2=map->flags;
				for(v2=0;v2<me->effect[i].value;v2++)
					w*=2;
				switch(me->effect[i].value2)
				{
					case 0:
						map->flags |= LevelFlags{w};
						break;
					case 1:
						map->flags &= ~LevelFlags{w};
						break;
					case 2:
						map->flags ^= LevelFlags{w};
						break;
				}
				if((w2&(MAP_UNDERWATER|MAP_OXYGEN)) && !(map->flags&(MAP_UNDERWATER|MAP_OXYGEN)))
					player.oxygen=127*256;

				if(map->flags&MAP_UNDERWATER)
					WaterPalette(GetDisplayMGL());
				else if(map->flags&MAP_LAVA)
					LavaPalette(GetDisplayMGL());
				else
					GetDisplayMGL()->RealizePalette();
				break;
			case EFF_OLDTOGGLE:
				//v=map->GetTile(me->effect[i].x,me->effect[i].y)->floor;
				v2=map->GetTile(me->effect[i].x,me->effect[i].y)->wall;

				if(v2)	// there was a wall here
				{
					if(!(me->effect[i].flags&EF_NOFX))
						MakeSound(SND_WALLDOWN,me->effect[i].x*TILE_WIDTH*FIXAMT,me->effect[i].y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,800);
					map->ContiguousTileChange(me->effect[i].x,me->effect[i].y,me->effect[i].value,0,!(me->effect[i].flags&EF_NOFX));	// so change to floor
				}
				else	// it was a floor
				{
					if(!(me->effect[i].flags&EF_NOFX))
						MakeSound(SND_WALLUP,me->effect[i].x*TILE_WIDTH*FIXAMT,me->effect[i].y*TILE_HEIGHT*FIXAMT,SND_CUTOFF,800);
					map->ContiguousTileChange(me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2,!(me->effect[i].flags&EF_NOFX));	// so change to wall with same roof
				}
				break;
			case EFF_LIFE:
				if(me->effect[i].flags&EF_PERMLIGHT)
					MonsterMaxLife(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				else
					MonsterLife(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				break;
			case EFF_LIGHTRECT:
				v=me->effect[i].value%256;
				v2=me->effect[i].value/256;

				map->LightRect(me->effect[i].x,me->effect[i].y,v,v2,me->effect[i].value2,(me->effect[i].flags&EF_PERMLIGHT));
				v=(v-me->effect[i].x)/2+me->effect[i].x;
				v2=(v2-me->effect[i].y)/2+me->effect[i].y;
				if(!(me->effect[i].flags&EF_NOFX))
						MakeSound(SND_LIGHTSON,(v*TILE_WIDTH)<<FIXSHIFT,
							(v2*TILE_HEIGHT)<<FIXSHIFT,SND_CUTOFF|SND_ONE,1300);
				break;
			case EFF_WEAPON:
				if(me->effect[i].value2==0 && GetCurrentWeaponType()==me->effect[i].value)	// don't reload if already using
					break;
				RemoveCurrentWeapon();
				if(me->effect[i].value!=0)
					PlayerGetWeapon(me->effect[i].value,0,0);
				break;
			case EFF_TAGTARGET:
				tagged=victim;
				break;
			case EFF_TAGMONS:
				tagged=FindMonster(me->effect[i].x,me->effect[i].y,me->effect[i].value);
				break;
			case EFF_MONSITEM:
				ChangeMonsItem(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				break;
			case EFF_TILEVAR:
				v2=map->GetTile(me->effect[i].x,me->effect[i].y)->wall;

				v=GetVar(me->effect[i].value);
				v=(word)v;
				if(me->effect[i].flags&EF_CONTIGUOUS)
					map->ContiguousTileChange(me->effect[i].x,me->effect[i].y,v,v2,0);
				else if(me->effect[i].flags&EF_ALL)
					map->AllTileChange(me->effect[i].x,me->effect[i].y,v,v2,0);
				else
					map->TileChange(me->effect[i].x,me->effect[i].y,v,v2,0);
				break;
			case EFF_LIFEAMT:
				MonsterLifeAmt(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				break;
			case EFF_AI:
				ChangeMonsterAI(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				break;
			case EFF_NAME:
				SetMonsterName(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].text);
				break;
			case EFF_COLOR:
				SetMonsterColor(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				break;
			case EFF_MONSBRIGHT:
				SetMonsterBright(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				break;
			case EFF_PLAYAS:
				player.playAs=me->effect[i].value;
				break;
			case EFF_MONSGRAPHICS:
				SetMonsterGraphics(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].text);
				break;
			case EFF_ITEMGRAPHICS:
				SetCustomItemSprites(me->effect[i].text);
				break;
			case EFF_VARBAR:
				v=GetVar(me->effect[i].value);
				v=(word)v;
				if(me->effect[i].flags&EF_PERMLIGHT)
					player.varbarMax=v;
				else
					player.varbar=v;
				player.varbarColor=me->effect[i].value2;
				break;
			case EFF_MAKEBULLET:
				int x, y;
				if (me->effect[i].x == 255)
				{
					if (tagged)
					{
						x = tagged->x >> FIXSHIFT;
						y = tagged->y >> FIXSHIFT;
					}
					else
						break;
				}
				else
				{
					x = me->effect[i].x*TILE_WIDTH + TILE_WIDTH/2;
					y = me->effect[i].y*TILE_HEIGHT + TILE_HEIGHT/2;
				}

				if(!VarMath(255,me->effect[i].text))
					PauseGame();	// pause if there's an error in the equation
				FireBullet(x<<FIXSHIFT, y<<FIXSHIFT, GetVar(255), me->effect[i].value2, me->effect[i].flags&EF_PERMLIGHT);
				break;
			case EFF_CHANGEBULLET:
				ChangeBullet(!(me->effect[i].flags&EF_NOFX),me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				break;
			case EFF_CHAT:
				coro::launch(std::bind(ShowImageOrFlic, me->effect[i].text, (me->effect[i].flags & EF_NOFX), me->effect[i].value));
				break;
			case EFF_TIMER:
				player.timer += me->effect[i].value;
				break;
			case EFF_CAMERAFOCUS:
				//FocusCameraOnPoint(me->effect[i].x, me->effect[i].y);
				break;
			case EFF_CAMERAGUY:
				//FocusOnMonster(!(me->effect[i].flags & EF_NOFX), me->effect[i].x, me->effect[i].y, me->effect[i].value);
				break;
			case EFF_CAMERASCROLL:
				//SetCameraScroll(me->effect[i].value, me->effect[i].value2);
				break;
			case EFF_PARTICLE:
				if (me->effect[i].value > 14)
					continue;
				else
					DoParticleEffect(me->effect[i].x * TILE_WIDTH + TILE_WIDTH / 2, me->effect[i].y * TILE_HEIGHT + TILE_HEIGHT / 2,me->effect[i].value);
				break;
			case EFF_MOVESPCL:
				curMap->PushSpecials(me->effect[i].x,me->effect[i].y,me->effect[i].value,me->effect[i].value2);
				break;
			case EFF_VARTILE:
				v = map->GetTile(me->effect[i].x, me->effect[i].y)->floor;
				SetVar(me->effect[i].value, v);
				//v2 = map->GetTile(me->effect[i].x, me->effect[i].y)->wall;
				break;
		}
	}
	if(me->uses>0)
	{
		me->uses--;
		if(!me->uses)
			me->x=255;	// begone!
	}
}

byte IsForceStart(special_t *me, Map* map)
{
	byte result = (me->trigger[0].type == TRG_FORCESTART);

	return result;
}

void CheckSpecialsAtInit(Map* map)
{
	int i;

	if (tagged && tagged->hp == 0)
		tagged = NULL;

	// do global specials first...
	for (i = 0;i < numSpecialsGlobal;i++)
	{
		if (IsForceStart(&spclGlobal[i], map))
			SpecialEffect(&spclGlobal[i], map);
	}

	// then do the non-chaining local specials...
	for (i = 0;i < numSpecials;i++)
	{
		if (IsForceStart(&spcl[i], map))
			EventOccur(EVT_SPECIAL, i, spcl[i].x, spcl[i].y, victim);
	}

	// now do the ones that may chain off of those...
	for (i = 0;i < numSpecials;i++)
	{
		if (spcl[i].x != 255 && IsTriggered(1, &spcl[i], map))
			SpecialEffect(&spcl[i], map);
	}

	// all the specials that occurred now go off - their chains had to go off first
	for (i = 0;i < nextEvent;i++)
	{
		if (events[i].type == EVT_SPECIAL)
		{
			victim = events[i].victim;
			SpecialEffect(&spcl[events[i].value], map);
		}
	}
}

void CheckSpecials(Map *map)
{
	int i;

	if(tagged && tagged->hp==0)
		tagged=NULL;

	// do global specials first...
	for (i = 0;i < numSpecialsGlobal;i++)
	{
		if (spclGlobal[i].x != 255 && IsTriggered(0, &spclGlobal[i], map))
		{
			SpecialEffect(&spclGlobal[i], map);
		}
	}

	// then do the non-chaining local specials...
	for(i=0;i<numSpecials;i++)
	{
		if(spcl[i].x!=255 && IsTriggered(0,&spcl[i],map))
		{
			EventOccur(EVT_SPECIAL,i,spcl[i].x,spcl[i].y,victim);
		}
	}

	// now do the ones that may chain off of those...
	for(i=0;i<numSpecials;i++)
	{
		if(spcl[i].x!=255 && IsTriggered(1,&spcl[i],map))
		{
			SpecialEffect(&spcl[i],map);
		}
	}

	// all the specials that occurred now go off - their chains had to go off first
	for(i=0;i<nextEvent;i++)
	{
		if(events[i].type==EVT_SPECIAL)
		{
			victim=events[i].victim;
			SpecialEffect(&spcl[events[i].value],map);
		}
	}
	ClearEvents();
}

void InitSpecialsForPlay(void)
{
	ClearEvents();

	victim=NULL;
	tagged=NULL;
}

byte outXes = 0, lastSpecialShown = 33;
void RenderSpecialXes(Map *map)
{
	auto [camx, camy] = GetCamera();
	camx -= GetDisplayMGL()->GetWidth() / 2;
	camy -= GetDisplayMGL()->GetHeight() / 2;

	bool outOfRange = false;
	bool showName	= false;
	byte closest	= 255;
	int squareDist	= 999 * 999;

	// check specials
	for(int i=0;i<numSpecials;i++)
	{
		const special_t &special = spcl[i];
		if(special.x!=255)
		{
			for(const effect_t &effect : special.effect)
			{
				if(effect.type==EFF_GOTOMAP)
				{
					showName = true;
				}
				if(effect.type==EFF_GOTOMAP || effect.type==EFF_WINLEVEL)
				{
					if (goodguy) // player exists
					{
						int myDist = (abs(goodguy->x / (TILE_WIDTH * FIXAMT) - map->special[i].x) * abs(goodguy->x / (TILE_WIDTH * FIXAMT) - map->special[i].x)) +
							(abs(goodguy->y / (TILE_HEIGHT * FIXAMT) - map->special[i].y) * abs(goodguy->y / (TILE_HEIGHT * FIXAMT) - map->special[i].y));

						if (abs(goodguy->x / (TILE_WIDTH * FIXAMT) - map->special[i].x) <= 6 &&
							abs(goodguy->y / (TILE_HEIGHT * FIXAMT) - map->special[i].y) <= 6)	// when player is close, add details
						{
							outOfRange = false;
							if (closest == 255 || myDist < squareDist)
							{
								closest = (byte)i;
								squareDist = myDist;
							}
						}
					}
					// if level is passed
					if(LevelIsPassed(player.worldProg,effect.value))
					{
						byte c=0;
						for (const levelData_t &levelData : player.worldProg->Levels())
							if(levelData.levelNum==effect.value)
							{
								c=((levelData.flags&LF_CANDLES)!=0);
								break;
							}

						byte k=0;
						if(player.ability[ABIL_KEYCHAIN])
							k=keyChainInLevel[effect.value]&(~player.worldProg->keychains);

						if(k&15)
							DrawRedX(special.x*TILE_WIDTH-camx+TILE_WIDTH/2-4+Random(9),
								 special.y*TILE_HEIGHT-camy+TILE_HEIGHT/2-4+Random(9),c,GetDisplayMGL());
						else
							DrawRedX(special.x*TILE_WIDTH-camx+TILE_WIDTH/2,
									 special.y*TILE_HEIGHT-camy+TILE_HEIGHT/2,c,GetDisplayMGL());
						break;
					}
				}
			}
		}
	}
	// Yoinked from Kid Mystic: Enchanted Edition. Shows destination level name
	if (outOfRange)
	{
		if (outXes > 0)
			outXes--;
		else
			lastSpecialShown = 33;	// after they full recede, stop showing it
	}
	else if (closest != 255)
	{
		if (lastSpecialShown != closest)
		{
			lastSpecialShown = closest;
			outXes = 0;
		}
		else if (outXes < 40)
		{
			outXes += 2;
			if (outXes > 40) outXes = 40;
		}
	}
	if (showName && lastSpecialShown < 33)
	{
		if (!(map->special[lastSpecialShown].effect[0].flags & EF_NOFX))
		{
			byte mNum = map->special[lastSpecialShown].effect[0].value;
			int cx = map->special[lastSpecialShown].x * TILE_WIDTH - camx + TILE_WIDTH / 2;
			int cy = map->special[lastSpecialShown].y * TILE_HEIGHT - camy + TILE_HEIGHT / 2;
			Map* m = curWorld.map[mNum];
			CenterPrintCompressed(cx + 1, cy - outXes + 1, m->name, outXes * 100 / 40, -31, 1);
			CenterPrintCompressed(cx, cy - outXes, m->name, outXes * 100 / 40, 0, 1);
		}
	}
}

void AdjustSpecialCoords(special_t *me,int dx,int dy)
{
	int i,x2,y2;

	for(i=0;i<NUM_TRIGGERS;i++)
	{
		switch(me->trigger[i].type)
		{
			case TRG_NONE:
				break;
			case TRG_STEP:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				break;
			case TRG_MONSTER:
				break;
			case TRG_PASSLEVELS:
				break;
			case TRG_PASSLEVEL:
				break;
			case TRG_SHOOT:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				break;
			case TRG_HAVEITEM:
				break;
			case TRG_CHAIN:
				if(me->trigger[i].x!=255 && me->trigger[i].y!=255)
				{
					me->trigger[i].x+=dx;
					me->trigger[i].y+=dy;
				}
				break;
			case TRG_STEPRECT:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				x2=me->trigger[i].value2%256;
				y2=me->trigger[i].value2/256;
				me->trigger[i].value2=(x2+dx)+(y2+dy)*256;
				break;
			case TRG_KILL:
				break;
			case TRG_FLOOR:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				break;
			case TRG_VAR:
				break;
			case TRG_TIMED:
				break;
			case TRG_DELAY:
				break;
			case TRG_RANDOM:
				break;
			case TRG_FLOORRECT:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				x2=me->trigger[i].value2%256;
				y2=me->trigger[i].value2/256;
				me->trigger[i].value2=(x2+dx)+(y2+dy)*256;
				break;
			case TRG_LIFE:
			case TRG_AWAKE:
				if(me->trigger[i].x!=255 && me->trigger[i].y!=255)
				{
					me->trigger[i].x+=dx;
					me->trigger[i].y+=dy;
				}
				break;
			case TRG_STEPTILE:
				break;
			case TRG_GETITEM:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				break;
			case TRG_ITEM:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				break;
			case TRG_COMPMAP:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				x2=me->trigger[i].value%256;
				y2=me->trigger[i].value/256;
				me->trigger[i].value=(x2+dx)+(y2+dy)*256;
				x2=me->trigger[i].value2%256;
				y2=me->trigger[i].value2/256;
				me->trigger[i].value2=(x2+dx)+(y2+dy)*256;
				break;
			case TRG_COMPVAR:
				break;
			case TRG_MONSINRECT:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				x2=me->trigger[i].value2%256;
				y2=me->trigger[i].value2/256;
				me->trigger[i].value2=(x2+dx)+(y2+dy)*256;
				break;
			case TRG_ITEMRECT:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				x2=me->trigger[i].value2%256;
				y2=me->trigger[i].value2/256;
				me->trigger[i].value2=(x2+dx)+(y2+dy)*256;
				break;
			case TRG_DIFFICULTY:
				break;
			case TRG_KEYPRESS:
				break;
			case TRG_PLAYAS:
				break;
			case TRG_AFFLICT:
			case TRG_MONSAGE:
			case TRG_MONSCOLOR:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				break;
			case TRG_EQUATION:
			case TRG_EQUVAR:
				break;
			case TRG_BULLETRECT:
				me->trigger[i].x+=dx;
				me->trigger[i].y+=dy;
				x2=me->trigger[i].value2%256;
				y2=me->trigger[i].value2/256;
				me->trigger[i].value2=(x2+dx)+(y2+dy)*256;
				break;
		}
	}
}

void AdjustSpecialEffectCoords(special_t *me,int dx,int dy)
{
	int i,x2,y2;

	for(i=0;i<NUM_EFFECTS;i++)
	{
		switch(me->effect[i].type)
		{
			case EFF_MESSAGE:
				break;
			case EFF_SOUND:
				break;
			case EFF_SONG:
				break;
			case EFF_WINLEVEL:
				break;
			case EFF_GOTOMAP:
				break;
			case EFF_TELEPORT:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				break;
			case EFF_CHANGETILE:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				break;
			case EFF_SUMMON:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				break;
			case EFF_LIGHT:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				break;
			case EFF_PICTURE:
				break;
			case EFF_ITEM:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				break;
			case EFF_SWAPMAP:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				x2=me->effect[i].value%256;
				y2=me->effect[i].value/256;
				me->effect[i].value=(x2+dx)+(y2+dy)*256;
				x2=me->effect[i].value2%256;
				y2=me->effect[i].value2/256;
				me->effect[i].value2=(x2+dx)+(y2+dy)*256;
				break;
			case EFF_KILLMONS:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_CHANGEMONS:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_CHANGETEAM:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_DELETESPCL:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				break;
			case EFF_VAR:
				break;
			case EFF_LIGHTRECT:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				x2=me->effect[i].value%256;
				y2=me->effect[i].value/256;
				me->effect[i].value=(x2+dx)+(y2+dy)*256;
				break;
			case EFF_LEVELFLAG:
				break;
			case EFF_OLDTOGGLE:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				break;
			case EFF_LIFE:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_WEAPON:
				break;
			case EFF_TAGMONS:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_MONSITEM:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_TILEVAR:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				break;
			case EFF_LIFEAMT:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_AI:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_NAME:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_COLOR:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_MONSBRIGHT:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_PLAYAS:
				break;
			case EFF_MONSGRAPHICS:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_MAKEBULLET:
				me->effect[i].x+=dx;
				me->effect[i].y+=dy;
				break;
			case EFF_MARKASBOSS:
			case EFF_AFFLICT:
			case EFF_CHANGEBULLET:
				if(me->effect[i].x!=255)
				{
					me->effect[i].x+=dx;
					me->effect[i].y+=dy;
				}
				break;
			case EFF_PARTICLE:
				me->effect[i].x			+= dx;
				me->effect[i].y			+= dy;
				break;
			case EFF_MOVESPCL:
				me->effect[i].x			+= dx;
				me->effect[i].y			+= dy;
				break;
			case EFF_VARTILE:
				me->effect[i].x			+= dx;
				me->effect[i].y			+= dy;
				break;
		}
	}
}

bool CheckSpecial(const special_t &me)
{
	for (const trigger_t &trg : me.trigger)
	{
		if (trg.type)
		{
			return true;
		}
	}
	return false;
}

void PrintSpecialComment(int x,int y,int mx,int my)
{
	for (const special_t &me : spcl)
	{
		if(me.x==mx && me.y==my)
		{
			for (const effect_t &eff : me.effect)
			{
				if(eff.type==EFF_MESSAGE && eff.text[0]=='/' && eff.text[1]=='/')
				{
					Print(x,y,&eff.text[2],0,1);
				}
			}
		}
	}
}
