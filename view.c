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

    assert_b(ERR != start_color(), "enable colour mode");
    assert_b(ERR != cbreak(), "disable buffering for immediate input");
    assert_b(ERR != noecho(), "disable user input echo");
    assert_b(ERR != nonl(), "disable newline translation");
    assert_b(ERR != curs_set(INVIS_CUR), "hide the cursor");
    assert_b(ERR != keypad(view.win, true), "enable translation of arrow keys");

    // todo - make these realistic
    assert_b(COLOR_PAIRS >= 16, "support enough colour pairs");
    assert_b(COLORS >= 256, "support enough colours");
    // note - each colour channel supports 1000 values
}

void view_destroy() {
    if (!isendwin())
        check_b(ERR != endwin(), "end ncurses");
}
