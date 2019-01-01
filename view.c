#include <stdlib.h>
#include <ncursesw/ncurses.h>

#include "view.h"

struct View_tag {
    int width;
};

View *view_init() {
    View *v = malloc(sizeof(View));
    return v;
}

void view_destroy(View *v) {
    free(v);
}
