#ifndef _BOX_H_
#define _BOX_H_

#include "shape.h"
#include "velocity.h"

extern inline void new_Box (Shape *);

static inline void set_data_of_box (Shape *, int, int, int, int);
static inline int  resize_box (Shape *, int, int);
static inline void update_apex_of_box (Shape *);

static inline void draw_box (Shape *);

static inline int box_in_screen (Shape *);
static inline int box_on_area_border (Shape *);
static inline int box_on_bottom_border (Shape *);

#endif
