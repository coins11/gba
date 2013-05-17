#ifndef _BOX_H_
#define _BOX_H_

#include "shape.h"
#include "velocity.h"

extern void new_Box (Shape *);

extern void draw_box (Shape *);
extern void erase_box (Shape *);

extern int  box_in_screen (Shape *);
extern int  box_on_area_border (Shape *);

extern void update_apex_of_box (Shape *);

#endif
