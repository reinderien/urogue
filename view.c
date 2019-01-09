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
    const unsigned pm = 1000; // Resolution of ncurses colours

    assert_b(COLOR_PAIRS >= 256, "support enough colour pairs");
    assert_b(COLORS >= PALSIZE, "support enough colours");

    for (unsigned p = 0; p < PALSIZE; p++) {
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
    assert_b(has_colors(), "check color support");
    assert_b(can_change_color(), "check color change support");

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

    assert_n(wmove(view.win, 0, 0), "move cur");
    for (NCURSES_COLOR_T c = 0; c < 256; c++) {
        NCURSES_COLOR_T f = c,
                        b = c,
                        p = c+1;
        assert_n(init_pair(p, f, b), "init scale pair");
        assert_n(wattron(view.win, COLOR_PAIR(p)), "set attr");
        assert_n(waddch(view.win, 'X'), "output char");
    }

    /*
    const int off = 2; // Offset between fore and back
    // Black through blue through white
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
    // Number of pairs
    const int n = sizeof(scale)/sizeof(*scale) - off;

    // Set up pairs
    NCURSES_COLOR_T pairs[n];
    for (int i = 0; i < n; i++) {
        NCURSES_COLOR_T b = scale[i],
                        f = scale[i+off],
                        p = i+1; // FBPAIR(f, b);
        pairs[i] = p;
        char action[64];
        snprintf(action, sizeof(action),
                 "init scale pair f=%d b=%d p=0x%04hX",
                 f, b, p);
        assert_n(init_pair(p, f, b), action);
    }

    // Test output
    assert_n(wmove(view.win, 0, 0), "move cur");
    for (int i = 0; i < n; i++) {
        assert_n(wattron(view.win, COLOR_PAIR(pairs[i])), "switch colour");
        assert_n(waddch(view.win, 'X'), "output char");
    }
    */
    // write a char to the screen with mvwaddch
    // (int) mvwaddch (WINDOW *, int, int, const chtype);	/* generated */
}

void view_splash() {
    wave();
}