# BOOKMAKE

A simple terminal text editor focused on writing, not code editing. Vim-inspired, but intentionally threadbare, with support for exporting to raw text, Markdown, HTML, and PDF via a LaTeX template. Fun stuff!

### Features

- [x] Text editor with support for italics, underline and bold.
- [ ] Configurable text + background themes.
- [ ] Live toggling between themes.
- [ ] Text export.
- [ ] Markdown/HTML export.
- [ ] PDF via editable + configurable LaTeX template export.
- [x] Cool bugs.
- [ ] Compiler warnings (*clean* software!).

### Usage

To switch between program states, use `F1` (edit), `F2` (command).

Edit mode will allow you to edit the text before you. Essentially Vim's insert mode.

To italicize, enbold, or underline text, use `CTRL` and `i`, `b`, or `u`, respectively.

Command mode will allow you to save/create files, changes themes, etc. It's essentially like Vim's normal mode.

Use `SHIFT + q` to exit while in command mode.

### Exit Codes

NOTE: These reference the error code printed out onto the terminal, not the error code that the program exits with. The program exits with the amount of init functions that failed, ie. if colors_init() and curses_init() both fail, the program exits with code 2.


- `0` means the program (should have) worked fine. If it returned 0 and didn't work, let me know, especially if your issue is not in the Known Bugs section.
- `20` means your terminal does not support color. I plan to add code to handle this, but for now, use a color terminal.
- `21` means your terminal is not capable of changing the default colors. Again, I plan to handle this, but it's difficult and frankly, I'm focusing on getting a working product on my machine, not a general-purpose product, at the moment.
- `80` means the terminal window is too small. By default, MINIMUM_WIDTH is set to 100 characters wide. Edit this or, ideally, make your terminal wider.

### Known Bugs

FYI: I write and test on Linux.

`LAVENDER TOWN` -- When displaying text in a color theme, changing tabs (in my case, changing dwm windows) screws up the color themes and makes them all lavender-y. I don't know what causes this. For now, just don't alt-tab? Beta and buggy software, folks!

`CURSOR LOCATOR` -- It's not exactly accurate, especially when dealing with backspaces and traversing across lines.
