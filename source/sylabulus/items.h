#ifndef ITEMS_H
#define ITEMS_H

#include <stdio.h>
#include "jamulspr.h"
#include "bitflags.h"

enum ItemType : word
{
	IT_NONE,
	// Basic items
	IT_HAMMERUP,
	IT_PANTS,
	IT_BRAIN,
	IT_CANDLE,
	IT_COIN,
	// Hammer powerups
	IT_PROJMIRROR,
	IT_PROJBOUNCE,
	IT_UNHAMMER,
	IT_UNPANTS,
	// Weapons
	IT_MISSILES,
	IT_LASERGUN,
	IT_BOMBS,
	IT_TOASTER,
	IT_BIGAXE,
	IT_LIGHTNING,
	IT_SPEARS,
	IT_MACHETE,
	IT_MINES,
	IT_FREEZERAY,
	// New weapons
	IT_BOOMERANG,
	IT_MEGAPHONE,
	IT_ROCKETEER,
	IT_LUNCHBOX,
	// Fancy weapons
	IT_TORCH,
	IT_TURRETWPN,
	IT_MINDCONTROL,
	IT_REFLECTOR,
	IT_JETPACK,
	IT_SWAPGUN,
	IT_SCANNER,
	IT_STOPWATCH,
	IT_RAINBOWGUN,
	IT_BLACKHOLE,
	IT_GLUEGUN,
	IT_BFG,
	IT_REGENERATOR,
	IT_DESTABILIZER,
	IT_DECOY,
	// Mech weapons
	IT_PWRARMOR,
	IT_MINISUB,
	// Food + powerups
	IT_TAKEOUT,
	IT_SHIELD,
	IT_RADIOFOOD,
	IT_PIZZA,
	IT_GARLIC,
	IT_ORBREG,
	IT_ORBEXPL,
	IT_ORBSHCK,
	IT_ACCEL,
	IT_STRONG,
	IT_INVIS,
	IT_AMMOSML,
	IT_AMMOMED,
	IT_INFAMMO,
	IT_SCUBA,
	IT_SUPREME,
	IT_WATERBOOTS,
	IT_POCKET,
	IT_BIGBRAIN,
	IT_BIGCANDLE,
	IT_BIGCOIN,
	// Keys
	IT_KEYONE,
	IT_KEYRED,
	IT_KEYGRN,
	IT_KEYBLU,
	IT_KEYCH1,
	IT_KEYCH2,
	IT_KEYCH3,
	IT_KEYCH4,
	IT_KEYCH5,
	IT_LOONYKEY,
	// Buildings / exits
	IT_EXITDOOR,
	IT_HOLETREE,
	IT_HOUSE,
	IT_CASTLE,
	IT_IGLOO,
	// Terrain / scenery
	IT_GRASS1,
	IT_GRASS2,
	IT_CONE,
	IT_CONEFALLEN,
	IT_MAILBOX,
	IT_BLUEBOX,
	IT_HYDRANT,
	IT_METALBARREL,
	IT_TOXICWASTE,
	IT_OILDRUM,
	IT_CRATEWOOD,
	IT_CRATEMETAL,
	// Signs / street objects
	IT_WOODPOST,
	IT_SIGNMETAL,
	IT_SIGNLETTERS,
	IT_SIGNSKULL,
	IT_SIGNLEFTWOOD,
	IT_SIGNRIGHTWOOD,
	IT_SIGNINFOWOOD,
	IT_SIGNDANGER,
	IT_STOPSIGN,
	IT_DONOTENTER,
	IT_SIGNLEFT,
	IT_SIGNRIGHT,
	IT_SIGNDOWN,
	IT_SIGNUP,
	IT_STREETLIGHT,
	IT_PATHSIGN1,
	IT_PATHSIGN2,
	IT_WOODBARREL,
	IT_LEVER1,
	IT_LEVER2,
	IT_SCONCE,
	IT_TRAFFIC1,
	IT_TRAFFIC2,
	IT_TRAFFIC3,
	// Trees / plants
	IT_TREE1,
	IT_TREE2,
	IT_TREE3,
	IT_BUSH1,
	IT_BUSH2,
	IT_BUSH3,
	IT_PALM1,
	IT_PALM2,
	IT_PALMFAT1,
	IT_PALMFAT2,
	IT_PINETREE,
	IT_XMASTREE,
	IT_FALLENTREE,
	IT_DEADTREE1,
	IT_DEADTREE2,
	IT_STUMP,
	IT_SEAWEED1,
	IT_SEAWEED2,
	IT_SEAWEED3,
	IT_CATTAILS,
	IT_TOADSTOOL,
	// Rocks
	IT_ROCKS1,
	IT_ROCKS2,
	IT_ROCKS3,
	IT_BRNROCKTAL,
	IT_BRNROCKSML,
	IT_FLATROCK,
	IT_PEBBLE,
	IT_BUBBLES,
	// Doors / blocks
	IT_DOORFONE,
	IT_DOORFRED,
	IT_DOORFGRN,
	IT_DOORFBLU,
	IT_DOORSONE,
	IT_DOORSRED,
	IT_DOORSGRN,
	IT_DOORSBLU,
	// Furniture / miscellaneous
	IT_CHAIRDR,
	IT_CHAIRDL,
	IT_CHAIRUL,
	IT_CHAIRUR,
	IT_TABLE,
	IT_TRASHCAN1,
	IT_TRASHCAN2,
	IT_TARGET,
	// Potions / collectibles
	IT_MANAPOTION,
	IT_RAGEPOTION,
	IT_PRESENT,
	IT_GEMSTONESML,
	IT_GEMSTONELRG,
	// Miscellaneous scenery
	IT_MINEBLOCK,
	IT_TALLGRASS,
	IT_HAYBALE,
	IT_POWDERKEG,
	IT_SAKURA1,
	IT_SAKURA2,
	IT_CRYSTAL1,
	IT_CRYSTAL2,
	IT_GONG,
	NUM_ORIGINAL_ITEMS = 175,
	IT_RANDOM = 65535
};
#define MAX_ITEMS			1000
#define CUSTOM_ID_START		1000

enum ItemAppearance : byte
{
	ITA_NONE,			// no shadow
	ITA_SHADOW,			// shadow
	ITA_GLOWING,		// glow draw
	ITA_LOONYKEY,		// loonycolor + shadow
	ITA_TILEIMG,		// tile image (conflicts with glowing/loonykey/shadow),
	ITA_MAX
};

enum ItemPassability : byte
{
	ITP_FREEWALK,		// does not impair movement
	ITP_SOLID,			// cannot walk through
	ITP_BULLETPROOF,	// solid + no bullets
	ITP_BARRIER,		// free walk + no bullets
	ITP_PICKUP,			// can pick up (conflicts with solid)
	ITP_MAX
};

enum ItemTrigger : byte
{
	ITRG_NONE,
	ITRG_PICKUP,		// if the player picks it up... (requires ITP_PICKUP!)
	ITRG_PROJECTILE,	// if any bullet hits the item
	ITRG_PLAYERBUMP,	// if player bumps into the item
	ITRG_FRIENDBUMP,	// if an entity on player team bumps into the item
	ITRG_ENEMYBUMP,		// if an entity outside player team bumps into the item
	ITRG_ENTITYBUMP,	// if any entity bumps into item, regardless of team
	ITRG_MINECART,		// if hit by a "vehicle"
	ITRG_MACHETE,		// if hit by a machete blade...
	ITRG_EXPLOSION,		// if hit by an explosion...
	ITRG_FIRE,			// if hit by a fire bullet...
	ITRG_FREEZE,		// if hit by an icy bullet...
	ITRG_ALWAYS,		// always happens whenever tiles animate
	ITRG_MAX
};

enum ItemBehavior : word
{
	ITB_NONE,
	ITB_BUBBLES,		// bubble particle, for bubble spots
	ITB_FIRE,			// fire particle, for sconces
	ITB_MAX
};

// themes
enum ItemThemes : dword
{
	ITH_COLLECTIBLE		= 1 << 0,	// player pickup: collectibles+keys
	ITH_WEAPON			= 1 << 1,	// player pickup: weapons
	ITH_POWERUP			= 1 << 2,	// player pickup: powerups+food
	ITH_OBSTACLE		= 1 << 3,	// obstacles
	ITH_BULLETPROOF		= 1 << 4,	// obstacles: cannot shoot thru
	ITH_PLANTS			= 1 << 5,	// obstacles: vegetation
	ITH_ROCKS			= 1 << 6,	// obstacles: rocks
	ITH_URBAN			= 1 << 7,	// obstacles: urban
	ITH_SIGNS			= 1 << 8,	// obstacles: signage
	ITH_DECOR			= 1 << 9,	// decoration (not obstacles or pickups)
	ITH_ENTRANCE		= 1 << 10,	// decoration: entrances
	ITH_LARGE			= 1 << 11,	// very big item, usually obstacles.
	ITH_CUSTOM			= 1 << 12,	// custom items! YAY!
};
BITFLAGS(ItemThemes)

// triggers
enum ItemTriggers : word
{
	ITR_NONE       = 0,	// can't be triggered
	ITR_GET        = 1 << 0,	// triggered on pickup
	ITR_SHOOT      = 1 << 1,	// triggers when shot (must be shootable)
	ITR_PLAYERBUMP = 1 << 2,	// triggers when stepped on/bumped into by player
	ITR_ENEMYBUMP  = 1 << 3,	// triggers when bumped by an enemy
	ITR_FRIENDBUMP = 1 << 4,	// step/bump by friendly monster
	ITR_CHOP       = 1 << 5,	// triggers when hit by machete or other chopper
	ITR_MINECART   = 1 << 6,	// triggers when crashed into by a minecart
	ITR_ALWAYS     = 1 << 7,	// triggers repeatedly, as fast as tiles animate
};
BITFLAGS(ItemTriggers)

// effects
// what occurs when you pick up the item if it's a pickup,
// or bump it if it's not
#define IE_NONE		0	// none
#define IE_DESTROY	1	// vanishes, with particles of color N (0-7, or -1 for no particles)
#define IE_HEAL		2	// heals N (negative harms)
#define IE_WEAPON	3	// gives you weapon N
#define IE_HAMMER	4	// gives you N hammers (can be negative of course)
#define IE_PANTS	5	// gives you N pants
#define IE_KEY		6	// gives you N yellow keys
#define IE_RKEY		7	// gives you N red keys (N is here, so you can use -1 to remove it)
#define IE_GKEY		8	// green
#define IE_BKEY		9	// blue
#define IE_LOONYKEY	10	// gives you N loonykeys (again, -1 to remove it)
#define IE_COIN		11	// gives you N coins
#define IE_CANDLE	12	// gives you N candles
#define IE_OXYGEN	13	// gives you N oxygen
#define IE_RAGE		14	// gives you N rage
#define IE_WEIRD	15	// the radioactive chinese effect - variety of possibilities
#define IE_BECOME	16	// changes to item N
#define IE_SUMMON	17	// summons monster N
#define IE_POWERUP	18	// power the player up with power up N, use -N to remove power up
#define IE_HEALRANDOM 19	// the random healing you get from chinese food
#define IE_BRAIN	20	// gives you N brains
#define IE_KEYCHAIN 21	// gives you keychain N (1-4), use -N to remove the keychain
#define IE_DOOR		22	// door of color N (0=yellow, 123=RGB), disappears if you trigger it with
						// the right key in hand.  If not, it does nothing
#define IE_ORBITER	23	// summon orbiter of type N (fixed)
#define IE_ORBITER2 24	// TODO: replace?
#define IE_PUSH		25	// get pushed
#define IE_INCVAR	26	// increment variable N
#define IE_DECVAR	27	// decrement variable N
#define IE_MOVE		28	// move up/down/left/right (only if it could move there as a pushable item)
#define IE_POCKET	29 	// gives N slots (player can have max of 8, starts with 1) 

#define IE_SCORE	30	// give N random points
#define IE_EXPLODE	31	// explode!!
#define IE_DROPITEM	32	// drop a random item

#define IE_MAX		31	// max # of effects

// Power ups. SERIALIZED in custom item definitions.
enum : int
{
	PU_REVERSE  = 1,  // reverse hammer
	PU_REFLECT  = 2,  // reflect hammer
	PU_SHIELD   = 3,  // energy shield
	PU_GARLIC   = 4,  // garlic
	PU_SPEED    = 5,  // particle accelerator effect
	PU_INVISO   = 6,  // invisibility
	PU_AMMO     = 7,  // infinite ammo
	PU_AMMO2    = 8,  // reload current weapon
	PU_CHEESE   = 9,  // supreme cheese
	PU_POISON   = 10, // poison
	PU_IGNITE	= 11,
	PU_FROZEN	= 12,
	PU_WEAKEN	= 13,
	PU_SLOW		= 14,
	PU_STRENGTH	= 15,
	PU_WATRWALK	= 16,
	MAX_POWERUP  // End marker. Add new powerups above this.
};
const char* GetPowerupName(int powerup);

struct item_t
{
	char name[32];
	char xofs,yofs,zofs;	// draw it offset by some pixels
	word sprNum;	// which sprite
	byte fromColor,toColor;	// if you want it to use offcolors
	char bright;	// if you don't want it to use normal brightness
	byte rarity;	// if can be dropped by monsters, how often?

	ItemThemes		theme;			// FLAGS for which themes it goes in
	ItemAppearance	appearance;		// how does it draw?
	ItemPassability passability;	// how do you pass thru it (or NOT!)
	ItemTrigger		triggerType;	// how do you trigger its effect
	ItemBehavior	behavior;		// misc behaviors
	byte			customJSP;		// does it use custom jsp

	byte effect;	// what it does when triggered
	int effectAmt;	// a modifier for the effect
	char msg[64];	// message when effect occurs
	word sound;		// sound when effect occurs
};

enum MapRenderFlags : word;

void InitItems(void);
void ExitItems(void);
void RenderItem(int x,int y,int type,char bright,MapRenderFlags flags);
void InstaRenderItem(int x,int y,int type,char bright,MGLDraw *mgl);
void DrawRedX(int x,int y,bool candle,MGLDraw *mgl);
item_t *GetItem(int type);
const item_t *GetBaseItem(int type);
word NumItems(void);
int NewItem(void);
void UpdateItems(void);
int GetRandomItem(void);
void SetupRandomItems(void);
int GetTotalRarity(void);
void DeleteItem(int itm);
int NumCustomSprites(void);
int NumItemSprites(void);
int BrainsGiven(int type);
int CandlesGiven(int type);
int FindItemByName(const char *name);

struct mapTile_t;
void UpdateItem(mapTile_t *m,int width,int offset);

class Guy;
struct mapTile_t;

class Map;
struct world_t;

void MoveMovableItem(int x,int y,Map *map,world_t *world);
void SetCustomItemSprites(const char* filename);

byte InteractWithItem(Guy *me,mapTile_t *m,int x,int y);
byte TriggerItem(Guy *me,mapTile_t *m,int x,int y);

class SwapTable;
void RepairItemToItem(int n);	// when item N is deleted, repair references to it and others in
								// all items
void RepairItemToSound(int n);	// when sound N is deleted, repair references to it and others in
								// all items
void RepairItemToTile(const SwapTable &table);	// when tiles are messed with, repair the references in items

struct ItemRenderExtents
{
	int left, right, up, down;
};
ItemRenderExtents GetItemRenderExtents();
void CalculateItemRenderExtents();

sprite_t* GetItemSprite(int spr);
sprite_t* GetAnimatedItemSprite(int spr);

#endif
