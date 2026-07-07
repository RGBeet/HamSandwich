#include "pause.h"
#include "player.h"
#include "progress.h"
#include "editor.h"
#include "intface.h"
#include "shop.h"
#include "dialogbits.h"
#include "music.h"
#include "game.h"
#include <memory>

#define PE_CONTINUE	0	// back to gameplay
#define PE_RETRY	1	// retry this level
#define PE_GIVEUP	2	// give up, go back to hub
#define PE_WORLDSEL 3	// world select screen
#define PE_EXIT		4	// exit entirely, to main menu or editor, wherever you came from
#define PE_SNDVOL	5	// sound volume adjust
#define PE_MUSVOL	6	// music volume adjust
#define PE_MUSIC	7	// music options menu
#define PE_CHEAT	8	// go to the cheat menu
#define PE_DONE		9	// a marker to indicate the list is done
#define PE_MORECHEATS 10	// next page of cheats
#define PE_MUSICMODE 11	// random/sequential/repeat
#define PE_SONG		12	// which song is playing
#define PE_MUSICTYPE 13 // music type adjust (which playlist, or official music)
#define PE_PREVCHEATS 14
#define PE_BZZT		15		// an invalid option
#define PE_SHOP		16	// go shopping, go back to playing
#define PE_WPNLOCK	17	// weapon lock
#define PE_HUDCHOICE   18

#define PE_CHEATS	50

struct pauseItem_t
{
	byte effect;
	char text[16];
};

static const pauseItem_t gamePause[]={
	{PE_CONTINUE,"Continue"},
	{PE_RETRY,"Retry"},
	{PE_GIVEUP,"Give Up"},
	{PE_WORLDSEL,"World Select"},
	{PE_SNDVOL,""},
	{PE_MUSIC,"Music Options"},
	{PE_WPNLOCK,"Weapon Lock"},
	{PE_SHOP,"Quit & Shop"},
	{PE_EXIT,"Exit Game"},
	{PE_DONE,""}
};

static const pauseItem_t shopPause[]={
	{PE_CONTINUE,"Continue"},
	{PE_SNDVOL,""},
	{PE_MUSIC,"Music Options"},
	{PE_SHOP,"Quit & Play"},
	{PE_EXIT,"Main Menu"},
	{PE_DONE,""}
};

static const pauseItem_t editPause[]={
	{PE_CONTINUE,"Continue"},
	{PE_RETRY,"Retry"},
	{PE_GIVEUP,"Give Up"},
	{PE_SNDVOL,""},
	{PE_MUSIC,"Music Options"},
	{PE_CHEAT,"Cheats!!"},
	{PE_WPNLOCK,"Weapon Lock"},
	{PE_EXIT,"Editor"},
	{PE_DONE,""}
};

static const pauseItem_t gameCheatPause[]={
	{PE_CONTINUE,"Continue"},
	{PE_RETRY,"Retry"},
	{PE_GIVEUP,"Give Up"},
	{PE_WORLDSEL,"World Select"},
	{PE_SNDVOL,""},
	{PE_MUSIC,"Music Options"},
	{PE_CHEAT,"Cheats!!"},
	{PE_WPNLOCK,"Weapon Lock"},
	{PE_SHOP,"Quit & Shop"},
	{PE_EXIT,"Exit Game"},
	{PE_DONE,""}
};

static const pauseItem_t cheatPause[]={
	{PE_CHEAT,"Stop Cheating!"},
	{PE_CHEATS+CHEAT_WINLEVEL,"Win Level"},
	{PE_CHEATS+CHEAT_HAMMERUP,"UltraHammerUp"},
	{PE_CHEATS+CHEAT_LIFE,"Max Health"},
	{PE_CHEATS+CHEAT_RAGE,"Max Rage"},
	{PE_CHEATS+CHEAT_SHIELD,"Energy Barrier"},
	{PE_CHEATS+CHEAT_BOOM,"KABLOOIE!"},
	{PE_CHEATS+CHEAT_FREEZE,"Freeze Enemies"},
	{PE_CHEATS+CHEAT_BRAINS,"All Brains"},
	{PE_CHEATS+CHEAT_KEYS,"All Keys"},
	{PE_MORECHEATS,"More Cheats"},
	{PE_DONE,""},
};

static const pauseItem_t cheatPause2[]={
	{PE_PREVCHEATS,"Prev. Cheats"},
	{PE_CHEATS+CHEAT_SCANNER,"Free Scanner"},
	{PE_CHEATS+CHEAT_AMMO,"Ammo Crate"},
	{PE_CHEATS+CHEAT_LIGHT,"Lights On"},
	{PE_CHEATS+CHEAT_WATER,"Walk On Water"},
	{PE_CHEATS+CHEAT_OXYGEN,"Infinite Oxygen"},
	{PE_CHEATS+CHEAT_NOSKID,"No-Skid Boots"},
	{PE_CHEATS+CHEAT_SPEED,"Super Speed"},
	{PE_CHEAT,"Stop Cheating!"},
	{PE_DONE,""},
};

static const pauseItem_t musicPause[]={
	{PE_MUSVOL,""},
	{PE_MUSICTYPE,""},
	{PE_MUSICMODE,""},
	{PE_SONG,""},
	{PE_MUSIC,"Back To Menu"},
	{PE_DONE,""},
};

namespace
{
	byte cursor = 0;
	char lastKey = 0;
	byte menuMode;
	std::unique_ptr<sprite_set_t> pauseSpr;
	int pauseX, pauseY=480;
	byte numItems;
	pauseItem_t menu[15];
	int msx, msy;
	char msBright, msDBright;
	byte oldc = 255;
	dword oldGamepad = ~0;
	char worldName[64];
	int pauseClock = 0;

	const byte volumeSpot[]={0,26,51,77,102,128,153,179,204,230,255};
}

void RenderPauseButton(byte b,int x,int y,int wid,char *txt,MGLDraw *mgl)
{
	if(cursor==b)
	{
		mgl->Box(x,y,x+wid,y+19,32*7 + 31);
		mgl->FillBox(x+1,y+1,x+wid-1,y+19-1,32*7 + 8);
		PrintWavy(x + 2, y + 4, txt, 0, 2, pauseClock, 0.5, 0.25);
	}
	else
	{
		mgl->Box(x,y,x+wid,y+19,32*3 + 16);
		Print(x + 2, y + 4, txt, 0, 2);
	}

}

void RenderUnpaused(void)
{
	if(pauseX < GetDisplayMGL()->GetWidth())
		RenderPauseMenu();
}

void RenderPauseMenu(void)
{
	int i;
	int msx2,msy2,cx,cy,cx2,cy2;
	int xx=32, yy=32;
	char msg[32];

	if (pauseY >= 480)
		return;
	pauseClock++;

	// get pause sprites (cursor)
	if (!pauseSpr)
	{
		pauseSpr = std::make_unique<sprite_set_t>("graphics/pause.jsp");
	}
	pauseSpr->GetSprite(11)->Draw(pauseX, pauseY, GetDisplayMGL()); // draw background

	//hello!!

	// world name
	if (curMap->flags&MAP_HUB)
	{
		Print(pauseX + xx, pauseY+yy, curWorld.map[0]->name, 8, 0);
		yy += 60;
	}
	else
	{
		Print(pauseX + xx, pauseY+yy, curMap->name, 8, 0);
		yy += 60;
		sprintf(worldName, curWorld.map[0]->name);
		PrintSimpleShadow(pauseX+xx, pauseY + yy, worldName, 1);
		yy += 8;
	}
	if (shopping)
	{
		sprintf(msg, "Shop 'til you drop!");
		PrintSimpleShadow(pauseX + xx, pauseY + yy, msg, 1);
		yy += 8;
	}

	for (i = 0;i < numItems;i++)
	{
		RenderPauseButton(i, pauseX+xx, pauseY+yy + 15 + 22*i, 64*3, menu[i].text, GetDisplayMGL());
	}

	yy = 92;
	xx = 320; // half screen width?
	if (!shopping)
		RenderCollectedStuff(pauseX+xx, pauseY+yy, GetDisplayMGL());
	else
		RenderShoppingStuff(pauseX+xx, pauseY+yy, GetDisplayMGL());

	pauseSpr->GetSprite(0)->DrawBright(msx,msy,GetDisplayMGL(),msBright/2);
	ClearSpriteConstraints();
}

void SetupSoundItems(void)
{
	int i;

	for(i=0;i<numItems;i++)
	{
		switch(menu[i].effect)
		{
			case PE_SNDVOL:
				sprintf(menu[i].text,"Sound: %d%%",(profile.sound*100/255));
				break;
			case PE_MUSVOL:
				sprintf(menu[i].text,"Volume: %d%%",(profile.music*100/255));
				break;
			case PE_MUSICTYPE:
				switch(profile.musicMode&7)
				{
					case 0:
						strcpy(menu[i].text,"Official Music");
						break;
					default:
						sprintf(menu[i].text,"Playlist %d",(profile.musicMode&7));
						break;
				}
				break;
			case PE_MUSICMODE:
				if((profile.musicMode&MUSIC_PLAYMODE)==MUSIC_RANDOM)
					strcpy(menu[i].text,"Random Play");
				else if((profile.musicMode&MUSIC_PLAYMODE)==MUSIC_SEQUENTIAL)
					strcpy(menu[i].text,"Sequential");
				else
					strcpy(menu[i].text,"Repeat");
				break;
			case PE_SONG:
				strncpy(menu[i].text,CurSongTitle(),15);
				menu[i].text[15]='\0';
				break;
		}
	}
}

static void SetupOptionItems()
{
	int i;

	for(i=0;i<numItems;i++)
	{
		switch(menu[i].effect)
		{
			case PE_WPNLOCK:
				if(profile.progress.wpnLock)
					strcpy(menu[i].text,"Wpn Lock: On");
				else
					strcpy(menu[i].text,"Wpn Lock: Off");
				break;
			case PE_HUDCHOICE:
				if(profile.progress.hudChoice == HudChoice::Advanced)
					strcpy(menu[i].text,"HUD: Advanced");
				else if(profile.progress.hudChoice == HudChoice::Classic)
					strcpy(menu[i].text,"HUD: Classic");
				else
					strcpy(menu[i].text,"HUD: Supreme");
				break;
		}
	}
}

void FillPauseMenu(const pauseItem_t *src)
{
	int i;

	for(i=0;i<15;i++)
	{
		if(src[i].effect==PE_DONE)
		{
			numItems=i;
			break;
		}
		else
		{
			memcpy(&menu[i],&src[i],sizeof(pauseItem_t));
			if(menu[i].effect>=PE_CHEATS)
			{
				// squigglize cheats that aren't owned
				if(!ItemPurchased(SHOP_CHEAT,menu[i].effect-PE_CHEATS) && !editing)
				{
					strcpy(menu[i].text,"????????");
					menu[i].effect=PE_BZZT;
				}
			}
		}
	}
	SetupSoundItems();
	SetupOptionItems();
}

void InitPauseMenu(void)
{
	lastKey = 0;
	cursor = 0;
	oldc = ~0;
	oldGamepad = ~0;

	if (!pauseSpr)
		pauseSpr = std::make_unique<sprite_set_t>("graphics/pause.jsp");

	std::vector<pauseItem_t> menu;

	menu.push_back({ PE_CONTINUE, "Continue" });

	if (!shopping) // not shopping
	{
		menu.push_back({ PE_RETRY, "Retry" });
		menu.push_back({ PE_GIVEUP, "Give Up" });
	}

	menu.push_back({ PE_SNDVOL, "" });
	menu.push_back({ PE_MUSIC, "Music Options" });

	if (editing || ItemPurchased(SHOP_MAJOR, MAJOR_CHEATMENU)) // editing OR cheats purchased
		menu.push_back({ PE_CHEAT, "Cheats!!" });

	if (!shopping)
		menu.push_back({ PE_WPNLOCK, "Weapon Lock" });

	if (editing) // editing: no shop option
	{
		menu.push_back({ PE_EXIT, "Editor" });
	}
	else if (shopping) // shopping: return to play game
	{
		menu.push_back({ PE_SHOP, "Quit & Play" });
		menu.push_back({ PE_EXIT, "Main Menu" });
	}
	else
	{
		menu.push_back({ PE_SHOP, "Quit & Shop" });
		menu.push_back({ PE_EXIT, "Exit Game" });
	}

	menu.push_back({ PE_DONE, "" });

	FillPauseMenu(menu.data());

	MakeNormalSound(SND_PAUSE);

	// cursor sprite
	if (!pauseSpr)
		pauseSpr = std::make_unique<sprite_set_t>("graphics/pause.jsp");

	pauseX = 0;
	pauseY = 480;
	menuMode = 0;
	msBright = 0;
	msDBright = 1;
	GetDisplayMGL()->MouseTap();
}

void ExitPauseMenu(void)
{
	pauseClock=0;
	pauseY = 480;
	pauseSpr.reset();
}

void UpdateUnpaused(void)
{
	//if(pauseX<640)
	//	pauseX+=15;
	if (pauseY < 480)
		pauseY += 40;
}

byte NextVolumeSpot(byte v)
{
	int i;

	for(i=0;i<10;i++)
	{
		if(v==volumeSpot[i])
			return volumeSpot[i+1];
	}
	if(v==volumeSpot[10])
		return volumeSpot[0];

	return 0;
}

byte PrevVolumeSpot(byte v)
{
	int i;

	for(i=1;i<11;i++)
	{
		if(v==volumeSpot[i])
			return volumeSpot[i-1];
	}
	if(v==volumeSpot[0])
		return volumeSpot[10];
	return 0;
}

PauseMenuResult UpdatePauseMenu(MGLDraw *mgl)
{
	byte c;
	int i;
	static byte reptCounter = 0;
	byte msOkay;
	int yy=32;

	int oldMsx = msx, oldMsy = msy;
	mgl->GetMouse(&msx,&msy);

	msBright+=msDBright;
	if(msBright>10)
		msDBright=-1;
	if(msBright<-2)
		msDBright=1;

	pauseX = 0;
	if (pauseY > 0)
	{
		pauseY -= 48;
		if (pauseY < 0)
			pauseY = 0;
	}

	if (curMap->flags & MAP_HUB && !shopping)
		yy += 60;
	else
		yy += 68;
	msOkay = 0;

	for (i = 0;i < numItems;i++)
	{
		int rx = pauseX + 42,
			ry = pauseY + yy + 15 + 22*i;
		if (PointInRect(msx, msy, rx, ry, rx + 64*3, ry+23)) // goto
		{
			cursor = i;
			msOkay = 1;
		}
	}

	c = GetControls() | GetArrows();
	dword gamepad = GetGamepadButtons();

	reptCounter++;
	if((!oldc) || (reptCounter>10))
		reptCounter=0;

	if((c&CONTROL_UP) && (!reptCounter))
	{
		cursor--;
		if(cursor==255)
			cursor=numItems-1;

		MakeNormalSound(SND_MENUCLICK);
	}
	if((c&CONTROL_DN) && (!reptCounter))
	{
		cursor++;
		if(cursor==numItems)
			cursor=0;

		MakeNormalSound(SND_MENUCLICK);
	}

	if((c&CONTROL_LF) && (!reptCounter))
	{
		switch(menu[cursor].effect)
		{
			case PE_SNDVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.sound=PrevVolumeSpot(profile.sound);
				JamulSoundVolume(profile.sound);
				break;
			case PE_MUSVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.music=PrevVolumeSpot(profile.music);
				SetMusicVolume(profile.music);
				break;
			case PE_SONG:
				PlayPrevSong();
				break;
			case PE_HUDCHOICE:
				MakeNormalSound(SND_MENUSELECT);
				switch (profile.progress.hudChoice)
				{
					case HudChoice::Supreme:
						profile.progress.hudChoice = HudChoice::Classic;
						break;
					case HudChoice::Advanced:
						profile.progress.hudChoice = HudChoice::Supreme;
						break;
					case HudChoice::Classic:
						profile.progress.hudChoice = HudChoice::Advanced;
						break;
					default:
						profile.progress.hudChoice = HudChoice::Supreme;
				}
				SetupOptionItems();
				break;
		}
		SetupSoundItems();
	}
	if((c&CONTROL_RT) && (!reptCounter))
	{
		switch(menu[cursor].effect)
		{
			case PE_SNDVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.sound=NextVolumeSpot(profile.sound);
				JamulSoundVolume(profile.sound);
				break;
			case PE_MUSVOL:
				MakeNormalSound(SND_MENUSELECT);
				profile.music=NextVolumeSpot(profile.music);
				SetMusicVolume(profile.music);
				break;
			case PE_SONG:
				PlayNextSong();
				break;
			case PE_HUDCHOICE:
				MakeNormalSound(SND_MENUSELECT);
				switch (profile.progress.hudChoice)
				{
					case HudChoice::Supreme:
						profile.progress.hudChoice = HudChoice::Advanced;
						break;
					case HudChoice::Advanced:
						profile.progress.hudChoice = HudChoice::Classic;
						break;
					case HudChoice::Classic:
						profile.progress.hudChoice = HudChoice::Supreme;
						break;
					default:
						profile.progress.hudChoice = HudChoice::Supreme;
				}
				SetupOptionItems();
				break;
		}
		SetupSoundItems();
	}
	if(((c&CONTROL_B1) && (!(oldc&CONTROL_B1))) ||
	   mgl->MouseTap())
	{
		MakeNormalSound(SND_MENUSELECT);
		i=0;
		switch(menu[cursor].effect)
		{
			case PE_CONTINUE:
				return PAUSE_CONTINUE;
			case PE_RETRY:
				return PAUSE_RETRY;
			case PE_GIVEUP:
				return PAUSE_GIVEUP;
			case PE_WORLDSEL:
				strcpy(profile.lastWorld,player.worldName);
				player.worldProg->levelOn=player.levelNum;
				SaveProfile();
				return PAUSE_WORLDSEL;
			case PE_SNDVOL:
				profile.sound=NextVolumeSpot(profile.sound);
				JamulSoundVolume(profile.sound);
				break;
			case PE_MUSVOL:
				profile.music=NextVolumeSpot(profile.music);
				SetMusicVolume(profile.music);
				break;
			case PE_MUSICMODE:
				if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
					profile.musicMode=MUSIC_OFFICIAL|MUSIC_REPEAT;
				else
				{
					i=profile.musicMode&MUSIC_LISTBITS;
					profile.musicMode=(profile.musicMode&MUSIC_PLAYMODE)+8;
					if(profile.musicMode>16)
						profile.musicMode=0;
					profile.musicMode+=i;
					PlaySong(curMap->song);
				}
				break;
			case PE_MUSICTYPE:
				i=profile.musicMode&MUSIC_PLAYMODE;
				profile.musicMode=(profile.musicMode&MUSIC_LISTBITS)+1;
				if(profile.musicMode>4)
					profile.musicMode=0;
				profile.musicMode+=i;
				if((profile.musicMode&MUSIC_LISTBITS)==MUSIC_OFFICIAL)
					profile.musicMode=MUSIC_OFFICIAL|MUSIC_REPEAT;
				PlaySong(curMap->song);
				break;
			case PE_SONG:
				ChooseNextSong();
				break;
			case PE_MUSIC:
				cursor=0;
				menuMode=3-menuMode;
				if(menuMode==3)
				{
					FillPauseMenu(musicPause);
				}
				else
				{
					if(!shopping && !editing)
					{
						// if cheats are available, use cheat game list instead
						if(ItemPurchased(SHOP_MAJOR,MAJOR_CHEATMENU))
							FillPauseMenu(gameCheatPause);
						else
							FillPauseMenu(gamePause);

					}
					else if(!shopping)
					{
						FillPauseMenu(editPause);
					}
					else
						FillPauseMenu(shopPause);
				}
				i=100;
				break;
			case PE_EXIT:
				if(!shopping && !editing)
				{
					strcpy(profile.lastWorld,player.worldName);
					player.worldProg->levelOn=player.levelNum;
				}
				SaveProfile();
				return PAUSE_EXIT;
			case PE_SHOP:
				if(!shopping)
				{
					strcpy(profile.lastWorld,player.worldName);
					player.worldProg->levelOn=player.levelNum;
				}
				SaveProfile();
				return PAUSE_SHOP;
			case PE_CHEAT:
				cursor=0;
				menuMode=1-menuMode;
				if(menuMode==1)
				{
					FillPauseMenu(cheatPause);
				}
				else
				{
					if(!editing)
					{
						// if cheats are available, use cheat game list instead
						if(ItemPurchased(SHOP_MAJOR,MAJOR_CHEATMENU))
							FillPauseMenu(gameCheatPause);
						else
							FillPauseMenu(gamePause);

					}
					else
					{
						FillPauseMenu(editPause);
					}
				}
				i=100;
				break;
			case PE_MORECHEATS:
				cursor=0;
				FillPauseMenu(cheatPause2);
				i=100;
				break;
			case PE_PREVCHEATS:
				cursor=0;
				FillPauseMenu(cheatPause);
				i=100;
				break;
			case PE_BZZT:
				MakeNormalSound(SND_TURRETBZZT);
				break;
			case PE_WPNLOCK:
				profile.progress.wpnLock = !profile.progress.wpnLock;
				SetupOptionItems();
				break;
			case PE_HUDCHOICE:
				switch (profile.progress.hudChoice)
				{
					case HudChoice::Supreme:
						profile.progress.hudChoice = HudChoice::Advanced;
						break;
					case HudChoice::Advanced:
						profile.progress.hudChoice = HudChoice::Classic;
						break;
					case HudChoice::Classic:
						profile.progress.hudChoice = HudChoice::Supreme;
						break;
					default:
						profile.progress.hudChoice = HudChoice::Supreme;
				}
				SetupOptionItems();
				break;
		}
		if(i==0 && menu[cursor].effect>=PE_CHEATS)
		{
			DoCheat(menu[cursor].effect-PE_CHEATS);
			return PAUSE_CONTINUE;
		}
		SetupSoundItems();
	}

	lastKey = mgl->LastKeyPressed();
	if(lastKey==27 || (gamepad & ~oldGamepad) & (1 << SDL_GAMEPAD_BUTTON_START))	// hit ESC to exit pause menu
	{
		MakeNormalSound(SND_MENUSELECT);
		lastKey=0;
		return PAUSE_CONTINUE;
	}

	oldc = c;
	oldGamepad = gamepad;

	return PAUSE_PAUSED;
}
