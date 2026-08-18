#ifndef MAP_H
#define MAP_H

#include <array>
#include "tile.h"
#include "items.h"
#include "special.h"
#include "marker.h"
#include "string_extras.h"
#include "bitflags.h"
#include "pathfinding.h"

constexpr int MAX_LIGHT = 16;
constexpr int MIN_LIGHT = -32;

// The maximum # of monsters you can put on the map.
constexpr int MAX_MAPMONS = 128;

// Maximum width or height.
constexpr int MAX_MAPSIZE = 200;

// Flags for calling map render.
enum MapRenderFlags : word
{
	MAP_SHOWLIGHTS      = 1 << 0,
	MAP_SHOWWALLS       = 1 << 1,
	MAP_SHOWPICKUPS     = 1 << 2,
	MAP_SHOWBADGUYS     = 1 << 3,
	MAP_SHOWSPECIALS    = 1 << 4,
	MAP_SHOWSELECT      = 1 << 5,
	MAP_SHOWOTHERITEMS  = 1 << 6,
	MAP_TEMPTORCH       = 1 << 7,
	MAP_ZOOMOUT         = 1 << 8,
};
BITFLAGS(MapRenderFlags)

constexpr MapRenderFlags MAP_SHOWITEMS = MAP_SHOWPICKUPS | MAP_SHOWOTHERITEMS;  // combine them in one

// Map flags. SERIALIZED.
enum LevelFlags : word
{
	MAP_SNOWING     = 1 << 0, // weather: snowing, conflicts wth rain
	MAP_RAIN        = 1 << 1, // weather: raining, conflicts wth snow
	MAP_HUB         = 1 << 2, // is a hub? conflicts with secret
	MAP_SECRET      = 1 << 3, // is a secret? conflicts with hub
	MAP_TORCHLIT    = 1 << 4, // mutually exclusive with torch lit
	MAP_WELLLIT     = 1 << 5, // mutually exclusive with torch lit
	MAP_STARS       = 1 << 6, // doesn't conflict with anything
	MAP_UNDERWATER  = 1 << 7,  // mutually exclusive with underlava and wavy
	MAP_LAVA        = 1 << 8, // mutually exclusive with underwater and wavy
	MAP_STEALTH     = 1 << 9, // doesn't conflict with anything
	MAP_WAVY        = 1 << 10, // mutually exclusive with underwater and underlava
	MAP_OXYGEN      = 1 << 11,  // mutually exclusive with underwater
};
BITFLAGS(LevelFlags)
constexpr int NUM_LVL_FLAGS = 12;
const char* MapFlagName(int flagIndex);  // expects 0, 1, 2, 3, not the constants above

enum MapType : byte
{
	MAP_TYPE_NORMAL,
	MAP_TYPE_HUB,			// doesn't count as a regular level
	MAP_TYPE_SECRET,		// doesn't count in level counts
	MAP_TYPE_BOSS,			// houses a boss
	MAP_TYPE_KEYCHAIN,		// is a certified keychain level!! WOOHOO!!
	MAP_TYPE_MAX,
};

enum MapWeather : byte
{
	MAP_WEATHER_NONE,
	MAP_WEATHER_RAIN,
	MAP_WEATHER_SNOW,
	MAP_WEATHER_SAKURA,
	MAP_WEATHER_FOG,
	MAP_WEATHER_MAX
};

enum MapLighting : byte
{
	MAP_LIGHT_NORMAL,		// normal lighting
	MAP_LIGHT_TORCH,		// torch-lit
	MAP_LIGHT_LANTERN,		// larger lighting
	MAP_LIGHT_STEALTH,		// anti-lighting, conflicts with other lighting
	MAP_LIGHT_MAX		
};

enum MapEnvironment : byte
{
	MAP_ENV_NORMAL,
	MAP_ENV_UNDERWATER,		// auto includes oxygen
	MAP_ENV_OXYGEN,			// like underwater, but NOT blue
	MAP_ENV_SUPERHOT,		// underlava = hurts you periodically!
	MAP_ENV_OUTERSPACE,		// low gravity
	MAP_ENV_TEST,			// the testing one?!
	MAP_ENV_MAX				// low gravity
};

enum MapTypeFlags : dword
{
	MAP_FLG_STARRY	= 1 << 0,
	MAP_FLG_WAVY	= 1 << 1,
};



// map updating modes
enum
{
	UPDATE_GAME,
	UPDATE_EDIT,
	UPDATE_FADE,
	UPDATE_FADEIN,
};

struct mapTile_t
{
	word floor;
	word wall;
	word item;		// byte -> word
	char light;
	char templight;
	byte opaque;
	byte select;
};

// SERIALIZED.
struct mapBadguy_t
{
	byte x,y;
	dword type;
	word item;		// byte -> word
};

struct world_t;

class Map
{
	public:
		explicit Map(byte size, const char *name);
		explicit Map(byte width, byte height, const char *name);
		explicit Map(const Map *m);

		~Map(void);

		void Init(world_t *wrld);
		void Render(world_t *world,int camX,int camY,MapRenderFlags flags);
		void RenderSelect(world_t *world,int camX,int camY,MapRenderFlags flags);

		byte DropItem(int x,int y,word itm);
		void PermaTorch(int x,int y,char brt);
		void TempTorch(int x,int y,char brt);
		void BrightTorch(int x,int y,char brt,byte size);
		void GlowCursor(int x,int y,char brt,byte size);
		void DimTorch(int x,int y,byte size);
		void LightSpecial(int x,int y,char brt,byte size);
		void LightRect(int x,int y,int x2,int y2,char brt,byte perm);
		void Update(byte mode,world_t *world);
		byte LOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *));
		byte TightLOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *));
		byte TightestLOS(int x,int y,int radius,int value,byte (*DoIt)(int,int,int,int,int,Map *));
		byte CheckLOS(int x,int y,int radius,int x2,int y2);

		int  ItemCount(word itm);
		int  ItemCountInRect(word itm,int x,int y,int x2,int y2);

		void Swap(int sx,int sy,int blkwidth,int blkheight,int dx,int dy);
		void Copy(int sx,int sy,int blkwidth,int blkheight,int dx,int dy);
		void TileChange(int x,int y,int floor,int wall,byte fx);
		void ContiguousTileChange(int x,int y,int floor,int wall,byte fx);
		void AllTileChange(int x,int y,int floor,int wall,byte fx);

		byte ItemChange(int x,int y, word item,byte fx);
		byte ContiguousItemChange(int x,int y,byte item,byte fx);
		byte AllItemChange(int x,int y,byte item,byte fx);

		byte FindGuy(int x,int y,byte size,Guy *g);

		byte Keychains(void);	// return bitflags for which keychains are in this level
		byte PushSpecials(int x, int y, int width, int height);

		// Returns true if the coordinate are valid.
		bool InRange(int x, int y) const { return x >= 0 && y >= 0 && x < width && y < height; }
		// Returns a dummy tile if the coordinates are invalid.
		mapTile_t *GetTile(int x, int y);
		// Returns `nullptr` if the coordinates are invalid.
		mapTile_t *TryGetTile(int x, int y);
		std::span<mapTile_t> Tiles() { return std::span{map, (size_t)(width * height)}; }
		std::span<mapTile_t const> Tiles() const { return std::span{(const mapTile_t*)map, (size_t)(width * height)}; }

		void FindNearBrain(int myx,int myy);
		void FindNearCandle(int myx,int myy);

		void SmoothLight(int x,int y);
		void SmoothLights(void);
		byte Resize(byte w,byte h);

		byte CompareRegions(int x,int y,int x2,int y2,int tx,int ty,byte checkMons);
		bool CanSeePath(Guy* me, world_t* world, int x1, int y1, int x2, int y2);

		byte width,height;
		mapTile_t *map;
		char name[64];
		char song[32];

		MapType type;
		MapWeather weather;
		MapLighting lighting;
		MapEnvironment environment;
		dword miscFlags;

		word numBrains;
		word numCandles;
		word itemDrops;	// how often items drop, a fixshifted percent
		word timer;

		std::array<mapBadguy_t, MAX_MAPMONS> badguy;
		std::array<special_t, MAX_SPECIAL> special;
		std::array<marker_t, MAX_MARKER> marker; // used to help pathfinding, a WIP!

		std::vector<PathNode> nodes;
		void InitPathNodes(world_t* world);
		PathNode* GetNode(int x, int y);

	private:
		void LOSPoints(int x,int y,int curx,int cury,int *p1x,int *p1y,int *p2x,int *p2y);
		void RenderStars(int camX, int camY);
};

byte PlaceItemCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TorchCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TempTorchCallback(int x,int y,int cx,int cy,int value,Map *map);
byte GlowCursorCallback(int x,int y,int cx,int cy,int value,Map *map);
byte TempBrightCallback(int x,int y,int cx,int cy,int value,Map *map);
byte FindGuyCallback(int x,int y,int cx,int cy,int value,Map *map);

class Guy;
void SpecialStepCheck(Map *map,int x,int y,Guy *me);
void SpecialTakeEffect(byte num,Map *map,special_t *spcl,Guy *victim);

void ZapWall(Map *map,int x,int y,word newFloor);
void SpecialShootCheck(Map *map,int x,int y);
void SpecialAnytimeCheck(Map *map);
void SpecialKillCheck(Map *map,byte type);

byte MapHasOxygenMechanic(Map *map);
bool CanWalkTile(int x, int y, Map* map, world_t* world);

void InitStars();

#endif
