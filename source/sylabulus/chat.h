#ifndef CHAT_H
#define CHAT_H

#include "jamultypes.h"
#include "mgldraw.h"

typedef struct Convo
{
public:
	char line[180];
	byte action;
	word next;
} Convo;

TASK(void) InitChat(int x,int y);
TASK(byte) UpdateChat(MGLDraw *mgl);
void RenderChat(MGLDraw *mgl);

#endif
