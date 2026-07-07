#include "profmenu.h"
#include "worldselect.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "textdialog.h"
#include "yesnodialog.h"
#include "shop.h"
#include "winpch.h"
#include "appdata.h"

static byte kcMode, kBtnNum, canHitKeys;
static int msx, msy;
static byte* backgd;
static std::unique_ptr<sprite_set_t> plSpr;
static char msBright, msDBright;
static const int PLAYMENU_OFFY = 160;

static char menuTxt[][16] = { "Adventure", "Tutorial", "Campaign", "Return"};
static char menuDesc[][64] = { // there's got to be a better way!
	"Select a world from the base game!",
	"Learn how to play at Tutorial World!",
	"Play the base campaign! (UNDER CONSTRUCTION!)",
	"Return to the main menu (Lame!)"
};
constexpr int PLAY_OPTIONS = 4;

static byte cursor;
static byte oldc;

void InitPlayMenu(MGLDraw *mgl)
{
	int i;

	msBright = 0;
	msDBright = 1;

	mgl->LoadBMP("graphics/pause.bmp");
	backgd = (byte*)malloc(640 * 480);
	plSpr = std::make_unique<sprite_set_t>("graphics/pause.jsp");

	for (i = 0;i < 480;i++)
		memcpy(&backgd[i * 640], &mgl->GetScreen()[i * mgl->GetWidth()], 640);

	oldc = ~0;
}

void ExitPlayMenu(void)
{
	free(backgd);
	plSpr.reset();
}

byte UpdatePlayMenu(int *lastTime,MGLDraw *mgl)
{
	byte c = GetControls() | GetArrows();
	int i;
	byte msOkay;
	int yy=32;

	int oldMsx = msx, oldMsy = msy;
	mgl->GetMouse(&msx,&msy);

	if (*lastTime > TIME_PER_FRAME * 5)
		*lastTime = TIME_PER_FRAME * 5;
	while (*lastTime >= TIME_PER_FRAME)
	{
		msBright += msDBright;
		if (msBright > 10)
			msDBright = -1;
		if (msBright < -2)
			msDBright = 1;

		mgl->Process();
		*lastTime -= TIME_PER_FRAME;
	}

	if (msx != oldMsx || msy != oldMsy)
	{

		int yy;
		// select stuff with cursor here
		for (i = 0;i < 6;i++)
		{
			yy = PLAYMENU_OFFY + i * 62;
			if (msx >= 100 && msx <= 500 && msy >= yy && msy <= yy + 55)
				cursor = i;
		}
	}
	oldMsx = msx;
	oldMsy = msy;

	msBright+=msDBright;
	if(msBright>10)
		msDBright=-1;
	if(msBright<-2)
		msDBright=1;

	if (c & ~oldc & CONTROL_UP)
	{
		cursor--;
		if (cursor == 255)
			cursor = PLAY_OPTIONS-1;

		MakeNormalSound(SND_MENUCLICK);
	}
	if (c & ~oldc & CONTROL_DN)
	{
		cursor++;
		if (cursor == PLAY_OPTIONS)
			cursor = 0;

		MakeNormalSound(SND_MENUCLICK);
	}

	if ((c & ~oldc & CONTROL_B1) || mgl->MouseTap())
	{
		if (cursor != 2)
		{
			MakeNormalSound(SND_MENUSELECT);
			return cursor + 2;
		}
		else
		{
			MakeNormalSound(SND_BOMBBOOM);
		}
	}

	oldc=c;
	c = mgl->LastKeyPressed();

	if (c == 27)
		return 1;	// exit

	return 0;
}


void RenderPlayMenu(MGLDraw *mgl)
{
	int i;
	int msx2, msy2;

	for (i = 0;i < 480;i++)
		memcpy(&mgl->GetScreen()[i * mgl->GetWidth()], &backgd[i * 640], 640);

	for (i = 0;i < PLAY_OPTIONS;i++)
	{
		if (i != cursor)
		{
			Print(48, PLAYMENU_OFFY+i*62, menuTxt[i], 0, 0);
		}
		else
		{
			Print(48, PLAYMENU_OFFY+i*62, menuTxt[i], 8, 0);
			// TODO: fix wavy brightness.
			//timeGetTime() / 10, 1, 0.5
		}
	}
	
	PrintSimpleShadow(96 + GetStrLength(menuTxt[cursor],0), PLAYMENU_OFFY+cursor*62+32, menuDesc[cursor], 1);

	// mouse cursor
	SetSpriteConstraints(13, 13, 627, 467);
	msx2 = msx;
	msy2 = msy;
	if (msx2 < 13)
		msx2 = 13;
	if (msy2 < 13)
		msy2 = 13;
	if (msx2 > 622)
		msx2 = 622;
	if (msy2 > 462)
		msy2 = 462;
	plSpr->GetSprite(0)->DrawBright(msx2, msy2, mgl, msBright / 2);

	plSpr->GetSprite(10)->Draw(SCRWID-32, 8, mgl);
	ClearSpriteConstraints();
}
//----------------

// This menu houses the World Select, Tutorial, Base Campaign (WIP), and Arena (WIP) options

TASK(void) PlayMenu(MGLDraw *mgl)
{
	byte done=0;
	int lastTime=1;

	InitPlayMenu(mgl);
	PlaySongForce("003worldpicker.ogg");

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done = UpdatePlayMenu(&lastTime,mgl);

		RenderPlayMenu(mgl);
		AWAIT mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();

		if(done==2)  // go to world select (play)
		{
			AWAIT WorldSelectMenu(mgl);
		}
		if(done==3)  // go to tutorial
		{
			AWAIT PlayWorld(mgl, "tutorial.dlw");
		}
		if(done==4)  // campaign not working yet
		{
			done=0;
		}
	}
	ExitPlayMenu();
}
