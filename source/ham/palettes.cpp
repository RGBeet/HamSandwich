#include "palettes.h"
#include "mgldraw.h"
#include <algorithm>

void WaterPalette(MGLDraw *mgl)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	for(int i=0; i<8; i++)
		for(int j=0; j<32; j++)
		{
			pal2[i*32+j] = pal[i*32+j];
			pal2[i*32+j].b = (byte) std::min((int) pal2[i*32+j].b + 4*40, 4*63);
		}

	mgl->SetSecondaryPalette(pal2);
}

void LavaPalette(MGLDraw *mgl)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	for(int i=0; i<8; i++)
		for(int j=0; j<32; j++)
		{
			pal2[i*32+j] = pal[i*32+j];
			pal2[i*32+j].r = (byte) std::min((int) pal2[i*32+j].r + 4*40, 4*63);
		}

	mgl->SetSecondaryPalette(pal2);
}

void DumbSidePalette(MGLDraw* mgl)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 32; j++)
		{
			const RGB& src = pal[i * 32 + j];
			RGB& dst = pal2[i * 32 + j];

			float r = src.r / 252.0f;
			float g = src.g / 252.0f;
			float b = src.b / 252.0f;

			float mx = std::max(r, std::max(g, b));
			float mn = std::min(r, std::min(g, b));
			float d = mx - mn;

			float h = 0.0f;
			float s = 0.0f;
			float v = mx;

			if (d != 0.0f)
			{
				s = d / mx;

				if (mx == r)
					h = 60.0f * fmod((g - b) / d, 6.0f);
				else if (mx == g)
					h = 60.0f * ((b - r) / d + 2.0f);
				else
					h = 60.0f * ((r - g) / d + 4.0f);

				if (h < 0)
					h += 360.0f;
			}

			// Complementary hue.
			h += 180.0f;
			if (h >= 360.0f)
				h -= 360.0f;

			// Adjust value of the blue/yellow complements.
			// Yellow -> blue: make blue brighter.
			if (h >= 180.0f && h < 270.0f)
				v = std::min(v * 1.20f, 1.0f);

			// Blue -> yellow: make yellow darker.
			else if (h >= 45.0f && h < 90.0f)
				v *= 0.6f;

			float c = v * s;
			float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
			float m = v - c;

			float nr, ng, nb;

			if (h < 60)
				nr = c, ng = x, nb = 0;
			else if (h < 120)
				nr = x, ng = c, nb = 0;
			else if (h < 180)
				nr = 0, ng = c, nb = x;
			else if (h < 240)
				nr = 0, ng = x, nb = c;
			else if (h < 300)
				nr = x, ng = 0, nb = c;
			else
				nr = c, ng = 0, nb = x;

			dst.r = (byte)((nr + m) * 252.0f);
			dst.g = (byte)((ng + m) * 252.0f);
			dst.b = (byte)((nb + m) * 252.0f);
		}

	mgl->SetSecondaryPalette(pal2);
}

void GreyPalette(MGLDraw* mgl)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	for (int i = 0; i < 256; ++i)
	{
		pal2[i] = pal[i & 31];
	}

	mgl->SetSecondaryPalette(pal2);
}

void DarkPalette(MGLDraw* mgl)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	for (int i = 0; i < 256; ++i)
	{
		pal2[i].r = (byte)(pal[i].r >> 1);
		pal2[i].g = (byte)(pal[i].g >> 1);
		pal2[i].b = (byte)(pal[i].b >> 1);
		pal2[i].a = 255;
	}
	pal2[175] = pal[175];
	pal2[255] = pal[255];

	mgl->SetSecondaryPalette(pal2);
}

void CyclePalette(MGLDraw *mgl, int c)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	for (int i = 0; i < 256; ++i)
	{
		pal2[i] = pal[(i + c) % 256];
	}

	mgl->SetSecondaryPalette(pal2);
}

void ReversePalette(MGLDraw *mgl)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	for(int i=0;i<8;i++)
		for(int j=0;j<32;j++)
		{
			pal2[i*32+j]=pal[i*32+(31-j)];
		}

	mgl->SetSecondaryPalette(pal2);
}

void GammaCorrect(MGLDraw *mgl, byte gamma)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	int r, g, b;
	for (int i = 0; i < 256; i++)
	{
		r = pal[i].r;
		g = pal[i].g;
		b = pal[i].b;
		r = (r * (gamma + 4)) / 4;
		g = (g * (gamma + 4)) / 4;
		b = (b * (gamma + 4)) / 4;
		if (r > 255)
			r = 255;
		if (g > 255)
			g = 255;
		if (b > 255)
			b = 255;
		pal2[i].r = r;
		pal2[i].g = g;
		pal2[i].b = b;
		pal2[i].a = 255;
	}

	mgl->SetSecondaryPalette(pal2);
}
