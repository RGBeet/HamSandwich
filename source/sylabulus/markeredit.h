#ifndef MARKEREDIT_H
#define MARKEREDIT_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "filedialog.h"
#include "mapdialog.h"
#include "editor.h"


// view marker info
void MarkerEdit_Init(int s);
void MarkerEdit_Update(int mouseX, int mouseY, int scroll, MGLDraw* mgl);
void MarkerEdit_Render(int mouseX, int mouseY, MGLDraw* mgl);
void MarkerEdit_Exit(void);

// pick list of markers
void MarkerPick_Init(int s);
void MarkerPick_Update(int mouseX, int mouseY, int scroll, MGLDraw* mgl);
void MarkerPick_Render(int mouseX, int mouseY, MGLDraw* mgl);

#endif
