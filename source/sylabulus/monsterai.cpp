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
void Guy::StartNewAnimation(byte sequenceNew, byte frameAdvance, byte action, int dxNew, int dyNew, int newReload, int sound)
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

void Guy::StartAnimMove(byte frameAdvance)
{
	if (this->seq == ANIM_MOVE)
		return;
	this->seq			= ANIM_MOVE;
	this->frm			= 0;
	this->frmTimer		= 0;
	this->frmAdvance	= frameAdvance;
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

void Guy::DampenSpeed(int speed)
{
	Dampen(&dx, FIXAMT/speed);
	Dampen(&dy, FIXAMT/speed);
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
	return checkReload ? (seqFrame && this->reload==0) : seqFrame;
}

// Returns whether animation is on the specified sequence and frame.
byte Guy::CheckSequenceFrames(byte sequence, byte frameMin, byte frameMax, bool checkReload)
{
	byte seqFrame = (seq == sequence) && (frm >= frameMin) && (frm <= frameMax);
	return checkReload ? (seqFrame && this->reload==0) : seqFrame;
}

byte Guy::CheckTargetWithinReach(int range, Guy* target)
{
	Guy* targetReal = (target != nullptr ? target : goodguy); // if target not filled out, default to "goodguy"
	return targetReal ? RangeToTarget(this, targetReal) < (range * FIXAMT) : 0;
}

byte Guy::PickRandomDirection(byte *newTimer, byte frames)
{
	byte newDirection = (byte)Random(8);
	facing = newDirection;
	if (newTimer)
		newTimer = &frames;
	return newDirection;
}

byte Guy::CheckRoll(int n, byte rollType)
{
	return Random(n) == 0;
}

void Guy::TryAddBaby(Map* map, world_t* world, int type, int offx, int offy, int newReload)
{
	Guy* g;
	g = AddBaby(x+offx, y+offy, 0, type, this);

	if (g && (!g->CanWalk(g->x, g->y, map, world)))
		RemoveGuy(g);

	if (newReload)
		reload = newReload;
}

void Guy::DoTransform(int newType)
{
	this->type	= newType;
	this->type	= aiType;
	this->hp	= MonsterHP(this->type);
	this->maxHP = this->hp;
	ham_strcpy(this->name, MonsterName(this->type));
	if (!this->friendly)
		player.enemiesSlain--;
}
