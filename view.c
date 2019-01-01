#include <locale.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>

#include "view.h"

struct View_tag {
    int width;
};

View *view_init() {
    View *v = malloc(sizeof(View));

    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    nonl();

    return v;
}

void view_destroy(View *v) {
    endwin();
    free(v);
}
