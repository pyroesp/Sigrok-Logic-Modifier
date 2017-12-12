#include "ressource.h"
#include "arg.h"
#include "meta.h"
#include "data.h"
#include "keys.h"

/* TODO list :
	+ Read sigrok (unzipped) files
		- Metadata file
			> Needed for probes
		- Logic files
			> Hard coded reading first file for now
                > Done through argv
	+ Display data as waveform
		> Done, could use some refinement
            > refinement done
	+ Move waveform left to right
		- click & drag
		- left/right arrow keys
			> Done
    + Add goto next/previous rising/falling edge
        > Implemented on keys 1-2 and 4-5 of the keypad
	+ Zoom in & out of waveform
		- Mouse wheel
		- Up/down arrow keys
			> Done
	- Add dots on waveform
        > Done
	- Toggle state when data point is clicked with left mouse
        > Done
        > Added selection mode to toggle multiple points
	- Show timestamp of dot when hovering over with mouse
	- Save modified sigrok file
        > Done

	Optional:
		- Show sample rate
		- Show file size
		- ...
*/


int main (int argc, char *argv[]){
    int refresh = 1;
    int selection_enabled = 0;
    int quit = 0;
    int mouse_x = 0, mouse_y = 0, mouse_start_x = 0, mouse_start_y = 0;
	unsigned int zoom = 1;
    unsigned int keys = 0, oldkeys = 0;
    unsigned int mouse_button_flag = 0, old_mouse_button_flag = 0;
	SDL_Window *w = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Rect selection_window;
	SDL_Event e;
	SrData *d = NULL;
    Arg p;

	printf("Starting application : \n\n");
    p = arg_ProgramInput(argc, argv);
    if (p.input && p.metadata){
        printf("File : %s.\nMeta : %s.\n", p.input, p.metadata);
        printf("Trigger on : %d.\nStart at data point : 0x%08X.\n", p.trigger, p.data_offset);
        d = data_InitStruct(p.input, p.metadata);
        if (!d)
            return -1;
        d->data_idx_start = p.data_offset;
    }
    else
        return -1;

	float freq = (float)d->md->sample_freq;
	if (d->md->sample_freq_unit == 'M')
        freq = freq * 1000000.0;
    else if (d->md->sample_freq_unit == 'k')
        freq = freq * 1000.0;
	printf("The sample frequency is %d%cHz, for a file size of %d.\nThis file represents %.3f seconds of data.\n",
            d->md->sample_freq, d->md->sample_freq_unit, d->data_size, (float)d->data_size / freq);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;
	w = SDL_CreateWindow("Sigrok Logic Modifier v0.0",
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!w)
		return -1;

    renderer = SDL_CreateRenderer(w, -1, 0);
    SDL_SetRenderDrawColor(renderer, 0, 0x88, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    d->block_height = 100; // should be a parameter or defined, but I'm too lazy

	// Main loop
	while (!quit){

        // Mouse stuff
        // only when dots are shown
        if (d->show_dots){
            mouse_button_flag = SDL_GetMouseState(&mouse_x, &mouse_y);
            // toggle single data point
            if (mouse_button_flag & SDL_BUTTON(SDL_BUTTON_LEFT) && !(old_mouse_button_flag & SDL_BUTTON(SDL_BUTTON_LEFT))){
                data_ToggleDataFromCoord(d, mouse_x, mouse_y);
                mouse_start_x = mouse_x;
                mouse_start_y = mouse_y;
                refresh = 1;
            }

            // if selection not enabled and mouse position moved while left button is pressed
            // then enable selection
            if (!selection_enabled)
                if (mouse_button_flag & SDL_BUTTON(SDL_BUTTON_LEFT))
                    if (mouse_start_x != mouse_x && mouse_start_y != mouse_y)
                        selection_enabled = 1;

            // if selection enabled, set the rectangle size and position,
            // and draw the rectangle in the refresh screen code below
            if (selection_enabled){
                selection_window = (SDL_Rect){.x = mouse_start_x, .y = mouse_start_y,
                                              .w = mouse_x - mouse_start_x, .h = mouse_y - mouse_start_y};
                refresh = 1;
            }

            // if selection is active and left mouse button has been released,
            // then toggle all data points inside selection box
            if (selection_enabled && !(mouse_button_flag & SDL_BUTTON(SDL_BUTTON_LEFT))){
                data_ToggleMultipleDataFromRect(d, selection_window);
                selection_enabled = 0;
                refresh = 1; // refresh to make the box disappear
            }

            old_mouse_button_flag = mouse_button_flag;
        }

        // Keyboard stuff
        keys_ReadInput(&keys);
        if (keys_Pressed(keys, KEY_ESC)){
            quit = 1;
        }
        // use release & pressed for a rising edge detection
        if (!keys_Pressed(oldkeys, KEY_S) && keys_Pressed(keys, KEY_S)){ // save
            data_BitToByte(d); // convert probe data back to bytes
            rewind(d->f); // make sure the you're at the start of the file
            if (!d->f)
                return -1;
            printf("Saving... %d\n", fwrite((const void*)d->raw_data, sizeof(unsigned char), d->data_size, d->f));
        }
        if (!keys_Pressed(oldkeys, KEY_Z) && keys_Pressed(keys, KEY_Z)){
            refresh = 1;
            data_UndoToggleSingle(d);
        }
        if (!keys_Pressed(oldkeys, KEY_SHIFT | KEY_Z) && keys_Pressed(keys, KEY_SHIFT | KEY_Z)){
            refresh = 1;
            data_UndoToggleAll(d);
        }
        if (keys_Pressed(keys, KEY_LEFT)){ // move left x points
            refresh = 1;
            d->data_idx_start -= d->nb_points / 3;
            if (d->data_idx_start < 0)
                d->data_idx_start = 0;
        }
        if (keys_Pressed(keys, KEY_RIGHT)){ // move left x points
            refresh = 1;
            d->data_idx_start += d->nb_points / 3;
            if (d->data_idx_start > (int)(d->data_size - d->nb_points))
                d->data_idx_start = (d->data_size - d->nb_points);
        }
        if (!keys_Pressed(oldkeys, KEY_KP_PLUS) && keys_Pressed(keys, KEY_KP_PLUS)){ // zoom in
            refresh = 1;
            zoom <<= 1;
            if (zoom >= 0x80)
                zoom = 0x80;
        }
        if (!keys_Pressed(oldkeys, KEY_KP_MINUS) && keys_Pressed(keys, KEY_KP_MINUS)){ // zoom out
            refresh = 1;
            zoom >>= 1;
            if (zoom <= 0)
                zoom = 1;
        }
        if (keys_Pressed(keys, KEY_KP_2)){ // find next falling edge in probe
            refresh = 1;
            data_moveToNextFallingEdge(d, p.trigger);
        }
        if (keys_Pressed(keys, KEY_KP_5)){ // find next rising edge in probe
            refresh = 1;
            data_moveToNextRisingEdge(d, p.trigger);
        }
        if (keys_Pressed(keys, KEY_KP_1)){ // find previous falling edge in probe
            refresh = 1;
            data_moveToPreviousFallingEdge(d, p.trigger);
        }
        if (keys_Pressed(keys, KEY_KP_4)){ // find previous rising edge in probe
            refresh = 1;
            data_moveToPreviousRisingEdge(d, p.trigger);
        }

        oldkeys = keys;

        // Keyboard stuff
        while (SDL_PollEvent(&e) != 0){
            switch (e.type){
                case SDL_QUIT: // quit
                    quit = 1;
                    break;
            }
        }

        // display stuff
        if (refresh){
            SDL_SetRenderDrawColor(renderer, 0xEE, 0xEE, 0xEE, 0xFF);
            SDL_RenderClear(renderer);
            if (selection_enabled){
                SDL_SetRenderDrawColor(renderer, 0x00, 0x55, 0xFF, 0xFF);
                SDL_RenderDrawRect(renderer, &selection_window);
            }
            data_DrawLogic(renderer, d, zoom);
            data_DrawTimeHatchMarks(renderer, d, 20);
            data_DrawDisplayBar(renderer, d, 21, 10);
            SDL_RenderPresent(renderer);
            refresh = 0;
        }
        SDL_Delay(50);
	}

	SDL_DestroyWindow(w);
	SDL_Quit();
    data_FreeData(d);
    d = NULL;
    return 0;
}
