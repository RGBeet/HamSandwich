#ifndef GUY_H
#define GUY_H

#include <memory>
#include <vector>
#include "map.h"
#include "monster.h"
#include "jamulspr.h"
#include "display.h"
#include "pathfinding.h"

#define ACTION_IDLE	0
#define ACTION_BUSY 1

class Guy final
{
	public:
		Guy(void);
		~Guy(void);

		// old functions
		void Update(Map *map,world_t *world);
		void EditorUpdate(Map *map);
		void Render(byte light);
		byte CanWalk(int xx,int yy,Map *map,world_t *world);
		void NextFrame(void);
		void SeqFinished(void);
		void MonsterControl(Map *map,world_t *world);
		byte CoconutBonk(int xx,int yy,Guy *him);
		byte AttackCheck(byte size,int xx,int yy,Guy *him);
		void AttackThem(void);
		void GetShot(int dx, int dy, byte damage, Map* map, world_t* world, bool bypassInvincible = false);
		void GetHealed(byte damage, Map* map, world_t* world, bool bypassInvincible);
		void CalculateRect(void);
		byte IsAwake(void);
		void SmoothPath(Map* map);

		// new functions
		byte IsInterface(void); // returns TRUE if guy's hp is currnetly being displayed on the interface (i.e. a boss)

		int x,y,z;
		int oldx,oldy;
		int dx,dy,dz;
		byte mapx,mapy;
		byte lastBumpX,lastBumpY;

		byte facing;

		// brain variables for ai
		byte mind;
		byte mind1;
		byte mind2;
		byte mind3;
		byte reload;

		byte ouch;
		byte action;
		word frmTimer;
		word frmAdvance;
		byte frm;
		byte seq;

		char bright;
		byte friendly; // which team? (TODO: add multi-team support)

		dword pushStamp;
		Guy *target;
		Guy *parent;

		Guy *lastAttacker; // last guy to have attacked

		int hp,maxHP;

		int type;		// what kind of monster?
		int aiType;		// behavior type (which function to use)
		int age;		// # of frames
		int size;		// takes monsSize

		int rectx,recty,rectx2,recty2;	// for collision checks
		word ID;	// just a copy of the guy's number
		byte item;	// what item you're carrying

		// 255 = 8.5 seconds? too short!

		// Status effects
		word poison;		// depletes HP over time
		word ignite;		// ALSO depletes HP over time, but affects enemies based on fire characteristics (NEW)
		word frozen;		// stops you in your tracks! affects enemies based on ice characteristics
		word weaken;		// take more damage (+50%)
		word slow;			// move at half frames

		word mindControl;		// temporarily shifted to other team
		byte mindControlTeam;	// which team you're on while mind controlled

		// Good afflictions
		word strength;		// take less damage (-25%)
		word speed;			// twice as fast

		byte fromColor,toColor;
		char brtChange;
		char name[32];

		std::unique_ptr<sprite_set_t> customSpr; // custom sprites

		// PATHFINDING
		std::vector<PathNode*> path; // pathfinding nodes
		word pathIndex; // current index in the path vector
		byte pathTimer; // timer for pathfinding updates

		void UpdatePathfinding(Map* map, int endX, int endY);
		void FollowPath(int speed=4);
};

extern Guy *goodguy;

void InitGuys(int max);
void ExitGuys(void);
void UpdateGuys(Map *map,world_t *world);
void EditorUpdateGuys(Map *map);
void RenderGuys(byte light);
Guy *AddGuy(int x,int y,int z,int type,byte friendly);
Guy *AddBaby(int x,int y,int z,int type,Guy *me);

Guy *GetGuy(word w);

void DeleteGuy(int x,int y,int type);
void AddMapGuys(Map *map);
Guy *FindVictim(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly);
Guy *FindVictims(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly);
Guy *FindVictims2(int x,int y,byte size,int dx,int dy,byte damage,Map *map,world_t *world,byte friendly);
word LockOnEvil(int x,int y);
word LockOnEvil2(int x,int y);
word LockOnGood(int x,int y);
word LockOnGood2(int x,int y);
byte GetGuyPos(word guy,int *x,int *y);
byte MonsterExists(int type);
void HealGoodguy(byte amt);
byte MossCheck(int x,int y);
void KillKids(Guy *g);
byte RaftNearby(void);
void GuySwap(int sx,int sy,int width,int height,int dx,int dy);
void ShiftGuys(char dx,char dy,Map *map);
void AddPygmy(Map *map,world_t *world,byte friendly);
void AddNinja(Map *map,world_t *world,byte friendly);
byte ControlMind(Guy *me);
void KillMonster(int x,int y,int type,byte nofx);
void MonsterLife(byte fx,int x,int y,int type,int newLife);
void MonsterMaxLife(byte fx,int x,int y,int type,int newLife);
byte CheckMonsterLife(int x,int y,int type,int life,byte flags);
byte SwapMe(int x,int y,byte size,Map *map);
int CountMonsters(int type);
int CountMonstersInRect(int type,int x,int y,int x2,int y2);
Guy *GetGuyOfType(int type);
void RemoveGuy(Guy *g);
void Telefrag(Guy *g, bool sphinxException = false);
byte FreezeGuy(Guy *me);
byte TryToPushItem(int x,int y,int destx,int desty,Map *map,world_t *world);
void ChangeMonster(byte fx,int x,int y,int type,int newtype);
void ChangeMonsterAI(byte fx,int x,int y,int type,int newtype);
void ChangeTeam(byte fx,int x,int y,int type,byte team);
void ChangeMonsItem(byte fx,int x,int y,int type,int newItem);
byte CheckMonsterAwake(int x,int y,int type,byte flags);
Guy *FindMonster(int x,int y,int type);
byte BadguyRegions(int x,int y,int x2,int y2,int tx,int ty);
void MonsterLifeAmt(byte fx,int x,int y,int type,int amt);
void SetMonsterName(byte fx,int x,int y,int type,char *name);
void SetMonsterGraphics(byte fx,int x,int y,int type,char *name);
void SetMonsterColor(byte fx,int x,int y,int type,int colCode);
void SetMonsterBright(byte fx,int x,int y,int type,int bright);

byte CheckMonsterColor(int x,int y,int type,byte color);
byte CheckMonsterProximity(int type,int proximity);
byte CheckMonsterLOS(int x, int y, int type, int radius);
byte CheckMonsterOuch(int x, int y, int type, byte flags);
byte CheckMonsterAge(int x, int y, int type, int frames, byte flags);

byte GoodguyInSight(Guy* me, Guy* goodguy, byte facing);
void SuckItUp(int x, int y, byte friendly);

Guy *GetGuyOfAIType(int type);

void FindMonsterBrain(int myx,int myy);
void FindMonsterCandle(int myx,int myy);

void SetPoisonFrames(Guy *g,word frames);
void SetIgniteFrames(Guy* g, word frames);
void SetFreezeFrames(Guy* g, word frames);
void SetWeakenFrames(Guy* g, word frames);
void SetSlownessFrames(Guy* g, word frames);
void SetMindControlFrames(Guy* g, word frames);
void SetStrengthFrames(Guy* g, word frames);
void SetSpeedFrames(Guy* g, word frames);

#endif
