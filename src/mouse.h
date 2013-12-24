#ifndef MOUSE_H
#define MOUSE_H

enum mouse_buttons {
    MOUSE_NO_BUTTON     = 0,
    MOUSE_LEFT_BUTTON   = 1,
    MOUSE_RIGHT_BUTTON  = 2,
    MOUSE_MID_BUTTON    = 4
};

int mouse_init(void);
void mouse_exit(void);
void mouse_get_status(unsigned *buttons, int *dx, int *dy);
void mouse_get_buttons(unsigned *buttons);
void mouse_get_delta(int *dx, int *dy);

#endif
