#pragma once

#include <stddef.h> // for wchar_t

typedef struct MobType_tag {
    wchar_t sym;
    int hp;
    const char *name, *desc;
} MobType;

extern const MobType *const allMobTypes;
extern const int mobTypeCount;
