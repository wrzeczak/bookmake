//------------------------------------------------------------------------------//
// BOOKMAKE -- A lightweight, bare-bones text editor focused on writing, not    //
// code editing. Built with NCURSES, taking cues from Vim.                      //
//------------------------------------------------------------------------------//

#include <stdio.h>

#include <ncurses.h>

//------------------------------------------------------------------------------

#define TARGET_WIDTH 120			// target width is 120 characters

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

//- utility functions ----------------------------------------------------------

// note: i'm pretty sure the lifetime of COLS is between initsrc() and endwin()
int width() {
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

// returns the amount of padding needed for the text, similar lifetime concerns
int padding() {
	int w = width();

	if(COLS <= TARGET_WIDTH) return 0;
	else return (COLS - TARGET_WIDTH) / 2;
}

//- init functions -------------------------------------------------------------

int curses_init() {
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho(); // controversial, this will be toggled when needed!

	refresh();

	return 0;
}

int init() {
	int curses_init_return = curses_init();

	return curses_init_return;
}

//------------------------------------------------------------------------------

int main(void) {
	int init_return = init();

	if(init_return != 0) {
		printf("INITIALIZATION ERROR: %d! Closing...\n", init_return);
		return init_return;
	}

	int w = width();
	int p = padding();
	int cols_was = COLS;

	getch();
	endwin();

	printf("Grabbed width: %d, COLS was %d, TARGET_WIDTH was %d, so padding was %d.\n", w, cols_was, TARGET_WIDTH, p);

	return 0;
}

