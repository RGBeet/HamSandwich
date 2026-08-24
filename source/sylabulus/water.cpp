#include "water.h"
#include "display.h"
#include "player.h"
#include "winpch.h"

#define WATER_WIDTH	(512)
#define WATER_HEIGHT (512)
#define WATERFIX (32)
#define WATERDAMPEN (31)

#define WATERBMP_WIDTH (256)
#define WATERBMP_HEIGHT (256)

short* water1, * water2;
static byte* waterbkgd;
int scroll;
int blorpX, blorpY, blorpTime;

enum WaterType {
	WBG_NORMAL,
	WBG_RUSHING,
	WBG_LAVA
};

byte waterType		= 0;
byte waterColor		= 3;
bool fancyWaterActive = true;

bool FancyWaterActive() {
	return true;
}

void InitWater(void)
{
	int i;
	byte* src;
	int width;

	scroll = 0;
	water1 = (short*)malloc(WATER_WIDTH * WATER_HEIGHT * sizeof(short));
	if (!water1)
		FatalError("out of memory!");
	water2 = (short*)malloc(WATER_WIDTH * WATER_HEIGHT * sizeof(short));
	if (!water2)
		FatalError("out of memory!");
	waterbkgd = (byte*)malloc(WATER_WIDTH * WATER_HEIGHT * sizeof(byte));
	if (!waterbkgd)
		FatalError("out of memory!");

	GetDisplayMGL()->LoadBMP("graphics/water.bmp");

	width = GetDisplayMGL()->GetWidth();
	src = GetDisplayMGL()->GetScreen();
	for (i = 0;i < WATER_WIDTH * WATER_HEIGHT;i++)
	{
		water1[i] = 0;
		water2[i] = 0;
		waterbkgd[i] = 32 * 3 + 5;// src[(i % WATER_WIDTH) + (i / WATER_WIDTH) * width];
	}
}

void ExitWater(void)
{
	free(water1);
	free(water2);
}

void SetupWater(int waterType)
{
	byte* src;
	int width, i, j;

	if (!curMap)
		return;

	printf("Water type is Type %d.", waterType);
	switch (waterType)
	{
		default:
		case MAP_WTR_WATER:
			GetDisplayMGL()->LoadBMP("graphics/water.bmp");
			waterColor = 3;
			break;
		case MAP_WTR_RAPIDS:
			GetDisplayMGL()->LoadBMP("graphics/rapid.bmp");
			waterColor = 3;
			break;
		case MAP_WTR_LAVA:
			GetDisplayMGL()->LoadBMP("graphics/lava.bmp");
			waterColor = 4;
			break;
	}

	width = GetDisplayMGL()->GetWidth();
	src = GetDisplayMGL()->GetScreen();
	for (j = 0;j < WATER_HEIGHT;j++)
		for (i = 0;i < WATER_WIDTH;i++)
		{
			water1[i + j * WATER_WIDTH] = 0;
			water2[i + j * WATER_WIDTH] = 0;
			waterbkgd[i + j * WATER_WIDTH] = src[(i % WATERBMP_WIDTH) + (j % WATERBMP_HEIGHT) * width];
		}
	blorpTime = 0;
}

inline short GetWaterBit(int x, int y)
{
	while (x < 0)
		x += WATER_WIDTH;
	while (x >= WATER_WIDTH)
		x -= WATER_WIDTH;
	while (y < 0)
		y += WATER_HEIGHT;
	while (y >= WATER_HEIGHT)
		y -= WATER_HEIGHT;

	return water1[x + y * WATER_WIDTH];
}

inline byte GetWaterBkgdBit(int x, int y)
{
	while (x < 0)
		x += WATER_WIDTH;
	while (x >= WATER_WIDTH)
		x -= WATER_WIDTH;
	while (y < 0)
		y += WATER_HEIGHT;
	while (y >= WATER_HEIGHT)
		y -= WATER_HEIGHT;

	return waterbkgd[x + y * WATER_WIDTH];
}

byte flip = 0;

void UpdateWater(void)
{
	int i, j;
	short* tmp;

	if (waterType == WBG_RUSHING)
	{
		scroll--;
		blorpY--;
	}

	if (scroll < 0)
		scroll += WATER_HEIGHT;

	if (waterColor == 4) // lava
	{
		// Lava moves slowly and has occasional large bubbles.
		if (Random(8) == 0)
		{
			WaterBlop(
				Random(WATER_WIDTH),
				Random(WATER_HEIGHT),
				(byte)(WATER_WIDTH / 8 + Random(WATER_WIDTH / 8))
			);
		}
	}
	else if (waterType == WBG_RUSHING)
	{
		for (i = 0; i < 40; i++)
			WaterBlop(
				Random(WATER_WIDTH),
				Random(WATER_HEIGHT),
				(byte)Random(WATER_WIDTH / 8)
			);
	}
	else
	{
		WaterBlop(
			Random(WATER_WIDTH),
			Random(WATER_HEIGHT),
			(byte)Random(WATER_WIDTH / 2)
		);
	}

	// DON'T change these values.
	// The simulation depends on this exact math.
	int neighborDiv = 2;
	int dampen = WATERDAMPEN;

	for (i = 0; i < WATER_WIDTH; i++)
		for (j = 0; j < WATER_HEIGHT; j++)
		{
			water2[i + j * WATER_WIDTH] =
				(
					GetWaterBit(i - 1, j) +
					GetWaterBit(i + 1, j) +
					GetWaterBit(i, j - 1) +
					GetWaterBit(i, j + 1)
					) / neighborDiv
				- water2[i + j * WATER_WIDTH];

			water2[i + j * WATER_WIDTH] =
				water2[i + j * WATER_WIDTH] * dampen / WATERFIX;
		}

	tmp = water1;
	water1 = water2;
	water2 = tmp;
}

byte WaterPixel(int x, int y)
{
	int camx, camy;
	short s, xofs, yofs;

	GetCamera(&camx, &camy);
	x += camx;
	y += camy;

	x /= 2;
	y /= 2;

	y += scroll;
	if (y < 0)
		y += WATER_HEIGHT;

	x = x % WATER_WIDTH;
	y = y % WATER_HEIGHT;

	xofs = GetWaterBit(x - 1, y) - GetWaterBit(x + 1, y);
	yofs = GetWaterBit(x, y - 1) - GetWaterBit(x, y + 1);

	int bgx = x + xofs / WATERFIX;
	int bgy = y + yofs / WATERFIX;

	// lava waves
	if (waterColor == 4)
	{
		int wave;

		wave =
			Sine(y * 2 + scroll) / 40 +
			Sine(y * 3 + scroll / 2) / 80;

		bgx += wave;

		if (bgx < 0)
			bgx += WATER_WIDTH;
		else if (bgx >= WATER_WIDTH)
			bgx -= WATER_WIDTH;
	}

	s = GetWaterBkgdBit(bgx, bgy) & 31;

	if (waterColor != 4) // normal water
	{
		s += xofs / WATERFIX;

		if (s < 0)
			s = 0;
		if (s > 31)
			s = 31;
	}
	else // lava
	{
		s += xofs / (WATERFIX * 8);
		s = 5 + (s * 16) / 31;

		if (s < 0)
			s = 0;
		if (s > 30)
			s = 30;
	}

	return (byte)s + 32 * waterColor;
}

void WaterRipple(int x, int y, short amt)
{
	int camx, camy;

	GetCamera(&camx, &camy);
	if ((x - camx + SCRWID/2) < 0 || (x - camx + SCRWID/2) > SCRWID ||
		(y - camy + SCRHEI/2) < 0 || (y - camy + SCRHEI/2) > SCRHEI * 2)
		return;

	x += SCRWID/2;
	y += SCRHEI/2;

	x /= 2;
	y /= 2;

	y += scroll;
	if (y < 0)
		y += WATER_HEIGHT;

	//	x=x-camx;
	//	y=y-camy;

	//	if(x<0 || y<0 || x>639 || y>479)
	//		return;	// no rippling from offscreen

		//x+=camx;
		//y+=camy;

	x = x % WATER_WIDTH;
	y = y % WATER_HEIGHT;

	water2[x + y * WATER_WIDTH] += amt;
	x -= 1;
	if (x < 0)
		x += WATER_WIDTH;
	water2[x + y * WATER_WIDTH] += amt / 2;
	x += 2;
	if (x >= WATER_WIDTH)
		x -= WATER_WIDTH;
	water2[x + y * WATER_WIDTH] += amt / 2;
	x--;
	if (x < 0)
		x += WATER_WIDTH;
	y++;
	if (y >= WATER_HEIGHT)
		y -= WATER_HEIGHT;
	water2[x + y * WATER_WIDTH] += amt / 2;
	y -= 2;
	if (y < 0)
		y += WATER_HEIGHT;
	water2[x + y * WATER_WIDTH] += amt / 2;
}

void WaterBlop(int x, int y, int width)
{
	int i, x2;
	short s;

	s = 1;
	for (i = x;i <= x + width;i++)
	{
		x2 = i;
		if (x2 > WATER_WIDTH)
			x2 -= WATER_WIDTH;
		water2[(x2 % WATER_WIDTH) + (y % WATER_HEIGHT) * WATER_WIDTH] += s;
		if ((i - x) < width / 2)
			s += WATERFIX * 3 / 4;
		else if (s > 1)
			s -= WATERFIX * 3 / 4;
	}
}
