#pragma once

#include <stdbool.h>

// C-style errors: 0 indicates success
bool  check_c( int result, const char *action);
void assert_c( int result, const char *action);

// ncurses errors: -1 indicates failure
bool  check_n( int result, const char *action);
void assert_n( int result, const char *action);

// generic boolean errors
bool  check_b(bool result, const char *action);
void assert_b(bool result, const char *action);

// null pointer errors
bool  check_p(    void *p, const char *action);
void assert_p(    void *p, const char *action);
