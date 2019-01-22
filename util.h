#pragma once

// Rather than including time.h here
struct timespec;

float time_delta(struct timespec t1, struct timespec t2);
