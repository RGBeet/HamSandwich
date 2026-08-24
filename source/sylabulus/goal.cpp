#include "goal.h"
#include "progress.h"
#include "editor.h"
#include "monster.h"
#include "player.h"
#include "bullet.h"
#include "shop.h"
#include "theater.h"
#include "gallery.h"
#include "steam.h"

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

	if(goal!=99)
	{
		t=0;
		for(i=0;i<100;i++)
			if(profile.progress.goal[i])
				t++;
		if(t>=99)
			CompleteGoal(99);
	}

	if(profile.progress.goal[goal])
		return;

	if(editing)
		return;

	if(tutorial)
		return;

	profile.progress.goal[goal]=1;
	Steam()->CompleteGoal(goal);
	coro::launch(std::bind(ShowGoalEarned, goal));

	if(shopping)
		SetupGalPix(curMap);
}

void GoalKilledSomebody(Guy *g,byte type,byte frozen)
{
	dword totalKills=0;
	int i;

	if(type==MONS_BOUAPHA)
	{
		if(profile.progress.kills[MONS_BOUAPHA]>=100)
			CompleteGoal(98);
	}

	if(totalKills>1000)
		CompleteGoal(46);
	if(totalKills>10000)
		CompleteGoal(47);
	if(totalKills>50000)
		CompleteGoal(48);
	if(totalKills>100000)
		CompleteGoal(49);

	if(player.combo>=20)
		CompleteGoal(10);
	if(player.combo>=100)
		CompleteGoal(11);
	if(player.combo>=500)
		CompleteGoal(12);

	totalKills=0;
	for(i=2;i<NUM_PROFILE_MONSTERS;i++)	// skip Bouapha
	{
		if(!(MonsterFlags(i,i)&(MF_INVINCIBLE|MF_NOHIT)) && !(MonsterTheme(i)&(MT_GOOD|MT_BITS)) &&
			(MonsterTheme(i)))
		{
			// only count those that can be hurt, aren't body parts, and aren't in the 'good' group
			if(!profile.progress.kills[i])
			{
				totalKills=1;
				break;
			}
		}
	}
	if(!totalKills)	// every one of them has been beaten!
		CompleteGoal(13);
}

void GoalTallyPage(int score,byte perfect,byte destruct,int time)
{
	if(tutorial)
		return;

	if(score>=5000)
		CompleteGoal(0);
	if(score>=20000)
		CompleteGoal(1);
	if(score==0)
		CompleteGoal(2);
	if(perfect)
		CompleteGoal(3);
	if(destruct)
		CompleteGoal(4);
	if(perfect && destruct)
		CompleteGoal(5);
	if(time<15*30)
		CompleteGoal(6);
	if(profile.playAs!=PLAY_BOUAPHA)
		CompleteGoal(50);
	if(profile.playAs==PLAY_SHROOM)
		CompleteGoal(51);

	if(profile.progress.totalCoins>=10000)
		CompleteGoal(81);
	if(profile.progress.totalCoins>=100000)
		CompleteGoal(82);
	if(profile.progress.totalCoins-profile.progress.coinsSpent>=25000)
		CompleteGoal(83);
}

void GoalTimeDist(void)
{
	if(profile.progress.totalTime>=30*60*60)
		CompleteGoal(14);
	if(profile.progress.totalTime>=30*60*60*10)
		CompleteGoal(15);
	if(profile.progress.footDistance>=168960)
		CompleteGoal(16);
	if(profile.progress.footDistance>=168960*100)
		CompleteGoal(17);
	if(profile.progress.raftDistance>=168960)
		CompleteGoal(18);
	if(profile.progress.driveDistance+profile.progress.cartDistance>=168960*5)
		CompleteGoal(19);
	if(profile.progress.underwaterTime>=30*60*60)
		CompleteGoal(60);
}

void GoalPurchase(void)
{
	if(AllPurchased())
		CompleteGoal(26);
}

void GoalWinLevel(void)
{
	dword d;

	d=CountLevelsPassed();
	CalcFinishedWorlds();

	if(d>=25)
		CompleteGoal(61);
	if(d>=100)
		CompleteGoal(62);
	if(d>=1000)
		CompleteGoal(63);
	if(profile.progress.finishedWorlds>0)
		CompleteGoal(64);
	if(profile.progress.finishedWorlds>=10)
		CompleteGoal(65);
	if(profile.progress.finishedWorlds>=50)
		CompleteGoal(66);
	if(profile.progress.finishedWorlds>=profile.progress.totalWorlds-1)
		CompleteGoal(67);
}

void GoalFire(void)
{
	if(profile.progress.hammersThrown>=5000)
		CompleteGoal(91);
	if(profile.progress.hammersThrown>=50000)
		CompleteGoal(92);
	if(profile.progress.rages>=50)
		CompleteGoal(93);
	if(profile.progress.rages>=500)
		CompleteGoal(94);
	if(profile.progress.shotsFired>=5000)
		CompleteGoal(96);
	if(profile.progress.shotsFired>=50000)
		CompleteGoal(97);
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
