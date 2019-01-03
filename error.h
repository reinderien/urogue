#pragma once

#include <stdbool.h>

bool  check_c( int result, const char *action);
void assert_c( int result, const char *action);
bool  check_b(bool result, const char *action);
void assert_b(bool result, const char *action);
bool  check_p(    void *p, const char *action);
void assert_p(    void *p, const char *action);
