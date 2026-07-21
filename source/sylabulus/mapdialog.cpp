#include "mapdialog.h"
#include "editor.h"
#include "dialogbits.h"
#include "textdialog.h"
#include "specialedit.h"
#include "repair.h"
#include <algorithm>

#define WD_NORMAL	0
#define WD_NAME		1

#define MAX_MAPSHOW		18
static int mapPos;
static byte asking,yesNo;
static char question[64];

char mapnames[MAX_MAPSHOW][40];
char newmapname[34]="";
static byte mapNum;
world_t *world;
static byte mode;

void InitMapDialog(world_t *wrld,byte currentMap)
{
	int i;

	mode=WD_NORMAL;

	world=wrld;
	mapNum=currentMap;
	if(mapNum>=world->numMaps)
		mapNum=0;

	mapPos=(mapNum/MAX_MAPSHOW)*18;
	for(i=0;i<MAX_MAPSHOW;i++)
		mapnames[i][0]='\0';
	i=0;

	while(i<MAX_MAPSHOW)
	{
		sprintf(mapnames[i],"%02d: %s",i+mapPos,world->map[i+mapPos]->name);
		i++;
		if(i+mapPos>=world->numMaps)
			break;
	}

	asking=0;
}

void ExitMapDialog(void)
{
}

static const int dialogueColor			= (32 * 3);
static const char mapDialogueButton[][8] = {
	"New",
	"Copy",
	"Rename",
	"Delete",
	"Move Up",
	"Move Dn"
};

void RenderMapDialog(int msx, int msy, MGLDraw* mgl)
{
	int i;

	// Original dialog size
	const int dialogW = 390;
	const int dialogH = 320;

	// Center on screen
	const int left = (mgl->GetWidth() - dialogW) / 2;
	const int top = (mgl->GetHeight() - dialogH) / 2;
	const int right = left + dialogW;
	const int bottom = top + dialogH;

	// Main box
	mgl->FillBox(left, top, right, bottom, 8);
	mgl->Box(left, top, right, bottom, 31);

	// Map list
	const int listLeft = left + 2;
	const int listTop = top + 2;
	const int listRight = left + 322;
	const int listBottom = top + 260;

	mgl->Box(listLeft, listTop, listRight, listBottom, 16);
	mgl->FillBox(listLeft + 1, listTop + 1, listRight - 1, listBottom - 1, 0);

	for (i = 0; i < MAX_MAPSHOW; i++)
	{
		if (i + mapPos >= world->numMaps)
			break;

		int rowTop = listTop + 3 + i * 14;
		int rowBottom = rowTop + 12;

		if (i + mapPos == mapNum)
			mgl->FillBox(listLeft + 3, rowTop, listRight - 3, rowBottom, dialogueColor+6);

		Print(listLeft + 5, rowTop + 1, mapnames[i], 0, 1);

		if (msx >= listLeft + 2 &&
			msx <= listRight - 2 &&
			msy >= rowTop &&
			msy <= rowBottom)
		{
			mgl->Box(listLeft + 2, rowTop - 1, listRight - 2, rowBottom + 1, 31);
		}
	}

	const int nameTop = top + 262;

	mgl->Box(left + 2, nameTop, left + 322, nameTop + 14, 16);
	mgl->FillBox(left + 3, nameTop + 1, left + 321, nameTop + 13, 0);

	Print(left + 4, nameTop + 2, newmapname, 0, 1);

	// More Levels...
	const int moreLeft = left + 2;
	const int moreTop = top + 278;
	const int moreRight = left + 117;
	const int moreBottom = moreTop + 14;
	if (msx >= moreLeft &&
		msx <= moreRight &&
		msy >= moreTop &&
		msy <= moreBottom)
	{
		mgl->FillBox(moreLeft, moreTop, moreRight, moreBottom, dialogueColor+8);
	}
	mgl->Box(moreLeft, moreTop, moreRight, moreBottom, 31);
	Print(moreLeft + 2, moreTop + 2, "More Levels...", 0, 1);

	// Right hand buttons.
	const int buttonX = left + 330;
	const int buttonY = top + 12;
	for(i=0;i<6;i++)
	{
		RenderButtonImage(msx, msy, buttonX, buttonY + i * 17, 56, 15, mapDialogueButton[i]);
	}

	RenderButtonImage(msx, msy, buttonX, top + 300, 56, 15, "Exit");

	// Author name
	RenderButtonImage(msx, msy, left + 2, top + 300, 100, 15, "Author Name");

	PrintLimited(
		left + 106,
		top + 302,
		left + 298,
		world->author,
		0,
		1);

	// Confirmation dialogue
	if (asking)
	{
		const int cx = mgl->GetWidth() / 2;
		const int cy = mgl->GetHeight() / 2;

		mgl->FillBox(cx - 140, cy - 10, cx + 140, cy + 60, 8);
		mgl->Box(cx - 140, cy - 10, cx + 140, cy + 60, 31);

		CenterPrint(cx, cy + 4, question, 0, 1);

		RenderButtonImage(msx, msy, cx - 65, cy + 40, 30, 15, "Yes");
		RenderButtonImage(msx, msy, cx + 35, cy + 40, 30, 15, "No");
	}

	// Rename dialogue
	if (mode == WD_NAME)
		RenderTextDialog(msx, msy, mgl);
}

void MapDialogYes(void)
{
	int i;

	switch(asking)
	{
		case 1:
			// delete the map
			if(mapNum!=0)	// can't delete the original one
			{
				SwapTable table{world->numMaps};
				table.Delete(mapNum);
				RepairLevels(table);
				delete world->map[mapNum];
				for(i=mapNum+1;i<=world->numMaps-1;i++)
				{
					world->map[i-1]=world->map[i];
				}
				world->map[world->numMaps-1]=NULL;
				world->numMaps--;
				mapNum--;
				mapPos=(mapNum/MAX_MAPSHOW)*MAX_MAPSHOW;
				EditorSelectMap(mapNum);
			}
			asking=0;

			for(i=0;i<MAX_MAPSHOW;i++)
				mapnames[i][0]='\0';
			i=0;

			while(i<MAX_MAPSHOW)
			{
				sprintf(mapnames[i],"%02d: %s",i+mapPos,world->map[i+mapPos]->name);
				i++;
				if(i+mapPos>=world->numMaps)
					break;
			}
			break;
	}
}

byte MapDialogKey(char key)
{
	int len;

	if(mode==WD_NAME)
	{
		TextDialogKey(key);
		if(TextDialogCommand()==TM_OKAY)
		{
			strcpy(world->author,GetText());
			mode=WD_NORMAL;
		}
		else if(TextDialogCommand()==TM_CANCEL)
		{
			mode=WD_NORMAL;
		}
		return 1;
	}
	if(asking)
	{
		if(key==27 || key=='n' || key=='N')
		{
			yesNo=0;
			asking=0;
		}
		if(key=='y' || key=='Y')
		{
			MapDialogYes();
		}
		return 1;
	}
	else
	{
		if(key==27)	// esc
			return 0;

		if(key==8)	// backspace
		{
			if(strlen(newmapname)>0)
				newmapname[strlen(newmapname)-1]='\0';
			return 1;
		}

		if(key==13)	// enter
		{
			return 0;	// exit, why not?
		}

		if(key=='|')
			key='/';	// don't allow vertical pipes, needed for the level list

		if(strlen(newmapname)<30)
		{
			len=strlen(newmapname);
			newmapname[len]=key;
			newmapname[len+1]='\0';
		}
	}
	return 1;
}

void MapDialogMoreMaps(void)
{
	int i;

	mapPos+=MAX_MAPSHOW;

	if(mapPos>=world->numMaps)
		mapPos=0;

	for(i=0;i<MAX_MAPSHOW;i++)
		mapnames[i][0]='\0';
	i=0;

	while(i<MAX_MAPSHOW)
	{
		sprintf(mapnames[i],"%02d: %s",i+mapPos,world->map[i+mapPos]->name);
		i++;
		if(i+mapPos>=world->numMaps)
			break;
	}
}
byte MapDialogClick(int msx, int msy, MGLDraw* mgl)
{
	int i;

	const int dialogW	= 390;
	const int dialogH	= 320;

	const int left		= (mgl->GetWidth() - dialogW) / 2;
	const int top		= (mgl->GetHeight() - dialogH) / 2;

	const int listLeft	= left + 2;
	const int listTop	= top + 2;
	const int listRight = left + 322;

	const int buttonX	= left + 330;
	const int buttonY	= top + 12;

	if (mode == WD_NAME)
	{
		TextDialogClick(msx, msy);

		if (TextDialogCommand() == TM_OKAY)
		{
			strcpy(world->author, GetText());
			mode = WD_NORMAL;
		}
		else if (TextDialogCommand() == TM_CANCEL)
		{
			mode = WD_NORMAL;
		}

		return 1;
	}

	if (asking)
	{
		int cx = mgl->GetWidth() / 2;
		int cy = mgl->GetHeight() / 2;

		if (PointInRect(msx, msy,
			cx - 65,
			cy + 40,
			cx - 35,
			cy + 55))
		{
			MapDialogYes();
		}
		else if (PointInRect(msx, msy,
			cx + 35,
			cy + 40,
			cx + 65,
			cy + 55))
		{
			asking = 0;
			yesNo = 0;
		}

		return 1;
	}

	// Select map from list
	for (i = 0; i < MAX_MAPSHOW; i++)
	{
		if (i + mapPos >= world->numMaps)
			break;

		int rowTop = listTop + 3 + i * 14;
		int rowBottom = rowTop + 12;

		if (PointInRect(msx, msy,
			listLeft + 2,
			rowTop,
			listRight - 2,
			rowBottom))
		{
			mapNum = i + mapPos;
			strcpy(newmapname, world->map[mapNum]->name);
			EditorSelectMap(mapNum);
			return 1;
		}
	}

	// More Levels
	if (PointInRect(msx, msy,
		left + 2,
		top + 278,
		left + 117,
		top + 292))
	{
		MapDialogMoreMaps();
		return 1;
	}

	// New level
	if (PointInRect(msx, msy,
		buttonX,
		buttonY,
		buttonX + 56,
		buttonY + 15))
	{
		if (world->numMaps < MAX_MAPS)
		{
			world->numMaps++;
			world->map[world->numMaps - 1] = new Map(0, newmapname);

			mapNum = world->numMaps - 1;
			EditorSelectMap(mapNum);
			mapPos = (mapNum / MAX_MAPSHOW) * MAX_MAPSHOW;
		}
	}

	// Copy level
	if (PointInRect(msx, msy,
		buttonX,
		buttonY + 17,
		buttonX + 56,
		buttonY + 32))
	{
		if (world->numMaps < MAX_MAPS)
		{
			world->numMaps++;
			world->map[world->numMaps - 1] = new Map(world->map[mapNum]);

			mapNum = world->numMaps - 1;
			EditorSelectMap(mapNum);
			mapPos = (mapNum / MAX_MAPSHOW) * MAX_MAPSHOW;
		}
	}

	// Rename level
	if (PointInRect(msx, msy,
		buttonX,
		buttonY + 34,
		buttonX + 56,
		buttonY + 49))
	{
		SDL_strlcpy(
			world->map[mapNum]->name,
			newmapname,
			sizeof(world->map[mapNum]->name));
	}

	// Delete level
	if (PointInRect(msx, msy,
		buttonX,
		buttonY + 51,
		buttonX + 56,
		buttonY + 66))
	{
		asking = 1;
		sprintf(question,
			"Really delete '%s'?",
			world->map[mapNum]->name);

		return 1;
	}

	// Move up
	if (PointInRect(msx, msy,
		buttonX,
		buttonY + 68,
		buttonX + 56,
		buttonY + 83))
	{
		if (world->numMaps > 1)
		{
			Map* m;

			SwapTable table{ world->numMaps };

			if (mapNum == 0)
			{
				m = world->map[world->numMaps - 1];
				world->map[world->numMaps - 1] = world->map[mapNum];
				world->map[mapNum] = m;

				table.Swap(mapNum, world->numMaps - 1);
				mapNum = world->numMaps - 1;
			}
			else
			{
				m = world->map[mapNum - 1];
				world->map[mapNum - 1] = world->map[mapNum];
				world->map[mapNum] = m;

				table.Swap(mapNum, mapNum - 1);
				mapNum--;
			}

			mapPos = (mapNum / MAX_MAPSHOW) * MAX_MAPSHOW;
			EditorSelectMap(mapNum);
			RepairLevels(table);
		}
	}

	// Move down
	if (PointInRect(msx, msy,
		buttonX,
		buttonY + 85,
		buttonX + 56,
		buttonY + 100))
	{
		if (world->numMaps > 1)
		{
			Map* m;

			SwapTable table{ world->numMaps };

			if (mapNum == world->numMaps - 1)
			{
				m = world->map[0];
				world->map[0] = world->map[mapNum];
				world->map[mapNum] = m;

				table.Swap(mapNum, 0);
				mapNum = 0;
			}
			else
			{
				m = world->map[mapNum + 1];
				world->map[mapNum + 1] = world->map[mapNum];
				world->map[mapNum] = m;

				table.Swap(mapNum, mapNum + 1);
				mapNum++;
			}

			mapPos = (mapNum / MAX_MAPSHOW) * MAX_MAPSHOW;
			EditorSelectMap(mapNum);
			RepairLevels(table);
		}
	}

	// Author name
	if (PointInRect(msx, msy,
		left + 2,
		top + 300,
		left + 102,
		top + 315))
	{
		mode = WD_NAME;
		InitTextDialog("Enter World Author:", "", 31);
	}

	// Exit
	if (PointInRect(msx, msy,
		buttonX,
		top + 300,
		buttonX + 56,
		top + 315))
	{
		return 0; // does the exiting
	}

	if (mapPos >= world->numMaps)
		mapPos = 0;

	for (i = 0; i < MAX_MAPSHOW; i++)
		mapnames[i][0] = '\0';

	i = 0;

	while (i < MAX_MAPSHOW && i + mapPos < world->numMaps)
	{
		sprintf(
			mapnames[i],
			"%02d: %s",
			i + mapPos,
			world->map[i + mapPos]->name);

		i++;
	}

	return 1;
}

void MapDialogScroll(int msz)
{
	if (msz > 0)
	{
		mapPos = std::max(mapPos - msz, 0);
	}
	else if (msz < 0)
	{
		mapPos = std::min(mapPos - msz, world->numMaps - 1);
	}

	int i=0;
	while(i<MAX_MAPSHOW)
	{
		sprintf(mapnames[i],"%02d: %s",i+mapPos,world->map[i+mapPos]->name);
		i++;
		if(i+mapPos>=world->numMaps)
			break;
	}
}
