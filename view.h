#pragma once

struct View_tag;
typedef struct View_tag View;

View *view_init();
void view_destroy(View **v);
