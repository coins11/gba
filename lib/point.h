#ifndef _POINT_H_
#define _POINT_H_

typedef struct point {
	int	x, y;

	void (*draw)(struct point *, hword);
	int  (*move)(struct point *, int, int);

	int (*distance)(struct point *, struct point *);

	void (*set)(struct point *, int, int);
} Point;

extern void new_Point(Point *);

extern int  point_distance(Point *, Point *);

// extern void int_draw_point(int, int, hword);
// extern void draw_point(Point *, hword);
extern void point_draw_point (Point *, hword);
extern void draw_point(int, int, hword);
extern int  move_point(Point *, int, int);

extern void set_point(Point *, int, int);

#endif
