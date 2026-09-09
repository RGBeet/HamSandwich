#include "tally.h"
#include "mgldraw.h"
#include "control.h"
#include "display.h"
#include "player.h"
#include "music.h"
#include "dialogbits.h"
#include "progress.h"
#include "editor.h"
#include "hiscore.h"
#include "goal.h"

#define CATEGORY_X	20
#define VALUE_X		280
#define RECORD_X	450
#define POINTS_X	620

enum {
	TALLY_BASE,
	TALLY_TIMEVAL,
	TALLY_TIMEPTS,
	TALLY_COMBOVAL,
	TALLY_COMBOPTS,
	TALLY_DSTRYPRC,
	TALLY_DSTRYMULT,
	TALLY_DSTRYPTS,
	TALLY_CANDLEMULT,
	TALLY_CANDLEPTS,
	TALLY_MODMULT,
	TALLY_MODPTS,
	TALLY_DAMAGEVAL,
	TALLY_PERFECTMULT,
	TALLY_PERFECTPTS,
	TALLY_TOTAL,
	NUM_TALLY
};

enum {
	TV_VALUE,
	TV_PERCENT,		// displayed as 0.00%
	TV_TIME,		// displayed as 00:00
	TV_MULT,		// displayed as x0.00
	TV_COMBO,		// displayed as x0
	TV_POINTS,		// displayed as +0
};

struct tally_t
{
	word x, y, tx, ty;
	char name[32];
	byte sprite;
	byte type;
	word vOffX, vOffY;
	float value, vDesired;
};

static tally_t tallies[NUM_TALLY];

tally_t defaultSetup[NUM_TALLY] = {
	{ // TALLY_BASE
		-280, 40 * 1, -280, 40 * 1,
		"Base Score",
		0, TV_VALUE, 0, 0, 0, 0
	},

	{ // TALLY_TIMEVAL
		-280, 40 * 2, -280, 40 * 2,
		"Time",
		0, TV_TIME, 0, 0, 0
	},

	{ // TALLY_TIMEPTS
		-280, 40 * 1, -280, 40 * 1,
		"\0",
		0, TV_POINTS, 0, 0, 0, 0
	},

	{ // TALLY_COMBOVAL
		-280, 40 * 3, -280, 40 * 3,
		"Combo",
		0, TV_COMBO, 0, 0, 0
	},

	{ // TALLY_COMBOPTS
		-280, 40 * 1, -280, 40 * 1,
		"\0",
		0, TV_POINTS, 0, 0, 0, 0
	},

	{ // TALLY_DSTRYPRC
		-280 + 120, 40 * 4, -280, 40 * 4,
		"Destruct",
		0, TV_PERCENT, 0, 0, 0
	},

	{ // TALLY_DSTRYMULT
		-280, 40 * 4, -280, 40 * 4,
		"\0",
		0, TV_MULT, 0, 0, 0
	},

	{ // TALLY_DSTRYPTS
		-280, 40 * 4, -280, 40 * 4,
		"\0",
		0, TV_POINTS, 0, 0, 0
	},

	{ // TALLY_CANDLEMULT
		-280, 40 * 7, -280, 40 * 7,
		"Candle",
		0, TV_MULT, 0, 0, 0
	},

	{ // TALLY_CANDLEPTS
		-280, 40 * 7, -280, 40 * 7,
		"\0",
		0, TV_POINTS, 0, 0, 0
	},

	{ // TALLY_MODMULT
		-280, 40 * 5, -280, 40 * 5,
		"Modifier",
		0, TV_MULT, 0, 0, 0
	},

	{ // TALLY_MODPTS
		-280, 40 * 5, -280, 40 * 5,
		"\0",
		0, TV_POINTS, 0, 0, 0
	},

	{ // TALLY_DAMAGEVAL
		-280, 40 * 6, -280, 40 * 6,
		"Dmg. Taken",
		0, TV_VALUE, 0, 0, 0
	},

	{ // TALLY_PERFECTMULT
		-280 + 120, 40 * 7, -280, 40 * 7,
		"\0",
		0, TV_MULT, 0, 0, 0
	},

	{ // TALLY_PERFECTPTS
		-280, 40 * 7, -280, 40 * 7,
		"\0",
		0, TV_POINTS, 0, 0, 0
	},

	{ // TALLY_TOTAL
		-280, 40 * 1, -280, 40 * 1,
		"Final Score",
		0, TV_VALUE, 0, 0, 0
	}
};

void SetTallyValue(int tally, int x, int y, float value)
{
	tallies[tally].tx = x;
	tallies[tally].ty = y;
	tallies[tally].vDesired = value;
}

static byte *backgd;
static byte curLine;
static int comboBonus;
static byte lineNum,skip;
static int baseScore,showBaseScore,finalScore,showFinalScore,points,totalCoins,showTotalCoins,bonusCoins,oldScore;
static char levelName[32];
static int tallyWait;
static float destructBonus,destructPct,perfectBonus,diffBonus,modifierBonus,candleBonus;
static score_t topTimes[3], topScores[3];

void InitTally(MGLDraw *mgl)
{
	int i;

	curLine=0;

	mgl->LoadBMP("graphics/profmenu.bmp");
	backgd=(byte *)malloc(640*480);

	for(i=0;i<480;i++)
		memcpy(&backgd[i*640],&mgl->GetScreen()[i*mgl->GetWidth()],640);

	memcpy(tallies, defaultSetup, sizeof(tally_t) * NUM_TALLY);

	baseScore=player.score;

	showBaseScore	= 0;
	showFinalScore	= 0;
	finalScore		= 0;

	points=player.score;
	lineNum=0;
	tallyWait=0;
	skip=0;

	// DESTRUCTION BONUS
	if (player.totalEnemies == 0)
	{
		destructPct = 1.0f;
	}
	else
	{
		if (player.enemiesSlain > player.totalEnemies)
			player.enemiesSlain = player.totalEnemies;
		destructPct = ((float)player.enemiesSlain / (float)player.totalEnemies);
		if (destructPct > 1.0f)
			destructPct = 1.0f;
	}
	destructBonus		=  0.5f + destructPct;
	destructPct			*= 100.0f;

	// COMBO BONUS
	if (player.bestCombo >= 2)
		comboBonus = 10 * player.bestCombo;
	else
		comboBonus = 0;

	// MODIFIER BONUS (bad modifiers increase, good modifiers decrease)
	modifierBonus		= 1.0f;
	tallies[TALLY_MODMULT].vDesired = modifierBonus;

	// CANDLE BONUS
	if (curMap->numCandles == 0)
	{
		candleBonus = 1.0f;
	}
	else
	{
		float candlePct = (float)player.candles / (float)curMap->numCandles;
		candleBonus = 1.0f + (candlePct * 0.5f);
	}
	tallies[TALLY_CANDLEMULT].vDesired = candleBonus;

	// PERFECTION BONUS
	if (player.perfect)
		perfectBonus = (player.totalEnemies > 1) ? 1.5f : 1.2f;
	else
		perfectBonus = 1.0f;
	tallies[TALLY_PERFECTPTS].vDesired = perfectBonus;

	// COIN BONUS
	totalCoins		= profile.progress.totalCoins - profile.progress.coinsSpent;
	showTotalCoins	= totalCoins;

	finalScore = player.score;
	printf("\nBase Score: IS %d POINTS.\n", finalScore);
	finalScore = (int)(finalScore - player.clock / 10);
	printf("+Time Penalty IS %d POINTS.\n", finalScore);
	finalScore = (int)((float)(finalScore * destructBonus));
	printf("+Destruct Bonus IS %d POINTS.\n", finalScore);
	finalScore = (int)((float)(finalScore * candleBonus));
	printf("+Candle Bonus IS %d POINTS.\n", finalScore);
	finalScore = (int)((float)(finalScore * modifierBonus));
	printf("+Modifier Bonus IS %d POINTS.\n", finalScore);
	finalScore = (int)((float)(finalScore * perfectBonus));
	printf("Final Score IS %d POINTS.\n", finalScore);

	showFinalScore = baseScore;
	SetTallyValue(TALLY_BASE, 60, 40, baseScore);
	SetTallyValue(TALLY_TOTAL, 60*1, 40 * (lineNum + 2), showFinalScore);

	mgl->LastKeyPressed();

	topTimes[0].score=9*60*60*30+99*30*60+59*30;
	GetTopTimes(topTimes,curWorld.map[player.levelNum]);
	topScores[0].score=0;
	GetTopScores(topScores, curWorld.map[player.levelNum]);

	if(profile.progress.bestCombo<player.bestCombo)
		profile.progress.bestCombo=player.bestCombo;

	if(!player.cheated)
	{
		if(player.bestCombo>player.levelProg->recordCombo)
		{
			player.gotRecords|=RECORD_COMBO;
			player.levelProg->recordCombo=player.bestCombo;
		}
		if(destructPct>player.levelProg->recordDestroy)
		{
			player.gotRecords|=RECORD_DESTROY;
			player.levelProg->recordDestroy=destructPct;
		}
		if(player.score>player.levelProg->recordBaseScore)
		{
			player.gotRecords|=RECORD_BASE;
			player.levelProg->recordBaseScore=player.score;
		}
	}

	if(profile.musicMode==MUSIC_OFFICIAL)
		StopSong();
	MakeNormalSound(SND_TALLY);
}

void ExitTally(void)
{
	if(!editing && verified)
		profile.progress.totalCoins=totalCoins+profile.progress.coinsSpent;
	free(backgd);
	if(!player.cheated)
		GoalTallyPage(finalScore,player.perfect,(player.enemiesSlain==player.totalEnemies),player.clock);
}

byte UpdateTally(int *lastTime,MGLDraw *mgl)
{
	char c;
	int i,j;

	if(*lastTime>TIME_PER_FRAME*5)
		*lastTime=TIME_PER_FRAME*5;

	while(*lastTime>=TIME_PER_FRAME)
	{
		mgl->Process();

		if(!skip)
		{

			tallyWait++;
		}
		else
		{

		}

		for (i = 0; i < NUM_TALLY; i++)
		{
			float diff = tallies[i].vDesired - tallies[i].value;
			float amount = fabsf(diff) / 4.0f;

			if (amount < 1.0f && diff != 0.0f)
				amount = 1.0f;

			if (diff > 0.0f)
			{
				tallies[i].value += amount;

				if (tallies[i].value > tallies[i].vDesired)
					tallies[i].value = tallies[i].vDesired;
			}
			else if (diff < 0.0f)
			{
				tallies[i].value -= amount;

				if (tallies[i].value < tallies[i].vDesired)
					tallies[i].value = tallies[i].vDesired;
			}

			int dx = tallies[i].tx - tallies[i].x;
			int dy = tallies[i].ty - tallies[i].y;

			int xmove = abs(dx) / 6;
			int ymove = abs(dy) / 6;

			if (xmove < 1 && dx != 0)
				xmove = 1;

			if (ymove < 1 && dy != 0)
				ymove = 1;

			if (dx > 0)
				tallies[i].x += xmove;
			else if (dx < 0)
				tallies[i].x -= xmove;

			if (dy > 0)
				tallies[i].y += ymove;
			else if (dy < 0)
				tallies[i].y -= ymove;
		}

		int pointdiff = 0;

		switch(lineNum)
		{
			case 0:	// base points + time
				if (tallyWait >= 30)
				{
					lineNum++;
					MakeNormalSound(SND_ITEMDROP);
					tallyWait = 0;

					oldScore = showFinalScore;
					showFinalScore -= (int)(player.clock/10);
					SetTallyValue(TALLY_TIMEVAL, 60*1, 40 * (lineNum + 1), player.clock);
					SetTallyValue(TALLY_TIMEPTS, 60*4, 40 * (lineNum + 1), showFinalScore-oldScore);

					SetTallyValue(TALLY_TOTAL, 60*1, 40 * (lineNum + 2), showFinalScore);
				}
				break;
			case 1:	// combo
				if (tallyWait >= 30)
				{
					lineNum++;
					MakeNormalSound(SND_ITEMDROP);
					tallyWait = 0;

					oldScore = showFinalScore;
					showFinalScore += comboBonus;
					SetTallyValue(TALLY_COMBOVAL, 60*1, 40 * (lineNum + 1), player.bestCombo);
					SetTallyValue(TALLY_COMBOPTS, 60*4, 40 * (lineNum + 1), showFinalScore-oldScore);

					SetTallyValue(TALLY_TOTAL, 60*1, 40 * (lineNum + 2), showFinalScore);
				}
				break;
			case 2:	// destruct % and bonus
				if(tallyWait>=30)
				{
					lineNum++;
					MakeNormalSound(SND_ITEMDROP);
					tallyWait = 0;
					destructBonus = 0.5f + destructPct / 100.0f;

					oldScore = showFinalScore;
					showFinalScore = (int)((float)showFinalScore * destructBonus);
					SetTallyValue(TALLY_DSTRYPRC, 60*1, 40 * (lineNum + 1), destructPct);
					SetTallyValue(TALLY_DSTRYMULT, 60*4, 40 * (lineNum + 1), destructBonus);
					SetTallyValue(TALLY_DSTRYPTS, 60*7, 40 * (lineNum + 1), showFinalScore-oldScore);

					SetTallyValue(TALLY_TOTAL, 60*1, 40 * (lineNum + 2), showFinalScore);
				}
				break;
			case 3:	// candle bonus
				if (tallyWait >= 30)
				{
					lineNum++;
					MakeNormalSound(SND_ITEMDROP);
					tallyWait = 0;

					oldScore = showFinalScore;
					showFinalScore = (int)((float)showFinalScore * candleBonus);
					SetTallyValue(TALLY_CANDLEMULT, 60 * 1, 40 * (lineNum + 1), candleBonus);
					SetTallyValue(TALLY_CANDLEPTS, 60 * 4, 40 * (lineNum + 1), showFinalScore - oldScore);

					SetTallyValue(TALLY_TOTAL, 60 * 1, 40 * (lineNum + 2), showFinalScore);
				}
			case 4:	// modifier bonus
				if (tallyWait >= 30)
				{
					lineNum++;
					MakeNormalSound(SND_ITEMDROP);
					tallyWait = 0;

					oldScore = showFinalScore;
					showFinalScore = (int)((float)showFinalScore * modifierBonus);
					SetTallyValue(TALLY_MODMULT, 60*1, 40 * (lineNum + 1), modifierBonus);
					SetTallyValue(TALLY_MODPTS, 60*4, 40 * (lineNum + 1), showFinalScore-oldScore);

					SetTallyValue(TALLY_TOTAL, 60*1, 40 * (lineNum + 2), showFinalScore);
				}
				break;
			case 5:	// perfect bonus
				if(tallyWait>=30)
				{
					lineNum++;
					MakeNormalSound(SND_ITEMDROP);
					tallyWait = 0;

					oldScore = showFinalScore;
					showFinalScore = (int)((float)(showFinalScore * perfectBonus));
					SetTallyValue(TALLY_DAMAGEVAL, 60*1, 40 * (lineNum + 1), player.damageTaken);
					SetTallyValue(TALLY_PERFECTMULT, 60*4, 40 * (lineNum + 1), perfectBonus);
					SetTallyValue(TALLY_PERFECTPTS, 60*7, 40 * (lineNum + 1), showFinalScore-oldScore);

					SetTallyValue(TALLY_TOTAL, 60*1, 40 * (lineNum + 2), showFinalScore);
				}
				break;
			case 6:	// end!
				if(tallyWait>=30)
				{
					lineNum++;
					MakeNormalSound(SND_ITEMDROP);
					tallyWait = 0;
				}
				break;
			case 7:	// all done, waiting for exit
				skip=1;
				break;
		}
		*lastTime-=TIME_PER_FRAME;
	}

	c=mgl->LastKeyPressed();

	if(c || GetTaps())
	{
		if(tallyWait<30)
			tallyWait=30;
		else
		{
			MakeNormalSound(SND_MENUSELECT);
			return 1;
		}
	}
	return 0;
}

static void TallyLine(byte n,int y,const char *category,const char *value,const char *points,const char *record,byte gotRec)
{
	char bright;

	if(n==255)
		bright=0;
	else if(lineNum>n)
		bright=0;
	else if(lineNum==n)
		bright=(tallyWait*2-30);
	else
		bright=-32;

	Print(CATEGORY_X,y,category,bright,2);
	Print(VALUE_X-GetStrLength(value,2),y,value,bright,1);
	if(gotRec && bright==0)
		Print(RECORD_X-GetStrLength(record,2),y-Random(2),record,bright-16+Random(32),2);
	else
		Print(RECORD_X-GetStrLength(record,2),y,record,bright,1);
	Print(POINTS_X-GetStrLength(points,2),y,points,bright,1);
}

static void Tally2Line(byte n,int y,const char *category,const char *value,const char *mult)
{
	char bright;

	if(lineNum>n)
		bright=0;
	else if(lineNum==n)
		bright=(tallyWait*2-30);
	else
		bright=-32;

	Print(CATEGORY_X,y,category,bright,2);
	Print(VALUE_X-GetStrLength(value,2),y,value,bright,2);
	Print(POINTS_X-GetStrLength(mult,2),y,mult,bright,2);
}

void CoinLine(byte n,int y,const char *title,const char *num)
{
	char bright;

	if(n==255)
		bright=0;
	else if(lineNum>n)
		bright=0;
	else if(lineNum==n)
		bright=(tallyWait*2-30);
	else
		bright=-32;

	Print(CATEGORY_X,y,title,bright,2);
	Print(VALUE_X-GetStrLength(num,2),y,num,bright,2);
}

void MakeTime(char *s,dword clock)
{
	sprintf(s,"%u:%05.2f",clock/(30*60),(float)((clock%(30*60))/30.0f));
}

void RenderTally(MGLDraw *mgl)
{
	int i;
	char s[32],s2[32],s3[32],top[32];

	for(i=0;i<480;i++)
		memcpy(&mgl->GetScreen()[i*mgl->GetWidth()],&backgd[i*640],640);

	for (i = 0;i < NUM_TALLY;i++)
	{
		switch (tallies[i].type)
		{
		case TV_VALUE:
			sprintf(s, "%d", (int)tallies[i].value);
			break;

		case TV_PERCENT:
			sprintf(s, "%0.1f%%", (float)tallies[i].value);
			break;

		case TV_TIME:
			MakeTime(s, (dword)tallies[i].value);
			break;

		case TV_MULT:
			sprintf(s, "(x %0.2f)", (float)tallies[i].value);
			break;

		case TV_COMBO:
			sprintf(s, "x %d", (int)tallies[i].value);
			break;

		case TV_POINTS:
			if((int)tallies[i].value > 0)
				sprintf(s, "(+%d pts.)", (int)tallies[i].value);
			else
				sprintf(s, "(%d pts.)", (int)tallies[i].value);
			break;

		default:
			sprintf(s, "%d", (int)tallies[i].value);
			break;
		}

		Print(tallies[i].x, tallies[i].y, tallies[i].name, 0, 2);
		Print(tallies[i].x+GetStrLength(tallies[i].name,2)+10, tallies[i].y+3, s, 0, 1);
	}

	//sprintf(s, "Line #%d, %d/30 Frames", lineNum, tallyWait);
}

//----------------

TASK(void) Tally(MGLDraw *mgl,const char *lvlName,byte countIt)
{
	byte done=0;
	int lastTime=1;

	strcpy(levelName,lvlName);

	InitTally(mgl);
	GetTaps();
	while(!done)
	{
		lastTime+=TimeLength();
		StartClock();

		done=UpdateTally(&lastTime,mgl);
		RenderTally(mgl);
		AWAIT mgl->Flip();

		if(!mgl->Process())
			done=1;
		EndClock();
	}

	ExitTally();
	SaveProfile();
}
