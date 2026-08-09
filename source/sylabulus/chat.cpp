#include "chat.h"
#include "game.h"
#include "display.h"
#include "guy.h"
#include "particle.h"
#include "pause.h"
#include "bullet.h"

#include <stdio.h>
#include <stdlib.h>
#include "appdata.h"
#include "ioext.h"


#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

struct ChatLine
{
	std::string speaker;
	std::string text;
	word image;
};

struct ChatFile
{
	std::string name;
	std::vector<ChatLine> lines;
};

static std::vector<ChatLine> chatLines;
static int chatLine		= 0;
static byte chatState	= 0;
static int chatAnim		= 0;
static byte chatClosing = 0;
static sprite_set_t* chatSpr;

TASK(void) InitChat(const char* str, byte nosnd, byte mode)
{
	char* fname;
	char nm[64];

	std::string copy = str;
	fname = strtok(copy.data(), ",\n");

	if (!fname)
		CO_RETURN;

	sprintf(nm, "user/chat/%s", fname);
	printf("InitChat: %s\n", nm);

	chatSpr = new sprite_set_t("graphics/chat.jsp");
	auto f = AppdataOpen(nm);

	if (!f)
	{
		printf("Could not open chat file: %s\n", nm);
		CO_RETURN;
	}

	SdlRwStream stream(f.get());

	chatLines.clear();
	chatLine = 0;

	chatAnim = 0;
	chatClosing = 0;
	chatState = 1;

	char line[256];

	while (stream.getline(line, std::size(line)))
	{
		if (!line[0])
			continue;

		ChatLine newLine;
		newLine.image = -1;

		char* separator = strchr(line, ':');

		if (separator)
		{
			*separator = '\0';

			newLine.speaker = line;
			newLine.text = separator + 1;

			if (!newLine.text.empty() && newLine.text[0] == ' ')
				newLine.text.erase(0, 1);
		}
		else
		{
			newLine.speaker.clear();
			newLine.text = line;
		}

		// find portrait # -> chat.jsp
		size_t imageSeparator = newLine.text.find('|');

		if (imageSeparator != std::string::npos)
		{
			std::string imageNumber = newLine.text.substr(imageSeparator + 1);

			newLine.text =
				newLine.text.substr(0, imageSeparator);

			if (!newLine.text.empty() && newLine.text.back() == ' ')
				newLine.text.pop_back();

			newLine.image = atoi(imageNumber.c_str()); // image number
		}

		chatLines.push_back(newLine);
	}
	printf("Chat started: %d lines\n", (int)chatLines.size());

	if (chatLines.empty())
		CO_RETURN;

	chatState = 1;

	if (!nosnd)
		MakeNormalSound(SND_MESSAGE);
	SendMessageToGame(MSG_CHATNOW, 0);

	CO_RETURN;
}

TASK(byte) UpdateChat(MGLDraw* mgl)
{
	if (!chatState)
		CO_RETURN 0;

	const int animLength = 10;

	if (!chatClosing && chatAnim < animLength)
	{
		chatAnim++;
		CO_RETURN 1;
	}

	if (chatClosing)
	{
		if (chatAnim > 0)
		{
			chatAnim--;
			CO_RETURN 1;
		}

		chatState = 0;
		chatClosing = 0;
		chatLine = 0;
		chatLines.clear();
		delete chatSpr;
		CO_RETURN 0;
	}

	if (chatLine >= (int)chatLines.size())
	{
		chatClosing = 1;
		CO_RETURN 1;
	}

	byte c = GetControls();
	byte taps = GetTaps() | GetArrowTaps();

	if (taps && (c & (CONTROL_B1 | CONTROL_B2)))
	{
		if (chatLine + 1 >= (int)chatLines.size())
		{
			chatClosing = 1; // close communication
		}
		else
		{
			chatLine++; // advance!
		}

		MakeNormalSound(SND_MENUCLICK);
	}

	CO_RETURN 1;
}

void RenderChat(MGLDraw* mgl)
{
	if (!chatState)
		return;

	if (chatLines.empty())
		return;

	if (chatLine >= (int)chatLines.size())
		return;

	ChatLine& line = chatLines[chatLine];

	const int boxX = 16;
	const int finalY = 350;
	const int startY = 480;

	const int boxW = 608;
	const int boxH = 114;

	const int animLength = 10;

	int anim = chatAnim;

	if (anim < 0)
		anim = 0;

	if (anim > animLength)
		anim = animLength;

	int eased = animLength - ((animLength - anim) * (animLength - anim)) / animLength;

	int boxY = startY + (finalY - startY) * eased / animLength;

	// bg
	mgl->FillBox(boxX,boxY,boxX+boxW,boxY+boxH,32*3+4);

	// border
	mgl->Box(boxX,boxY,boxX+boxW,boxY+2,32*3+16);
	mgl->Box(boxX,boxY+boxH-2,boxX+boxW,boxY+boxH,32*3+16);
	mgl->Box(boxX,boxY,boxX + 2,boxY + boxH,32*3+16);
	mgl->Box(boxX + boxW-2,boxY,boxX+boxW,boxY+boxH,32*3+16);

	// portrait
	if (line.image >= 0)
		chatSpr->GetSprite(line.image)->Draw(boxX-44,boxY-128,mgl);

	// name
	if (!line.speaker.empty())
		Print(boxX+12,boxY+10,line.speaker.c_str(),0,2);

	// dialogue
	PrintRect(boxX+12,boxY+32,boxX+boxW-12,boxY+boxH-12,18,line.text.c_str(),1);

	// continue indicator
	if (!chatClosing)
		Print(boxX+boxW-130,boxY+boxH-18,"[Press FIRE/SPCL]",0,1);
}
