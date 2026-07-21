#ifndef SPECIALEDIT_H
#define SPECIALEDIT_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "filedialog.h"
#include "mapdialog.h"
#include "editor.h"

// level specials
void SpecialEdit_Init(int s);
void SpecialEdit_Exit(void);
void SpecialEdit_Update(int mouseX,int mouseY,int scroll,MGLDraw *mgl);
void SpecialEdit_Render(int mouseX,int mouseY,MGLDraw *mgl);

// global specials
void GlobalSpecialEdit_Init(int s);
void GlobalSpecialEdit_Exit(void);
void GlobalSpecialEdit_Update(int mouseX, int mouseY, int scroll, MGLDraw* mgl);
void GlobalSpecialEdit_Render(int mouseX, int mouseY, MGLDraw* mgl);

// trigger/effect pick
void TriggerEffectPick_Init(int s);
void TriggerEffectPick_Exit(void);
void TriggerEffectPick_Update(int mouseX, int mouseY, int scroll, MGLDraw* mgl, byte mode);
void TriggerEffectPick_Render(int mouseX, int mouseY, MGLDraw* mgl, byte mode);

void SpecialEdit_Key(char k);

void SetSpecialCoords(int x,int y);
void SetSpecialRect(int x,int y,int x2,int y2);

void SpecialEdit_Help(void);

char* SpecialEditNotes(byte mode, byte type);

float GetWorldPercentage(const char text[32]);

#endif
