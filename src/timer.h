#ifndef TIMER_H
#define TIMER_H

typedef void __interrupt (*timer_handler)();

void timer_init(void);
void timer_exit(void);
void timer_set_handler(timer_handler handler);

#endif
