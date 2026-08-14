#ifndef SHOP_H
#define SHOP_H

#include "mgldraw.h"
#include "bitflags.h"

#define SHOP_DEMO	0

// purchase types
#define STYP_MODIFIER			0
#define STYP_FEATURE			1
#define STYP_WORLD				2
#define STYP_CHARACTER			3
#define STYP_GAMEMODE			4
#define STYP_CHEAT				5

// negative mods
#define MDFY_ENEMYDMG			0	// enemies deal more damage
#define MDFY_ENEMYSPD			1	// enemies move twice as fast
#define MDFY_LUDICROUS			2	// getting hit starts a timer
#define MDFY_STATEFCT			3	// burn/poison also weakens the player
#define MDFY_RELOAD				4	// player fires weapons slower
#define MDFY_PLAYERDMG			5	// player deals less damage
#define MDFY_ENEMYHEAL			6	// (most) enemies heal over time

// positive mods
#define MDFY_BRAINRADAR			7	// locate brains if lost
#define MDFY_CANDLERADAR		8	// locate candles if lost
#define MDFY_ANGRY				9	// gain rage twice as fast

#define CHET_AMMOCRATE			0	// ammo supreme crate
#define CHET_AURADMG			1	// gives aura damage for 10s
#define CHET_KABLOOIE			2	// explodes the screen
#define CHET_MAXPANTS			3	// =4 pants, gives a hammer too
#define CHET_KEYMASTER			4	// skeleton key
#define CHET_MEDIC				5	// full health
#define	CHET_ALLBRAINS			6	// ALL the brains!
#define CHET_INVISIBLE			7	// invisible cloak drop
#define CHET_SHIELD				8	// energy barrier drop
#define CHET_POCKET				9	// max pocket space
#define CHET_RAGE				10	// full rage bar
#define CHET_STONESKIN			11	// strength
#define CHET_SUPERFAST			12	// super fast

#define FEAT_EDITOR				0	// editor! (not in demo)
#define FEAT_ARCADE				1	// arcade
#define FEAT_CHEATS				2	//

#define MODE_BACKWARDS			0	// backwards sounds

// Playable characters. SERIALIZED in profile, specials, and leaderboards.
enum : byte
{
	PLAY_BOUAPHA  = 0,
	PLAY_HAPPY    = 1,
	PLAY_LUNATIC  = 2,
	PLAY_SHROOM   = 3,
	PLAY_LUNACHIK = 4,
	PLAY_MECHA    = 5,

	MAX_PLAYAS // End marker. Add new items above this.
};
const char* GetPlayableCharacterName(int playAs);

// shop item flags (used in the player progress file)
enum ShopItemFlags : byte
{
	SIF_AVAILABLE = 1 << 0,
	SIF_BOUGHT    = 1 << 1,
	SIF_ACTIVE    = 1 << 2, // for locker items that can be toggled, is it on or off?
};
BITFLAGS(ShopItemFlags)

// modes you can buy in lockers
#define MODE_RASTER		1
#define MODE_TEENY		6

class Map;

bool CanPlayWorld(const char *fname);
bool ItemPurchased(byte type,byte num);
bool AllOfTypePurchased(byte type);
byte NumOfTypePurchased(byte type);
bool AllPurchased(void);
byte NumPurchased();
byte NumLockersOpen();

byte ShopItemNumber(byte type,word num);
//bool AllLockersOpen(void);
void SetupShops(Map *map);
void DefaultShopAvailability(void);

TASK(void) InitShopping(int x,int y);
TASK(byte) UpdateShopping(MGLDraw *mgl);
void RenderShopping(MGLDraw *mgl);

float ShopPercent(void);

#define NUM_SHOP_ITEMS		(24)
#define NUMBUILTINWORLDS	(0)

#endif
