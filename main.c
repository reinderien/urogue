#include <stdlib.h>

#include "error.h"
#include "mobs.h"
#include "view.h"

static View *view = NULL;

static void exitHook() {
    view_destroy(&view);
}

int main(int argc, const char **argv) {
    assert_c(atexit(exitHook), "register exit hook");

    view = view_init();

    return 0;
}
