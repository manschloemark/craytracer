#ifndef TIMER_H
#define TIMER_H

#include <time.h>

typedef struct {
	float start;
	float end;
} timer;

#define TIMER_START(t) t.start = clock()
#define TIMER_END(t) t.end = clock()
#define TIMER_DURATION(t) (t.end - t.start)
#define TIMER_DURATION_S(t) (t.end - t.start) / CLOCKS_PER_SEC

#endif
