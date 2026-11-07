#ifndef MONSTER_H
#define MONSTER_H

/* this file does the AI and everything else for each monster type.
   It knows all about the different types, as opposed to guy.cpp which
   just sort of keeps track of the list of guys. */

#include "jamulspr.h"
#include "display.h"

// the monster types
#define MONS_NONE	  0
#define MONS_BOUAPHA  1
#define MONS_BONEHEAD 2
#define MONS_BAT	  3
#define MONS_SPIDER	  4
#define MONS_BIGSPDR  5
#define MONS_ZOMBIE	  6
#define MONS_EGGSAC	  7
#define MONS_MAMASPDR 8
#define MONS_PYGMY	  9 // gnome
#define MONS_SERPENT  10
// the parts of Mattie
#define MONS_MATHEAD  11
#define MONS_MATSKULL 12
#define MONS_MATBRAIN 13
#define MONS_MATBODY  14
#define MONS_MATCLAW1 15
#define MONS_MATCLAW2 16
#define MONS_MATTAIL  17
// more normal monsters
#define MONS_GINGER	  18
#define MONS_PUMPKIN  19
#define MONS_BABYTHING 20
#define MONS_MOSS	  21
#define MONS_MOSSGRANDE 22
#define MONS_MAGMAZOID 23
#define MONS_SHROOM	  24
#define MONS_MUSH	  25
// the parts of The Thing
#define MONS_THING	  26
#define MONS_THINGTENT 27
#define MONS_THINGTENTTIP 28
// normal monsters again
#define MONS_SUPERZOMBIE 29
#define MONS_STICKMAN 30
#define MONS_BABYSEAL 31
#define MONS_ISOZOID  32
#define MONS_SNOWGUY  33
#define MONS_PENGUIN  34
#define MONS_ZOMBONI  35
// the Yeti Bros.
#define MONS_SVEN	  36
#define MONS_BJORN	  37
// normal monsters again
#define MONS_GEOZOID  38
#define MONS_MUMBLE	  39
#define MONS_DJINNI	  40
#define MONS_LAMP	  41
#define MONS_CACTUS	  42
#define MONS_ROLLER	  43
#define MONS_LICH	  44
#define MONS_DUSTDEVIL 45
#define MONS_MECHABOUAPHA 46
// Sphinx parts
#define MONS_SPHXARM1 47
#define MONS_SPHXARM2 48
#define MONS_SPHINX   49
// more normal monsters
#define MONS_FREAKAZOID 50
#define MONS_CENTIBODY 51
#define MONS_CENTIHEAD 52
#define MONS_WACKO	  53
#define MONS_BOILER	  54
#define MONS_GREATPK  55
#define MONS_ULTRAZOID 56
// the Asylum bosses
#define MONS_DRL	  57
#define MONS_SDZL	  58

#define MONS_SANTA	  59

// expansion "monsters"
#define MONS_MINECART 60	// mine cart Bouapha rides on
#define MONS_RAFT	  61	// raft Bouapha rides on
#define MONS_PWRBOUAPHA 62	// Bouapha in power armor

// expansion monsters
#define MONS_VAMPIRE  63
#define MONS_COFFIN	  64
#define MONS_GHOST	  65
#define MONS_BURNER	  66
#define MONS_LEFTY	  67
#define MONS_PYGMY2	  68 // gnome
#define MONS_PYGMY3	  69 // gnome
#define MONS_PKSTEIN  70
#define MONS_KNIGHT	  71
#define MONS_TRICEROID 72
#define MONS_COUNTESS 73
#define MONS_ALIENEGG 74
#define MONS_BABYALIEN 75
#define MONS_ALIEN	  76
#define MONS_ROBOPK	  77
#define MONS_SHOCKTR  78
#define MONS_ROBOT1	  79
#define MONS_ROBOT2	  80
#define MONS_ROBOFACTORY 81
#define MONS_TURRET	  82
#define MONS_BUNNY	  83
#define MONS_KONGOR	  84
#define MONS_SQUASH	  85
#define MONS_MINIPYGMY 86 // gnome
#define MONS_LOONYBOT 87
#define MONS_LOONYCORE 88
#define MONS_LOONYGUN 89
#define MONS_LOONYSHIP 90

// -- fun pack
#define MONS_FRIENDLY   91
#define MONS_GOODTURRET 92
#define MONS_ROLLER2	93
#define MONS_ALIENEGG2	94

#define MONS_MAT2HEAD	95
#define MONS_MAT2SKULL	96
#define MONS_MAT2BRAIN	97
#define MONS_MAT2BODY	98
#define MONS_MAT2TAIL	99

#define MONS_SHAMAN2	100
#define MONS_JALAPENO	101

#define MONS_GENERATOR1	102
#define MONS_GENERATOR2	103
#define MONS_GENERATOR3	104
#define MONS_GENERATOR4	105

#define MONS_SHARK		106
#define MONS_MADBUG		107
#define MONS_WIZARD		108
#define MONS_EVILCLONE	109
#define MONS_BOB		110
#define MONS_MULTIMOSS  111
#define MONS_MOSS2		112
#define MONS_SNOWBALL	113
#define MONS_SNOWBALL2	114
#define MONS_SNOWBLOW	115
#define MONS_BOOMKIN	116
#define MONS_MUMBLE2	117
#define MONS_GOODROBOT	118
#define MONS_GOODROBOT2 119
#define MONS_XENOMAMA	120
#define MONS_ROLLER3	121
#define MONS_ROLLER4	122
#define MONS_DARKVAMP	123
#define MONS_GNOME		124
#define MONS_NOBODY		125
#define MONS_FRIENDLY2	126
#define MONS_TROOPER2	127
#define MONS_PUMPKIN2	128
#define MONS_CRAZYBONE	129
#define MONS_CREEPAZOID 130

#define MONS_LAZYBONE	131
#define MONS_STAREYBAT	132
#define MONS_MADMAN		133
#define MONS_PYGMYACCT	134 // gnome
#define MONS_CRAZYPANTS	135
#define MONS_YERFDOG	136
#define MONS_ELITESEAL	137
#define MONS_CONE		138
#define MONS_PYGMYDIVER	139 // gnome
#define MONS_PUFFYFISH	140
#define MONS_PUFFYFISH2 141
#define MONS_OLAF		142
#define MONS_SPIKEY		143
#define MONS_BOMBOT		144
#define MONS_FOLLOWBUNNY 145
#define MONS_AUTOZOID	146
#define MONS_CHEST		147
#define MONS_JELLOFISH	148
#define MONS_KINGCONE	149
#define MONS_MAILBOXER	150
#define MONS_PIRANHA	151
#define MONS_YUGO		152
#define MONS_MINISUB	153	// bouapha in minisub
#define MONS_PUNKIN		154
#define MONS_SEACUKE	155
#define MONS_SNKYSHRK2	156
#define MONS_STARFISH	157
#define MONS_UNDERZOID	158
#define MONS_WETSEAL	159
#define MONS_WETWILLY	160
#define MONS_CRABPUFF	161
#define MONS_PATCH		162
#define MONS_PATCH2		163	// quickly producing patch
#define MONS_PATCH3		164	// boomkin patch
#define MONS_PATCH4		165	// squash patch
#define MONS_PATTY		166
#define MONS_DOZER		167
#define MONS_DARKCOFFIN	168
#define MONS_MUTANT		169
#define MONS_WHATSIT	170
#define MONS_SHOCKGHOST 171
#define MONS_MECHAZOID	172
#define MONS_SUCKER1	173	// sucks from anywhere in the world
#define MONS_SUCKER2	174	// sucks harder
#define MONS_SUCKER3	175	// sucks over limited range
#define MONS_SUCKER4	176	// sucks harder
#define MONS_BLOWER1	177	// blows from anywhere in the world
#define MONS_BLOWER2	178	// blows harder
#define MONS_BLOWER3	179	// blows over limited range
#define MONS_BLOWER4	180	// blows harder
#define MONS_BEETLE		181
#define MONS_SCARAB		182
#define MONS_FROSTBITER 183
#define MONS_MISLTURRET 184
#define MONS_DEATHTURRET 185
#define MONS_UNDERMAGMA	186
#define MONS_PARKEDCAR	187
#define MONS_PARKED2	188
#define MONS_TRAFFIC	189
#define MONS_TRAFFIC2	190
#define MONS_PLAYMECHA	191
#define MONS_SHROOMLORD	192
#define MONS_BIGBAT		193
#define MONS_HUGEBAT	194
#define MONS_WOLF		195
#define MONS_WOLF2		196
#define MONS_PATROLLR	197
#define MONS_PATROLUD	198
#define MONS_DPATROLLR	199
#define MONS_DPATROLUD	200
#define MONS_WEATHERMAN	201
#define MONS_XENOEGG	202
#define MONS_PLAYSHROOM 203
#define MONS_PUNKBUNNY  204
#define MONS_LUNACHICK	205
#define MONS_SKELETON	206
#define MONS_HOTSHROOM	207
#define MONS_MINIZOID	208
#define MONS_MINIWACKO	209
#define MONS_JACKFROST	210


#define ENT_NONE				0
#define ENT_BOUAPHA				1

// world 1
#define ENT_BONEHEAD			2
#define ENT_MUMBLE				3
#define ENT_ZOMBIE				4
#define ENT_LANTERN				5
#define ENT_HARDHAT				6
#define ENT_FATCROW				7

#define ENT_KAPTANA				8
#define ENT_AQUAZOID			9
#define ENT_TSUCHIZOID			10
#define ENT_BUNNY				11
#define ENT_BUNNYPATH			12
#define ENT_BUNNYFOLLOW			13
#define ENT_BUNNYJIMBO			14
#define ENT_NINJABREAD			15
#define ENT_PUMPKIN				16
#define ENT_KABOOCHA			17
#define ENT_CRAZYBUSH			18
#define ENT_MIKASA				19
#define ENT_EYEBALLTREE			20
#define ENT_EASTEREGG			21
#define ENT_HAPPYSTICKMAN		22
#define ENT_NOHMANNCOMEDY		23
#define ENT_NOHMANNTRAGEDY		24
#define ENT_NOHMANNMASKCOMEDY	25
#define ENT_NOHMANNMASKTRAGEDY	26

// recolors and misc
#define ENT_YERFDOGHELPER		27
#define ENT_YERFDOGBOSS			28
#define ENT_NOBODY				29
#define ENT_SPIDER				30
#define ENT_BIGSPIDER			31
#define ENT_EGGSAC				32
#define ENT_MAMASPIDER			33
#define ENT_SHROOM				34

#define ENT_CRAZYBONE			35
#define ENT_TUTUNCOMMON			36
#define ENT_BOMBIE				37
#define ENT_BOBBISCUIT			38
#define ENT_PUMPKINPATCH		39
#define ENT_BOOMKIN				40
#define ENT_SQUASH				41
#define ENT_JALAPENO			42
#define ENT_MUTANTHARDHAT		43
#define ENT_MADKAP				44

#define ENT_HOTNSOUR			45
#define ENT_DEATHCAP			46
#define ENT_TURRET				47
#define ENT_MISSILETURRET		48
#define ENT_DEATHTURRET			49
#define ENT_GOODTURRET			50

// vehicles/mechs
#define ENT_MINECART			51
#define ENT_POWERARMOR			52
#define ENT_MINISUB				53
#define ENT_YOUGO				54

// world 2
#define ENT_CREEPINGCONE		55
#define ENT_AUTOZOID			56
#define ENT_CARPARKED			57
#define ENT_TRAFFIC				58
#define ENT_TRAFFIC2			59
#define ENT_MAILBOXER			60
#define ENT_WETWILLY			61
#define ENT_STOPSIGN			62
#define ENT_DULLDRUM			63
#define ENT_BODZHA				64
#define ENT_PUNKBUNNY			65
#define ENT_MOLEY				66
#define ENT_GNOME				67
#define ENT_MARVIN				68
#define ENT_GNOMEACCOUNTANT		69
#define ENT_MECHATRUCK			70
#define ENT_KINGCONE			71
#define ENT_TECKHEAD			72
#define ENT_EDISONCOIL			73
#define ENT_EDISONBOSS			74
#define ENT_EDISONBRAIN			75
#define ENT_PUNKIN				76
#define ENT_JACKO				77

// recolors and variants
#define ENT_REDBONE				78
#define ENT_KEROSENELAMP		79
#define ENT_OVENGEN				80
#define ENT_PATROLLERH			81
#define ENT_PATROLLERV			82
#define ENT_DECOYBOUAPHA		83
#define ENT_CACTON				84
#define ENT_RADOZOID			85
#define ENT_MUTANTZOMBIE		86
#define ENT_SHAMISEN			87
#define ENT_RAFT				88

#define ENT_MAXID				89

#define NUM_MONSTERS	243					// Can grow as needed
#define NUM_PROFILE_MONSTERS 211			// Do not change without redoing the profile format

// fake monster codes
#define MONS_GOODGUY	(-1)	// any goodguy
#define MONS_BADGUY		(-2)	// any badguy
#define MONS_ANYBODY	(-3)	// anybody
#define MONS_NONPLAYER	(-4)	// anybody but the player
#define MONS_TAGGED		(-5)	// the tagged monster
#define MONS_PLAYER		(-6)	// the player
#define MONS_BOSS		(-7)	// the current boss

// the animations
#define ANIM_IDLE	0
#define ANIM_MOVE	1
#define ANIM_ATTACK 2
#define ANIM_DIE	3
#define ANIM_A1		4
#define ANIM_A2		5
#define ANIM_A3		6
#define ANIM_A4		7
#define ANIM_A5		8
#define NUM_ANIMS	9

#define ANIM_LENGTH	24

constexpr byte FRAME_INVIS = 254;
constexpr byte FRAME_END = 255;

// flags
enum MonsterFlags_ : dword
{
	MF_FLYING     = 1 << 0,
	MF_WATERWALK  = 1 << 1,
	MF_ONEFACE    = 1 << 2,
	MF_ENEMYWALK  = 1 << 3,	// other enemies can stomp all over this one (but not Bouapha)
	MF_NOMOVE     = 1 << 4,	// doesn't move when hit
	MF_AQUATIC    = 1 << 5,	// can ONLY move on water/lava, not land
	MF_INVINCIBLE = 1 << 6, // totally invulnerable to harm
	MF_SPRITEBOX  = 1 << 7,	// use the sprite's rect for collision checks instead of standard size-box method
	MF_FACECMD    = 1 << 8,	// this monster's "facing" value should just be added to the sprite number, it's calculated by his AI (only useful for MF_ONEFACE monsters)
	MF_NOGRAV     = 1 << 9,
	MF_FREEWALK   = 1 << 10, // Bouapha can walk right through it
	MF_WALLWALK   = 1 << 11, // walk through walls
	MF_NOSHADOW   = 1 << 12, // doesn't cast a shadow
	MF_GHOST      = 1 << 13, // draw using ghost draw
	MF_NOHIT      = 1 << 14, // bullets pass through it
	MF_GLOW       = 1 << 15, // draw using glow draw
};
BITFLAGS(MonsterFlags_)

// themes
enum MonsterThemes : dword
{
	MT_NONE     = 0,
	MT_GOOD		= 1 << 0,
	MT_EVIL		= 1 << 1,
	MT_SPOOKY	= 1 << 2,
	MT_ZOMBIE	= 1 << 3,
	MT_VAMPIRE	= 1 << 4,
	MT_SPIDER	= 1 << 5,
	MT_PYGMY	= 1 << 6,
	MT_ZOID		= 1 << 7,
	MT_BOSS		= 1 << 8,
	MT_MINIBOSS = 1 << 9,
	MT_WACKY	= 1 << 10,
	MT_PUMPKIN	= 1 << 11,
	MT_THINGY	= 1 << 12,
	MT_VEGGIE	= 1 << 13,
	MT_ARCTIC	= 1 << 14,
	MT_DESERT   = 1 << 15,
	MT_VEHICLE	= 1 << 16,
	MT_GENERATE	= 1 << 17,
	MT_TRAP		= 1 << 18,
	MT_ALIEN	= 1 << 19,
	MT_HIGHTECH	= 1 << 20,
	MT_ANIMAL	= 1 << 21,
	MT_HUMAN	= 1 << 22,
	MT_URBAN	= 1 << 23,
	MT_AQUATIC	= 1 << 24,
	MT_UNDERSEA = 1 << 25,
	MT_FLYING	= 1 << 26,
	MT_BITS		= 1 << 27,
};


/*
	Remove themes:
	- thingy, vampire, zoid, pygmy (too specific, pygmies are also replaced by gnomes)

	new theme roster:
	0. goodguy
	1. badguy
	2. spooky (+thingy/vampire)
	3. vegetation (+pumpkin)
	4. oriental
	5. urban
	6. puzzle
	7. boss
	8. miniboss
	9. aquatic
	10. flying
	11. generator
	12. cosmic
	13. human
	14. wacky
	16. bits

	- goodguy (they help bouapha)
	- badguy (self-explanatory)
	- spooky (common theme, all typical undead creatures)
	- boss
	- miniboss (not as bossy)
	- pumpkin (for keychain levels)
	- oriental/urban (for each world)
	- trap (for puzzles)
	- generate (function categories)
	- machine
	- human
	- aquatic
*/

BITFLAGS(MonsterThemes)
constexpr int NUM_MONSTHEMES = 28;

typedef void (*Monster_AIFunc)(Guy *,Map *,world_t *,Guy *);

struct MonsterType
{
	char name[32];
	byte fromCol,toCol;
	char brtChg;
	Monster_AIFunc AI;
	byte size;
	byte framesPerDir;
	word hp;
	word points;
	char sprName[32];
	sprite_set_t *spr;
	MonsterFlags_ flags;
	MonsterThemes theme;
	byte anim[NUM_ANIMS][ANIM_LENGTH];
	word height;
};

void InitMonsters(void);
void ExitMonsters(void);

void PurgeMonsterSprites(void);

void ChangeOffColor(dword type,byte from,byte to);
byte MonsterSize(dword type);
const byte *MonsterAnim(dword type,byte anim);
MonsterFlags_ MonsterFlags(dword type,dword aiType);
MonsterThemes MonsterTheme(dword type);
byte MonsterFrames(dword type,dword aiType);
word MonsterHP(dword type);
word MonsterPoints(dword type);
const char *MonsterName(word type);
Monster_AIFunc MonsterAI(dword type);
int8_t MonsterBrightnessChange(dword type);

void MonsterDraw(
	int x, int y, int z,
	dword type, bool isBouapha,
	byte seq, byte frm, byte facing,
	char bright,
	byte ouch, byte poison, byte frozen,
	byte fromCol, byte toCol, uint8_t brtChg,
	const sprite_set_t* set,
	byte flags
);
void InstaRenderMonster(int x,int y,dword type,char bright,MGLDraw *mgl);
int InstaRenderScannedMonster(int x,int y,dword type,char bright,MGLDraw *mgl);

const sprite_t* GetMonsterSprite2(dword type, byte seq, byte frm, byte facing);
const sprite_t* GetMonsterSprite(Guy* me);

void FaceGoodguy(Guy *me,Guy *goodguy);
void FaceGoodguy2(Guy *me,Guy *goodguy);
void FaceGoodguy3(Guy *me,Guy *goodguy);
int RangeToTarget(Guy *me,Guy *goodguy);
int RangeToPoint(Guy* me, int x, int y);
void FlailLock(Guy *me);
void FacePoint(Guy* me, int x, int y);

void DrawMe(Guy* g, const sprite_set_t* set);
byte IsEntity(Guy* g, word targetType);
#endif
