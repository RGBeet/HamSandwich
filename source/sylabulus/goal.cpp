#include "goal.h"
#include "progress.h"
#include "editor.h"
#include "monster.h"
#include "player.h"
#include "bullet.h"
#include "shop.h"
#include "gallery.h"

static const char goalDesc[][48]={
	"Rising Star", "beating Noh-Mann in the Demo",
	"Justice Imagined", "beating EDISON in the Demo",
	"Field Research", "scanning every monster in the Demo",
	"Cranium Mania", "... collecting 500 brains?! Yuck!",
	"Killer Combo", "getting a x20 combo",
	"Treasure Hunt", "buying everything in Triton's Treasures",
	"Anger Management", "raging 30 times",
	"Perfect Marks", "getting a Perfect bonus",
	"Marathon Man", "walking 5 miles in Bouapha's shoes",
	"Bullet Time", "beating a monster while time is stopped",
	"Deep Blue Shield", "die while using an Energy Shield",
	"Uber Eater", "eating 9,000 calories",
	"Ice To Meet You", "freezing five monsters at once",
	"Easy as Pumpkin Pie", "beating a keychain level without taking damage",
	"Around the World", "getting 100% on all included worlds",
	"Marshmallow Head", "completing a Campaign with all modifiers",
};

void PrintGoalInfo(int x,int y,byte goal)
{
	char s[128];

	sprintf(s,"Goal #%03d: ",goal+1);

	if(profile.progress.goal[goal])
	{
		strcat(s,goalDesc[goal*2]);
	}
	else
		strcat(s,"???????");

	Print(x,y,s,0,2);
	if(profile.progress.goal[goal])
	{
		sprintf(s,"Earned for %s.",goalDesc[goal*2+1]);
		PrintGlowRect(x+30,y+16,620,y+20+60,18,s,2);
	}
}

void CompleteGoal(byte goal)
{
	// reworking
	return;

	int i,t;

	if(profile.progress.goal[goal])
		return;

	if(editing || tutorial)
		return;

	profile.progress.goal[goal]=1;
	coro::launch(std::bind(ShowGoalEarned, goal));

	if(shopping)
		SetupGalPix(curMap);
}

void GoalKilledSomebody(Guy *g,byte type,byte frozen)
{
	// add stuff here later
}

void GoalTallyPage(int score,byte perfect,byte destruct,int time)
{
	if(tutorial)
		return;
	// add stuff here later
}

void GoalTimeDist(void)
{
	// add stuff here later
}

void GoalPurchase(void)
{
	// add stuff here later
}

void GoalWinLevel(void)
{
	// add stuff here later
}

void GoalFire(void)
{
	// add stuff here later
}

// -------------------------------------------------
static byte noKeyTime;

void InitShowGoal(MGLDraw *mgl,byte num)
{
	char txt[64];

	noKeyTime=60;

	MakeNormalSound(SND_ALLCANDLE);
	GetDisplayMGL()->LoadBMP("graphics/galgoal.bmp");

	CenterPrint(320,110,"Gallery Goal Complete!",0,0);
	sprintf(txt,"\"%s\"",goalDesc[num*2]);
	CenterPrint(320,220,txt,0,0);
	sprintf(txt,"Earned for %s.",goalDesc[num*2+1]);
	CenterPrint(320,340,txt,-32,1);

	GetTaps();
	GetArrowTaps();
	mgl->LastKeyPressed();
}

void ExitShowGoal(void)
{
	GetDisplayMGL()->ClearScreen();
	//GetDisplayMGL()->Flip();  // TODO: is this needed?

	RestoreGameplayGfx();
}

byte UpdateShowGoal(int *lastTime,MGLDraw *mgl)
{
	byte t;
	char k;

	k=mgl->LastKeyPressed();
	t=GetTaps()|GetArrowTaps();

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();
		*lastTime-=TIME_PER_FRAME;
		if(noKeyTime)
			noKeyTime--;
	}

	if(!noKeyTime)
	{
		if(k==27)
			return 1;

		if(t&ANY_BUTTONS)
			return 1;
	}

	return 0;
}

void RenderShowGoal(MGLDraw *mgl)
{
//	int i;

//	mgl->Flip();

	//for(i=0;i<480;i++)
	//	memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&image[i*640],640);
}

TASK(void) ShowGoalEarned(byte num)
{
	byte done=0;
	int lastTime=1;

	InitShowGoal(GetDisplayMGL(),num);

	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();
		done=UpdateShowGoal(&lastTime,GetDisplayMGL());
		RenderShowGoal(GetDisplayMGL());

		AWAIT GetDisplayMGL()->Flip();

		if(!GetDisplayMGL()->Process())
			done=1;
		EndClock();
	}

	ExitShowGoal();
}
