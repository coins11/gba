#ifndef _POINT_H_
#define _POINT_H_

typedef struct point {
	int	x, y;

	void (*draw)(struct point *, hword);
	int  (*move)(struct point *, int, int);

	int (*distance)(struct point *, struct point *);

	void (*set)(struct point *, int, int);
} Point;

extern inline void new_Point(Point *);

extern inline void draw_point(int, int, hword);

static inline int point_distance(Point *, Point *);
static inline void point_draw_point (Point *, hword);

static inline int  move_point(Point *, int, int);
static inline void set_point(Point *, int, int);

#endif
