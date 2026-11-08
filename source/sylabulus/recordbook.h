#ifndef RECORDBOOK_H
#define RECORDBOOK_H

#include "mgldraw.h"

TASK(void) RecordBook(MGLDraw *mgl);
TASK(void) ModifierMenu(MGLDraw* mgl);

float CalcPlayPercent(void);

#endif
