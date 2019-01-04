#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "view.h"

bool check_c(int result, const char *action) {
    if (!result)
        return true;
    view_destroy();
    fprintf(stderr, "Failed to %s (C %d: %s)\n",
            action, result, strerror(errno));
    return false;
}

bool check_b(bool result, const char *action) {
    if (result)
        return true;
    view_destroy();
    fprintf(stderr, "Failed to %s (C %s)\n",
            action, strerror(errno));
    return false;
}

bool check_n(int result, const char *action) {
    return check_b(result != -1, action);
}

bool check_p(void *p, const char *action) {
    return check_b(p != NULL, action);
}

void assert_c(int result, const char *action) {
    if (!check_c(result, action))
        exit(errno);
}

void assert_n(int result, const char *action) {
    if (!check_n(result, action))
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
