#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>

#include "error.h"
#include "view.h"

#define INVIS_CUR 0

struct {
    WINDOW *win;
} static view;

static void init_palette() {
    const unsigned pm = 1000, // Resolution of ncurses colours
                   palsize = RRES*GRES*BRES; // Palette colours

    assert_b(COLOR_PAIRS >= 0x10000, "support enough colour pairs");
    assert_b(COLORS >= palsize, "support enough colours");

    for (unsigned p = 0; p < palsize; p++) {
        unsigned r = pm * ( p                % RRES) / (RRES-1),
                 g = pm * ((p /  RRES      ) % GRES) / (GRES-1),
                 b = pm * ((p / (RRES*GRES)) % BRES) / (BRES-1);
        assert_n(init_color(p, r, g, b), "add colour index");
    }
}

void view_init() {
    // Set entire locale based on environment
    // Recommended by man ncurses(3X)
    assert_p(setlocale(LC_ALL, ""), "set the locale");

    view.win = initscr();
    assert_p(view.win, "start ncurses");

    assert_n(start_color(), "enable colour mode");
    assert_n(cbreak(), "disable buffering for immediate input");
    assert_n(noecho(), "disable user input echo");
    assert_n(nonl(), "disable newline translation");
    assert_n(curs_set(INVIS_CUR), "hide the cursor");
    assert_n(keypad(view.win, true), "enable translation of arrow keys");

    init_palette();
}

void view_destroy() {
    if (!isendwin())
        check_n(endwin(), "end ncurses");
}

static void wave() {
    // Draw an animated wave emanating from the center outwards, and filling
    // from black to a grey pattern

    const NCURSES_COLOR_T scale[] = {
        C2P(0,0,0),
        C2P(0,0,1),
        C2P(0,0,2),
        C2P(0,1,2),
        C2P(1,3,3),
        C2P(2,3,4),
        C2P(2,4,5),
        C2P(3,5,5),
        C2P(3,6,5),
        C2P(4,6,5),
        C2P(5,6,5)
    };

    // write a char to the screen with mvwaddch
    // (int) mvwaddch (WINDOW *, int, int, const chtype);	/* generated */

    // attron(COLOR_PAIR(BG_COLOR));
    // int wattron(WINDOW *win, int attrs);

    // * These apply to the first 256 color pairs.
    // #define COLOR_PAIR(n)	((n << 8) & 0xFF00)

    // #define A_COLOR	     	0xFF00
    // #define NCURSES_BITS(mask,shift) (mask << ((shift) + 8))
    // #define NCURSES_CAST(type,value) (type)(value)
    // typedef unsigned chtype;
    // #define NCURSES_ATTR_SHIFT       8


}

void view_splash() {
    wave();
}