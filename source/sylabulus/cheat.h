#ifndef CHEAT_H
#define CHEAT_H

#include "sound.h"
#include "display.h"
#include "player.h"
#include "message.h"
#include "bullet.h"

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
#define NUM_CHEATS				13	// super fast

void InitCheater(void);
void CheatKey(char c);
void DoCheat(byte w);
const char *CheatName(byte w);

#endif
