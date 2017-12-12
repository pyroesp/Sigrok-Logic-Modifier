#ifndef _RESSOURCE_H
#define _RESSOURCE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>

#define SCREEN_HEIGHT 480
#define SCREEN_WIDTH 1280

#define UNDO_BUFFER 64

typedef struct{
	unsigned char total_probes;
	unsigned char *probe_used; // 1 used, 0 unused
	char **probe_name;

	unsigned char sample_freq;
	char sample_freq_unit;
}SrMetaData;

// Data coordinates on the window
// Makes it easier to toggle
typedef struct{
    unsigned int x, y;
}SrCoord;

typedef struct{
    SrMetaData *md; // data info

    FILE *f; // logic file
    unsigned char *raw_data; // raw data from file
    unsigned char **probe_data; // probes data
    int data_size; // size of data

    int data_idx_start; // start of the data to display
    int nb_points; // number of points to display on screen
    unsigned char show_dots; // 0 = no dots
    unsigned int block_height; // height of pulses
    SrCoord **pos; // position on screen of data point of a probe
    int *undo[UNDO_BUFFER]; // list of pointers to probe_data to undo the changes done

    SDL_Renderer *r; // renderer
}SrData;

#endif
