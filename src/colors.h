//------------------------------------------------------------------------------//
// BOOKMAKE -- colors.h -- contains enums defining NCURSES colors pairs, the    //
// color_init() function, and some logic to set color pairs depending on termi- //
// nal support of color features.                                               //
//------------------------------------------------------------------------------//

#pragma once

#include <stdio.h>
#include <assert.h>

#include <ncurses.h>

//------------------------------------------------------------------------------

const int THEMENO = 4;   // number of themes defined in this enum
const int THEMEOFF = 16; // the offset of the themes in the enum

// colors enum
enum {
	// colors for the info bar
	MODE_EDIT_IFBP = 8,
	MODE_CMD_IFBP,
	// colors for theming, text editor background and foreground
	TT_DARK_MODE = 16,
	TT_LIGHT_MODE,
	TT_OCEAN_MODE,
	TT_PINK_MODE
};

#define COLOR_OCEAN 96
#define COLOR_GREY  97
#define COLOR_GRAY  97
#define COLOR_PINK  98

//------------------------------------------------------------------------------

// returns nothing because i don't even know how this would fail
int colors_init();

// debug function that prints every theme onto a window, likely interact
void print_themes(WINDOW * interact);

//------------------------------------------------------------------------------

// note: must be called before drawing anything with NCURSES
// TODO: make this tolerate terminals with no color support
int colors_init() {
	if(!has_colors()) return 20;
	if(!can_change_color()) return 21;

	start_color(); // <--- call first because of this line!

	// because i can't set them equal in the enum
	assert(THEMEOFF == TT_DARK_MODE);

	// mute/customize some of the colors
	init_color(COLOR_RED, 700, 0, 0);
	init_color(COLOR_WHITE, 1000, 1000, 1000);

	init_color(COLOR_OCEAN, 0, 0, 400);
	init_color(COLOR_GRAY,  100, 100, 100);
	init_color(COLOR_GREY,  100, 100, 100);
	init_color(COLOR_PINK,  1000, 800, 800);

	// define infobar color pairs
	init_pair(MODE_EDIT_IFBP, COLOR_RED, COLOR_WHITE);
	init_pair(MODE_CMD_IFBP, COLOR_BLUE, COLOR_WHITE);

	// define theme color pairs
	init_pair(TT_DARK_MODE, COLOR_WHITE, COLOR_GRAY);
	init_pair(TT_LIGHT_MODE, COLOR_BLACK, COLOR_WHITE);
	init_pair(TT_OCEAN_MODE, COLOR_WHITE, COLOR_OCEAN);
	init_pair(TT_PINK_MODE, COLOR_BLACK, COLOR_PINK);

	return 0; // all is well
}

//------------------------------------------------------------------------------

void print_themes(WINDOW * interact) {
	const char * debug_message = "This is text in this color theme!";

	for(int y = 0; y < THEMENO; y++) {
		wattron(interact, COLOR_PAIR(THEMEOFF + y));

		mvwprintw(interact, (y * 2) + 1, 1, "%s", debug_message);

		wattron(interact, A_BOLD);

		mvwprintw(interact, (y * 2) + 2, 1, "%s", debug_message);

		wattroff(interact, A_BOLD);
		wattroff(interact, COLOR_PAIR(16 + y));

		wrefresh(interact);
	}
}

