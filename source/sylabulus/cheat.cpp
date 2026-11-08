#include "winpch.h"
#include "cheat.h"
#include "shop.h"
#include "gallery.h"
#include "editor.h"
#include <ctype.h>

const char cheatName[NUM_CHEATS][16]={
	"Ammo Crate",
	"Killer Aura",
	"Bombs Away!",
	"Pairs & Pairs",
	"Keymaster",
	"Healthy Glow",
	"Brainiac",
	"Secret Agent",
	"Small Shield",
	"MEGA RAGE!",
	"Stoneskin",
	"Super Speed"
};

#define NUM_TYPE_CHEATS	5

char cheatCode[NUM_TYPE_CHEATS][12]={
#ifdef IGF
	"igfsupreme",	// get everything
	"igfcheese",	// get 100,000 coins and 50 keys
#else
	"scummypunk",	// get all cheats
	"leartiste",	// get the editor
	"rosebud",		// get $100
	"loadsamoney",	// get $500
	"reset",		// get $100
#endif
};

char lastKeys[17];	// the last 16 letter keys pressed

void InitCheater(void)
{
	int i;

	for(i=0;i<16;i++)
		lastKeys[i]='\0';

	lastKeys[16]='\0';	// a padding 0, so strcmp can be used
}

void CheatKey(char c)
{
	int i,j;

	// scoot the existing letters over 1
	for(i=0;i<15;i++)
		lastKeys[i]=lastKeys[i+1];
	// and stick the new one on the end
	lastKeys[15]=tolower(c);

	for(i=0;i<NUM_TYPE_CHEATS;i++)
	{
		if(!strcmp(cheatCode[i],&lastKeys[16-strlen(cheatCode[i])]))
		{
			// cheat was triggered

			switch(i)
			{
				case 0:	// cheats
					for(j=0;j<NUM_CHEATS;j++)
					{
						// cheater!!
					}
					SetupShops(curMap);
					SetupGalPix(curMap);
					break;
				case 1:	// editor
					NewBigMessage("THE EDITOR IS YOURS!",60);
					break;
				case 2:	// rosebud
					profile.progress.totalCoins += 100;
					MakeNormalSound(SND_COINGET);
					break;
				case 3:	// loadsamoney
					profile.progress.totalCoins += 500;
					MakeNormalSound(SND_COINGET);
					break;
				case 4:	// reset
					ResetAllPurchases();
					MakeNormalSound(SND_TURNEVIL);
					NewBigMessage("RESET!",60);
					break;
			}
			lastKeys[15]='\0';	// scrap the highest letter, effectively clearing the buffer
		}
	}
}

/*
#define CHET_AMMOCRATE			0	// ammo supreme crate
#define CHET_AURADMG			1	// gives aura damage for 10s
#define CHET_KABLOOIE			2	// explodes the screen
#define CHET_MAXPANTS			3	// =4 pants, gives a hammer too
#define CHET_KEYMASTER			4	// skeleton key
#define CHET_MEDIC				5	// full health
#define	CHET_ALLBRAINS			6	// ALL the brains!
#define CHET_INVISIBLE			7	// invisible cloak drop
#define CHET_SHIELD				8	// energy barrier drop
#define CHET_POCKET				9	// max pocket space
#define CHET_RAGE				10	// full rage bar
#define CHET_STONESKIN			11	// strength
#define CHET_SUPERFAST			12	// super fast
*/

byte CanCheat(byte w)
{
	if (curMap->type == MAP_TYPE_HUB)
		return 0;

	switch (w)
	{
		case CHET_ALLBRAINS: // won't work on boss levels
			return player.brains < curMap->numBrains && !(curMap->type == MAP_TYPE_BOSS);
		case CHET_MAXPANTS:
			return (player.hamSpeed > 0); // max pants
		case CHET_KEYMASTER:
			return (player.keys[0] < 3 || player.keys[1] == 0 && player.keys[2] == 0 || player.keys[3] == 0);
		case CHET_SUPERFAST:
			return (player.speed[0] == 0); // not speedy
		case CHET_INVISIBLE:
			return (player.invisibility[0] == 0); // not invisible
		case CHET_RAGE:
			return (player.rage < 127 * 256); // not at max rage
		case CHET_POCKET:
			return (player.wpnSlots < 8); // not at max pockets
		case CHET_STONESKIN:
			return (goodguy && goodguy->strength == 0); // not already strong
		case CHET_MEDIC:
			return (goodguy && goodguy->hp < goodguy->maxHP); // not at max health
		case CHET_SHIELD:
			return (player.shield == 0); // not shielded
		case CHET_AMMOCRATE:
			return (player.ammoCrate == 0); // not ammo'd
	}
	return 1;
}

void DoCheat(byte w)
{
	int cost = 1;

	// editing bypasses cheatzy requirement, otherwise, check if the cheat is available and if the player has enough cheatzies to spend
	if (!editing && (!CanCheat(w) || ((profile.progress.totalCheatzies - profile.progress.cheatziesSpent) < cost)))
	{
		// NOPE!
		MakeNormalSound(SND_TURRETBZZT);
		return;
	}

	player.cheated = 1;
	profile.progress.cheatziesSpent++;
	if(!editing && verified)
		profile.progress.cheats++;

	switch(w)
	{
		case CHET_MAXPANTS:
			if(player.hammers<1)
				player.hammers=1;
			player.hamSpeed=0;
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHET_MEDIC:
			PlayerHeal(128);
			NewMessage("Aaaaah",30,0);
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHET_SHIELD:
			player.shield=255;
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHET_KABLOOIE:
		{
			MakeNormalSound(SND_BOMBIELEAP);
			auto [cx, cy] = GetCamera();
			cx-=320;
			cy-=240;
			for(int i=0;i<60;i++)
			{
				FireBullet((cx+Random(640))<<FIXSHIFT,(cy+Random(480))<<FIXSHIFT,
							0,BLT_BOOM,1);
			}
			ShakeScreen(10);	// make the screen shake!
			break;
		}
		case CHET_ALLBRAINS:
			player.brains=curMap->numBrains;
			SetPlayerGlow(128);
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHET_KEYMASTER:
			player.keys[0]=3;
			player.keys[1]=1;
			player.keys[2]=1;
			player.keys[3]=1;
			MakeNormalSound(SND_CHEATWIN);
			NewMessage("I am the keymaster!",30,0);
			break;
		case CHET_AMMOCRATE:
			MakeNormalSound(SND_AMMOCRATE);
			if(GetFirstAvailableWeaponSlot() == 255) // give a gun if you have no weapon
				TakeWeapon(WPN_AK8087,1);
			player.ammoCrate=255;
			NewMessage("Ammo Crate!",30,0);
			break;
		case CHET_SUPERFAST:
			player.speed[1]=30*60;
			SetSpeedFrames(goodguy,30*30);
			CalculateMusicSpeed(); // 2x
			MakeNormalSound(SND_SPEEDUP);
			break;
		case CHET_RAGE:
			player.rage=127*256;
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHET_POCKET:
			player.wpnSlots = 8;
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHET_AURADMG:
			NewMessage("Mad Aura!",30,0);
			PlayerSetAuraDamage(30*30);
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHET_STONESKIN:
			goodguy->strength = 30*10;
			MakeNormalSound(SND_CHEATWIN);
			break;
		case CHET_INVISIBLE:
			player.invisibility[0] = 30*20;
			player.invisibility[1] = 30*20;
			MakeNormalSound(SND_CHEATWIN);
			break;
	}
}

const char *CheatName(byte w)
{
	return cheatName[w];
}
