#include <stddef.h>

#include "mobs.h"

struct MobType_tag {
    wchar_t sym;
    int hp;
    const char *name, *desc;
} const allMobTypesArray[] = {
    {'x', 10, "food", "stuffs"}
};

const MobType *const allMobTypes = allMobTypesArray;
const int mobTypeCount = sizeof(allMobTypes)/sizeof(MobType);
