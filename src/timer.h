#ifndef TIMER_H
#define TIMER_H

int timer_init(double ticks_per_sec);
void timer_exit(void);
double timer_ticks_per_sec(void);
unsigned int timer_get_ticks(void);
unsigned int timer_get_ticks_delta(void);
double timer_get_time(void);
double timer_get_time_delta(void);

#endif
