#include "winpch.h"
#include "monsterai.h"
#include "guy.h"
#include "sound.h"
#include "bullet.h"
#include "player.h"

// Handles hurt/death noise stuff for most enemies.
void Guy::HandleOuchNoises(int ouchSnd, int deathSnd)
{
	if(hp>0)
		MakeSound(ouchSnd,x,y,SND_CUTOFF|SND_RANDOM,1200);
	else
		MakeSound(deathSnd,x,y,SND_CUTOFF,1200);
}

// Start a new animation.
void Guy::StartNewAnimation(byte sequenceNew, word frameAdvance, byte action, int dxNew, int dyNew, int newReload, int sound)
{
	this->seq			= sequenceNew;
	this->frm			= 0;
	this->frmTimer		= 0;
	this->frmAdvance	= frameAdvance;
	this->action		= action;
	this->dx			= dxNew;
	this->dy			= dyNew;
	reload		= (newReload > 0) ? newReload : reload;

	if (sound > 0)
		MakeSound(sound, x, y, SND_CUTOFF, 1200);
}

// Start a new animation.
void Guy::StartNewAnimation(byte sequenceNew, word frameAdvance, byte action, int dxNew, int dyNew, int newReload, int sound)
{
	this->seq			= sequenceNew;
	this->frm			= 0;
	this->frmTimer		= 0;
	this->frmAdvance	= frameAdvance;
}

void Guy::StartAnimMove(word frameAdvance)
{
	if (this->seq == ANIM_MOVE)
		return;
	this->seq			= ANIM_MOVE;
	this->frm			= 0;
	this->frmTimer		= 0;
	this->frmAdvance	= frameAdvance;
}

void Guy::StartAnimIdle(word frameAdvance)
{
	if (this->seq == ANIM_IDLE)
		return;
	this->seq = ANIM_IDLE;
	this->frm = 0;
	this->frmTimer = 0;
	this->frmAdvance = frameAdvance;
}

int Guy::GetSpaceInFrontX(int amt)
{
	return x + Cosine(facing * 32) * amt;
}

int Guy::GetSpaceInFrontY(int amt)
{
	return y + Sine(facing * 32) * amt;
}

int Guy::GetFacingX()
{
	return Cosine(facing * 32);
}

int Guy::GetFacingY()
{
	return Sine(facing * 32);
}

void Guy::SetNewSpeed(int speed)
{
	dx = Cosine(facing * 32) * speed;
	dy = Sine(facing * 32) * speed;
}

// Flips the direction the Guy is facing.
void Guy::FlipFacing()
{
	this->facing=(this->facing+4)&7;
}

void Guy::DampenSpeed(int speed)
{
	Dampen(&dx, FIXAMT/speed);
	Dampen(&dy, FIXAMT/speed);
}

void Guy::ClampSpeed(int speed)
{
	Clamp(&dx, FIXAMT * speed);
	Clamp(&dy, FIXAMT * speed);
}

void Guy::AccelerateSpeed(int speed)
{
	dx += Cosine(facing * 32) * speed;
	dy += Sine(facing * 32) * speed;
}

void Guy::AccelerateToTargetSpeed(int speed, int smoothness)
{
	int targetDX = Cosine(facing * 32) * 8;
	int targetDY = Sine(facing * 32) * 8;
	dx += (targetDX - dx) / 8;
	dy += (targetDY - dy) / 8;
	Clamp(&dx, FIXAMT * speed);
	Clamp(&dy, FIXAMT * speed);
}

// Simple way to fire a bullet as a Guy.
void Guy::DoFireBullet(int bulletType, byte reloadFrames, int spaceInFront, int sound)
{
	FireBullet(x+Cosine(facing*32)*spaceInFront, y+Sine(facing*32)*spaceInFront, facing*32, bulletType, friendly);
	reload = reloadFrames;
	if (sound > 0)
		MakeSound(sound,x,y,SND_CUTOFF,1200);
}

void Guy::DoFireBulletAngled(int bulletType, byte angleOffset, int spaceInFront)
{
	FireBullet(x + Cosine(facing * 32) * spaceInFront, y + Sine(facing * 32) * spaceInFront, (facing * 32 + angleOffset)&255, bulletType, friendly);
}

// Returns whether animation is on the specified sequence and frame.
byte Guy::CheckSequenceFrame(byte sequence, byte frame, bool checkReload)
{
	byte seqFrame = (seq == sequence) && (frm == frame);
	return checkReload ? (seqFrame && this->reload == 0) : (seqFrame && this->frmTimer < 32);
}

// Returns whether animation is on the specified sequence and frame.
byte Guy::CheckSequenceFrames(byte sequence, byte frameMin, byte frameMax, bool checkReload)
{
	byte seqFrame = (seq == sequence) && (frm >= frameMin) && (frm <= frameMax);
	return checkReload ? (seqFrame && this->reload==0) : seqFrame;
}

// Checks the frame only, but ALSO checks whether reload is empty
// OR frameTimer is under 32 (which means it only fires ONCE!)
byte Guy::CheckFrame(byte frame, bool checkReload)
{
	byte framePass = (frm == frame);
	return checkReload ? (framePass && this->reload == 0) : (framePass && this->frmTimer < 32);
}

byte Guy::CheckTargetWithinReach(int range, Guy* target)
{
	Guy* targetReal = (target != nullptr ? target : goodguy); // if target not filled out, default to "goodguy"
	return targetReal ? RangeToTarget(this, targetReal) < (range * FIXAMT) : 0;
}

// Checks whether the target is within sight (uses LOS 
byte GuyCheckTargetWithinSight(Guy *me, Guy *target, Map *map)
{
	Guy* targetReal = (target != nullptr ? target : goodguy); // if target not filled out, default to "goodguy"
	return targetReal ? (GoodguyInSight(me, target, me->facing) && map->FindGuy(me->mapx, me->mapy, 8, targetReal)) : 0;
}

// Tells the guy to pick another direction. Can be used to reset the "direction timer".
byte Guy::PickRandomDirection(byte *newTimer, byte frames)
{
	byte newDirection = (byte)Random(8);
	facing = newDirection;
	if (newTimer)
		newTimer = &frames;
	return newDirection;
}

int GetModifiedRoll(int n)
{
	return n;
}

// Does a random roll, then returns whether the roll landed on zero. May be modified depending on roll type.
byte Guy::CheckRoll(int n, byte rollType)
{
	return Random(GetModifiedRoll(n)) == 0;
}

// Returns a random roll influenced by
int Guy::GetRoll(int n, byte rollType)
{
	return Random(GetModifiedRoll(n));
}

// Quickyl
Guy* Guy::TryAddBaby(Map* map, world_t* world, int type, int offx, int offy, int newReload)
{
	Guy* g;
	g = AddBaby(x+offx, y+offy, 0, type, this);

	if (g && (!g->CanWalk(g->x, g->y, map, world)))
		RemoveGuy(g);

	if (newReload)
		reload = newReload;

	return g;
}

// Transforms the guy into another guy of type "newType".
void Guy::DoTransform(int newType)
{
	this->type		= newType;
	this->aiType	= type;
	this->hp		= MonsterHP(this->type);
	this->maxHP		= this->hp;
	ham_strcpy(this->name, MonsterName(this->type));
	if (!this->friendly)
		player.enemiesSlain--;
}

// Tells the guy to walk around randomly. Commonly used for enemy AI.
void Guy::WalkAround()
{
	mind	= 1;
	mind1	= 1;
}

// Tells the guy to pick a new direction if the selected mind timer is depleted. Commonly used for enemy AI.
void Guy::TryGetNewDirection(byte* mindVal, byte timerSet, Guy* target, bool getBackOnTrack)
{
	Guy* targetReal = (target != nullptr) ? target : goodguy;

	if (--(*mindVal) > 0)
		return;

	if (getBackOnTrack)
	{
		if (targetReal && CheckRoll(3, ROLL_RANDWALK) == 0)
			mind = 0;
		else
			PickRandomDirection();
		*mindVal = Random(timerSet) + 1;
	}
	else
		PickRandomDirection(mindVal, Random(timerSet) + 1);
}

// Tells the guy to SPIN AROUND! Commonly used for death animations (e.g. Bouapha, Pengulon).
void Guy::Speen(byte dir, byte frameAdvance)
{
	this->facing		= (this->facing+dir)&7;
	this->frmAdvance	= frameAdvance;
}

void Guy::SelfDestruct(Map *map, world_t *world)
{
	this->hp = 1;
	this->GetShot(0, 0, 1, map, world);
}

void Guy::FaceMovement()
{
	if(this->dx>FIXAMT)
	{
		if(this->dy>FIXAMT)
			this->facing=1;
		else if(this->dy<-FIXAMT)
			this->facing=7;
		else
			this->facing=0;
	}
	else if(this->dx<-FIXAMT)
	{
		if(this->dy>FIXAMT)
			this->facing=3;
		else if(this->dy<-FIXAMT)
			this->facing=5;
		else
			this->facing=4;
	}
	else
	{
		if(this->dy>FIXAMT)
			this->facing=2;
		else if(this->dy<-FIXAMT)
			this->facing=6;
	}
}
