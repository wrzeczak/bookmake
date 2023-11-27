//------------------------------------------------------------------------------//
// BOOKMAKE -- A lightweight, bare-bones text editor focused on writing, not    //
// code editing. Built with NCURSES, taking cues from Vim.                      //
//------------------------------------------------------------------------------//

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <ncurses.h>

#include "colors.h"

//------------------------------------------------------------------------------

#define TARGET_WIDTH 120 // target width is 120 characters
#define MINIMUM_WIDTH 100 // minimum acceptable width

bool debrief = true; // set to false if post-execution debug info is not needed

// window states
enum {
	MODE_EDIT,
	MODE_CMD
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
// void colors_init(); // initalizes curses color pairs, defined in colors.h

// --- infobar functions ---
void edit_mode_infobar(WINDOW * interact, bool bold, bool italic, bool underline);    // displays the edit-mode infobar
void cmd_mode_infobar(WINDOW * interact);     // same as above in command mode
void clear_infobar(WINDOW * interact);		  // sets the infobar to blank
void remove_infobar(WINDOW * interact);		  // removes the infobar

// --- cursor functions ---
int cursor_back(int * cxp, int * cyp);    // returns 1 if BOF
int cursor_forward(int * cxp, int * cyp); // returns 1 if EOF
int cursor_newline(int * cxp, int * cyp); // returns 1 if EOF

void cursor_pos(int cx, int cy); // render a thing showing cursor position

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
	raw();
	keypad(stdscr, TRUE);
	noecho(); // controversial, this will be toggled when needed!

	if(COLS < MINIMUM_WIDTH) return 80;

	return 0;
}


int init() {
	int final_return = 0;
	int curses_return = curses_init();

	if(curses_return != 0) {
		printf("CURSES INITIALIZATION FAILED: %02d!\n", curses_return);
		final_return++;
	}

	// from colors.h
	int colors_return = colors_init();

	if(colors_return != 0) {
		printf("COLOR INITIALIZATION FAILED: %02d!\n", colors_return);
		final_return++;
	}

	assert(MINIMUM_WIDTH < TARGET_WIDTH); // just to be sure

	return final_return;
}

//------------------------------------------------------------------------------

void clear_infobar(WINDOW * interact) {
	wattron(interact, COLOR_PAIR(MODE_CLEAR_IFBP));

	char infill[width() - 5 + 1]; // -4 for the side-to-side padding, +1 for '\0'

	memset(infill, ' ', width() - 4);

	infill[width() - 5 + 1] = '\0';

	mvwprintw(interact, LINES - 2, 2, "%s", infill);

	wattroff(interact, COLOR_PAIR(MODE_CLEAR_IFBP));
}

void edit_mode_infobar(WINDOW * interact, bool bold, bool italic, bool underline) {
	wattroff(interact, A_ITALIC);
	wattroff(interact, A_BOLD);
	wattroff(interact, A_UNDERLINE);

	clear_infobar(interact);
	box(interact, 0, 0);

	wattron(interact, COLOR_PAIR(MODE_EDIT_IFBP));
	wattron(interact, A_BOLD);

	mvwprintw(interact, LINES - 2, 2, "[F1] EDIT MODE   ");

	wattroff(interact, A_BOLD);

	const char * unpadded_message = "File draw -- currently empty :(";
	int padded_message_len = width() - 22;

	char message[256];

	snprintf(message, padded_message_len, "%-*s", padded_message_len, unpadded_message);

	mvwprintw(interact, LINES - 2, 21, "%s", message);
	wrefresh(interact);

	if(bold) wattron(interact, A_BOLD);
	mvwaddch(interact, LINES - 2, width() - 5, 'B');
	wattroff(interact, A_BOLD);
	if(italic) wattron(interact, A_ITALIC);
	mvwaddch(interact, LINES - 2, width() - 4, 'I');
	wattroff(interact, A_ITALIC);

	if(underline) wattron(interact, A_UNDERLINE);
	mvwaddch(interact, LINES - 2, width() - 3, 'U');
	wattroff(interact, A_UNDERLINE);

	// mvwaddch(interact, LINES - 2, width() - 2, ' ');

	wattroff(interact, COLOR_PAIR(MODE_EDIT_IFBP));
	if(italic) wattron(interact, A_ITALIC);
	if(underline) wattron(interact, A_UNDERLINE);
	if(bold) wattron(interact, A_BOLD);
}

void cmd_mode_infobar(WINDOW * interact) {
	clear_infobar(interact);
	box(interact, 0, 0);

	wattron(interact, COLOR_PAIR(MODE_CMD_IFBP));
	wattron(interact, A_BOLD);

	mvwprintw(interact, LINES - 2, 2, "[F2] COMMAND MODE   ");

	wattroff(interact, A_BOLD);

	const char * unpadded_message = "SHIFT + Q to exit. F1 to enter edit mode. Other commands not implemented.";
	int padded_message_len = width() - 22;

	char message[256];

	snprintf(message, padded_message_len, "%-*s", padded_message_len, unpadded_message);

	mvwprintw(interact, LINES - 2, 21, "%s", message);
	wrefresh(interact);

	wattroff(interact, COLOR_PAIR(MODE_CMD_IFBP));
}

/*
void remove_infobar(WINDOW * interact) {
	(void*) interact;
	return;
}
*/

//------------------------------------------------------------------------------

// moves the cursor back, up a line if necessary, and returns 1 if reaches the
// beginning of the file
int cursor_back(int * restrict cxp, int * restrict cyp) {
	assert(cxp != cyp);

	if(*(cxp) == 2 && *(cyp) < 2) return 1;

	if(*(cxp) <= 2 && *(cyp) >= 2) {
		*(cxp) = width() - 2;
		*(cyp) -= 1;
	} else {
		*(cxp) -= 1;
	}

	return 0;
}

// moves the cursor forward, down a line if needed, and returns 1 if it reaches
// the end of the file (bottom of the screen)
int cursor_forward(int * restrict cxp, int * restrict cyp) {
	assert(cxp != cyp);

	if(*(cyp) >= LINES - 2) return 1;

	if(*(cxp) >= width() - 3 && *(cyp) < LINES - 2) {
		*(cxp) = 2;
		*(cyp) += 1;
	} else {
		*(cxp) += 1;
	}

	return 0;
}

int cursor_newline(int * restrict cxp, int * restrict cyp) {
	assert(cxp != cyp);

	if(*(cyp) >= LINES - 3) return 1;

	*(cxp) = 1;
	*(cyp) += 1;

	return 0;
}

void cursor_pos(int cx, int cy) {
	mvwprintw(stdscr, 1, 1, "[ %03d, %03d ]", cx, cy);
	refresh();
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

	attron(COLOR_PAIR(TT_DARK_MODE));
	wattron(interact, COLOR_PAIR(TT_DARK_MODE));

	touchwin(interact);
	touchwin(stdscr);

	box(interact, 0, 0);

	clear_infobar(interact);
	wrefresh(interact);
	refresh();


	bool program_should_exit = false;

	int state = MODE_EDIT;

	// cursor position storage
	int cx = 1;
	int cy = 1;

	cursor_pos(cx, cy);
	int c = 0;

	bool italic = false;
	bool underline = false;
	bool bold = false;

	while(!program_should_exit) {
		switch(state) {
			case MODE_EDIT:
				edit_mode_infobar(interact, bold, italic, underline);
				wrefresh(interact);
				break;
			case MODE_CMD:
				cmd_mode_infobar(interact);
				wrefresh(interact);
				break;
			default:
				cmd_mode_infobar(interact);
				break;
		}

		c = getch();
		cursor_pos(cx, cy);

		// input handler
		switch(c) {
			case (int) 'Q':
				if(state == MODE_CMD) program_should_exit = true;
				break;
			case KEY_F(1):
				state = MODE_EDIT;
				break;
			case KEY_F(2):
				state = MODE_CMD;
				break;
			case ('b' & 0x1f):
				if(state == MODE_EDIT) {
					if(bold) {
						bold = false;
						wattroff(interact, A_BOLD);
					} else {
						bold = true;
						wattron(interact, A_BOLD);
					}
				}
				break;
			case ('u' & 0x1f):
				if(state == MODE_EDIT) {
					if(underline) {
						underline = false;
						wattroff(interact, A_UNDERLINE);
					} else {
						underline = true;
						wattron(interact, A_UNDERLINE);
					}
				}
				break;
			case ('i' & 0x1f):
				if(state == MODE_EDIT) {
					if(italic) {
						italic = false;
						wattroff(interact, A_ITALIC);
					} else {
						italic = true;
						wattron(interact, A_ITALIC);
					}
				}
				break;
			case KEY_DC:
			case KEY_BACKSPACE:
				if(state == MODE_EDIT) {
					c = (int) ' ';
					mvwaddch(interact, cy, cx, (char) c);
					cursor_back(&cx, &cy);
					cursor_pos(cx, cy);
				}
				break;
			case (int) '\n':
				if(state == MODE_EDIT) {
					cursor_newline(&cx, &cy);
					cursor_pos(cx, cy);
					goto __input_exit;

				}
				break;
			default:
				if(state == MODE_EDIT) {
					cursor_forward(&cx, &cy);
					mvwaddch(interact, cy, cx, (char) c);
				}
				break;
		}

		__input_exit:
		refresh();
		wrefresh(interact);
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

