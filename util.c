#include "util.h"
#include <time.h>  // timespec

float time_delta(struct timespec t1, struct timespec t2) {
    return t2.tv_sec - t1.tv_sec + 1e-9*(t2.tv_nsec - t1.tv_nsec);
}
