#include "player.h"
#include "guy.h"
#include "display.h"
#include "intface.h"
#include "winpch.h"
#include "game.h"
#include "items.h"

/*
	Adapted from LL2's spell menu. Used for
	picking between Bouapha's hammer and special
	weapon(s).

	Pockets add more spaces for special weapons.
*/

typedef struct pick_t
{
	byte pos;		// where on the wheel it currently is
	byte rightpos;
	byte weapon;	// which skill it's tied to
	byte ammo;
	byte pic;
	byte updown;	// should it have updown arrows
} pick_t;

byte numPicks;
byte circleSize;
static int pickBright, pickDBright;
static char lastKey = 0;
static byte pickWait, vertWait, wpnPick;

#define MAX_PICKS	8
pick_t picks[MAX_PICKS];

// center print outline from LL2
void CenterPrintOutline(int x, int y, const char* s)
{
	Print(x, y, s, 0, 1);
}

void InitPickMenu(void)
{
	int i,j=0;
	byte ok, picked;

	MakeNormalSound(SND_BLOCKPUSH); // todo: add new sound

	if (player.hammers > 0)
		j++;
	
	numPicks	= j+player.wpnSlots;
	wpnPick		= 0;

	ok=0;

	for(i=0;i<numPicks;i++)
	{
		picks[(i+wpnPick)%numPicks].pos=(byte)(i*255/numPicks);
		picks[(i+wpnPick)%numPicks].rightpos=picks[(i+wpnPick)%numPicks].pos;
	}

	for (i = 0;i < player.wpnSlots;i++)
	{
		picks[i+j].weapon = player.weapons[i];
		picks[i+j].ammo = player.ammunition[i];
	}

	if (player.hammers > 0)
	{
		picks[0].weapon = 255;
		picks[0].ammo = 0;
	}
	
	pickWait	= 10;
	vertWait	= 0;
	circleSize	= 0;

	pickBright	= 0;
	pickDBright = 1;
	// idk
}

const char* GetWeaponSlotName(byte i)
{
	char s[4];
	snprintf(s, sizeof(s), "%d", i);
	if (picks[i].weapon > 0)
		return s;
	else
		return "Nothing.";
}

const int GetWeaponSlotIcon(byte i)
{
	return GetWeaponIcon(picks[i].weapon);
}

void RenderPickMenu(MGLDraw* mgl)
{
	int i, x, y;
	char s[64], verb[16];

	for (i = 0;i < numPicks;i++)
	{
		x = Cosine(picks[i].pos + 192) * circleSize;
		y = Sine(picks[i].pos + 192) * circleSize;
		x /= FIXAMT;
		y /= FIXAMT;
		x += SCRWID/2;
		y += SCRHEI/2;

		GetIntfaceSprite(119)->Draw(x, y, mgl);
		GetItemSprite(GetWeaponIcon(picks[i].weapon))->DrawBright(x, y, mgl, i==wpnPick ? pickBright : 0);
		CenterPrint(x, y + 20, GetWeaponName(picks[i].weapon), 0, 2);

		switch(picks[i].weapon)
		{
			case 0:
				sprintf(s, "0/0");
				break;
			case 255: // hammer
				sprintf(s, "Infinite");
				break;
			default:
				sprintf(s, "%d/%d", picks[i].ammo, WeaponMaxAmmo(picks[i].weapon));
				break;
		}
		if (picks[i].weapon == 0)
		CenterPrint(x+1, y+39, s, 8, 1);
		CenterPrint(x, y+38, s, 0, 1);

		// render the interface 
		// InstaRenderItem(x-2,y+5,player.items[potionChoice].type,8*(spellpick==i),mgl);
	}

	sprintf(s,"Press LEFT or RIGHT to cycle through Weapons");
	CenterPrint(SCRWID/2, SCRHEI - circleSize/2, s, 0, 2);
}


void MovePickMenu(void)
{
	int i;

	for (i = 0;i < numPicks;i++)
	{
		picks[(i + wpnPick) % numPicks].rightpos = (byte)(i * 255 / numPicks);
	}
	// ???
	MakeNormalSound(SND_WORLDTURN);
}

byte UpdatePickMenu(MGLDraw* mgl)
{
	byte c, wp;
	int i,j=0;

	if (player.hammers > 0)
		j++;

	pickBright += pickDBright;
	if (pickBright > 10)
		pickDBright = -1;
	if (pickBright < -2)
		pickDBright = 1;

	if (pickWait)
		pickWait--;
	if (vertWait)
		vertWait--;

	if (circleSize < 120)
		circleSize += 20;

	for (i = 0; i < numPicks; i++)
	{
		picks[i].pos = TurnToward(picks[i].pos, picks[i].rightpos, 20 - numPicks);
	}

	c	= GetControls();
	wp	= wpnPick;

	if (pickWait)
		c &= CONTROL_B3;

	if (numPicks > 1)
	{
		if (c & CONTROL_LF)
		{
			if (picks[0].pos == picks[0].rightpos)
			{
				wpnPick--;
				if (wpnPick >= numPicks)
					wpnPick = numPicks - 1;
				MovePickMenu();
			}
		}
		else if (c & CONTROL_RT)
		{
			if (picks[0].pos == picks[0].rightpos)
			{
				wpnPick++;
				if (wpnPick >= numPicks)
					wpnPick = 0;
				MovePickMenu();
			}
		}
	}

	if((c&CONTROL_B3)==0)
	{
		MakeNormalSound(SND_MENUSELECT); // TODO: add new sound
		if (wpnPick == 0 && player.hammers>0) // player picks hammer
		{
			player.activeSlot = 255;
		}
		else // player picks weapon!?
		{
			player.activeSlot = wpnPick-j;
		}
		return 0;
	}
	return 1;
}
