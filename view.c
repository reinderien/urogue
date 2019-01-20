#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
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

static void wave_palette(int *po, int *pn, int *wo, int *wn) {
    // Black through blue through white
    const int N = 2*NC - 1; // number of colours
    NCURSES_COLOR_T scale[N];
    for (int i = 0; i < NC; i++)
        scale[i] = RGB2P(0, 0, i);
    for (int i = NC; i < N; i++)
        scale[i] = RGB2P(i-NC+1, i-NC+1, NC-1);

    // Set up pairs for "point"
    *po = 1; // point pair offset
    *pn = N; // point pair count
    for (int i = 0; i < *pn; i++) {
        NCURSES_COLOR_T b = scale[0], // back
                        f = scale[i]; // fore
        NCURSES_PAIRS_T p = i + *po;  // pair ID
        assert_n(init_pair(p, f, b), "init pair");
    }

    // Set up pairs for "wave"
    const int off = 2; // offset between fore and back
    *wo = *po + *pn;   // wave pair offset
    *wn = N - off;     // wave pair count
    for (int i = 0; i < *wn; i++) {
        NCURSES_COLOR_T b = scale[i],     // back
                        f = scale[i+off]; // fore
        NCURSES_PAIRS_T p = i + *wo;      // pair ID
        assert_n(init_pair(p, f, b), "init pair");
    }
}

static void wave() {
    // Draw an animated wave emanating from the center outwards, and filling
    // from black to a grey pattern

    int po, pn, wo, wn; // point offset+count; wave offset+count
    wave_palette(&po, &pn, &wo, &wn);

    int Y, X;
    getmaxyx(view.win, Y, X);
    const int ym = Y/2, xm = X/2;
    // assert_n(wmove(view.win, ym, xm), "move cur");

    for (int c = 0; c < pn; c++) {
        assert_n(wattron(view.win, COLOR_PAIR(c + po)), "switch colour");
        assert_n(mvwaddch(view.win, ym, xm, '.'), "output char");
        assert_n(wrefresh(view.win), "refresh");
        assert_c(usleep(200000), "usleep");
    }
}

void view_splash() {
    wave();
}