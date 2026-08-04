#include "winpch.h"
#include "specialedit.h"
#include <math.h>
#include "dialogbits.h"
#include "filedialog.h"
#include "items.h"
#include "textdialog.h"
#include "monsteredit.h"
#include "bulletedit.h"
#include "itemedit.h"
#include "vars.h"
#include "terrainedit.h"
#include "soundedit.h"
#include "edithelp.h"
#include "viewdialog.h"
#include "shop.h"
#include "player.h"
#include "spclnotes.h"
#include "worldselect.h"

static byte mode;
static marker_t mrkr;

static byte rightClick;
static byte markerNum;

#define MMODE_NORMAL	0
#define MMODE_PICK		1

void MarkerEdit_Init(int m)
{
	marker_t* target = GetMarker(m);
	markerNum = m;
	if (!target) // marker doesn't exist? go away!
		return;
	GetDisplayMGL()->MouseTap();
	mode = MMODE_NORMAL;
	// setup
}


void MarkerEdit_Update(int mouseX, int mouseY, int scroll, MGLDraw* mgl)
{
	switch(mode)
	{
		case 0:
			if (mgl->MouseTap())
			{
				rightClick = 0;
				CheckButtons(mouseX, mouseY);
			}
			if (mgl->RMouseTap())
			{
				rightClick = 1;
				CheckButtons(mouseX, mouseY);
			}
			break;
		case 1:

			break;

	}
}

void MarkerEdit_Render(int mouseX, int mouseY, MGLDraw* mgl)
{
	char s[16];
	int i;

	mgl->ClearScreen();

	Print(2, 2, "MARKER EDIT", 0, 1);
}

void MarkerEdit_Exit(void)
{
	marker_t* target = GetMarker(markerNum);

	if (!(target && target->type))// marker doesn't exist? go away!
		return;
	memcpy(target, &mrkr, sizeof(marker_t));
}

void MarkerEdit_Key(char k)
{
	switch (mode)
	{
	default:
		if (k == 27)
		{
			SetEditMode(EDITMODE_EDIT);
			MarkerEdit_Exit();
			PickedTile(-1);
			return;
		}
		break;
	}
}
