#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>

#include "error.h"
#include "view.h"

#define INVIS_CUR 0

struct View_tag {
    WINDOW *win;
};

View *view_init() {
    View *v = malloc(sizeof(View));
    assert_p(v, "allocate view struct");

    // Set entire locale based on environment
    // Recommended by man ncurses(3X)
    setlocale(LC_ALL, "");

    v->win = initscr();
    assert_p(v->win, "start ncurses");

    assert_b(ERR != start_color(), "enable colour mode");
    assert_b(ERR != cbreak(), "disable buffering for immediate input");
    assert_b(ERR != noecho(), "disable user input echo");
    assert_b(ERR != nonl(), "disable newline translation");
    assert_b(ERR != curs_set(INVIS_CUR), "hide the cursor");
    assert_b(ERR != keypad(v->win, true), "enable translation of arrow keys");

    return v;
}

void view_destroy(View *v) {
    check_b(ERR != endwin(), "end ncurses");
    free(v);
}
