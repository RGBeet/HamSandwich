#include "winpch.h"
#include "shop.h"
#include "cheat.h"
#include "items.h"
#include "dialogbits.h"
#include "scanner.h"
#include "theater.h"
#include "arcade.h"
#include "moron.h"
#include "gallery.h"
#include "goal.h"

struct ShopItem
{
	const char name[32];
	const char description[128];
	word cost;	// how many Y$
	byte type;	// what type
	byte item;	// which # of the items of "type" it is
};

struct Shop
{
	const char name[32];
	int keeperSprite;				// which init frame of shopkeeper.jsp to use
	byte x;
	byte y;
	std::vector<ShopItem*> items;	// the currently available items
};

static const ShopItem shop[NUM_SHOP_ITEMS] = {
	{ SHOP_DEMO, 100, STYP_MODIFIER, MDFY_ENEMYDMG },
	{ SHOP_DEMO, 100, STYP_MODIFIER, MDFY_ENEMYSPD },

	{ SHOP_DEMO, 100, STYP_MODIFIER, MDFY_LUDICROUS },
	{ SHOP_DEMO, 100, STYP_MODIFIER, MDFY_STATEFCT },
	{ SHOP_DEMO, 100, STYP_MODIFIER, MDFY_RELOAD },

	{ SHOP_DEMO, 100, STYP_MODIFIER, MDFY_PLAYERDMG },
	{ SHOP_DEMO, 500, STYP_FEATURE, FEAT_CHEATS },
	{ SHOP_DEMO, 300, STYP_FEATURE, FEAT_ARCADE },

	{ SHOP_DEMO, 100, STYP_CHEAT, CHET_AMMOCRATE },
	{ SHOP_DEMO, 100, STYP_CHEAT, CHET_AURADMG },
	{ SHOP_DEMO, 100, STYP_CHEAT, CHET_MAXPANTS},

	{ SHOP_DEMO, 100, STYP_CHEAT, CHET_KABLOOIE },
	{ SHOP_DEMO, 100, STYP_CHEAT, CHET_KEYMASTER },
	{ SHOP_DEMO, 2000, STYP_CHARACTER, 1 },
	{ SHOP_DEMO, 2000, STYP_CHARACTER, 2 },
};

ShopItem shopItems[NUM_SHOP_ITEMS] = {
	// modifier
	{ "Devilish Damage", "Enemies deal additional damage against player and allies.", 100, STYP_MODIFIER, MDFY_ENEMYDMG },
	{ "Sinister Speed", "Enemies move super fast!", 100, STYP_MODIFIER, MDFY_ENEMYSPD },
	{ "Ludicrious!", "Starts a countdown after getting hit.", 100, STYP_MODIFIER, MDFY_LUDICROUS },
	{ "Fragility", "Weakens the player upon getting burnt or poisoned.", 100, STYP_MODIFIER, MDFY_STATEFCT },
	{ "Slow Draw", "Player fires weapons slower.", 100, STYP_MODIFIER, MDFY_RELOAD },
	{ "Damage Lite", "Player deals less damage.", 100, STYP_MODIFIER, MDFY_PLAYERDMG },
	{ "Monster Medic", "Enemies regain HP over time.", 100, STYP_MODIFIER, MDFY_ENEMYHEAL },
	// features
	{ "Level Editor", "Make your own levels with developer tools!", 1000, STYP_FEATURE, FEAT_EDITOR },
	{ "Arcade Corner", "Play some minigames, win some coins!", 500, STYP_FEATURE, FEAT_ARCADE },
	{ "Cheat Book", "Game too hard? Use your Cheatzys (C$) and cheat your way to the win!", 1000, STYP_FEATURE, FEAT_CHEATS },
	// cheats
	{ "Ammo Crate", "Unlimited ammo, for a short time.", 100, STYP_CHEAT, CHET_AMMOCRATE },
	{ "Killer Aura", "Defeat your foes by standing near them.", 100, STYP_CHEAT, CHET_AURADMG },
	{ "Bombs Away!", "Commence spontaneous explosions upon your screen.", 100, STYP_CHEAT, CHET_KABLOOIE },
	{ "Pairs Upon Pairs", "Maximize your fire rate instantly!", 100, STYP_CHEAT, CHET_MAXPANTS },
	{ "Keymaster", "Immediately obtain a Skeleton Key, which unlocks most doors.", 100, STYP_CHEAT, CHET_KEYMASTER },
	{ "MEDIC!", "Defeat your foes by standing near them.", 100, STYP_CHEAT, CHET_MEDIC },
	{ "Brainiac", "Instantly acquire all brains.", 100, STYP_CHEAT, CHET_ALLBRAINS },
	{ "Secret Agent", "Gain temporary invisibility.", 100, STYP_CHEAT, CHET_INVISIBLE },
	{ "Small Shield", " Gain temporary invincibility.", 100, STYP_CHEAT, CHET_SHIELD },
	{ "Killer Aura", "Defeat your foes by standing near them.", 100, STYP_CHEAT, CHET_POCKET },
	{ "MEGA RAGE!", "Fill your rage bar to the MAX!!!", 100, STYP_CHEAT, CHET_RAGE },
	{ "Stoneskin", "Gives you temporary Strength.", 100, STYP_CHEAT, CHET_STONESKIN },
	{ "Super Speed", "Gives you tempoerary Super Speed.", 100, STYP_CHEAT, CHET_SUPERFAST },
	// game mode
	{ "Subliminal Mode", "Margoloh a si esrevinu eht dna, sdrawkcab si gnihtyreve.", 1000, STYP_GAMEMODE, FEAT_CHEATS },
};

Shop shopList[1] = {
	{"Triton's Treasures", 0, 2, 2, {} }
};

int curSelection	= 0;
int scrollOffset	= 0;

///------------------------------------------

static char shopTxt[256];
static byte buying, buyMode;
static byte shopSize;
static byte cursor;
static Shop* currentShop;

void ClearShop()
{
	currentShop		= nullptr;
	curSelection	= 0;
	scrollOffset	= 0;
	buying			= false;
}

void PickShop(byte val)
{

}

// INIT SHOPPING!
static byte modeToToggle;

TASK(void) InitShopping(int x, int y)
{
	int i;
	char tmp[32], tmp2[32], tmp3[64];

	modeToToggle = 0;

	GetTaps();
	GetArrowTaps();

}

byte IsPurchased(word j)
{
	return 0; //!(profile.progress.purchase[j] & SIF_BOUGHT)
}

byte MatchesType(word j)
{
	return 0; //!(profile.progress.purchase[j] & SIF_BOUGHT)
}

bool CanPlayWorld(const char *fname)
{
	return true;
}

bool AllOfTypePurchased(byte type)
{
	int j;
	for(j=0;j<NUM_SHOP_ITEMS;j++)
		if(!IsPurchased(j) && MatchesType(j))
			return false;

	return true;	// they've all been bought!
}

byte NumOfTypePurchased(byte type)
{
	int j, i = 0;
	for(j=0;j<NUM_SHOP_ITEMS;j++)
		if (!IsPurchased(j) && MatchesType(j))
			return ++i;
	return i;
}

bool AllPurchased(void)
{
	int j;
	for (j = 0;j < NUM_SHOP_ITEMS;j++)
		if (!IsPurchased(j))
			return false;
	return true;	// they've all been bought!
}

byte NumPurchased()
{
	int j, i = 0;
	for(j=0;j<NUM_SHOP_ITEMS;j++)
		for (j = 0;j < NUM_SHOP_ITEMS;j++)
			if (!IsPurchased(j))
				++i;
	return i;
}

float ShopPercent(void)
{
	int j;
	int bought;

	bought=0;
	for(j=0;j<NUM_SHOP_ITEMS;j++)
		if(profile.progress.purchase[j]&SIF_BOUGHT)
			bought++;

	return ((float)bought*100.0f/(float)NUM_SHOP_ITEMS);
}

bool ItemPurchased(byte type,word num)
{
	return IsPurchased(num);	// true if you've bought it
}

byte ItemsInShop(byte shopNum)
{
	return shopList[shopNum].items.size();
}

ShopItem* GetRandomItemFromShop(byte shopNum, ShopItem* invalid1 = nullptr, ShopItem* invalid2 = nullptr, ShopItem* invalid3 = nullptr)
{
	ShopItem* validItems[NUM_SHOP_ITEMS];
	int numValid = 0;

	for (int i = 0; i < NUM_SHOP_ITEMS; i++)
	{
		ShopItem* item = &shopItems[i];

		if (item->type != shopNum)
			continue;

		if (item == invalid1 || item == invalid2 || item == invalid3)
			continue;

		validItems[numValid++] = item;
	}

	if (numValid == 0)
		return nullptr;

	return validItems[Random(numValid)];
}

void SetupShops(Map* map)
{
	int i, j, k;

	for (i = 1; i < 1; i++) // set up for future shops
	{
		k = ItemsInShop(i);
		printf("There are %d items available for Shop %s.", k, currentShop->name);
		ShopItem* itm[4] = {};

		for (j = 0; j < std::min(4, k); j++)
		{
			itm[j] = GetRandomItemFromShop(i, itm[0], itm[1], itm[2]);
			if (!itm[j])
				break;
			printf("Added %s to Shop %s!", itm[j]->name, currentShop->name);
			currentShop->items.push_back(itm[j]);
		}
	}
}

void DefaultShopAvailability(void)
{
	int i;

	for(i=0;i<NUM_SHOP_ITEMS;i++)
	{
		profile.progress.purchase[i]=SIF_AVAILABLE;
	}
}

const char *GetModifierName(byte w)
{
	switch(w)
	{
	}
	return "Unknown!?";
}

void SetObtainText(void)
{
	char tmp[32],tmp2[32],tmp3[64];

	// ad
	switch(shop[buying].type)
	{

	}
}

TASK(void) BuyItem(void)
{
	int i;
	word cost;

	//	if(shop[buying].shop<11 && shop[buying].shop>0 && ItemPurchased(SHOP_DISCOUNT,shop[buying].shop-1))
	//		cost=shop[buying].cost*9/10;
	//	else
	cost=shop[buying].cost;

	buyMode=1;
	if(profile.progress.totalCoins-profile.progress.coinsSpent>=cost)
	{
		profile.progress.coinsSpent+=cost;
		SetObtainText();

		profile.progress.purchase[buying]&=(~SIF_AVAILABLE);
		profile.progress.purchase[buying]|=SIF_BOUGHT;
		MakeNormalSound(SND_BUYITEM);

		SetupShops(curMap);
	}
	else
	{
		strcpy(shopTxt,"Hey buddy, you don't have enough coins to buy that!");
	}
	GoalPurchase();
}

// UPDATE SHOPPING
TASK(byte) UpdateShopping(MGLDraw *mgl)
{
	byte c;

	goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	goodguy->dx=0;
	goodguy->dy=0;

	if(shopSize<150)
		shopSize+=8;

	if(mgl->LastKeyPressed()==27)
		CO_RETURN 0;	// ESC to cancel shopping

	c=GetTaps()|GetArrowTaps();

	if(buyMode==0)
	{
		if(c&CONTROL_LF)
			cursor=0;
		if(c&CONTROL_RT)
			cursor=1;

		if(c&CONTROL_B1)
		{
			if(cursor==1)
				AWAIT BuyItem();
			else
				CO_RETURN 0;
		}
	}
	else
	{
		if(c&CONTROL_B1)
		{
			if(modeToToggle)
			{
				profile.progress.purchase[modeToToggle]^=SIF_ACTIVE;
				if(CanDoTeeny())
				{
					GetDisplayMGL()->LoadBMP("graphics/gamepal.bmp");
					AWAIT GetDisplayMGL()->Flip();
				}
				SetupShops(curMap);
			}
			CO_RETURN 0;
		}
	}

	CO_RETURN 1;
}

void RenderShopButton(byte on,int x,int y,int wid,const char *txt,MGLDraw *mgl)
{
	if(!on)
	{
		mgl->Box(x,y,x+wid,y+19,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+19-1,32*3+4);
		Print(x + 2, y + 3, txt, 0, 2);
	}
	else
	{
		mgl->Box(x,y,x+wid,y+19,32+31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+19-1,32*7+8);
		PrintWavy(x + 2, y + 3, txt, 0, 2, timeGetTime()/10, 1, 0.5);
	}
}

void RenderShopping(MGLDraw *mgl)
{
	mgl->FillBox(320-shopSize,240-shopSize/2,320+shopSize,240+shopSize/2,32*3+4);

	if(shopSize>2)
		mgl->Box(320-shopSize+1,240-shopSize/2+1,320+shopSize-1,240+shopSize/2-1,32*3+16);

	if(buyMode==0)
	{
		if(shopSize>=150)
		{
			PrintRect(320-shopSize+4,240-shopSize/2+4,320+shopSize-4,240+shopSize/2-4,18,shopTxt,2);

			RenderShopButton((cursor==0),320-shopSize+10,240+shopSize/2-27,95,"No Thanks!",mgl);
			RenderShopButton((cursor==1),320+shopSize-105,240+shopSize/2-27,95,"Yes Please!",mgl);
		}
	}
	else
	{
		if(shopSize>=150)
		{
			PrintRect(320-shopSize+4,240-shopSize/2+4,320+shopSize-4,240+shopSize/2-4,18,shopTxt,2);
			RenderShopButton(1,320-48,240+shopSize/2-27,95,"Thank you!",mgl);
		}
	}
}

static const char charName[][16] = {
	"Bouapha",
	"Happy Stick Man",
	"Dr. Lunatic",
	"Shtupid Shroom",
	"LunaChick",
	"MechaBouapha",
};
static_assert(std::size(charName) == MAX_PLAYAS, "Must give new playable character a name");

const char* GetPlayableCharacterName(int playAs)
{
	if (playAs >= 0 && playAs < MAX_PLAYAS)
		return charName[playAs];
	return "???";
}

