#include <stdlib.h>

#include "error.h"
#include "mobs.h"
#include "view.h"

static void exitHook() {
    view_destroy();
}

int main(int argc, const char **argv) {
    assert_c(atexit(exitHook), "register exit hook");

    view_init();

    return 0;
}
