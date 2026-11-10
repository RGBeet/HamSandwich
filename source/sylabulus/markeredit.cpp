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
static byte helpRemember;
static byte valueSelect;

#define ID_EXIT			1
#define ID_MRKRTYPU		2
#define ID_MRKRTYPD		3
#define ID_MRKRVAL1		4
#define ID_MRKRVAL2		5
#define ID_MRKRWID		6
#define ID_MRKRHEI		7
#define ID_LABLVAL1		8
#define ID_LABLVAL2		9
#define ID_TEXT			100

#define MMODE_NORMAL	0
#define MMODE_TYPE		1
#define MMODE_VAL1		2
#define MMODE_VAL2		3
#define MMODE_WIDTH		4
#define MMODE_HEIGHT	5
#define MMODE_PICKGUY	6
#define MMODE_HELP		7

static void SetupMarkerEditSetupButtons(void);

static const char markerTypeName[][32] = {
	"Unused",
	"Enemy Path",
	"Particle Rectangle",
};

void MarkerEdit_Init(int m)
{
	printf("Attempting to find marker...\n");
	marker_t* target = GetMarker(m);

	if (!target)
	{
		printf("No Marker Loaded.\n");
	}
	else
	{
		printf("Marker #%d Loaded!\n",m);
	}

	markerNum = m;

	GetDisplayMGL()->MouseTap();

	memcpy(&mrkr, target, sizeof(marker_t));

	mode = MMODE_NORMAL;
	rightClick = 0;
	valueSelect = 0;

	SetupMarkerEditSetupButtons();
}

static void SaveMarker(void)
{
	printf("Attempting to save marker...\n");
	marker_t* target = GetMarker(markerNum);

	if (!target)
	{
		printf("No Marker Saved.\n");
	}
	else
	{
		printf("Marker #%d Saved!\n", markerNum);
	}

	memcpy(target, &mrkr, sizeof(marker_t));
}

static void ExitClick(int id)
{
	if (rightClick)
		return;

	SaveMarker();
	SetEditMode(EDITMODE_EDIT);
	MakeNormalSound(SND_MENUCLICK);
	PickedTile(-1);
}

static void TypeUpClick(int id)
{
	if (rightClick)
	{
		mrkr.type = 0;
		MakeNormalSound(SND_PAINTSPLAT);
		return;
	}

	mrkr.type++;

	if (mrkr.type >= N_MRKTYPES)
		mrkr.type = 0;

	MakeNormalSound(SND_MENUCLICK);
	SetupMarkerEditSetupButtons();
}

static void TypeDownClick(int id)
{
	if (rightClick)
	{
		mrkr.type = 0;
		MakeNormalSound(SND_PAINTSPLAT);
		return;
	}

	if (!mrkr.type)
		mrkr.type = N_MRKTYPES - 1;
	else
		mrkr.type--;

	MakeNormalSound(SND_MENUCLICK);
	SetupMarkerEditSetupButtons();
}

static void Value1Click(int id)
{
	if (rightClick)
	{
		mrkr.value = 0;
		MakeNormalSound(SND_PAINTSPLAT);
		return;
	}

	valueSelect = 1;
	mode = MMODE_VAL1;

	MakeNormalSound(SND_MENUCLICK);
	InitTextDialog("Enter a number for Value 1!", "", 16);
}

static void Value2Click(int id)
{
	if (rightClick)
	{
		mrkr.value2 = 0;
		MakeNormalSound(SND_PAINTSPLAT);
		return;
	}

	valueSelect = 2;
	mode = MMODE_VAL2;

	MakeNormalSound(SND_MENUCLICK);
	InitTextDialog("Enter a number for Value 2!", "", 16);
}

static void MonsterClick(int id)
{
	if (rightClick)
	{
		SetEditMode(EDITMODE_PICKENEMY);
		ClearButtons();
		MonsterEdit_Init(EDITMODE_MARKER, EditorGetWorld());
		mode = MMODE_PICKGUY;
	}
	else
	{
		switch (mrkr.value)
		{
		case MONS_GOODGUY:
			mrkr.value = MONS_BADGUY;
			break;
		case MONS_BADGUY:
			mrkr.value = MONS_ANYBODY;
			break;
		case MONS_ANYBODY:
			mrkr.value = MONS_NONPLAYER;
			break;
		case MONS_NONPLAYER:
			mrkr.value = MONS_TAGGED;
			break;
		case MONS_TAGGED:
			mrkr.value = MONS_PLAYER;
			break;
		case MONS_PLAYER:
			mrkr.value = MONS_BOUAPHA;
			break;
		default:	// if on a specific monster
			mrkr.value = MONS_GOODGUY;
			break;
		}
		SetupMarkerEditSetupButtons();
	}
}

static void ParticleClick(int id)
{
	if (rightClick)
	{
		mrkr.value++;
		if (mrkr.value >= MAX_PARTICLES)
			mrkr.value = 0;
	}
	else
	{
		if (mrkr.value == 0)
			mrkr.value = MAX_PARTICLES-1;
		else
			mrkr.value--;
	}

	SetupMarkerEditSetupButtons();
}

void MarkerEdit_Update(int mouseX, int mouseY, int scroll, MGLDraw* mgl)
{
	switch (mode)
	{
		case MMODE_NORMAL:
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

		case MMODE_VAL1:
		case MMODE_VAL2:
			if (mgl->MouseTap())
				TextDialogClick(mouseX, mouseY);

			if (TextDialogCommand() != 0)
			{
				if (TextDialogCommand() == TM_OKAY)
				{
					if (mode == MMODE_VAL1)
						mrkr.value = atoi(GetText());
					else
						mrkr.value2 = atoi(GetText());
				}
				mode = MMODE_NORMAL;
				valueSelect = 0;
				SetupMarkerEditSetupButtons();
			}
			break;
		case MMODE_PICKGUY:
			mode = MMODE_NORMAL;
			mrkr.value = EditorGetLastPick();
			ClearButtons();
			SetupMarkerEditSetupButtons();
			break;
	}
}

void MarkerEdit_Render(int mouseX, int mouseY, MGLDraw* mgl)
{
	char s[64];

	mgl->ClearScreen();

	Print(2, 2, "MARKER EDIT", 0, 1);

	// Marker type
	DrawFillBox(0, 40, 639, 40, 31);

	Print(20, 65, "Marker Type:", 0, 1);

	if (mrkr.type < N_MRKTYPES)
		sprintf(s, "%s", markerTypeName[mrkr.type]);
	else
		sprintf(s, "Unknown (%d)", mrkr.type);

	Print(150, 65, s, 0, 1);

	sprintf(s, "X: %03d, Y: %03d", mrkr.x1, mrkr.y1);
	Print(500, 2, s, 0, 1);

	DrawFillBox(0, 100, 639, 100, 31);

	DrawFillBox(0, 430, 639, 430, 31);
	Print(20, 440, "Left-click to edit, right-click to reset.", 0, 1);

	if (mode == MMODE_NORMAL)
		RenderButtons(mouseX, mouseY, mgl);
	else
		RenderButtons(-1, -1, mgl);

	switch (mode)
	{
		case MMODE_VAL1:
		case MMODE_VAL2:
		case MMODE_WIDTH:
		case MMODE_HEIGHT:
			RenderTextDialog(mouseX, mouseY, mgl);
			break;
		case MMODE_HELP:
			RenderEditHelp(mouseX, mouseY, mgl);
			break;
	}
}

void MarkerEdit_Exit(void)
{
	SaveMarker();
}

void MarkerEdit_Key(char k)
{
	switch (mode)
	{
		case MMODE_VAL1:
		case MMODE_VAL2:
			TextDialogKey(k);
			break;
		default:
			if (k == 27)
			{
				printf("Exiting Menu?\n");
				SaveMarker();
				SetEditMode(EDITMODE_EDIT);
				PickedTile(-1);
				return;
			}
			break;
	}
}

static void SetupMarkerEditSetupButtons(void)
{
	char s[32];
	ClearButtons();

	MakeButton(BTN_NORMAL, ID_EXIT, 0,
		480, 460, 158, 14,
		"Exit Marker Editor", ExitClick);

	// Marker type controls
	MakeButton(BTN_NORMAL, ID_MRKRTYPU, 0,
		260, 65, 30, 14, "++", TypeUpClick);

	MakeButton(BTN_NORMAL, ID_MRKRTYPD, 0,
		294, 65, 30, 14, "--", TypeDownClick);

	switch (mrkr.type)
	{
	case MRK_PART:
		MakeButton(BTN_STATIC, ID_TEXT, 0,
			20, 125, 125, 14,
			"Randomly spawn", 0);

		MakeButton(BTN_NORMAL, ID_MRKRVAL1, 0,
			150, 125, 120, 14,
			GetParticleName(mrkr.value), ParticleClick);

		MakeButton(BTN_STATIC, ID_TEXT + 1, 0,
			280, 125, 75, 14,
			"at rate of", 0);

		sprintf(s, "%d", mrkr.value2);
		MakeButton(BTN_NORMAL, ID_MRKRVAL2, 0,
			360, 125, 50, 14, s, Value2Click);
		break;

	case MRK_PATH:
		MakeButton(BTN_STATIC, ID_TEXT, 0,
			20, 125, 135, 14,
			"Act as path node for path #", 0);

		sprintf(s, "%d", mrkr.value);
		MakeButton(BTN_NORMAL, ID_MRKRVAL1, 0,
			240, 125, 120, 14,
			s, Value1Click);

		MakeButton(BTN_STATIC, ID_TEXT + 1, 0,
			290, 125, 80, 14,
			"of priority", 0);

		sprintf(s, "%d", mrkr.value2);
		MakeButton(BTN_NORMAL, ID_MRKRVAL2, 0,
			375, 125, 50, 14, s, Value2Click);
		break;
	}
}
