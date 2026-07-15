#ifndef WORLD_IO_SYLABULUS_H
#define WORLD_IO_SYLABULUS_H

#include "jamultypes.h"
#include "string_extras.h"

struct world_t;

bool Syl_GetWorldName(const char* fname, StringDestination name, StringDestination author);
byte Syl_LoadWorld(world_t* world, const char* fname);
byte Syl_SaveWorld(const world_t* world, const char* fname);

#endif
