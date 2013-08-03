#ifndef _TOUCH_H_
#define _TOUCH_H_

#define COLOR_BLACK     0

extern inline int touch_shapes (Shape *);
extern inline int touch_shapes_of_list (Shape *, Shape **l);

extern inline int move_touch_test_of_box    (Shape *, int, int);
extern inline int move_touch_test_of_circle (Shape *, int, int);

#endif
