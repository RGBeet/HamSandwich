#include "winpch.h"
#include "intface.h"
#include "player.h"
#include "shop.h"

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

constexpr int SPR_WEAPONBOX   = 3;
constexpr int SPR_BRAINOMETER = 4;
constexpr int SPR_HAMMERBOX   = 5;
constexpr int SPR_KEYBOX      = 6;
constexpr int SPR_OXYGAUGE    = 8;
constexpr int SPR_ENEMYLIFE   = 11;
constexpr int SPR_COINBOX     = 62;
constexpr int SPR_STEALTH     = 63;
constexpr int SPR_WPNNAME     = 22;
constexpr int SPR_LOONYKEY    = 50;
constexpr int SPR_CANDLE      = 51;
constexpr int SPR_KEYCH       = 52;
constexpr int SPR_BRAIN       = 56;
constexpr int SPR_RAGE        = 57;
constexpr int SPR_LOCK        = 65;
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
	INTF_LOCK,
	INTF_WEAPON,
	INTF_SCORE,
	INTF_HAMMERS,
	INTF_ENEMY,
	INTF_COINS,
	INTF_TIME,
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
	{0,-50,0,38,	// keys
	 999,
	 IV_KEYS,0,
	 0,0,
	 0,0,
	 0},
	{56,-47,56,39,	// varbar
	 SPR_VARBAR,
	 IV_SMALLMETER,128,
	 18,2,
	 0,0,
	 0},
	{38,-39,38,31,	// rage meter
	 SPR_RAGEGAUGE,
	 IV_SMALLMETER,128,
	 17,1,
	 0,100,
	 0},
	{330,-50,330,-1,	// oxygen
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
	{SCRWID-1,-102,SCRWID-1,25,	// brains
	 SPR_BRAINOMETER,
	 IV_VERTMETER,64,
	 -6,10,
	 0,50,
	 0},
	{SCRWID-79,-70,SCRWID-79,16,	// weapon lock
	 SPR_LOCK,
	 IV_LOCK,2,
	 0,0,
	 0,0,
	 0},
	{232,-70,232,10,	// weapon
	 SPR_WPNBAR,
	 IV_SMALLMETER,64,
	 1,2,
	 0,10,
	 1},
	{SCRWID-1,-50,SCRWID-1,-1,	// score
	 SPR_SCORE,
	 IV_NUMBER,6,
	 -54,3,
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
	{SCRWID-1,SCRHEI+40,SCRWID-1,SCRHEI-1,	// coins
	 SPR_COINBOX,
	 IV_NUMBER,2,
	 -18,-14,
	 0,0,
	 0},
	{SCRWID - 79,-50,SCRWID - 79,-1,	// time
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
	intfaceSpr = std::make_unique<sprite_set_t>("graphics/intface.jsp");
	oldIntfaceSpr = std::make_unique<sprite_set_t>("graphics/intface_old.jsp");
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

void DrawVertMeter(int x,int y,int value,int height,MGLDraw *mgl)
{
	byte c;

	if(value==0)
		return;

	c=32;

	mgl->FillBox(x,y+height-(value-1),x,y+height,c+7);
	mgl->FillBox(x+1,y+height-(value-1),x+1,y+height,c+12);
	mgl->FillBox(x+2,y+height-(value-1),x+2,y+height,c+15);
	mgl->FillBox(x+3,y+height-(value-1),x+3,y+height,c+12);
	mgl->FillBox(x+4,y+height-(value-1),x+4,y+height,c+7);
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
		x+=9*(length-strlen(s));
		length=strlen(s);
	}
	for(i=0;i<length;i++)
	{
		intfaceSpr->GetSprite(s[i]-'0'+SPR_NUMBERS_SML)->Draw(x,y,mgl);
		x+=9;
	}
}

void DrawSmallNumber(int x, int y, int value, byte length, MGLDraw* mgl, int strw = 0)
{
	DrawNumber(x,y,value,length,mgl,SPR_NUMBERS_SML, strw);
}

void DrawBigNumber(int x, int y, int value, byte length, MGLDraw* mgl, int strw = 0)
{
	DrawNumber(x, y, value, length, mgl, SPR_NUMBERS_LRG, strw);
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
		yy+=10;
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

void DrawLock(int x,int y,MGLDraw *mgl, int value)
{
	if(value)
		intfaceSpr->GetSprite(67)->Draw(x,y,mgl);
}

void UpdateInterface(Map *map)
{
	int i,j;

	intfFlip=1-intfFlip;
	for(i=0;i<NUM_INTF;i++)
	{
		switch(i)	// whatever is unique to each
		{
			case INTF_LIFE:
				intf[i].vDesired = player.life*128/goodguy->maxHP;
				if(intf[i].value<intf[i].vDesired-10)
					intf[i].value += 3;
				if(intf[i].value>intf[i].vDesired+10)
					intf[i].value -= 3;
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

// Option to use classic Dr. L interface, with these differences:
// - Pause menu is still Supreme's. Options too different, impractical to replace.
// - Weapons will use item appearance instead of baked sprite if edited from base.
// - New oxygen meter in between the reflect indicator and the hammer icons.
// Missing elements of Supreme's interface:
// - No powerup meter, since classic Dr. L had 5 powerups of Supreme's 7 and no meter.
// - No combo indicator.
// - No coin meter.
// - TODO: No stealth indicator.
// - TODO: No varbar.
static void RenderInterfaceOld(MGLDraw *mgl)
{
	byte life = player.life*128/goodguy->maxHP;
	byte rage = player.rage / 256;
	byte hmrFlags = player.hammerFlags;
	byte hammers = player.hammers;
	int brains = curMap->numBrains ? 128 - (player.brains * 128 / curMap->numBrains) : 0;
	int score = player.score;
	byte wpn = player.weapon;
	int ammo = player.ammo;
	byte hamSpeed = player.hamSpeed;

	int i;
	static byte flip = 0;
	char s[16];

	flip++;

	for (i = 0; i < 2; i++)
	{
		if (life > curLife)
			curLife++;
		else if (life < curLife)
			curLife--;
		if (life > curLife)
			curLife++;
		else if (life < curLife)
			curLife--;

		if (brains > curBrains)
			curBrains++;
		else if (brains < curBrains)
			curBrains--;
	}

	oldIntfaceSpr->GetSprite(OLD_SPR_LIFEMETER)->Draw(5, 3, mgl);
	DrawLifeMeter(7, 8, curLife);
	oldIntfaceSpr->GetSprite(OLD_SPR_RAGEGAUGE)->Draw(5, 3, mgl);
	DrawRageMeter(7, 29, (rage >= life), rage);

	// hammer speed gauge
	oldIntfaceSpr->GetSprite(OLD_SPR_MINIGAUGE)->Draw(139, 3, mgl);
	DrawHammerSpeed(141, 8, hamSpeed * 2);

	// hammer reverse indicator
	oldIntfaceSpr->GetSprite(OLD_SPR_MINIGAUGE)->Draw(148, 3, mgl);
	if (hmrFlags & 1)
		DrawLitGauge(150, 8, 112);

	// hammer reflect indicator
	oldIntfaceSpr->GetSprite(OLD_SPR_MINIGAUGE)->Draw(157, 3, mgl);
	if (hmrFlags & 2)
		DrawLitGauge(159, 8, 175);

	// oxygen indicator
	int x = 167;
	if (curMap->flags & (MAP_UNDERWATER | MAP_OXYGEN))
	{
		oldIntfaceSpr->GetSprite(OLD_SPR_MINIGAUGE)->Draw(x - 1, 3, mgl);
		if (player.oxygen==0)
		{
			if (intfFlip)
				DrawSmallGauge(x + 1, 8, 13, 4*32+16);
		}
		else if (player.oxygen<127*256/4)
			DrawSmallGauge(x + 1, 8, 14*player.oxygen/(127*256+1), 4*32+16);
		else if (player.oxygen<127*256/2)
			DrawSmallGauge(x + 1, 8, 14*player.oxygen/(127*256+1), 5*32+16);
		else
			DrawSmallGauge(x + 1, 8, 14*player.oxygen/(127*256+1), 1*32+16);
		x += 9;
	}

	// number of hammers
	for (i = 0; i < hammers; i++)
		oldIntfaceSpr->GetSprite(OLD_SPR_IFHAMMER)->Draw(x + i * 19, 3, mgl);

	DrawScore(432, 2, score, mgl);

	// Enemy life gauge
	if (monsTimer)
	{
		monsTimer--;
		if (curMonsLife < monsHP)
		{
			curMonsLife += 4;
			if (curMonsLife > monsHP)
				curMonsLife = monsHP;
		}
		if (curMonsLife > monsHP)
		{
			curMonsLife -= 4;
			if (curMonsLife < monsHP)
				curMonsLife = monsHP;
		}

		oldIntfaceSpr->GetSprite(OLD_SPR_LIFEMETER)->Draw(6, 453, mgl);
		DrawLifeMeter(8, 458, curMonsLife);
		// if the monster is dead, the name blinks
		Print(11, 461, monsName, 1, 1);
		if (monsAlive || (flip & 2) == 0)
			Print(10, 460, monsName, 0, 1);
	}

	// secondary weapons
	if (wpn)
	{
		// Use the frame in intface_old.jsp if it exists and the item isn't modified.
		// Many of these would match anyways, but e.g. Zap Wand is at a different offset.
		byte item = weaponToItem[wpn];
		if (wpn <= WPN_SWAPGUN && memcmp(GetItem(item), GetBaseItem(item), sizeof(item_t)) == 0)
			oldIntfaceSpr->GetSprite(OLD_SPR_WEAPONS - 1 + wpn)->Draw(595, 30, mgl);
		else
		{
			oldIntfaceSpr->GetSprite(OLD_SPR_WEAPONS + WPN_SWAPGUN)->Draw(595, 30, mgl);
			InstaRenderItem(595+22, 30+22, item, 0, mgl);
		}

		if (wpn != WPN_PWRARMOR && wpn != WPN_MINISUB)
		{
			sprintf(s, "%02d", ammo);
			Print(621, 61, s, 1, 1);
			Print(620, 60, s, 0, 1);
		}
		else
		{
			sprintf(s, "%3d", ammo / 10);
			Print(601, 61, s, 1, 1);
			Print(600, 60, s, 0, 1);
		}
	}

	// the almighty keyring
	oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING)->Draw(KEYRINGX, KEYRINGY, mgl);
	if (PlayerKeys(3))
		oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING + 1)->Draw(KEYRINGX, KEYRINGY, mgl);
	if (PlayerKeys(1))
		oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING + 2)->Draw(KEYRINGX, KEYRINGY, mgl);
	if (PlayerKeys(2))
		oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING + 3)->Draw(KEYRINGX, KEYRINGY, mgl);
	i = PlayerKeys(0);
	if (i)
		oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING + 4)->Draw(KEYRINGX, KEYRINGY, mgl);
	if (i > 1)
		oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING + 5)->Draw(KEYRINGX, KEYRINGY, mgl);
	if (i > 2)
		oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING + 6)->Draw(KEYRINGX, KEYRINGY, mgl);

	if (PlayerKeyChain(0))
		oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING + 7)->Draw(KEYRINGX, KEYRINGY, mgl);
	if (PlayerKeyChain(1))
		oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING + 8)->Draw(KEYRINGX, KEYRINGY, mgl);
	if (PlayerKeyChain(2))
		oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING + 9)->Draw(KEYRINGX, KEYRINGY, mgl);
	if (PlayerKeyChain(3))
		oldIntfaceSpr->GetSprite(OLD_SPR_KEYRING + 10)->Draw(KEYRINGX, KEYRINGY, mgl);

	// the brainometer
	oldIntfaceSpr->GetSprite(OLD_SPR_BRAINOMETER)->Draw(617, 342, mgl);
	if (curBrains)
		DrawFillBox(620, 347 + 127 - (curBrains - 1), 635, 347 + 127, 96 + 13 + (curBrains / 8));
	oldIntfaceSpr->GetSprite(OLD_SPR_BRAINOMETER + 1)->Draw(617, 342, mgl);
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
	PrintGlow(639-TILE_WIDTH-GetStrLength(combo,2),479-18,combo,0,2);

	InstaRenderItem(639-TILE_WIDTH/2,479-38,ITM_LOONYKEY,0,mgl);
	sprintf(combo,"%u",profile.progress.loonyKeys-profile.progress.loonyKeysUsed);
	Print(639-TILE_WIDTH-GetStrLength(combo,2)-1,479-38,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2)+1,479-38,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2),479-38+1,combo,-32,2);
	Print(639-TILE_WIDTH-GetStrLength(combo,2),479-38-1,combo,-32,2);
	PrintGlow(639-TILE_WIDTH-GetStrLength(combo,2),479-38,combo,0,2);
}

void RenderInterface(MGLDraw *mgl)
{
	if (shopping)
		return RenderInterfaceShopping(mgl);
	if (profile.progress.hudChoice == HudChoice::Classic)
		return RenderInterfaceOld(mgl);

	int i;
	char combo[16];

	//sprintf(combo,"%d:%02d:%02d",(profile.progress.totalTime/(30*60*60)),(profile.progress.totalTime/(30*60))%60,(profile.progress.totalTime/30)%60);
	//PrintGlow(5,240,combo,0,2);

	if(curMap && curMap->flags&MAP_STEALTH)
	{
		if (player.stealthy)
			intfaceSpr->GetSprite(SPR_STEALTH)->Draw(625,460,mgl);
		else
			intfaceSpr->GetSprite(SPR_STEALTH)->DrawOffColor(625,460,mgl,1,5,0);
	}

	for(i=0;i<NUM_INTF;i++)
	{
		if (i == INTF_WEAPON && player.weapon) // has wepon
		{
			int xx = intf[i].x - 21;
			int yy = intf[i].y - 8;
			intfaceSpr->GetSprite(SPR_WPNICON)->Draw(xx, yy, mgl);
			intfaceSpr->GetSprite(SPR_WPNNAME + player.weapon - 1)->Draw(intf[i].x+20, intf[i].y+1, mgl);
		}

		intfaceSpr->GetSprite(intf[i].spr)->Draw(intf[i].x,intf[i].y,mgl);

		switch (i)
		{
			case INTF_LIFE:
				DrawBigMeter(intf[i].x + intf[i].vOffX, intf[i].y + intf[i].vOffY, intf[i].value, intf[i].valueLength, 12, mgl);
				break;
			case INTF_POWERUP:
				DrawPowerupBar(intf[i].x + intf[i].vOffX, intf[i].y + intf[i].vOffY, mgl);
				break;
		}

		switch(intf[i].valueType)
		{
			case IV_EVILMETER:
				DrawEvilMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				if(monsAlive || intfFlip)
					PrintGlow(intf[i].x+intf[i].vOffX+2,intf[i].y+intf[i].vOffY-1,monsName,0,2);
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
				DrawBigNumber(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
				break;
			case IV_TIME:
				DrawSmallNumber(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value % 60,intf[i].valueLength,mgl, 2);
				if(intf[i].value >=60)
					DrawSmallNumber(intf[i].x+intf[i].vOffX-intf[i].otherVal,intf[i].y+intf[i].vOffY,intf[i].value / 60,intf[i].valueLength, mgl);
				break;
			case IV_VERTMETER:
				DrawVertMeter(intf[i].x+intf[i].vOffX,intf[i].y+intf[i].vOffY,intf[i].value,intf[i].valueLength,mgl);
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
		}
	}

	DrawPortrait(0, 0, mgl); // nice lil portrait

	sprintf(combo,"Combo x%d",curCombo);
	PrintGlow(240,comboY,combo,0,2);
}

void RenderCollectedStuff(int x,int y,MGLDraw *mgl)
{
	int p;

	if((player.worldProg->keychains&KC_LOONY))
		intfaceSpr->GetSprite(SPR_LOONYKEY)->Draw(x+183,y+57,mgl);
	if((player.levelProg->flags&LF_CANDLES))
		intfaceSpr->GetSprite(SPR_CANDLE)->Draw(x+181,y+20,mgl);

	if(PlayerBrains()>=curMap->numBrains)
		intfaceSpr->GetSprite(SPR_BRAIN)->Draw(x+144,y+1,mgl);

	if(player.worldProg->keychains&KC_KEYCH1)
		intfaceSpr->GetSprite(SPR_KEYCH+3)->Draw(x+171,y+29,mgl);
	if(player.worldProg->keychains&KC_KEYCH2)
		intfaceSpr->GetSprite(SPR_KEYCH)->Draw(x+150,y+42,mgl);
	if(player.worldProg->keychains&KC_KEYCH3)
		intfaceSpr->GetSprite(SPR_KEYCH+2)->Draw(x+161,y+29,mgl);
	if(player.worldProg->keychains&KC_KEYCH4)
		intfaceSpr->GetSprite(SPR_KEYCH+1)->Draw(x+160,y+42,mgl);

	p=(int)player.worldProg->percentage;
	if(p>100)
		p=100;
	if(p<0)
		p=0;

	DrawSmallNumber(x+146,y+58,p,3,mgl);
}
