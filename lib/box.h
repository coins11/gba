#ifndef _BOX_H_
#define _BOX_H_

#include "shape.h"
#include "velocity.h"

extern inline void new_Box (Shape *);

extern inline void set_data_of_box (Shape *, int, int, int, int);
extern inline int  resize_box (Shape *, int, int);
extern inline void update_apex_of_box (Shape *);

extern inline void draw_box (Shape *);

extern inline int box_in_screen (Shape *);
extern inline int box_on_area_border (Shape *);
extern inline int box_on_bottom_border (Shape *);

#endif
