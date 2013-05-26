#ifndef _BOX_H_
#define _BOX_H_

#include "shape.h"
#include "velocity.h"

extern inline void new_Box (Shape *);

extern inline void update_apex_of_box (Shape *);

extern inline void draw_box (Shape *);
extern inline void erase_box (Shape *);

extern inline int  box_in_screen (Shape *);
extern inline int  box_on_area_border (Shape *);

#endif
