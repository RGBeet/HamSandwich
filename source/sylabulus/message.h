#ifndef MESSAGE_H
#define MESSAGE_H

#include "display.h"

struct message_t
{
	int x,y;
	int dx,dy;
	int timer;
	int maxTime;
	int bright;
	char brightDir;
	char msg[32];
	byte priority;
};

void InitMessage(void);
void NewMessage(const char *txt,int maxTime,byte priority);
void NewBigMessage(const char *txt,int maxTime);
void UpdateMessage(void);
void RenderMessage(int clock);
byte NoRepeatNewMessage(const char *txt,int maxTime,byte priority);
const char *VariableMsg(const char *txt);

#endif
