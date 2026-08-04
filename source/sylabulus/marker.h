#ifndef MARKER_H
#define MARKER_H

#include "bitflags.h"

#define MRK_NONE    0
#define MRK_PATH    1   //

#define MAX_MARKER 128

struct marker_t
{
    byte type; //
    byte x1,x2,y1,y2; // coordinates
    int value;
    int value2;
};

void InitMarkers(std::span<marker_t> list);

int NewMarker(byte x,byte y); // square
int NewMarker(byte x1,byte y1,byte x2,byte y2); // rectangle

int GetMarker(byte x,byte y);
int GetMarker(byte x1,byte y1,byte x2, byte y2);
marker_t *GetMarker(int i);
void DeleteMarker(int i);
void SlideMarkers(Map *map,int dx,int dy);

void AdjustMarkerCoords(marker_t *me,int dx,int dy);
bool CheckMarker(const marker_t& me);
void CheckMarkers(Map* map);

#endif
