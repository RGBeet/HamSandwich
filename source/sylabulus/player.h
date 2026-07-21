#ifndef PLAYER_H
#define PLAYER_H

#include "jamultypes.h"
#include "guy.h"
#include "control.h"
#include "game.h"
#include "display.h"
#include "bullet.h"
#include "world.h"
#include "intface.h"
#include "progress.h"

// Secondary weapon defines.
// SERIALIZED in items and in specials. Add to end, don't reorder.
enum : byte
{
	WPN_NONE,
	WPN_MISSILES,
	WPN_AK8087,
	WPN_BOMBS,
	WPN_FLAME,
	WPN_PWRARMOR,
	WPN_BIGAXE,
	WPN_LIGHTNING,
	WPN_SPEAR,
	WPN_MACHETE,
	WPN_MINES,
	WPN_TURRET,
	WPN_MINDCONTROL,
	WPN_REFLECTOR,
	WPN_JETPACK,
	WPN_SWAPGUN,
	WPN_TORCH,
	WPN_SCANNER,
	WPN_MINISUB,
	WPN_FREEZE,
	WPN_STOPWATCH,

	MAX_WEAPONS
};
const char* GetWeaponName(byte weapon);
const int GetWeaponIcon(byte weapon);

// vehicles you could be on
enum : byte
{
	VE_NONE,
	VE_MINECART,
	VE_RAFT,
	VE_YUGO,
};

const char* GetAfflictName(byte weapon);
const char* GetKeychainName(byte keychain);
const char* GetParticleName(byte particle);

struct player_t
{
	// per-world value
	char worldName[32];
	worldData_t *worldProg;
	// values reset for each level
	levelData_t *levelProg;
	byte shield;
	byte levelNum;
	byte keys[4];
	int  boredom;

	byte hammers;
	byte hamSpeed;

	byte weapon;
	byte lastWeapon;
	int  ammo;
	byte reload;
	byte wpnReload;


	// wepaon stuff
	byte weapons[8];
	int	ammunition[8];
	byte activeSlot;
	byte wpnSlots;	// number of weapons slots (1-5)

	int life;
	int brains;
	int coins;
	int candles;
	int timer;

	byte pushPower;	// for pushing pushy blocks
	HammerFlags hammerFlags;

	byte vehicle;
	byte garlic;
	byte speed;	// accelerated
	byte rageClock;
	word rage;
	word varbar, varbarMax;
	byte varbarColor;
	byte invisibility;
	byte jetting;
	byte lavaTimer;
	byte stealthy;
	byte perfect;
	byte timeStop;
	byte ammoCrate;
	byte torch;
	byte cheesePower;
	byte gotRecords;

	bool pendingLeaderboardUpload;
	byte spotted;

	int oxygen;
	int comboClock;
	int clock;
	int kills;
	int damageTaken;
	int bestCombo;
	int combo;
	int totalEnemies;
	int enemiesSlain;

	byte cheated;
	dword score;
	int var[8];	// local vars, cleared each new level
	byte ability[6];
	int brainX,brainY,candleX,candleY;
	word brainTime,candleTime;
	byte playAs;
};

extern player_t player;

void InitPlayer(byte level,const char *fname);
void ExitPlayer(void);
void PlayerControlMe(Guy *me,mapTile_t *mapTile,world_t *world);
void PlayerControlPowerArmor(Guy *me,mapTile_t *mapTile,world_t *world);
void PlayerControlMiniSub(Guy *me,mapTile_t *mapTile,world_t *world);
byte PlayerHasHammer(void);
void PlayerWinLevel(byte isSecret);
void PlayerRenderInterface(MGLDraw *mgl);
int  PlayerBrains(void);
void SetPlayerHP(int hp);
byte PlayerKeyChain(byte w);
byte PlayerKeys(byte w);
void PlayerLoseKey(byte w);
void PlayerHeal(byte amt);
void PlayerLoadGame(byte which);
void PlayerSaveGame(byte which);
byte PlayerShield(void);
void SetPlayerGlow(byte v);
byte GetPlayerGlow(void);
byte PlayerPushMore(void);
void PoisonVictim(Guy *me,byte amt);
byte PlayerIsPoisoned(void);
void ToggleWaterwalk(void);
byte PlayerCanWaterwalk(void);
void SetTportClock(byte tp);
byte GetTportClock(void);
byte StealWeapon(void);

byte GetCurrentWeaponType();
int GetCurrentWeaponAmmo();
bool PlayerCanFireWeapon();
byte ReduceCurrentWeaponAmmo(int amt);
byte SetCurrentWeaponAmmo(int amt);

int WeaponMaxAmmo(byte wpn);
byte PlayerGetWeapon(byte wpn,int x,int y);
byte PlayerPowerup(int powerup);
void PlayerRadioactiveFood(void);
void PlayerChineseFood(void);
void KeyChainAllCheck(void);
void PlayerGetBrain(int amt);
void PlayerGetCandle(int amt);
void PlayerGetCoin(int amt);

void SetPlayerStart(int x,int y);
void PutPlayerAtStart(Guy *g);

byte ControlCheck(byte c);

void ShouldCheckControls(byte n);

byte GetCurrentWeaponType(void);
int GetCurrentWeaponAmmo(void);

void SetWeaponAmmo(byte slot, byte wpn, int ammo);
byte TakeWeapon(byte wpn, int ammo);
byte GetFirstAvailableWeaponSlot(void);
byte RemoveCurrentWeapon(void);

byte PlayerUsingMechWeapon(void);
byte PlayerAddPockets(int amt);
byte PlayerCanThrowHammers(void);

#define ANY_BUTTONS		(CONTROL_B1|CONTROL_B2|CONTROL_B3|CONTROL_B4)
#define ATK_BUTTONS		(CONTROL_B1|CONTROL_B2)

#endif
