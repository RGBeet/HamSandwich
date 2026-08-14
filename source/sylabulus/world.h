#ifndef WORLD_H
#define WORLD_H

#include "map.h"
#include "tile.h"
#include "string_extras.h"
#include "bitflags.h"

constexpr int MAX_MAPS = 64;


enum TerrainType : byte // up to 256 different terrain types?! WOW!!
{
	TRN_NORMAL,
	TRN_SOLID,			// solid enemies cannot pass through
	TRN_ICE,			// slippery
	TRN_MUD,			// slows you down
	TRN_WATER,			// makes you drown
	TRN_LAVA,			// burns you
	TRN_RUBBER,			// makes you boing
	TRN_QUICKSAND,

	// conveyor (pushes you) 
	TRN_CNVYUP,
	TRN_CNVYDN,
	TRN_CNVYLF,
	TRN_CNVYRG,

	TRN_PUSHABLE,		// if this is the roof of wall, the wall is pushale
	TRN_PUSHON,			// only push on terrain can have things pushed over it.

	TRN_SKY,			//
	TRN_CLIFF,			// pushes you down!
	NUM_TERRAIN_TYPES,
};

enum TerrainChange : byte
{
	TRN_NOCHANGE,
	TRN_ANIM,
	TRN_STEP,
	TRN_DESTRUCT,
	NUM_TRNCHANGE_TYPES
};

enum TerrainRestriction : byte
{
	TRN_NORESTRICT,
	TRN_NOENEMY,
	TRN_NOGHOST,
	TRN_NOGOODGUY,
	NUM_TRNRESTRICT_TYPES
};

enum TerrainPathType : word
{
	TRN_MINECART	= 1 << 0,
	TRN_BUNNY		= 1 << 1,
	TRN_DRIVE		= 1 << 2,
};


// terrain flags
enum TileFlags : dword
{
	TF_SOLID      = 1 << 0,
	TF_ICE        = 1 << 1,
	TF_MUD        = 1 << 2,
	TF_WATER      = 1 << 3,
	TF_LAVA       = 1 << 4,
	TF_PUSHY      = 1 << 5,
	TF_PUSHON     = 1 << 6,
	TF_ANIM       = 1 << 7,
	TF_STEP       = 1 << 8,
	TF_DESTRUCT   = 1 << 9,
	TF_TRANS      = 1 << 10,
	TF_MINECART   = 1 << 11,
	TF_BUNNY      = 1 << 12,
	TF_NOGHOST    = 1 << 13,
	TF_NOENEMY    = 1 << 14,
	TF_RUBBER     = 1 << 15,
	TF_SHADOWLESS = 1 << 16,
};
BITFLAGS(TileFlags)

// terrain struct for pre-sylabulus worlds
struct terrain_old_t
{
	TileFlags flags;
	word next;
};

// larger struct for sylabulus worlds
struct terrain_t
{
	TerrainType type;				// base terrain type
	TerrainChange change;			// animation stuff

	byte transparent;				// is the roof transparent?
	byte shadowless;				// is the wall shadowless?
	byte value;
	word next;

	TerrainRestriction restrict;	// who cannot walk on this
	word pathType;					// minecart/bunny path stuff
	
	word stepNoise;					// what noise it makes when stepped on
	byte fancy;						// does it use fancy fx?
};

struct world_t
{
	byte numMaps;
	Map	 *map[MAX_MAPS];
	Tilegfx tilegfx;
	word numTiles;
	terrain_t terrain[NUMTILES];
	char author[64];

	std::span<Map *const> Maps() const { return std::span{map, numMaps}; }
	std::span<terrain_t> Terrain() { return std::span{terrain, numTiles}; }
	std::span<terrain_t const> Terrain() const { return std::span{terrain, numTiles}; }

	// new stuff starts here

	std::array<special_t, 128> special; // testing! global specials
};

extern byte keyChainInLevel[MAX_MAPS];

byte NewWorld(world_t *world,MGLDraw *mgl);
bool LoadWorld(world_t *world,const char *fname);
bool SaveWorld(const world_t *world,const char *fname);
void FreeWorld(world_t *world);

void InitWorld(world_t *world);
bool GetWorldName(const char *fname, StringDestination name, StringDestination author);

class SwapTable;
void RepairTileToTile(world_t *w, const SwapTable &table);

void LocateKeychains(world_t *w);

terrain_t *GetTerrain(world_t *w,word tile);
byte IsTerrainAqueous(world_t* w, word tile);
byte IsTerrainSolid(world_t* w, word tile);
byte IsTerrainAqueousOrSolid(world_t* w, word tile);

#endif
