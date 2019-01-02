#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "mobs.h"
#include "view.h"

static View *view = NULL;

static void exitHook() {
    puts("Exiting...");
    if (view)
        view_destroy(view);
}

int main(int argc, const char **argv) {
    if (atexit(exitHook)) {
        perror("Failed to register exit hook");
        return errno;
    }

    view = view_init();

    return 0;
}
