#ifndef WORLDSELECT_H
#define WORLDSELECT_H

#include "mgldraw.h"
#include <string>
#include <cstddef>

TASK(byte) WorldSelectMenu(MGLDraw *mgl);

const char *WorldFName(void);
struct worldDesc_t
{
	char fname[64];
	char name[64];
	char author[64];
	float percentage;
	bool dimmed;
};

bool Scannable(const std::string& name);
float GetWorldPercentage(const char text[32]);

#endif
