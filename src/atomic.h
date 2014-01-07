#ifndef ATOMIC_H
#define ATOMIC_H

void disable_interrupts(void);
void enable_interrupts(void);

#pragma aux disable_interrupts = "cli";
#pragma aux enable_interrupts = "sti";

#endif
