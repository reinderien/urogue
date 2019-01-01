#include "mobs.h"
#include "view.h"

int main(int argc, const char **argv) {
    View *view = view_init();

    view_destroy(view);

    return 0;
}
