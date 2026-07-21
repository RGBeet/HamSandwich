#ifndef SPCLGLOBAL_H
#define SPCLGLOBAL_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "editor.h"

// global special menu
void InitGlobalSpecialMenu(world_t* wrld);
void ExitGlobalSpecialMenu(void);
void RenderGlobalSpecialMenu(world_t* wrld, int msx, int msy, MGLDraw* mgl);
void UpdateGlobalSpecialMenu(world_t* wrld, int msx, int msy, MGLDraw* mgl);

#endif
