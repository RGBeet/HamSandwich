#include "winpch.h"
#include <math.h>
#include "player.h"
#include "special.h"
#include "specialedit.h"
#include "dialogbits.h"
#include "editor.h"

static int gsFirst=0;
static byte select;

#define ID_EXIT			1
#define ID_PAGEUP		2
#define ID_PAGEDN		3

#define ID_SELECT		16
#define ID_MOVEUP		16*2
#define ID_MOVEDN		16*3
#define ID_COPY			16*4
#define ID_DELETE		16*5


static void PickSpecialClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	SpecialEdit_Init(id-ID_SELECT);
	SetEditMode(EDITMODE_SPECIAL);
}

static byte GetNumSpecialTriggers(special_t *spcl)
{
	int i;
	byte b=0;
	for (i = 0; i < 7; i++)
	{
		if (spcl->trigger[i].type) // is used
			b++;
	}
	return b;
}

static byte GetNumSpecialEffects(special_t *spcl)
{
	int i;
	byte b=0;
	for(i=0; i<31; i++)
	{
		if (spcl->effect[i].type) // is unused
			b++;
	}
	return b;
}

static void MakeSpecialMenu(void)
{
	world_t* wrld = EditorGetWorld();
	char s[16];
	int i, pos;

	ClearButtons(ID_SELECT, ID_DELETE + 15); // clear it out
	for (i = 0;i < 16;i++)
	{
		int n = gsFirst + i;
		if (n >= 128) // 128 max!!
			break;

		sprintf(s, "%02d TRG, %02d EFF",GetNumSpecialTriggers(&wrld->special[n]), GetNumSpecialEffects(&wrld->special[n]));
		MakeButton(BTN_RADIO, ID_SELECT + i, 0, 45, 40 + i * 24, 200, 17, s, PickSpecialClick);
	}
}

static void PageUpClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	if (gsFirst + 16 < 128)
		gsFirst += 16;
	MakeSpecialMenu();
}

static void PageDownClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	if (gsFirst - 16 >= 0)
		gsFirst -= 16;
	MakeSpecialMenu();
}

static void ExitClick(int id)
{
	MakeNormalSound(SND_MENUCLICK);
	SetEditMode(EDITMODE_EDIT);
}

// GLOBAL EDIT MENU STUFF
void RenderGlobalSpecialMenu(world_t *wrld, int msx, int msy, MGLDraw* mgl)
{
	char s[16];
	int i;

	mgl->ClearScreen();

	Print(8,8,"GLOBAL SPECIALS",0,1);

	sprintf(s, "#%03d-%03d", gsFirst, gsFirst+15);
	Print(154,8,s,0,1);

	DrawLine(8,20,SCRWID-8,20,32*1+16);
	DrawLine(8,SCRHEI-20,SCRWID-8,SCRHEI-20,32*1+16);

	// draw em up!
	for (i = gsFirst; i < gsFirst + 16; i++)
	{
		if (i >= 128)
			break;
		int y = 44 + (i - gsFirst) * 24;
		sprintf(s, "#%03d", i);
		Print(8, y, s, 0, 1);
	}
	RenderButtons(msx, msy, mgl);

}

void UpdateGlobalSpecialMenu(world_t *wrld, int msx, int msy, MGLDraw* mgl)
{
	int i;

	if (mgl->MouseTap())
	{
		CheckButtons(msx, msy);
	}
}

void InitGlobalSpecialMenu(world_t *wrld)
{
	gsFirst = 0;
	MakeSpecialMenu();

	MakeButton(BTN_RADIO, ID_EXIT, 0, 513, 439, 119, 17, "Exit Spcl. Menu", ExitClick);
	MakeButton(BTN_RADIO, ID_PAGEUP, 0, 15, 439, 79, 17, "Page Up", PageDownClick);
	MakeButton(BTN_RADIO, ID_PAGEDN, 0, 98, 439, 79, 17, "Page Dn.", PageUpClick);
}
