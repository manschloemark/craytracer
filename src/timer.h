#ifndef TIMER_H
#define TIMER_H

#include <stdlib.h>
#include <time.h>

typedef struct {
	struct timespec *start;
	struct timespec *end;
} timer;

static timer timer_new() {
	timer t = {};
	struct timespec *s = (struct timespec *)malloc(sizeof(struct timespec));
	struct timespec *e = (struct timespec *)malloc(sizeof(struct timespec));
	t.start = s;
	t.end = e;
	timespec_get(t.start, TIME_UTC);
	timespec_get(t.end, TIME_UTC);
	return t;
}

static void timer_free(timer *t) {
	free(t->start);
	free(t->end);
}

#define TIMER_START(t) clock_gettime(CLOCK_MONOTONIC, t.start)
#define TIMER_END(t) clock_gettime(CLOCK_MONOTONIC, t.end)
#define TIMER_DURATION_S(t) (int)(t.end->tv_sec - t.start->tv_sec)

#endif
