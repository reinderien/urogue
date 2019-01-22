#include <locale.h>    // setlocale
#include <ncursesw/ncurses.h>

#include "error.h"
#include "view.h"
#include "wave.h"

#define INVIS_CUR 0   // For curs_set

View view;

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

void view_splash() {
    wave();
}