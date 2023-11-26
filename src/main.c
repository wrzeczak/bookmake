//------------------------------------------------------------------------------//
// BOOKMAKE -- A lightweight, bare-bones text editor focused on writing, not    //
// code editing. Built with NCURSES, taking cues from Vim.                      //
//------------------------------------------------------------------------------//

#include <stdio.h>
#include <assert.h>

#include <ncurses.h>

//------------------------------------------------------------------------------

#define TARGET_WIDTH 120 // target width is 120 characters
#define MINIMUM_WIDTH 100 // minimum acceptable width

bool debrief = true; // set to false if post-execution debug info is not needed

// window states
enum {
	MODE_EDIT,
	MODE_CMD
};

// infobar color pair indicies
enum {
	MODE_EDIT_IFBP = 64, // Edit Mode InFoBarPair
	MODE_CMD_IFBP
};

//------------------------------------------------------------------------------

// --- utility functions ---
/*
returns the width of the text window -- might be the width of the console,
might be less! the ncurses window will consume the whole screen, but the
editing window will only expand to TARGET_WIDTH.
*/
int width(); // see above
int padding(); // returns the amount of padding the text window needs

// --- init functions ---
/*
calls all the init functions in order and does file management stuff,
configuring colors, etc.
calls curses_init() (for now, will call more as they are added...)
*/
int init(); // see above -- returns non-zero error code if fails
int curses_init(); // initializes curses -- who knew! -- returns error code
void colors_init(); // initalizes curses color pairs, see enum above

// --- edit-mode functions ---
void edit_mode_infobar(WINDOW * interact); // displays the edit-mode infobar

//- utility functions ----------------------------------------------------------

int width() {
	// note: pretty sure the lifetime of COLS is between initscr() and endwin()
	if(COLS < TARGET_WIDTH) return COLS;
	else return TARGET_WIDTH;
	/*
	   if the terminal is full-screen, which is what i imagine will be the use
	   for this program (certainly how i'll use it!) then COLS may well be
	   larger than TARGET_WIDTH; obviously, if it's smaller, then I will just
	   use COLS. put this into a new function so i don't need to write that
	   math out explicitly every time and risk screwing it up.
	*/
}

int padding() {
	if(COLS <= TARGET_WIDTH) return 0;
	else return (COLS - TARGET_WIDTH) / 2;
}

//- init functions -------------------------------------------------------------

int curses_init() {
	initscr();
	start_color();
	raw();
	keypad(stdscr, TRUE);
	noecho(); // controversial, this will be toggled when needed!

	if(COLS < MINIMUM_WIDTH) return 80;

	return 0;
}

// this init function returns nothing because i don't even know how this would fail
void colors_init() { // note: does not call start_color()!
	init_color(COLOR_RED, 700, 0, 0);
	init_color(COLOR_WHITE, 1000, 1000, 1000);
	init_pair(MODE_EDIT_IFBP, COLOR_RED, COLOR_WHITE);
}

int init() {
	int curses_return = curses_init();

	if(curses_return != 0) {
		printf("CURSES INITIALIZATION FAILED: %02d!\n", curses_return);
	}

	colors_init();

	assert(MINIMUM_WIDTH < TARGET_WIDTH); // just to be sure

	return curses_return;
}

//------------------------------------------------------------------------------

void edit_mode_infobar(WINDOW * interact) {
	wattron(interact, COLOR_PAIR(MODE_EDIT_IFBP));
	wattron(interact, A_BOLD);

	mvwprintw(interact, LINES - 2, 2, "EDIT MODE   ");

	wattroff(interact, A_BOLD);

	mvwprintw(interact, LINES - 2, 14, "File drawer -- currently empty :(            Press SHIFT + Q to exit!");
	wrefresh(interact);

	wattroff(interact, COLOR_PAIR(MODE_EDIT_IFBP));
}

//------------------------------------------------------------------------------

int main(void) {
	int init_return = init();
	refresh();

	if(init_return != 0) {
		printf("INITIALIZATION ERROR! Crashing and burning...\n");
		return init_return;
	}

	WINDOW * interact;

	interact = newwin(LINES, width(), 0, padding());
	box(interact, 0, 0);

	wrefresh(interact);
	refresh();

	bool program_should_exit = false;

	int state = MODE_EDIT;

	while(!program_should_exit) {
		if(state == MODE_EDIT) edit_mode_infobar(interact);

		int c = getch();

		if(c == (int) 'Q') program_should_exit = true;
	}

	//--------------------------------------------------------------------------

	// debrief vars

	bool had_color = has_colors();

	int cols_was = COLS;
	int lines_was = LINES;
	int colors_was = COLORS;
	int color_pairs_was = COLOR_PAIRS;

	int width_was = width();
	int padding_was = padding();

	//--------------------------------------------------------------------------

	// de-init functions
	delwin(interact);
	endwin();

	//--------------------------------------------------------------------------

	// toggleable debug debrief

	if(debrief) {
		printf("\n--- DEBRIEF ---\n\n");
		printf("The terminal %s in color mode.\n", had_color ? "ran" : "did not run");
		if(had_color) printf("The terminal supported %d COLORS and %d COLOR_PAIRS.\n", colors_was, color_pairs_was);
		printf("The numbers, Mason! COLS was %d, LINES was %d. Thusly, width was %d and padding %d, with a TARGET_WIDTH of %d and MINIMUM_WIDTH of %d.\n", cols_was, lines_was, width_was, padding_was, TARGET_WIDTH, MINIMUM_WIDTH);


		printf("\n--- DEBRIEF END ---\n\n");
	}

	return 0;
}

