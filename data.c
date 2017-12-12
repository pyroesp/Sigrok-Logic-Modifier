#include "data.h"

#define DATA_VERTICAL_OFFSET (10)
#define SHOW_DOTS_MIN_SPACE_REQ (4)

#define RISING_EDGE(probe, idx) (!d->probe_data[probe][idx - 1] && d->probe_data[probe][idx])
#define FALLING_EDGE(probe, idx) (d->probe_data[probe][idx - 1] && !d->probe_data[probe][idx])

SrData* data_InitStruct(char *path_logicdata, char *path_metadata){
    unsigned int i;
    SrData *d;
    d = (SrData*)calloc(1, sizeof(SrData));
    if (!d)
        return NULL;
    d->md = meta_InitStruct();
    if (d->md == NULL)
        return NULL;

    d->f = fopen(path_logicdata, "rb+");
    if (!d->f)
        return NULL;

    i = 0;
    while (fgetc(d->f) != EOF)
        i++;
    if (!feof(d->f))
        return NULL;
    d->data_size = i;
    rewind(d->f);

    if (meta_GetData(path_metadata, d->md) == -1)
        return NULL;

    d->raw_data = (unsigned char*)malloc(sizeof(unsigned char) * d->data_size);
    if (!d->raw_data)
        return NULL;
    d->probe_data = (unsigned char**)malloc(sizeof(unsigned char*) * d->md->total_probes);
    d->pos = (SrCoord**)malloc(sizeof(SrCoord*) * d->md->total_probes);
    for (i = 0; i < d->md->total_probes; i++){
        d->probe_data[i] = (unsigned char*)calloc(d->data_size, sizeof(unsigned char));
        d->pos[i] = (SrCoord*)calloc(d->data_size, sizeof(SrCoord));
    }

    for (i = 0; i < UNDO_BUFFER; i++){
        d->undo[i] = NULL;
    }

    fread(d->raw_data, sizeof(unsigned char), d->data_size, d->f);
    data_ByteToBit(d);
    return d;
}

void data_ByteToBit(SrData *d){
    int point;
    unsigned char probe;
	unsigned char *data = d->raw_data;
    for (point = 0; point < d->data_size; point++, data++)
        for (probe = 0; probe < d->md->total_probes; probe++)
            if (d->md->probe_used[probe])
                d->probe_data[probe][point] = !!(*data & (1 << probe));
}

void data_BitToByte(SrData *d){
    int point;
    unsigned char probe;
    for (point = 0; point < d->data_size; point++){
        for (probe = 0; probe < d->md->total_probes; probe++){
            if (d->md->probe_used[probe]){
                if (d->probe_data[probe][point])
                    d->raw_data[point] |= (1 << probe);
                else
                    d->raw_data[point] &= ~(1 << probe);
            }
        }
    }
}


int data_moveToNextRisingEdge(SrData *d, unsigned char probe){
    int point;
    if (probe > d->md->total_probes)
        return -1;
    if (!d->md->probe_used[probe])
        return -1;
    for (point = d->data_idx_start + 1; !RISING_EDGE(probe, point) && point < (d->data_size - d->nb_points); point++);
    if (point < (d->data_size - d->nb_points))
        d->data_idx_start = point;
    return 0;
}

int data_moveToPreviousRisingEdge(SrData *d, unsigned char probe){
    int point;
    if (probe > d->md->total_probes)
        return -1;
    if (!d->md->probe_used[probe])
        return -1;
    if (d->data_idx_start == 0)
        return -1;
    for (point = d->data_idx_start - 1; !RISING_EDGE(probe, point) && point > 0; point--);
    if (point > 0)
        d->data_idx_start = point;
    return 0;
}


int data_moveToNextFallingEdge(SrData *d, unsigned char probe){
    int point;
    if (probe > d->md->total_probes)
        return -1;
    if (!d->md->probe_used[probe])
        return -1;
    for (point = d->data_idx_start + 1; !FALLING_EDGE(probe, point) && point < (d->data_size - d->nb_points); point++);
    if (point < (d->data_size - d->nb_points))
        d->data_idx_start = point;
    return 0;
}

int data_moveToPreviousFallingEdge(SrData *d, unsigned char probe){
    int point;
    if (probe > d->md->total_probes)
        return -1;
    if (!d->md->probe_used[probe])
        return -1;
    if (d->data_idx_start == 0)
        return -1;
    for (point = d->data_idx_start - 1; !FALLING_EDGE(probe, point) && point > 0; point--);
    if (point > 0)
        d->data_idx_start = point;
    return 0;
}

void data_ToggleMultipleDataFromRect(SrData *d, SDL_Rect r){
    unsigned char probe;
    int point;
    if (r.w < 0){
        r.x += r.w;
        r.w *= -1;
    }
    if (r.h < 0){
        r.y += r.h;
        r.h *= -1;
    }

    if (d->show_dots)
        for(point = d->data_idx_start; point < d->data_size && point < (d->data_idx_start + d->nb_points); point++)
            for (probe = 0; probe < d->md->total_probes; probe++)
                if (d->md->probe_used[probe])
#define CHECK_X ((r.x <= (int)d->pos[probe][point].x) && ((r.x + r.w) >= (int)d->pos[probe][point].x))
#define CHECK_Y ((r.y <= (int)d->pos[probe][point].y) && ((r.y + r.h) >= (int)d->pos[probe][point].y))
                    if (CHECK_X && CHECK_Y){
                        d->probe_data[probe][point] = !d->probe_data[probe][point];
                        data_AddToUndo(d, probe, point);
                    }
#undef CHECK_X
#undef CHECK_Y
}

void data_ToggleDataFromCoord(SrData *d, int mouse_x, int mouse_y){
    // A dot is 3x3 pixels with center point being the data point translated to [x:y]
    // By calculating the w and h we can increase the "toggle-box" with the zoom
    int point, probe;
    int w, h;
    w = (SCREEN_WIDTH / d->nb_points) / SHOW_DOTS_MIN_SPACE_REQ;
    h = w;
    if (d->show_dots)
        for (point = d->data_idx_start; point < d->data_size && point < (d->data_idx_start + d->nb_points); point++)
            for (probe = 0; probe < d->md->total_probes; probe++)
                if (d->md->probe_used[probe])
#define CHECK_X (((mouse_x - w) <= (int)d->pos[probe][point].x) && ((mouse_x + w) >= (int)d->pos[probe][point].x))
#define CHECK_Y (((mouse_y - h) <= (int)d->pos[probe][point].y) && ((mouse_y + h) >= (int)d->pos[probe][point].y))
                    if (CHECK_X && CHECK_Y){
                        d->probe_data[probe][point] = !d->probe_data[probe][point];
                        data_AddToUndo(d, probe, point);
                    }
#undef CHECK_X
#undef CHECK_Y
}


int data_DrawProbe(SrData *d, unsigned char probe, SDL_Renderer *r, unsigned int x, unsigned int y, unsigned int zoom){
    int point;
    float x_offset, y_offset;
    SDL_Rect rect;

    if (probe > d->md->total_probes)
        return -1;

    if (!d->md->probe_used[probe])
        return -1;

    x_offset = zoom;
    y_offset = d->block_height;

    for (point = d->data_idx_start; point < d->data_size && point < (d->data_idx_start + d->nb_points); point++){
        if (point > 0){
            if (RISING_EDGE(probe, point) || FALLING_EDGE(probe, point)){
                SDL_SetRenderDrawColor(r, 0x00, 0x00, 0x00, 0xFF);
                SDL_RenderDrawLine(r, (int)(x), (int)(y + y_offset - DATA_VERTICAL_OFFSET) - 1, (int)(x), (int)y + 1);
            }
        }
        if (d->probe_data[probe][point]){
            SDL_SetRenderDrawColor(r, 0x00, 0xCC, 0x11, 0xFF);
            SDL_RenderDrawLine(r, (int)x, (int)y, (int)(x + x_offset), (int)y);
            if (d->show_dots){
                rect = (SDL_Rect){.x = x - 1, .y = y - 1, .w = 3, .h = 3};
                SDL_RenderDrawRect(r, &rect);
            }
            d->pos[probe][point].y = y;
        }else{
            SDL_SetRenderDrawColor(r, 0xCC, 0x11, 0x00, 0xFF);
            SDL_RenderDrawLine(r, (int)x, (int)(y + y_offset - DATA_VERTICAL_OFFSET), (int)(x + x_offset), (int)(y + y_offset - DATA_VERTICAL_OFFSET));
            if (d->show_dots){
                rect = (SDL_Rect){.x = x-1, .y = y - 1 + y_offset - DATA_VERTICAL_OFFSET , .w = 3, .h = 3};
                SDL_RenderDrawRect(r, &rect);
            }
            d->pos[probe][point].y = y + y_offset - DATA_VERTICAL_OFFSET;
        }
        d->pos[probe][point].x = x;
        x += (unsigned int)x_offset;
    }

    return 0;
}

void data_DrawLogic(SDL_Renderer *r, SrData *d, unsigned int zoom){
    unsigned int probe;
    d->nb_points = SCREEN_WIDTH / zoom;
    d->show_dots = 0;
    if (zoom >= SHOW_DOTS_MIN_SPACE_REQ) // at least 4 pixels between each data point
        d->show_dots = 1;
    if (d->nb_points < 2)
        d->nb_points = 2;
    for (probe = 0; probe < d->md->total_probes; probe++)
        if (d->md->probe_used[probe])
            data_DrawProbe(d, probe, r, 0, d->block_height * probe + (SCREEN_HEIGHT / d->md->total_probes), zoom);
    return;
}

void data_DrawTimeHatchMarks(SDL_Renderer *r, SrData *d, unsigned char h){
    unsigned int i;
    float points_per_pixel = (float)d->data_size / (float)SCREEN_WIDTH;
    float time;
    float dummy1 = 0.99, dummy2 = 1.01;
    float freq = (float)d->md->sample_freq * ((d->md->sample_freq_unit == 'M')?1000000.0:(d->md->sample_freq_unit == 'k')?1000.0:1.0);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 0xFF);
    SDL_RenderDrawLine(r, 0, SCREEN_HEIGHT, 0, SCREEN_HEIGHT - h);
    for (i = 1; i < SCREEN_WIDTH; i++){
        SDL_SetRenderDrawColor(r, 0, 0, 0, 0xFF);
        time = ((float)i * points_per_pixel) / freq;
        if (time > dummy1 && time < dummy2){
            SDL_RenderDrawLine(r, i, SCREEN_HEIGHT, i, SCREEN_HEIGHT - h);
            dummy1 += 1.0;
            dummy2 += 1.0;
        }
    }
}

void data_DrawDisplayBar(SDL_Renderer *r, SrData *d, unsigned char w, unsigned char h){
    unsigned int x = 0;
    unsigned int y = SCREEN_HEIGHT - h;
    float x_offset = 0;
    SDL_Rect rect = (SDL_Rect){.x = x, .y = y, .w = SCREEN_WIDTH, .h = h};
    SDL_SetRenderDrawColor(r, 0, 0, 0, 0xFF);
    SDL_RenderFillRect(r, &rect);
    x_offset = ((float)(SCREEN_WIDTH-w) / (float)d->data_size);
    x_offset = x_offset * (float)d->data_idx_start + 0.5;
    rect = (SDL_Rect){.x = (int)x_offset, .y = y, .w = w, .h = h};
    SDL_SetRenderDrawColor(r, 0xAA, 0xAA, 0xAA, 0xFF);
    SDL_RenderFillRect(r, &rect);
}

void data_FreeData(SrData *d){
    unsigned int probe;
	for (probe = 0; probe < d->md->total_probes; probe++){
        free(d->probe_data[probe]);
        free(d->pos[probe]);
        d->probe_data[probe] = NULL;
        d->pos[probe] = NULL;
    }
	meta_FreeData(d->md);
    free(d->raw_data);
    d->probe_data = NULL;
    d->pos = NULL;
    d->raw_data = NULL;
	d->md = NULL;
	fclose(d->f);
}

void data_AddToUndo(SrData *d, int probe, int idx){
    int i;
    for (i = 0; i < UNDO_BUFFER; i++){
        if (d->undo[i] == NULL)
            break;
    }
    if (i == UNDO_BUFFER){ // undo buffer is full
        memmove((void*)&d->undo[0], (void*)&d->undo[1], sizeof(int*) * (UNDO_BUFFER - 1));
        i--;
    }
    d->undo[i] = (int*)&d->probe_data[probe][idx];
}

void data_UndoToggleSingle(SrData *d){
    int i;
    for (i = UNDO_BUFFER - 1; i >= 0; i--){
        if (d->undo[i] != NULL){
            (*d->undo[i]) = !(*d->undo[i]); //toggle the data point
            d->undo[i] = NULL; // remove from undo list
            break;
        }
    }
}

void data_UndoToggleAll(SrData *d){
    int i;
    for (i = UNDO_BUFFER - 1; i >= 0; i--){
        if (d->undo[i] != NULL){
            (*d->undo[i]) = !(*d->undo[i]); //toggle the data point
            d->undo[i] = NULL; // remove from undo list
        }
    }
}

void data_RemoveFromUndo(SrData *d){
    int i;
    // search for last idx put in the undo buffer
    for (i = UNDO_BUFFER - 1; d->undo[i] != NULL && i >= 0; i--);
    if (i >= 0)
        d->undo[i] = NULL;
}
