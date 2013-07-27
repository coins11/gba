#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#define SPACE_NUM  16

extern inline void update_mn (Shape *);
extern inline void update_mn_all (Shape *);
extern inline int  same_space_int (int, int);
extern inline int  same_space (Shape *, Shape *);
#endif
