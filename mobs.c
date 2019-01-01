
#include "mobs.h"

const MobType allMobTypesArray[] = {
    {'x', 10, "food", "stuffs"}
},
*const allMobTypes = allMobTypesArray;

const int mobTypeCount = sizeof(allMobTypes)/sizeof(*allMobTypes);
