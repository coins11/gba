#ifndef _CIRCLE_H_
#define _CIRCLE_H_

#include "velocity.h"
#include "shape.h"

extern void new_Circle (Shape *);

extern int all_on_circle  (int (*)(int, int), Shape *, int, int);
extern int some_on_circle (int (*)(int, int), Shape *, int, int);
extern void do_on_circle (void (*)(int, int, hword), Shape *, int, int);

extern int all_circle_border  (Shape *, int (*)(int, int));
extern int some_circle_border (Shape *, int (*)(int, int));
extern void do_circle_border (Shape *, void (*)(int, int, hword));

extern void draw_circle (Shape *);
extern void erase_circle (Shape *);

extern int circle_in_screen (Shape *);
extern int circle_on_area_border (Shape *);

#endif
