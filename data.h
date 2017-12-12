#ifndef _DATA_H
#define _DATA_H

#include "ressource.h"
#include "meta.h"

SrData* data_InitStruct(char *path_logicdata, char *path_metadata);
// Deconstruct data to bits
void data_ByteToBit(SrData *d);
// Reconstruct probe data to bytes
void data_BitToByte(SrData *d);

int data_moveToNextRisingEdge(SrData *d, unsigned char probe);
int data_moveToPreviousRisingEdge(SrData *d, unsigned char probe);
int data_moveToNextFallingEdge(SrData *d, unsigned char probe);
int data_moveToPreviousFallingEdge(SrData *d, unsigned char probe);

void data_ToggleMultipleDataFromRect(SrData *d, SDL_Rect r);
// Collision box whether or not a data points position is the same as the mouse position increases
// with the zoom to make it easier to toggle
// this collision box is invisible to the user
void data_ToggleDataFromCoord(SrData *d, int mouse_x, int mouse_y);
int data_DrawProbe(SrData *d, unsigned char probe, SDL_Renderer *r, unsigned int x, unsigned int y, unsigned int zoom);
void data_DrawLogic(SDL_Renderer *r, SrData *d, unsigned int zoom);

void data_DrawTimeHatchMarks(SDL_Renderer *r, SrData *d, unsigned char h);
void data_DrawDisplayBar(SDL_Renderer *r, SrData *d, unsigned char w, unsigned char h);

void data_FreeData(SrData *d);

void data_AddToUndo(SrData *d, int probe, int idx);
void data_UndoToggleSingle(SrData *d);
void data_UndoToggleAll(SrData *d);
void data_RemoveFromUndo(SrData *d);

#endif // _DATA_H
