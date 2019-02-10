#include <ctype.h>    // for toupper()
#include <ncurses.h>  // for getch()
#include <stdlib.h>   // for atexit()

#include "error.h"
#include "mobs.h"
#include "view.h"

static void exitHook() {
    view_destroy();
}

int main(int argc, const char **argv) {
    assert_c(atexit(exitHook), "register exit hook");

    view_init();

    view_splash();

    while (toupper(getch()) != 'Q');

    return 0;
}
