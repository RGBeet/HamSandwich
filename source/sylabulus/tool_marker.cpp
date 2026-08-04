#include "tool_marker.h"
#include "dialogbits.h"
#include "editor.h"
#include "terrainedit.h"
#include "markeredit.h"
#include "marker.h"

MarkerTool::MarkerTool(void)
{
	lastX = -1;
	lastY = -1;
}

MarkerTool::~MarkerTool(void)
{
}

void MarkerTool::Update(int msx, int msy)
{
	if (msx < 380 || msy < 400 || msx>639 || msy>479)
		return;

	if (GetDisplayMGL()->MouseTap())
	{
	}

	if (GetDisplayMGL()->RMouseTap())
	{
	}
}

void MarkerTool::Render(int msx, int msy)
{

}

void MarkerTool::SetInk(void)
{
}

void MarkerTool::StartPlop(void)
{
	EditorGetTileXY(&lastX, &lastY);
	Map* m = EditorGetMap();

	if (lastX < 0 || lastY < 0 || lastX >= m->width || lastY >= m->height)
		return;

	int s = GetMarker((byte)lastX, (byte)lastY);
	if (s == -1)
	{
		s = NewMarker((byte)lastX, (byte)lastY);
		if (s == -1)
		{
			MakeNormalSound(SND_BOMBBOOM);
			return;
		}
	}
	MarkerEdit_Init(s);
	SetEditMode(EDITMODE_MARKER);
}

void MarkerTool::PlopOne(int x, int y)
{
}

void MarkerTool::Plop(void)
{
}

void MarkerTool::ShowTarget(void)
{
	static byte col = 0;
	col = 255 - col;

	auto [cx, cy] = GetCamera();

	auto [tileX, tileY] = EditorGetTileXY();

	int x1 = tileX * TILE_WIDTH - (cx - GetDisplayMGL()->GetWidth() / 2);
	int y1 = tileY * TILE_HEIGHT - (cy - GetDisplayMGL()->GetHeight() / 2);

	int x2 = x1 + TILE_WIDTH - 1;
	int y2 = y1 + TILE_HEIGHT - 1;

	DrawBox(x1, y1, x2, y2, col);
}

void MarkerTool::SuckUp(int x, int y)
{
}

void MarkerTool::StartErase(void)
{
	lastX = -1;
	lastY = -1;
	Erase();
}

void MarkerTool::Erase(void)
{
	auto [x, y] = EditorGetTileXY();
	Map* m = EditorGetMap();

	if (x != lastX || y != lastY)
	{
		if (mapTile_t* target = m->TryGetTile(x, y); target && target->select)
		{
			DeleteMarker(GetMarker((byte)x, (byte)y));
		}

		MakeNormalSound(SND_MENUCLICK);
		lastX = x;
		lastY = y;
	}
}

void MarkerTool::BrushUp(void)
{
}
