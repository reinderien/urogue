#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>

#include "view.h"

struct View_tag {
    WINDOW *win;
};

View *view_init() {
    View *v = malloc(sizeof(View));
    if (!v) {
        perror("Failed to allocate view struct");
        exit(ENOMEM);
    }

    // Set entire locale based on environment
    // Recommended by man ncurses(3X)
    setlocale(LC_ALL, "");

    v->win = initscr();
    if (!v->win) {
        perror("Failed to start ncurses");
        exit(errno);
    }

    if (cbreak() || // disable buffering, immediate input
        noecho() || // don't echo user input
        nonl()   || // disable newline translation
        keypad(v->win, true) // enable translation of arrow keys
    ) {
        perror("Failed to configure ncurses");
        exit(errno);
    }

    return v;
}

void view_destroy(View *v) {
    if (endwin())
        perror("Warning: failed to end ncurses");
    free(v);
}
