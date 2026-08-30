#include "world.h"
#include "player.h"
#include "repair.h"
#include "items.h"
#include "worldstitch.h"
#include "log.h"
#include "appdata.h"
#include "world_io.h"
#include "map.h"
#include "special.h"

byte keyChainInLevel[MAX_MAPS];

byte NewWorld(world_t *world,MGLDraw *mgl)
{
	int i;

	ClearCustomSounds();

	world->numMaps=1;
	mgl->LoadBMP("tilegfx/tiles.bmp");

	world->numTiles=400;
	world->tilegfx.numTiles = world->numTiles;
	world->tilegfx.SetTiles(mgl->GetScreen(), mgl->GetWidth(), mgl->GetHeight());

	// reset all the terrain
	memset(world->terrain,0,sizeof(terrain_t)*NUMTILES);

	for(i=0;i<MAX_MAPS;i++)
		world->map[i]=NULL;

	world->map[0]=new Map(0,"New World");
	strcpy(world->author,"Nobody");

	ExitItems();
	InitItems();

	// global specials
	world->special.fill({}); // testing!
	for (special_t& me : world->special)
		me.x = 255;
	

	if(!world->map[0])
		return 0;
	return 1;
}

bool LoadWorld(world_t *world,const char *fname)
{
	char code[32];

	auto f = AppdataOpen(fname);
	if(!f)
		return false;

	SDL_ReadIO(f, code, 8);
	SDL_SeekIO(f, 0, SDL_IO_SEEK_SET);
	code[8]='\0';

	if(!strcmp(code, "HAMSWCH!"))
	{
		f.reset();
		return Syl_LoadWorld(world, fname); // project sylabulus
	}
}

bool SaveWorld(const world_t *world, const char *fname)
{
	world->map[0]->type = MAP_TYPE_HUB;
	std::string namebuf;
	printf("[EDITOR] Saving World: %s\n", fname);
	Syl_SaveWorld(world, fname);
	AppdataSync();
	return true;
}

bool GetWorldName(const char *fname, StringDestination name, StringDestination author)
{
	char code[9];

	owned::SDL_IOStream f = AppdataOpen(fname);
	if(!f)
		return false;

	SDL_ReadIO(f,code,8);
	code[8]='\0';

	f.reset();
	return Syl_GetWorldName(fname, name, author);
}

void FreeWorld(world_t *world)
{
	int i;

	ClearCustomSounds();
	for(i=0;i<MAX_MAPS;i++)
		if(world->map[i])
			delete world->map[i];
}

void InitWorld(world_t *world)
{
	SetCurrentTilegfx(&world->tilegfx);
	FillGlobalSpecialUseData(world->special);
}

void RepairTileToTile(world_t *w, const SwapTable &table)
{
	int i;

	for(i=0;i<NUMTILES;i++)
		w->terrain[i].next=table.GetSwap(w->terrain[i].next);
}

void LocateKeychains(world_t *w)
{
	int i;

	for(i=0;i<MAX_MAPS;i++)
	{
		if(i>=w->numMaps)
			keyChainInLevel[i]=0;
		else
		{
			keyChainInLevel[i]=w->map[i]->Keychains();
		}
	}
}

terrain_t *GetTerrain(world_t *w,word tile)
{
	if (tile < NUMTILES)
		return &w->terrain[tile];

	static terrain_t fake;
	fake = {{}};
	return &fake;
}

byte IsTerrainAqueous(world_t* w, word tile)
{
	terrain_t* terrain = GetTerrain(w, tile);

	if (!terrain)
		return 0;

	return terrain->type == TRN_WATER || terrain->type == TRN_LAVA;
}

byte IsTerrainSolid(world_t* w, word tile)
{
	terrain_t* terrain = GetTerrain(w, tile);

	if (!terrain)
		return 0;

	return terrain->type == TRN_SOLID;
}

byte IsTerrainAqueousOrSolid(world_t* w, word tile)
{
	terrain_t* terrain = GetTerrain(w, tile);

	if (!terrain)
		return 0;

	return terrain->type == TRN_WATER || terrain->type == TRN_LAVA || terrain->type == TRN_SOLID;
}
