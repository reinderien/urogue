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

void view_init() {
    // Set entire locale based on environment
    // Recommended by man ncurses(3X)
    assert_p(setlocale(LC_ALL, ""), "set the locale");

    view.win = initscr();
    assert_p(view.win, "start ncurses");

    assert_n(start_color(), "enable colour mode");
    assert_b(has_colors(), "check color support");
    assert_b(COLOR_PAIRS >= 256, "support enough colour pairs");
    assert_b(COLORS >= 256, "support enough colours");

    assert_n(cbreak(), "disable buffering for immediate input");
    assert_n(noecho(), "disable user input echo");
    assert_n(nonl(), "disable newline translation");
    assert_n(curs_set(INVIS_CUR), "hide the cursor");
    assert_n(keypad(view.win, true), "enable translation of arrow keys");
}

void view_destroy() {
    if (!isendwin())
        check_n(endwin(), "end ncurses");
}

static void wave() {
    // Draw an animated wave emanating from the center outwards, and filling
    // from black to a grey pattern

    // Black through blue through white
    const int N = 2*NC - 1; // number of colours
    NCURSES_COLOR_T scale[N];
    for (int i = 0; i < NC; i++)
        scale[i] = RGB2P(0, 0, i);
    for (int i = NC; i < N; i++)
        scale[i] = RGB2P(i-NC+1, i-NC+1, NC-1);

    // Set up pairs
    const int off = 2,     // offset between fore and back
              P = N - off; // number of pairs
    for (int i = 0; i < P; i++) {
        NCURSES_COLOR_T b = scale[i],     // back
                        f = scale[i+off], // fore
                        p = i+1;          // pair ID
        assert_n(init_pair(p, f, b), "init pair");
    }

    // Test output
    assert_n(wmove(view.win, 0, 0), "move cur");
    for (int i = 0; i < P; i++) {
        assert_n(wattron(view.win, COLOR_PAIR(i+1)), "switch colour");
        assert_n(waddch(view.win, 'X'), "output char");
    }
}

void view_splash() {
    wave();
}