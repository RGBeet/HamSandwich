#include "winpch.h"
#include "intface.h"
#include "player.h"
#include "shop.h"
#include <algorithm>

constexpr int SPR_LIFEMETER		= 68;
constexpr int SPR_POWERUP		= 69;
constexpr int SPR_RAGEGAUGE		= 70;
constexpr int SPR_VARBAR		= 71;
constexpr int SPR_NUMBERS_LRG	= 74; // 0-9
constexpr int SPR_NUMBERS_SML	= 97; // also 0-9
constexpr int SPR_WPNICON		= 72; // WIP - make it so the weapon is drawn in the interface
constexpr int SPR_WPNBAR		= 73;

constexpr int SPR_ICN_KEY		= 84; //keycard icons (rgb and ylw)
constexpr int SPR_ICN_HAMMER	= 88;
constexpr int SPR_ICN_PANTS		= 89;
constexpr int SPR_ICN_SPRING	= 90;
constexpr int SPR_ICN_REVGEM	= 91;

// todo: add these as indicators
constexpr int SPR_ICN_AMMOBOX	= 92;
constexpr int SPR_ICN_SHIELD	= 93;
constexpr int SPR_ICN_CLOAK		= 94;
constexpr int SPR_ICN_SQUEEZY	= 95;
constexpr int SPR_ICN_SPEEDY	= 96;
constexpr int SPR_SCORE			= 107;
constexpr int SPR_COINS			= 108;
constexpr int SPR_BRAINOMETER	= 109;
constexpr int SPR_COLON			= 112;
constexpr int SPR_COMBO			= 114;
constexpr int SPR_STICKMAN		= 115;

constexpr int SPR_WEAPONBOX   = 3;
constexpr int SPR_HAMMERBOX   = 5;
constexpr int SPR_KEYBOX      = 6;
constexpr int SPR_OXYGAUGE    = 8;
constexpr int SPR_ENEMYLIFE   = 111;
constexpr int SPR_COINBOX     = 62;
constexpr int SPR_STEALTH     = 63;
constexpr int SPR_WPNNAME     = 22;
constexpr int SPR_LOONYKEY    = 50;
constexpr int SPR_CANDLE      = 51;
constexpr int SPR_KEYCH       = 52;
constexpr int SPR_BRAIN       = 56;
constexpr int SPR_RAGE        = 57;
constexpr int SPR_LOCK        = 110;
constexpr int SPR_PORTRAIT    = 66;
constexpr int SPR_HEAD		  = 67;

constexpr int SPR_TIME = 999;

constexpr int OLD_SPR_LIFEMETER   = 0;
constexpr int OLD_SPR_IFHAMMER    = 1;
constexpr int OLD_SPR_MINIGAUGE   = 2;
constexpr int OLD_SPR_NUMBERS     = 3;
constexpr int OLD_SPR_WEAPONS     = 13;
constexpr int OLD_SPR_KEYRING     = 29;
constexpr int OLD_SPR_BRAINOMETER = 40;
constexpr int OLD_SPR_RAGEGAUGE   = 47;

constexpr int KEYRINGX = 40;
constexpr int KEYRINGY = 38;

static std::unique_ptr<sprite_set_t> intfaceSpr;
static std::unique_ptr<sprite_set_t> oldIntfaceSpr;

static byte curLife=0;
static byte curBrains=0;
static byte monsAlive=0;
static int curMonsLife;
static int comboY,curCombo;

// these are for the enemy life meter
static char monsName[32];
static int monsHP;
static word monsTimer;

enum {
	INTF_POWERUP,
	INTF_KEYS,
	INTF_VARBAR,
	INTF_RAGE,
	INTF_OXYGEN,
	INTF_LIFE,
	INTF_BRAINS,
	INTF_WEAPON,
	INTF_LOCK,
	INTF_SCORE,
	INTF_HAMMERS,
	INTF_ENEMY,
	INTF_COINS,
	INTF_TIME,
	INTF_STEALTH,
	INTF_COMBO,
	INTF_COUNTDOWN,
	NUM_INTF,
};

/*

	Portrait of player
	Life meter
	Effects meter
	Rage meter

	Hammers/pants/spring/reverse
	Weapon (icon, name, ammo meter)
	Time
	Score
	Brains
	Coins

	Oxygen


*/

enum {
	IV_NONE,
	IV_BIGMETER,
	IV_SMALLMETER,
	IV_NUMBER,
	IV_VERTMETER,
	IV_ICONS,
	IV_KEYS,
	IV_DIAL,  // the oxygen dial
	IV_EVILMETER,
	IV_TIME,
	IV_LOCK,
	IV_COMBO
};

static const byte weaponToItem[] = {
	ITM_NONE,
	ITM_MISSILES,
	ITM_AK8087,
	ITM_BOMBS,
	ITM_FLAME,
	ITM_PWRARMOR,
	ITM_BIGAXE,
	ITM_LIGHTNING,
	ITM_SPEAR,
	ITM_MACHETE,
	ITM_MINES,
	ITM_TURRETWPN,
	ITM_MINDCONTROL,
	ITM_REFLECTOR,
	ITM_JETPACK,
	ITM_SWAPGUN,
	ITM_TORCH,
	ITM_SCANNER,
	ITM_MINISUB,
	ITM_FREEZERAY,
	ITM_STOPWATCH,
};
static_assert(std::size(weaponToItem) == MAX_WEAPONS, "Weapons need item specified for Classic HUD display");

struct intface_t
{
	int  x,y,tx,ty;
	byte spr;
	byte valueType;
	byte valueLength;
	int  vOffX,vOffY;
	int  value,vDesired;
	byte otherVal;
};

intface_t defaultSetup[NUM_INTF]={
	{56,-50,56,20,	// power ups
	 SPR_POWERUP,
	 IV_SMALLMETER,64,
	 2,2,
	 0,0,
	 0},
	{1,-50,1,38,	// keys
	 999,
	 IV_KEYS,0,
	 0,0,
	 0,0,
	 0},
	{56,-47,56,39,	// varbar
	 SPR_VARBAR,
	 IV_SMALLMETER,128,
	 1,1,
	 0,0,
	 0},
	{38,-39,38,31,	// rage meter
	 SPR_RAGEGAUGE,
	 IV_SMALLMETER,128,
	 17,1,
	 0,100,
	 0},
	{1,-50,1,-1,	// oxygen
	 SPR_OXYGAUGE,
	 IV_DIAL,192,
	 14,12,
	 0,128,
	 0},
	{40,-68,40,3,		// life meter
	 SPR_LIFEMETER,
	 IV_BIGMETER,128,
	 17,2,
	 0,128,
	 0},
	{SCRWID-1,-102,SCRWID-1,29,	// brains
	 SPR_BRAINOMETER,
	 IV_VERTMETER,64,
	 -8,9,
	 0,50,
	 8},
	{232,-70,232,10,	// weapon
	 SPR_WPNBAR,
	 IV_SMALLMETER,64,
	 1,2,
	 0,10,
	 1},
	{290,-70,290,16,	// weapon lock
	 SPR_LOCK,
	 IV_LOCK,2,
	 0,0,
	 0,0,
	 0},
	{SCRWID-20,-50,SCRWID-20,1,	// score
	 SPR_SCORE,
	 IV_NUMBER,6,
	 20,8,
	 0,1000,
	 0},
	{188,1,188,1,		// hammers
	 999,
	 IV_ICONS,0,
	 0,0,
	 0,0,
	 0},
	{0,SCRHEI+40,0,SCRHEI-1,		// enemy life
	 SPR_ENEMYLIFE,
	 IV_EVILMETER,196,
	 2,-16,
	 0,196,
	 0},
	{SCRWID-1,-50,SCRWID-1,-50,	// coins
	 SPR_COINS,
	 IV_NUMBER,3,
	 -27,7,
	 0,0,
	 0},
	{SCRWID - 100,-50,SCRWID - 100,-1,	// time
	 SPR_TIME,
	 IV_TIME,2,
	 -19,3,
	 0,0,
	 20},
	{1,-50,1,-1,	// stealth
	 999, // render sprite specially
	 IV_NONE,0,
	 0,0,
	 0,0,
	 0},
	{1,SCRHEI + 40,1,SCRHEI + 40,	// c-c-c-combo!!!
	 SPR_COMBO, // render sprite specially
	 IV_COMBO,0,
	 76,-20,
	 0,0,
	 0},
	{SCRWID/2 - 50,SCRHEI+20,SCRWID/2 - 50,SCRHEI - 20,	// time
	 SPR_TIME,
	 IV_TIME,2,
	 -19,3,
	 0,0,
	 20},
};

static byte intfFlip;

static intface_t intf[NUM_INTF];

void InitInterface(void)
{
	intfaceSpr		= std::make_unique<sprite_set_t>("graphics/intface.jsp");
	oldIntfaceSpr	= std::make_unique<sprite_set_t>("graphics/intface_old.jsp");
	memcpy(intf,defaultSetup,sizeof(intface_t)*NUM_INTF);
}

void ExitInterface(void)
{
	oldIntfaceSpr.reset();
	intfaceSpr.reset();
}

void ResetInterface(void)
{
	comboY=-22;
	curCombo=0;
	intfFlip=0;
	curLife=0;
	monsTimer=0;
	curBrains=0;
	memcpy(intf,defaultSetup,sizeof(intface_t)*NUM_INTF);

	// Static defaults above don't know currency screen size, so apply an adjustment on init.
	int w = GetDisplayMGL()->GetWidth(), h = GetDisplayMGL()->GetHeight();
	for (int i = 0; i < NUM_INTF; ++i)
	{
		if (intf[i].x >= SCRWID/2)
			intf[i].x += w - SCRWID;
		if (intf[i].tx >= SCRWID/2)
			intf[i].tx += w - SCRWID;
		if (intf[i].y >= SCRHEI/2)
			intf[i].y += h - SCRHEI;
		if (intf[i].ty >= SCRHEI/2)
			intf[i].ty += h - SCRHEI;
	}
}

void DrawLifeMeter(int x,int y,byte amt)
{
	byte c;

	if(amt==0)
		return;
	amt--;
	if(amt>128)
		amt=128;

	if(amt>64+16)
		c=44;		// in the green
	else if(amt>32)
		c=174;		// in the yellow
	else
		c=140;		// in the red!!

	DrawFillBox(x,y+1,x,y+12,c-6);
	DrawFillBox(x+amt,y+1,x+amt,y+12,c-6);
	if(amt>1)
	{
		DrawFillBox(x+1,y+1,x+1,y+12,c-1);
		DrawFillBox(x+amt-1,y+1,x+amt-1,y+12,c-1);
		if(amt>3)
		{
			// here's the short parts on top and bottom
			DrawFillBox(x+2,y,x+(amt-2),y,c-3);
			DrawFillBox(x+2,y+13,x+(amt-2),y+13,c-3);
			// and here's the normal part
			DrawFillBox(x+2,y+1,x+amt-2,y+1,c);
			DrawFillBox(x+2,y+12,x+amt-2,y+12,c);
			DrawFillBox(x+2,y+2,x+amt-2,y+2,c+8);
			DrawFillBox(x+2,y+3,x+amt-2,y+11,c+4);
		}
	}
}

void DrawRageMeter(int x,int y,byte enuf,byte amt)
{
	byte c;
	static byte blink=0;

	blink=1-blink;

	if(amt==0)
		return;
	amt--;

	if(enuf)
	{
		if(blink)
			c=48;
		else
			c=44;
	}
	else
	{
		if(blink)
			c=140;
		else
			c=134;
	}

	if(amt>0)
	{
		DrawFillBox(x,y,x+amt,y+1,c);
	}
}

void DrawOxygenMeter(int x,int y,byte amt)
{
	byte c;

	if(amt==0)
		return;
	amt--;

	if(amt>64)
		c=32*1+16;
	else if(amt>32)
		c=32*5+16;
	else
		c=32*4+16;

	if(amt>0)
	{
		DrawFillBox(x,y,x+amt,y+1,c);
	}
}

void DrawScore(int x,int y,int score,MGLDraw *mgl)
{
	int i,j,n;

	if(score<0)		// I don't think you can have a negative score
		score=0;

	j=100000000;			// 100 million
	for(i=0;i<9;i++)
	{
		n=(score/j)%10;
		oldIntfaceSpr->GetSprite(n+OLD_SPR_NUMBERS)->Draw(x,y,mgl);
		j/=10;
		x+=23;
	}
}

void DrawSmallGauge(int x, int y, byte b, byte c)
{
	if (b > 0)
	{
		DrawFillBox(x+1,y+13-b,x+1,y+13,c+2);
		DrawFillBox(x+2,y+13-b,x+2,y+13,c);
	}
	if (b > 2)
	{
		DrawFillBox(x,y+12-(b-2),x,y+12,c-4);
		DrawFillBox(x+3,y+12-(b-2),x+3,y+12,c-4);
	}
}

void DrawHammerSpeed(int x,int y,byte spd)
{
	byte b;

	if(spd>=32)
		return;

	b=(32-spd)*14/32;
	b--;

	DrawFillBox(x+1,y+13-b,x+1,y+13,143);
	DrawFillBox(x+2,y+13-b,x+2,y+13,141);
	if(b>2)
	{
		DrawFillBox(x,y+12-(b-2),x,y+12,138);
		DrawFillBox(x+3,y+12-(b-2),x+3,y+12,138);
	}
}

void DrawLitGauge(int x,int y,byte c)
{
	DrawFillBox(x+1,y,x+1,y+13,c+2);
	DrawFillBox(x+2,y,x+2,y+13,c);
	DrawFillBox(x,y+1,x,y+12,c-4);
	DrawFillBox(x+3,y+1,x+3,y+12,c-4);
}

void ShowEnemyLife(char *name,byte formerLife,byte life,byte alive)
{
	curMonsLife=formerLife;
	monsHP=life;
	strcpy(monsName,name);
	monsTimer=90;	// 3 seconds
	monsAlive=alive;
	intf[INTF_ENEMY].ty=GetDisplayMGL()->GetHeight()-1;
}

void RenderRage(byte size,MGLDraw *mgl)
{
	intfaceSpr->GetSprite(SPR_RAGE+size)->Draw(320,240,mgl);
}

void DrawBigMeter(int x, int y, int value, int length, int height, MGLDraw* mgl)
{
	if (value <= 0)
		return;

	if (value > length)
		value = length;

	byte c = 32;

	if (value < length * 2 / 3)
		c = 162;
	if (value < length / 4)
		c = 128;

	for (int row = 0; row < height; row++)
	{
		float t = (height <= 1) ? 0.0f : (float)row / (height - 1);

		// Distance from center (0 at center, 1 at edge)
		float d = fabsf(t - 0.5f) * 2.0f;

		byte shade;

		if (d < 0.25f)
			shade = c + 26; // brightest center
		else if (d < 0.5f)
			shade = c + 15;
		else if (d < 0.75f)
			shade = c + 12;
		else
			shade = c + 10;

		mgl->FillBox(x, y + row, x + value - 1, y + row, shade);
	}
}

void DrawEvilMeter(int x,int y,int value,int length,MGLDraw *mgl)
{
	byte c;

	if(value==0)
		return;

	if(value>length)
		value=length;

	c=32;

	if(value<length*2/3)
		c=162;
	if(value<length/4)
		c=128;

	mgl->FillBox(x,y,x+value-1,y,c+6);
	mgl->FillBox(x,y+1,x+value-1,y+1,c+8);
	mgl->FillBox(x,y+2,x+value-1,y+4,c+11);
	mgl->FillBox(x,y+5,x+value-1,y+5,c+22);
	mgl->FillBox(x,y+6,x+value-1,y+11,c+11);
	mgl->FillBox(x,y+12,x+value-1,y+12,c+8);
	mgl->FillBox(x,y+13,x+value-1,y+13,c+6);
}

void DrawSmallMeter(int x,int y,int value,byte red,MGLDraw *mgl)
{
	byte c;

	if(value==0)
		return;

	c=32;

	if(red==1)
		c=128;
	else if(red==2)
		c=36;
	else if(red==3)
		c=228;
	else if(red>=16)
		c=4+(red-16)*32;

	mgl->FillBox(x,y,x+value-1,y,c+7);
	mgl->FillBox(x,y+1,x+value-1,y+1,c+12);
	mgl->FillBox(x,y+2,x+value-1,y+2,c+15);
	mgl->FillBox(x,y+3,x+value-1,y+3,c+12);
	mgl->FillBox(x,y+4,x+value-1,y+4,c+7);
}

void DrawVertMeter(int x, int y, int value, int height, MGLDraw* mgl, int width=5)
{
	if (value <= 0)
		return;

	if (value > height)
		value = height;

	byte c = 32;

	int top = y + height - (value - 1);
	int bottom = y + height;

	for (int col = 0; col < width; col++)
	{
		float t = (width <= 1) ? 0.0f : (float)col / (float)(width - 1);

		// 0 at center, 1 at edges
		float d = fabsf(t - 0.5f) * 2.0f;

		byte shade;

		if (d < 0.25f)
			shade = c + 15;
		else if (d < 0.75f)
			shade = c + 12;
		else
			shade = c + 7;

		mgl->FillBox(x + col, top, x + col, bottom, shade);
	}
}

void DrawNumber(int x,int y,int value,byte length,MGLDraw *mgl, int type, int chw, int strw = 0)
{
	int i;
	char s[8];

	if(value<0)		// I don't think you can have a negative score
		value=0;
	if(value>999999)
		value=999999;

	sprintf(s,"%0*d", strw, value);

	if(strlen(s)<length)
	{
		x+=chw*(length-strlen(s));
		length=strlen(s);
	}
	for(i=0;i<length;i++)
	{
		intfaceSpr->GetSprite(s[i]-'0'+type)->Draw(x,y,mgl);
		x+=chw;
	}
}

void DrawSmallNumber(int x, int y, int value, byte length, MGLDraw* mgl, int strw = 0)
{
	DrawNumber(x,y,value,length,mgl,SPR_NUMBERS_SML, 9, strw);
}

void DrawBigNumber(int x, int y, int value, byte length, MGLDraw* mgl, int strw = 0)
{
	DrawNumber(x, y, value, length, mgl, SPR_NUMBERS_LRG, 16, strw);
}

int GetDigits(int number)
{
	int digits = 0;
	if (number < 0) digits = 1; // Keep this line if '-' counts as a digit.
	while (number) {
		number /= 10;
		digits++;
	}
	return digits;
}

void DrawNewScore(int x, int y, int value, byte length, MGLDraw* mgl, int strw = 0)
{
	int xx=0;
	for (int i = 0; i < length; i++)
		xx -= 16; // width of each digit
	DrawNumber(x+xx, y, value, length, mgl, SPR_NUMBERS_LRG, 16, strw);
}

void DrawTime(int x,int y,int value,byte length,MGLDraw *mgl)
{
	int i;
	char s[8];

	if(value<0)		// I don't think you can have a negative score
		value=0;
	if(value>999999)
		value=999999;

	sprintf(s,"%d",value);

	if(strlen(s)<length)
	{
		x+=9*(length-strlen(s));
		length=strlen(s);
	}
	for(i=0;i<length;i++)
	{
		if(i==2)
		{
			//extra distance for colon
			x+=2;
		}
		intfaceSpr->GetSprite(s[i]-'0'+SPR_NUMBERS_SML)->Draw(x,y,mgl);
		x+=9;
	}
}

void DrawDial(int x,int y,int value,byte length,MGLDraw *mgl)
{
	int ang;
	int tx,ty,i;

	ang=(64+256-value)&255;

	if(value==0)
		intfaceSpr->GetSprite(8)->DrawOffColor(x-14,y-12,mgl,1,4,intfFlip*8);
	if(value<192/4)
		intfaceSpr->GetSprite(10)->Draw(x-14,y-12,mgl);
	else if(value<192/2)
		intfaceSpr->GetSprite(9)->Draw(x-14,y-12,mgl);

	x*=FIXAMT;
	y*=FIXAMT;
	for(i=0;i<8;i++)
	{
		tx=(x+Cosine(ang)*i)/FIXAMT;
		ty=(y-Sine(ang)*i)/FIXAMT;
		mgl->FillBox(tx,ty,tx,ty,31);
	}
}

void DrawPULightning(int x, int y, int width, byte color, MGLDraw* mgl)
{
	byte* scrn;
	int pitch;
	int i, y2, b, db;

	pitch = mgl->GetWidth();
	scrn = mgl->GetScreen();
	scrn += y * pitch + x;

	y2 = Random(4);
	b = Random(8);

	db = Random(2) ? 1 : -1;

	for (i = 0;i < width;i++)
	{
		int py = y + y2;

		if (py >= 0 && py < mgl->GetHeight() - 1 &&
			x + i >= 0 && x + i < mgl->GetWidth())
		{
			scrn[y2 * pitch] = color + b;
			scrn[y2 * pitch + pitch] = color + b;
		}

		b += db;

		if (b > 7)
		{
			b = 7;
			db = -1;
		}
		if (b < 0)
		{
			b = 0;
			db = 1;
		}

		switch (Random(3))
		{
		case 1:
			y2 = std::min(y2 + 1, 3);
			break;
		case 2:
			y2 = std::max(y2 - 1, 0);
			break;
		}

		scrn++; // move right one pixel
	}
}

byte POWERUP_WIDTH = 128;

void DrawPowerupBar(int x,int y,MGLDraw *mgl)
{
	byte width[7],th;
	byte color[7],tc;
	int i,swap;

	width[0]=player.shield*POWERUP_WIDTH/240;
	color[0]=32*3+16;	// blue for shield
	width[1]=player.garlic*POWERUP_WIDTH/255;
	color[1]=32*0+20;	// light grey for garlic
	width[2]=player.speed*POWERUP_WIDTH/255;
	color[2]=32*7+16;	// aqua for speed
	width[3]=player.invisibility*POWERUP_WIDTH/255;
	color[3]=32*6+16;	// purple for invis
	width[4]=player.ammoCrate*POWERUP_WIDTH/255;
	color[4]=32*4+16;	// red for ammo
	if(goodguy)
		width[5]=goodguy->poison*POWERUP_WIDTH/255;
	else
		width[5]=0;
	color[5]=32*1+16;	// green for poison
	width[6]=player.cheesePower*POWERUP_WIDTH/255;
	color[6]=32*5+16;	// yellow for cheese

	swap=1;
	while(swap)
	{
		swap=0;
		for(i=0;i<6;i++)
		{
			if(width[i]< width[i+1])
			{
				th = width[i];
				tc = color[i];
				width[i] = width[i+1];
				color[i] = color[i+1];
				width[i+1] = th;
				color[i+1] = tc;
				swap = 1;
			}
		}
	}

	for(i=0;i<6;i++)
	{
		if(width[i]>0)
		{
			DrawPULightning(x,y, width[i],color[i],mgl);
		}
	}
}

void DrawPortrait(int x, int y, MGLDraw* mgl)
{
	intfaceSpr->GetSprite(SPR_PORTRAIT)->Draw(x, y, mgl);
	intfaceSpr->GetSprite(SPR_HEAD)->Draw(x + 19, y + 19, mgl); // todo: add more players
}

void DrawKeys(int x,int y,MGLDraw *mgl)
{
	int i;
	int xx=0;
	int yy=0;

	if (player.keys[1] || player.keys[2] || player.keys[3])
	{

		if (player.keys[1])
		{
			intfaceSpr->GetSprite(SPR_ICN_KEY + 1)->Draw(x+xx, y, mgl);
			xx += 10;
		}
		if (player.keys[2])
		{
			intfaceSpr->GetSprite(SPR_ICN_KEY + 2)->Draw(x + xx, y, mgl);
			xx += 10;
		}
		if (player.keys[3])
		{
			intfaceSpr->GetSprite(SPR_ICN_KEY + 3)->Draw(x + xx, y, mgl);
			xx += 10;
		}
		yy+=12;
	}

	xx=0;
	for (i = 0;i < player.keys[0];i++)
	{
		intfaceSpr->GetSprite(SPR_ICN_KEY)->Draw(x+xx, y+yy, mgl);
		xx += 10;
	}
}

// draw melee stats
void DrawHammers(int x,int y,MGLDraw *mgl)
{
	int i,p;
	int xx=0, yy=0;

	for (i = 0;i < player.hammers;i++) {
		intfaceSpr->GetSprite(SPR_ICN_HAMMER)->Draw(x+xx, y+yy, mgl);
		yy += 11;
	}

	if (player.hammerFlags & HMR_REVERSE)
	{
		intfaceSpr->GetSprite(SPR_ICN_REVGEM)->Draw(x+xx, y+yy, mgl);
		yy += 11;
	}


	if (player.hammers > 0)
		xx += 11;
	yy=0;


	p=(16 - player.hamSpeed)/4;
	for(i=0;i<p;i++)
	{
		intfaceSpr->GetSprite(SPR_ICN_PANTS)->Draw(x+xx, y+yy, mgl);
		yy += 11;
	}


	if (player.hammerFlags & HMR_REFLECT)
	{
		intfaceSpr->GetSprite(SPR_ICN_SPRING)->Draw(x+xx, y + yy, mgl);
	}
}

void DrawStealth(int x, int y, MGLDraw* mgl)
{
	if (curMap && curMap->flags & MAP_STEALTH)
	{
		if (player.stealthy)
			intfaceSpr->GetSprite(SPR_STEALTH)->Draw(x, y, mgl);
		else // not so stealthy...
			intfaceSpr->GetSprite(SPR_STEALTH)->DrawOffColor(x, y, mgl, 1, 5, 0);
	}
}

void DrawLock(int x,int y,MGLDraw *mgl, int value)
{
	if(value)
		intfaceSpr->GetSprite(SPR_LOCK)->Draw(x,y,mgl);
}

void UpdateInterface(Map *map)
{
	int i,j;
	int xx=0,yy=1;

	// don't show score on hub level (can't do squat with it anyways!)
	if (map->flags & MAP_HUB)
	{
		intf[INTF_SCORE].tx = SCRWID-20;
		intf[INTF_SCORE].ty = -50;
	}
	else
	{
		intf[INTF_SCORE].tx = SCRWID-20;
		intf[INTF_SCORE].ty = yy;
		yy += 29;
	}

	// have at least 1 coin
	if (!player.coins)
	{
		intf[INTF_COINS].tx = SCRWID - 1;
		intf[INTF_COINS].ty = -50;
	}
	else
	{
		intf[INTF_COINS].tx = SCRWID - 1;
		intf[INTF_COINS].ty = yy;
		yy += 17;
	}

	if (player.brains < map->numBrains) // not enough brains
	{
		intf[INTF_BRAINS].tx = SCRWID-1;
		intf[INTF_BRAINS].ty = yy;
	}
	else
	{
		intf[INTF_BRAINS].tx = SCRWID - 1;
		intf[INTF_BRAINS].ty = -102;
	}

	yy = 20;
	// portrait + hp bar stays in place

	// status bar comes first???
	if (player.shield || player.garlic || player.speed || player.invisibility || player.ammoCrate || (goodguy && goodguy->poison) || player.cheesePower)
	{
		intf[INTF_POWERUP].tx = 56;
		intf[INTF_POWERUP].ty = yy;
		yy += 11;
	}
	else
	{
		intf[INTF_POWERUP].tx = 56;
		intf[INTF_POWERUP].ty = -10;
	}

	// then rage bar
	if (player.rage > 0 && player.ability[ABIL_RAGE])
	{
		intf[INTF_RAGE].tx = 38;
		intf[INTF_RAGE].ty = yy;
		yy += 8;
	}
	else
	{
		intf[INTF_RAGE].tx = 38;
		intf[INTF_RAGE].ty = -10;
	}

	// then custom variable bar
	if (player.varbarMax > 0)
	{
		intf[INTF_VARBAR].tx = 56;
		intf[INTF_VARBAR].ty = yy;
		yy += 8;
	}
	else
	{
		intf[INTF_VARBAR].tx = 56;
		intf[INTF_VARBAR].ty = -10;
	}

	// KEYS / OXYGEN
	yy=38;

	// keys
	if (player.keys[0] + player.keys[1] + player.keys[2] + player.keys[3]) // if keys, then...
	{
		intf[INTF_KEYS].tx = 1;
		intf[INTF_KEYS].ty = yy;
		yy += 23;
	}
	else
	{
		intf[INTF_KEYS].tx = -32;
		intf[INTF_KEYS].ty = yy;
	}

	// oxygen
	if (map->flags & (MAP_UNDERWATER | MAP_OXYGEN))
	{
		intf[INTF_OXYGEN].tx = 1;
		intf[INTF_OXYGEN].ty = yy;
		yy += 23;
	}
	else
	{
		intf[INTF_OXYGEN].tx = -32;
		intf[INTF_OXYGEN].ty = yy;
	}

	// stealth
	if (curMap && curMap->flags & MAP_STEALTH)
	{
		intf[INTF_STEALTH].tx = 1;
		intf[INTF_STEALTH].ty = yy;
		yy += 23;
	}
	else
	{
		intf[INTF_STEALTH].tx = -32;
		intf[INTF_STEALTH].ty = yy;
	}

	if (player.weapon)
	{
		intf[INTF_WEAPON].tx	= 232;
		intf[INTF_WEAPON].ty	= 10;
		intf[INTF_LOCK].tx		= 290;
		intf[INTF_LOCK].ty		= 16;
	}
	else
	{
		intf[INTF_WEAPON].tx	= 232;
		intf[INTF_WEAPON].ty	= -70;
		intf[INTF_LOCK].tx		= 290;
		intf[INTF_LOCK].ty		= -64;
	}

	yy = intf[INTF_ENEMY].ty;
	if (yy > SCRHEI - 30)
		yy = SCRHEI - 30;


	if (player.comboClock && player.combo > 1)
	{
		intf[INTF_COMBO].ty = yy;
		curCombo = player.combo;
	}
	else
		intf[INTF_COMBO].ty = SCRHEI + 50;

	intfFlip=1-intfFlip;
	for(i=0;i<NUM_INTF;i++)
	{
		switch(i)	// whatever is unique to each
		{
			case INTF_LIFE:
				intf[i].vDesired = player.life*128/goodguy->maxHP;
				if(intf[i].value<intf[i].vDesired-10)
					intf[i].value += 4;
				if(intf[i].value>intf[i].vDesired+10)
					intf[i].value -= 4;
				if(intf[i].value>128)
					intf[i].value = 128;
				break;
			case INTF_RAGE:
				intf[i].vDesired=player.rage*intf[i].valueLength/(127*256);
				break;
			case INTF_VARBAR:
				if (player.varbarMax>0)
					if (player.varbar>player.varbarMax)
						intf[i].vDesired=intf[i].valueLength;
					else
						intf[i].vDesired=player.varbar*intf[i].valueLength/player.varbarMax;
				else
					intf[i].vDesired=0;
				break;
			case INTF_OXYGEN:
				intf[i].vDesired=player.oxygen*intf[i].valueLength/(127*256);
				break;
			case INTF_SCORE:
				intf[i].vDesired=player.score;
				break;
			case INTF_TIME:
				intf[i].vDesired= (int)(player.clock/30);
				break;
			case INTF_COUNTDOWN:
				intf[i].vDesired = (int)(player.timer);
				break;
			case INTF_COINS:
				intf[i].vDesired=player.coins;
				break;
			case INTF_WEAPON:
				if(player.weapon>0)
					intf[i].vDesired=player.ammo*intf[i].valueLength/WeaponMaxAmmo(player.weapon);
				else
					intf[i].vDesired=0;
				break;
			case INTF_LOCK:
				intf[i].vDesired = bool(profile.progress.wpnLock) ^ bool(GetControls() & CONTROL_B3);
				break;
			case INTF_COMBO:
				intf[i].vDesired = player.combo;
				break;
			case INTF_BRAINS:
				int b;

				b=map->numBrains-player.brains;
				if(b<0)
					b=0;
				if(map->numBrains>0)
					intf[i].vDesired=b*intf[i].valueLength/map->numBrains;
				else
					intf[i].vDesired=0;
				break;
			case INTF_ENEMY:
				if(monsTimer)
				{
					monsTimer--;
					if(curMonsLife<monsHP)
					{
						curMonsLife+=4;
						if(curMonsLife>monsHP)
							curMonsLife=monsHP;
					}
					if(curMonsLife>monsHP)
					{
						curMonsLife-=4;
						if(curMonsLife<monsHP)
							curMonsLife=monsHP;
					}
					intf[i].vDesired=curMonsLife*intf[i].valueLength/128;
					intf[i].value=curMonsLife*intf[i].valueLength/128;
					intf[i].ty=GetDisplayMGL()->GetHeight()-1;
				}
				else
				{
					intf[i].vDesired=0;
					intf[i].ty=GetDisplayMGL()->GetHeight()-1+30;
				}
				break;
		}
		if(intf[i].value<intf[i].vDesired)
			intf[i].value++;
		if(intf[i].value<intf[i].vDesired-100)
			intf[i].value+=10;
		if(intf[i].value<intf[i].vDesired-1000)
			intf[i].value+=100;
		if(intf[i].value>intf[i].vDesired)
			intf[i].value--;
		if(intf[i].value>intf[i].vDesired+100)
			intf[i].value-=10;
		if(intf[i].value>intf[i].vDesired+1000)
			intf[i].value-=100;

		for(j=0;j<2;j++)
		{
			if(intf[i].x>intf[i].tx)
				intf[i].x--;
			if(intf[i].x<intf[i].tx)
				intf[i].x++;
			if(intf[i].y>intf[i].ty)
				intf[i].y--;
			if(intf[i].y<intf[i].ty)
				intf[i].y++;
		}
	}
}


// special mutant interface when shopping
void RenderInterfaceShopping(MGLDraw *mgl)
{
	char combo[16];

	InstaRenderItem(639-TILE_WIDTH/2,479-8,ITM_COIN,0,mgl);
	sprintf(combo,"%u",profile.progress.totalCoins-profile.progress.coinsSpent);
	Print(639-TILE_WIDTH-GetStrLength(combo,2)-1,479-18,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2)+1,479-18,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2),479-18+1,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2),479-18-1,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2),479-18,combo,0,2);

	InstaRenderItem(639-TILE_WIDTH/2,479-38,ITM_LOONYKEY,0,mgl);
	sprintf(combo,"%u",profile.progress.loonyKeys-profile.progress.loonyKeysUsed);
	Print(639-TILE_WIDTH-GetStrLength(combo,2)-1,479-38,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2)+1,479-38,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2),479-38+1,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2),479-38-1,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2),479-38,combo,0,2);
}

void RenderInterface(MGLDraw *mgl)
{
	if (shopping)
		return RenderInterfaceShopping(mgl);

	int i;
	char combo[16];

	//sprintf(combo,"%d:%02d:%02d",(profile.progress.totalTime/(30*60*60)),(profile.progress.totalTime/(30*60))%60,(profile.progress.totalTime/30)%60);
	//PrintGlow(5,240,combo,0,2);

	for(i=0;i<NUM_INTF;i++)
	{
		intfaceSpr->GetSprite(intf[i].spr)->Draw(intf[i].x,intf[i].y,mgl);

		switch (i)
		{
			case INTF_LIFE:
				DrawBigMeter(intf[i].x + intf[i].vOffX, intf[i].y + intf[i].vOffY, intf[i].value, intf[i].valueLength, 12, mgl);
				break;
			case INTF_POWERUP:
				DrawPowerupBar(intf[i].x + intf[i].vOffX, intf[i].y + intf[i].vOffY, mgl);
				break;
			case INTF_SCORE:
				DrawNewScore(intf[i].x + intf[i].vOffX, intf[i].y + intf[i].vOffY, intf[i].value, intf[i].valueLength, mgl);
				break;
			case INTF_STEALTH:
				DrawStealth(intf[i].x + intf[i].vOffX, intf[i].y + intf[i].vOffY, mgl);
				break;
			case INTF_WEAPON:
				intfaceSpr->GetSprite(SPR_WPNICON)->Draw(intf[i].x-21, intf[i].y-8, mgl);
				intfaceSpr->GetSprite(SPR_WPNNAME+player.weapon-1)->Draw(intf[i].x, intf[i].y-6, mgl);
				break;
		}

		switch(intf[i].valueType)
		{
			case IV_EVILMETER:
				DrawEvilMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				if(monsAlive || intfFlip)
					PrintSimpleShadow(intf[i].x+intf[i].vOffX+2,intf[i].y+intf[i].vOffY,monsName,1);
				break;
			case IV_SMALLMETER:
				if(i!=INTF_RAGE || player.rage/256>=player.life)
				{
					if(i==INTF_RAGE && intfFlip==0)
						DrawSmallMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,2,mgl);
					else if (i==INTF_VARBAR)
						DrawSmallMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,16+player.varbarColor,mgl);
					else
						DrawSmallMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,0,mgl);
				}
				else
					DrawSmallMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,1,mgl);
				break;
			case IV_NUMBER:
				DrawSmallNumber(intf[i].x + intf[i].vOffX, intf[i].y + intf[i].vOffY, intf[i].value, intf[i].valueLength, mgl);
				break;
			case IV_TIME:
				intfaceSpr->GetSprite(SPR_COLON+1)->Draw(intf[i].x+intf[i].vOffX-21, intf[i].y+intf[i].vOffY, mgl);
				DrawSmallNumber(intf[i].x+intf[i].vOffX-21, intf[i].y+intf[i].vOffY, intf[i].value/60, intf[i].valueLength, mgl);
				intfaceSpr->GetSprite(SPR_COLON)->Draw(intf[i].x+intf[i].vOffX, intf[i].y+intf[i].vOffY, mgl);
				DrawSmallNumber(intf[i].x+intf[i].vOffX+7,intf[i].y+intf[i].vOffY, intf[i].value%60,intf[i].valueLength,mgl, 2);
				break;
			case IV_VERTMETER:
				DrawVertMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl, intf[i].otherVal);
				break;
			case IV_KEYS:
				DrawKeys(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,mgl);
				break;
			case IV_ICONS:
				DrawHammers(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,mgl);
				break;
			case IV_DIAL:
				DrawDial(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				break;
			case IV_LOCK:
				DrawLock(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,mgl,intf[i].value);
				break;
			case IV_COMBO: // new thing
				int comboClock = player.comboClock;
				int frame = (comboClock > 0)
					? SPR_STICKMAN + ((comboClock / 4) % 2)
					: 117;
				sprintf(combo, "x%d", curCombo);
				PrintWavy(intf[i].x + intf[i].vOffX, intf[i].y + intf[i].vOffY, combo, 0, 2, player.clock, 1, 1);
				DrawSmallNumber(intf[i].x + intf[i].vOffX, intf[i].y + intf[i].vOffY, intf[i].value, intf[i].valueLength, mgl);
				intfaceSpr->GetSprite(frame)->Draw(intf[i].x + intf[i].vOffX + ((comboClock > 0) ? comboClock : 0), intf[i].y + intf[i].vOffY, mgl);
				break;
		}
	}

	DrawPortrait(0, 0, mgl); // nice lil portrait
}

void DrawFancyLine(int x, int y, int color, int width, MGLDraw* mgl)
{
	int seg = width / 5;

	DrawLine(x, y, x + seg - 1, y, 32 * color + 4);
	DrawLine(x + seg, y, x + seg * 2 - 1, y, 32 * color + 8);
	DrawLine(x + seg * 2, y, x + seg * 3 - 1, y, 32 * color + 16);
	DrawLine(x + seg * 3, y, x + seg * 4 - 1, y, 32 * color + 8);
	DrawLine(x + seg * 4, y, x + width - 1, y, 32 * color + 4);
}

void RenderShoppingStuff(int x, int y, MGLDraw *mgl)
{
	char combo[16];
	int p;
	int yy = 20;

	sprintf(combo, "Shopping Info");
	Print(x, y+yy, combo, 0, 2);
	yy += 15;

	DrawFancyLine(x, y+yy, 7, 100, mgl);
	yy += 15;

	InstaRenderItem(x, y+yy+6, ITM_COIN, 0, GetDisplayMGL());
	sprintf(combo,"D$%u",profile.progress.totalCoins-profile.progress.coinsSpent);
	PrintSimpleShadow(x+32, y+yy, combo, 1);

	yy += 27;
	InstaRenderItem(x, y+yy+4, ITM_LOONYKEY, 0, GetDisplayMGL());
	sprintf(combo,"%u LoonyKeys",profile.progress.loonyKeys-profile.progress.loonyKeysUsed);
	PrintSimpleShadow(x+32, y+yy, combo, 1);

	yy += 27;
	sprintf(combo, "%u/%u Shopped", NumPurchased(), NUMSHOPITEMS);
	PrintSimpleShadow(x, y + yy, combo, 1);


}

void RenderCollectedStuff(int x,int y,MGLDraw *mgl)
{
	char msg[32];
	float p;
	int xx=0, yy=20;

	// draw the level info (only if it's not a hub level)
	if (curMap->flags & MAP_HUB)
	{
		// hub level
	}
	else
	{
		sprintf(msg, "Level Info");
		Print(x+xx, y+yy, msg, 0, 2);
		yy += 15;

		DrawFancyLine(x+xx, y+yy, 7, 100, mgl);
		yy += 5;

		if (curMap->numBrains == 0)
		{
			sprintf(msg, "N/A");
			intfaceSpr->GetSprite(SPR_CANDLE)->DrawOffColor(x + xx, y + yy, mgl, 1, 7, -8);
			PrintSimpleShadow(x + xx + 21, y + yy, msg, 1);
		}
		else if ((player.levelProg->flags & LF_CANDLES))
		{
			sprintf(msg, "ALL Candles!");
			intfaceSpr->GetSprite(SPR_CANDLE)->DrawOffColor(x+xx, y+yy, mgl, 1, 7, 0);
			PrintSimpleShadow(x+xx+21, y + yy, msg, 1);
		}
		else
		{
			snprintf(msg, sizeof(msg), "%.2f%% Candles", 100.0f * player.candles / curMap->numCandles);
			intfaceSpr->GetSprite(SPR_CANDLE)->DrawOffColor(x + xx, y + yy, mgl, 1, 0, 0);
			PrintSimpleShadow(x+xx+21, y + yy, msg, 1);
		}
		yy += 20;

		if (curMap->numBrains == 0)
		{
			sprintf(msg, "N/A");
			intfaceSpr->GetSprite(SPR_BRAIN)->DrawOffColor(x + xx, y + yy, mgl, 1, 7, -8);
			PrintSimpleShadow(x + xx + 21, y + yy, msg, 1);
		}
		else if (PlayerBrains() < curMap->numBrains)
		{
			snprintf(msg, sizeof(msg), "%.2f%% Brains", 100.0f * player.brains / curMap->numBrains);
			intfaceSpr->GetSprite(SPR_BRAIN)->DrawOffColor(x + xx, y + yy, mgl, 1, 0, 0);
			PrintSimpleShadow(x+xx+21, y+yy, msg, 1);
		}
		else
		{
			sprintf(msg, "ALL Brains!");
			intfaceSpr->GetSprite(SPR_BRAIN)->DrawOffColor(x + xx, y + yy, mgl, 1, 7, 0);
			PrintSimpleShadow(x+xx+21, y+yy, msg, 1);
		}
		yy += 20;
	}

	sprintf(msg, "World Info");
	Print(x + xx, y + yy, msg, 0, 2);
	yy += 15;

	DrawFancyLine(x + xx, y + yy, 7, 100, mgl);
	yy += 5;

	// draw keychains

	if((player.worldProg->keychains&KC_LOONY))
	{
		intfaceSpr->GetSprite(SPR_LOONYKEY)->Draw(x+183,y+57,mgl);
	}

	int keych = 0;

	xx = 0;
	for(int i=0;i<4;i++)
	{
		if (player.worldProg->keychains & (1 << i)) // check each keychain
		{
			keych++;
			intfaceSpr->GetSprite(SPR_KEYCH + i)->DrawOffColor(x + xx, y + yy, mgl, 1, 7, 0);
		}
		else
			intfaceSpr->GetSprite(SPR_KEYCH + i)->DrawOffColor(x + xx, y + yy, mgl, 1, 0, -4);
		xx += 16;
	}
	yy += 12;

	xx=0;
	snprintf(msg, sizeof(msg), "%d/4 Keychains", keych);
	PrintSimpleShadow(x+xx, y+yy, msg, 1);

	p=(float)player.worldProg->percentage;
	if(p>100)
		p=100;
	if(p<0)
		p=0;
	yy += 15;

	snprintf(msg, sizeof(msg), "%.2f%% Complete",p);
	PrintSimpleShadow(x + xx, y + yy, msg, 1);
}
