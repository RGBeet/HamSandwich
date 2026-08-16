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
	byte shop;
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

ShopItem shopItems[NUM_SHOP_ITEMS] = {
	// modifier
	{ SHOP_DEMO, "Devilish Damage", "Enemies deal additional damage against player and allies.", 100, STYP_MODIFIER, MDFY_ENEMYDMG },
	{ SHOP_DEMO, "Sinister Speed", "Enemies move super fast!", 100, STYP_MODIFIER, MDFY_ENEMYSPD },
	{ SHOP_DEMO, "Ludicrious!", "Starts a countdown after getting hit.", 100, STYP_MODIFIER, MDFY_LUDICROUS },
	{ SHOP_DEMO, "Fragility", "Weakens the player upon getting burnt or poisoned.", 100, STYP_MODIFIER, MDFY_STATEFCT },
	{ SHOP_DEMO, "Slow Draw", "Player fires weapons slower.", 100, STYP_MODIFIER, MDFY_RELOAD },
	{ SHOP_DEMO, "Damage Lite", "Player deals less damage.", 100, STYP_MODIFIER, MDFY_PLAYERDMG },
	{ SHOP_DEMO, "Monster Medic", "Enemies regain HP over time.", 100, STYP_MODIFIER, MDFY_ENEMYHEAL },
	// features
	{ SHOP_DEMO, "Level Editor", "Make your own levels with developer tools!", 1000, STYP_FEATURE, FEAT_EDITOR },
	{ SHOP_DEMO, "Arcade Corner", "Play some minigames, win some coins!", 500, STYP_FEATURE, FEAT_ARCADE },
	{ SHOP_DEMO, "Cheat Book", "Game too hard? Use your Cheatzys (C$) and cheat your way to the win!", 1000, STYP_FEATURE, FEAT_CHEATS }, // #10
	// cheats
	{ SHOP_DEMO, "Ammo Crate", "Unlimited ammo, for a short time.", 100, STYP_CHEAT, CHET_AMMOCRATE },
	{ SHOP_DEMO, "Killer Aura", "Defeat your foes by standing near them.", 100, STYP_CHEAT, CHET_AURADMG },
	{ SHOP_DEMO, "Bombs Away!", "Commence spontaneous explosions upon your screen.", 100, STYP_CHEAT, CHET_KABLOOIE },
	{ SHOP_DEMO, "Pairs Upon Pairs", "Maximize your fire rate instantly!", 100, STYP_CHEAT, CHET_MAXPANTS },
	{ SHOP_DEMO, "Keymaster", "Immediately obtain a Skeleton Key, which unlocks most doors.", 100, STYP_CHEAT, CHET_KEYMASTER },
	{ SHOP_DEMO, "Healthy Glow", "Full Heal.", 100, STYP_CHEAT, CHET_MEDIC },
	{ SHOP_DEMO, "Brainiac", "Instantly acquire all brains.", 100, STYP_CHEAT, CHET_ALLBRAINS },
	{ SHOP_DEMO, "Secret Agent", "Gain temporary invisibility.", 100, STYP_CHEAT, CHET_INVISIBLE },
	{ SHOP_DEMO, "Small Shield", " Gain temporary invincibility.", 100, STYP_CHEAT, CHET_SHIELD },
	{ SHOP_DEMO, "Hammer Space", "Maximum pocket space!", 100, STYP_CHEAT, CHET_POCKET },
	{ SHOP_DEMO, "MEGA RAGE!", "Fill your rage bar to the MAX!!!", 100, STYP_CHEAT, CHET_RAGE },
	{ SHOP_DEMO, "Stoneskin", "Gives you temporary Strength.", 100, STYP_CHEAT, CHET_STONESKIN },
	{ SHOP_DEMO, "Super Speed", "Gives you tempoerary Super Speed.", 100, STYP_CHEAT, CHET_SUPERFAST },
	// game mode
	{ SHOP_DEMO, "Subliminal Mode", "Margoloh a si esrevinu eht dna, sdrawkcab si gnihtyreve.", 1000, STYP_GAMEMODE, 0 },
};

Shop shoppes[1] = {
	{"Triton's Treasures", 0, 6, 7, {} }
};

int curSelection	= 0;
int scrollOffset	= 0;

///------------------------------------------

static char shopTxt[256];
static byte buying, buyMode, canBuy;
static word shopSize;
static byte cursor;


static Shop* currentShop;
static byte mallMode = 0;

void ExitShop()
{
	currentShop		= nullptr;
	curSelection	= 0;
	scrollOffset	= 0;
	buying			= false;
}

// you have a shop now!
void EnterShop(Shop *shopToLoad)
{
	printf("[SHOP] Entering shop %s!\n", shopToLoad->name);
	currentShop		= shopToLoad;
	curSelection	= 0;
	scrollOffset	= 0;
	buying			= true;
	buyMode			= 2;
	cursor			= 1;

	canBuy			= currentShop->items.size()>0;
	MakeNormalSound(SND_SHOPBELL);
	SendMessageToGame(MSG_SHOPNOW, 0);
}

static byte modeToToggle;


void ResetAllPurchases()
{
	int i;
	for(i=0;i<NUM_SHOP_ITEMS;i++)
	{
		switch (shopItems[i].type)
		{
			case STYP_CHEAT: // require the cheat book
				profile.progress.purchase[i] = 0; // not available 
				break;
			default:
				profile.progress.purchase[i] = SIF_AVAILABLE; // it's available, but not purchased
				break;
		}
	}
}

byte IsAvailable(word j)
{
	return (profile.progress.purchase[j] & SIF_AVAILABLE);
}

byte IsPurchased(word j)
{
	return (profile.progress.purchase[j] & SIF_BOUGHT);
}

byte MatchesType(word j, byte type)
{
	return shopItems[j].type == type;
}

byte MatchesShop(word j, byte shop)
{
	return shopItems[j].shop == shop;
}

int GetRandomChoice(int max)
{
	return (profile.randomSeed >> 16) % max;
}

std::vector<ShopItem*> GetRandomItemForShop(byte shopNum, int amount)
{
	int i;
	std::vector<ShopItem*> validItems;

	for (i=0; i<NUM_SHOP_ITEMS; i++)
	{
		if (IsPurchased(i) || !IsAvailable(i))
			continue;

		if (MatchesShop(i,shopNum))
			validItems.push_back(&shopItems[i]);
	}

	amount = std::min(amount, (int)validItems.size());

	std::vector<ShopItem*> output;

	for (i=0; i<amount; i++)
	{
		int choice = GetRandomChoice((int)validItems.size());
		output.push_back(validItems[choice]);

		// push the chosen item to the back, then remove it
		validItems[choice] = validItems.back();
		validItems.pop_back();
	}

	return output;
}

void SetupShops(Map* map)
{
	int i, j, k;
	printf("Setup Shops moment");

	if (shopping == 1)
	{
		shopping = 2;
		printf("Randomizing shoppes");

		MGL_srand(profile.randomSeed); // seeded random
		// randomize the shop contents based on the profile seed
		for (i = 0; i<1; i++)
		{
			printf("Items for %s:",shoppes[i].name);
			shoppes[i].items = GetRandomItemForShop(i,6);
			for (j=0; j<shoppes[i].items.size(); j++)
			{
				if(j+1<shoppes[i].items.size())
					printf(" %s,",shoppes[i].items[j]->name);
				else
					printf(" and %s.\n", shoppes[i].items[j]->name);
			}
		}
	}
}

// initialize whenever bouapha runs into custom object
TASK(void) InitShopping(int x, int y)
{
	int i;
	char tmp[32], tmp2[32], tmp3[64];

	modeToToggle = 0;

	GetTaps();
	GetArrowTaps();

	for (i = 0; i < 1; i++)
	{
		if (shoppes[i].x == x && shoppes[i].y == y)
		{
			EnterShop(&shoppes[i]); // we found the shop!
			break;
		}
	}
}

bool CanPlayWorld(const char *fname)
{
	return true;
}

bool AllOfTypePurchased(byte type)
{
	int j;
	for(j=0;j<NUM_SHOP_ITEMS;j++)
		if(!IsPurchased(j) && MatchesType(j,type))
			return false;

	return true;	// they've all been bought!
}

byte NumOfTypePurchased(byte type)
{
	int j, i = 0;
	for(j=0;j<NUM_SHOP_ITEMS;j++)
		if (IsPurchased(j) && MatchesType(j,type))
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
			if (IsPurchased(j))
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
	return shoppes[shopNum].items.size();
}

void DefaultShopAvailability(void)
{
	int i;

	for(i=0;i<NUM_SHOP_ITEMS;i++)
	{
		profile.progress.purchase[i]=SIF_AVAILABLE;
	}
}

void SetObtainText(void)
{
	char tmp[32],tmp2[32],tmp3[64];

}

word GetItemIndex(ShopItem* itm)
{
	for (int i = 0; i < NUM_SHOP_ITEMS; i++)
	{
		if (&shopItems[i] == itm)
			return i;
	}
	return 511;
}

TASK(void) BuyItem()
{
	int i;
	ShopItem* itemToBuy = currentShop->items[curSelection];
	int totalCost = itemToBuy->cost;

	if (profile.progress.totalCoins - profile.progress.coinsSpent >= totalCost)
	{
		word index = GetItemIndex(itemToBuy);

		if (index < NUM_SHOP_ITEMS)
		{
			byte normalPurchase = 1;
			MakeNormalSound(SND_TURNGOOD);

			profile.progress.coinsSpent += totalCost;				// "buy" the item


			switch (itemToBuy->type)
			{
				case STYP_FEATURE: // some features have special values?
					// check what item it is...
					switch (itemToBuy->item)
					{
						case FEAT_EDITOR:
							// unlock da level editor
							break;
						case FEAT_ARCADE:
							// unlock da arcade
							break;
						case FEAT_CHEATS:
							for(i=0;i<NUM_SHOP_ITEMS;i++)
							{
								if (shopItems[i].type == STYP_CHEAT)
									profile.progress.purchase[i] |= SIF_AVAILABLE; // all cheats are now available! woot!!
							}
							// unlock da cheat zone
							break;
					}
					break;
			}

			if (normalPurchase) // not available because single-time purchase.
			{
				profile.progress.purchase[index] &= ~SIF_AVAILABLE;
				profile.progress.purchase[index] |= SIF_BOUGHT;
			}

			for(i=0;i<currentShop->items.size();i++)
			{
				if (currentShop->items[i] == itemToBuy)
					currentShop->items.erase(currentShop->items.begin()+i);
			}

			printf(
				"Index #%d is now %d.\n",
				index,
				profile.progress.purchase[index]);

			SetupShops(curMap);
		}
	}
	else
	{
		MakeNormalSound(SND_TURNEVIL);
	}
}

// UPDATE SHOPPING
TASK(byte) UpdateShopping(MGLDraw *mgl)
{
	byte c;

	goodguy->x=(goodguy->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
	goodguy->y=(goodguy->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
	goodguy->dx=0;
	goodguy->dy=0;

	c=GetTaps()|GetArrowTaps();

	if (buyMode == 2)
	{
		if (shopSize < 320)
			shopSize += 32;
		else
			buyMode = 0;
	}
	else if (buyMode == 3)
	{
		if (shopSize > 0)
			shopSize -= 32;
		else
			CO_RETURN 0;
	}

	if(buyMode==0)
	{
		if (mgl->LastKeyPressed() == 27)
			buyMode = 3;

		int numItems = currentShop->items.size();

		if (c & CONTROL_DN)
		{
			if (curSelection < numItems - 1)
			{
				curSelection++;
				MakeNormalSound(SND_MENUCLICK);
			}
				
		}

		if (c & CONTROL_UP)
		{
			if (curSelection > 0)
			{
				curSelection--;
				MakeNormalSound(SND_MENUCLICK);
			}
		}

		if (curSelection < scrollOffset)
			scrollOffset = curSelection;

		if (curSelection >= scrollOffset + 6)
			scrollOffset = curSelection - 5;

		if (canBuy)
		{
			if (c & CONTROL_LF && cursor != 0)
			{
				cursor = 0;
				MakeNormalSound(SND_WORLDTURN);
			}

			if (c & CONTROL_RT && cursor != 1)
			{
				cursor = 1;
				MakeNormalSound(SND_WORLDTURN);
			}
		}

		if (c & CONTROL_B1)
		{
			if (cursor == 0)
			{
				// attempt to buy the item
				AWAIT BuyItem();
			}
			else
			{
				// exit
				buyMode = 3;
			}
		}
	}
	else if (buyMode == 1)
	{
		if(c&CONTROL_B1)
		{
			buyMode = 3;
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

void GetFullItemName(ShopItem* item, char name[], int maxLen)
{
	const char* prefix = "";
	switch (item->type)
	{
	case STYP_MODIFIER:
		prefix = "Modifier: ";
		break;
	case STYP_FEATURE:
		prefix = "Feature: ";
		break;
	case STYP_CHEAT:
		prefix = "Cheat: ";
		break;
	case STYP_GAMEMODE:
		prefix = "Game Mode: ";
		break;
	}
	snprintf(name, maxLen, "%s%s", prefix, item->name);
}

void RenderShopping(MGLDraw* mgl)
{
	int border = 8;
	int padding = 12;

	int x1 = (SCRWID / 2) - shopSize;
	int x2 = (SCRWID / 2) + shopSize;
	int hy = shopSize * SCRHEI / SCRWID;
	int y1 = (SCRHEI / 2) - hy;
	int y2 = (SCRHEI / 2) + hy;

	// Background
	mgl->FillBox(x1, y1, x2, y2, 32 * 3 + 4);

	for (int i = 0; i < border; i++)
	{
		mgl->Box(
			x1 + i,
			y1 + i,
			x2 - i,
			y2 - i,
			32 * 3 + 8 + i);
	}

	int ix1 = x1 + border + padding;
	int ix2 = x2 - border - padding;
	int iy1 = y1 + border + padding;
	int iy2 = y2 - border - padding;

	if (shopSize < 320)
		return;

	// shopkeeper
	int keeperBottom = iy1 + 80;

	char s[32];

	sprintf(s, "Sprite #%d", currentShop->keeperSprite);
	Print(ix1, iy1, s, 0, 1);

	PrintRect(
		ix1 + 80,
		iy1,
		ix2 - ix1 - 80,
		keeperBottom - iy1,
		240,
		currentShop->name,
		1);

	// items
	const int rowh = 30;
	const int maxVisibleItems = 6;
	int ly1 = keeperBottom + 8;
	int ly2 = ly1 + rowh * maxVisibleItems;

	int visibleItems = std::min(6,(int)currentShop->items.size());

	char fullName[64];
	for (int i = 0; i < visibleItems; i++)
	{
		int index = scrollOffset + i;

		if (index >= currentShop->items.size())
			break;

		ShopItem* itm = currentShop->items[index];

		if (curSelection < 0 || curSelection >= (int)currentShop->items.size())
			continue;

		int yy = ly1 + i * rowh;

		// Selection
		if (index == curSelection)
		{
			for (int j = 0; j < 2; j++)
			{
				DrawBox(
					ix1 + 16 - j,
					yy - j,
					ix1 + 256 + j,
					yy + 20 + j,
					32 * 5 + 8);
			}
		}

		// Icon
		// DrawShopItemIcon(itm, ix1 + 4, yy + 3);

		// Name
		GetFullItemName(itm, fullName, sizeof(fullName));
		Print(
			ix1 + 30,
			yy + 4,
			fullName,
			0,
			2);

		// Cost
		sprintf(s, "%d $Y", itm->cost);

		Print(
			ix2 - 60,
			yy + 4,
			s,
			0,
			2);
	}

	// description
	int descY = ly2 + 8;

	if (curSelection >= 0 && curSelection < (int)currentShop->items.size())
	{
		ShopItem* itm = currentShop->items[curSelection];
		GetFullItemName(itm, fullName, sizeof(fullName));

		Print(
			ix1,
			descY,
			fullName,
			0,
			2);

		PrintRect(
			ix1,
			descY + 20,
			ix2 - ix1,
			40,
			240,
			itm->description,
			1);
	}

	// buttons
	int buttonY = iy2 - 28;

	int bx1		= ix1+80;
	int bx2		= bx1+70;
	int ex1		= ix2-150;
	int ex2		= ex1+70;

	if (cursor == 0)
	{
		for (int j = 0; j < 2; j++)
		{
			DrawBox(
				bx1 - j,
				buttonY - j,
				bx2 + j,
				buttonY + 20 + j,
				32 * 5 + 8);
		}
	}
	else
	{
		for (int j = 0; j < 2; j++)
		{
			DrawBox(
				ex1 - j,
				buttonY - j,
				ex2 + j,
				buttonY + 20 + j,
				32 * 5 + 8);
		}
	}

	// Buy
	if (canBuy)
		CenterPrint(
			bx1 + 10,
			buttonY,
			"BUY",
			0,
			2);

	// Exit
	CenterPrint(
		ex1 + 10,
		buttonY,
		"EXIT",
		0,
		2);
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

