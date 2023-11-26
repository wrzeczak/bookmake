# BOOKMAKE

A simple terminal text editor focused on writing, not code editing. Vim-inspired, but intentionally threadbare, with support for exporting to raw text, Markdown, HTML, and PDF via a LaTeX template. Fun stuff!

### Features

- [ ] Text editor with a light and dark mode and support for italics and bold.
- [ ] Configurable text + background themes.
- [ ] Text export.
- [ ] Markdown/HTML export.
- [ ] PDF via editable + configurable LaTeX template export.
- [x] Cool bugs.
- [ ] Compiler warnings (*clean* software!).

### Exit Codes

- `0` means the program (should have) worked fine. If it returned 0 and didn't work, let me know.
- `20` means your terminal does not support color. I plan to add code to handle this, but for now, use a color terminal.
- `21` means your terminal is not capable of changing the default colors. Again, I plan to handle this, but it's difficult and frankly, I'm focusing on getting a working product on my machine, not a general-purpose product, at the moment.
- `80` means the terminal window is too small. By default, MINIMUM_WIDTH is set to 100 characters wide. Edit this or, ideally, make your terminal wider.

### Known Bugs

FYI: I write and test on Linux.

When displaying text in a color theme, changing tabs (in my case, changing dwm windows) screws up the color themes and makes them all lavender-y. I don't know what causes this. For now, just don't alt-tab? Beta and buggy software, folks!
