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

TASK(void) InitChat(const char* str, byte nosnd, byte mode);
TASK(byte) UpdateChat(MGLDraw *mgl);
void RenderChat(MGLDraw *mgl);

#endif
