#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "view.h"

bool check_c(int result, const char *action) {
    if (result) {
        view_destroy();
        fprintf(stderr, "Failed to %s (%d, %d: %s)\n",
                action, result, errno, strerror(errno));
    }
    return result == 0;
}

bool check_b(bool result, const char *action) {
    if (!result) {
        view_destroy();
        fprintf(stderr, "Failed to %s (%d: %s)\n",
                action, errno, strerror(errno));
    }
    return result;
}

bool check_p(void *p, const char *action) {
    return check_b(p != NULL, action);
}

void assert_c(int result, const char *action) {
    if (!check_c(result, action))
        exit(errno);
}

void assert_b(bool result, const char *action) {
    if (!check_b(result, action))
        exit(errno);
}

void assert_p(void *p, const char *action) {
    if (!check_p(p, action))
        exit(errno);
}
