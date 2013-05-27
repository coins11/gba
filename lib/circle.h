#ifndef _CIRCLE_H_
#define _CIRCLE_H_

#include "velocity.h"
#include "shape.h"

extern inline void new_Circle (Shape *);

extern inline void set_data_of_circle (Shape *, int, int, int);

extern inline int all_on_circle  (int (*)(int, int), Shape *, int, int);
extern inline int some_on_circle (int (*)(int, int), Shape *, int, int);
extern inline void do_on_circle (void (*)(int, int, hword), Shape *, int, int);

extern inline int all_circle_border  (Shape *, int (*)(int, int));
extern inline int some_circle_border (Shape *, int (*)(int, int));
extern inline void do_circle_border (Shape *, void (*)(int, int, hword));

extern int circle_border (Shape *, int, void (*)(void) );

extern inline void draw_circle (Shape *);

extern inline int circle_in_screen (Shape *);
extern inline int circle_on_area_border (Shape *);

#endif
