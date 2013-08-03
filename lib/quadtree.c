#include "gba.h"
#include "point.h"
#include "velocity.h"
#include "shape.h"
#include "quadtree.h"
#include "debug.h"
#include "libgba/gba_systemcalls.h"
#include "print.h"

static inline byte
separate_bit (byte n)
{
	n = ((n << 2) | n) & 0x33; // 0x33 = 00 11 00 11
	n = ((n << 1) | n) & 0x55; // 0x55 = 01 01 01 01

	return n;
}

static inline byte
morton_number (int x, int y)
{
	byte mx, my;
	int i, d;

	d = Div(LCD_WIDTH, SPACE_NUM);
	mx = SPACE_NUM - 1;
	for (i = 1; i <= SPACE_NUM; i++) {
		if (x < (d * i)) {
			mx = i - 1;
			break;
		}
	}

	d = Div(LCD_HEIGHT, SPACE_NUM);
	my = SPACE_NUM - 1;
	for (i = 1; i <= SPACE_NUM; i++) {
		if (y < (d * i)) {
			my = i - 1;
			break;
		}
	}

	return separate_bit(mx) | (separate_bit(my) << 1);
}

static inline int
mn_index (int x1, int y1, int x2, int y2)
{
	byte ul = morton_number(x1, y1);
	byte br = morton_number(x2, y2);
	byte sm = ul ^ br;
	int i;
	int l[] = {85, 21, 5, 1};

	for (i = 0; (sm >> (i * 2)) != 0; i++);

	if (sm == 0) {
		return (int)(ul) + l[0]; 
	} else if (i < 4) {
		return (int)( (ul >> (i * 2) ) ) + l[i];
	} else {
		return 0;
	}
}

static inline int
box_mn (Shape *s)
{
	Box *b  = &(s->as.box);

	return mn_index(b->apex[0].x, b->apex[0].y, b->apex[3].x, b->apex[3].y);
}

static inline int
circle_mn (Shape *s)
{
	Circle *c = &(s->as.circle);

	return mn_index( s->p.x - c->r, s->p.y - c->r, s->p.x + c->r, s->p.y + c->r );
}


inline int
get_move_box_mn (Shape *s, int x, int y)
{
	int x1, y1, x2, y2;
	Box *b = &(s->as.box);

	if (x < 0 && y < 0) {
		x1 = b->apex[0].x + x;
		y1 = b->apex[0].y + y;
		x2 = b->apex[3].x;
		y2 = b->apex[3].y;
	} else if (x >= 0 && y >= 0) {
		x1 = b->apex[0].x;
		y1 = b->apex[0].y;
		x2 = b->apex[3].x + x;
		y2 = b->apex[3].y + y;
	} else if (y < 0) {
		x1 = b->apex[2].x;
		y1 = b->apex[2].y;
		x2 = b->apex[1].x + x;
		y2 = b->apex[1].y + y;
	} else {
		x1 = b->apex[1].x;
		y1 = b->apex[1].y;
		x2 = b->apex[2].x + x;
		y2 = b->apex[2].y + y;
	}
	
	return mn_index(x1, y1, x2, y2);
}

inline int
get_move_circle_mn (Shape *s, int x, int y)
{
	int x1, y1, x2, y2;
	Circle *c = &(s->as.circle);

	if (x < 0 && y < 0) {
		x1 = s->p.x - c->r + x;
		y1 = s->p.y - c->r + y;
		x2 = s->p.x;
		y2 = s->p.y;
	} else if (x >= 0 && y >= 0) {
		x1 = s->p.x - c->r;
		y1 = s->p.y - c->r;
		x2 = s->p.x + c->r + x;
		y2 = s->p.y + c->r + y;
	} else if (y < 0) {
		x1 = s->p.x - c->r;
		y1 = s->p.y - c->r;
		x2 = s->p.x + c->r + x;
		y2 = s->p.y + c->r + y;
	} else {
		x1 = s->p.x + c->r;
		y1 = s->p.y + c->r;
		x2 = s->p.x - c->r + x;
		y2 = s->p.y - c->r + y;
	}

	return mn_index(x1, y1, x2, y2);
}

inline void
update_mn (Shape *s)
{
	s->mn = ( s->type == 2 ? box_mn(s) : circle_mn(s) );
}

inline int
mn_class (int mn, int c)
{
	int i;
	int l[] = {1, 5, 21, 85};

	if (mn < l[c]) {
		return -1;
	} else {
		for (i = c; i < 3; i++) {
			if (mn < l[i + 1]) {
				return (mn - l[i]) >> ((i - c) * 2);
			}
		}
		return (mn - l[i]) >> ((i - c) * 2);
	}
}

inline void
update_mn_all(Shape *s)
{
	inline int f (Shape *s1, Shape *s2) {
		s2->update_mn(s2);
		return 0;
	}

	run_two_side_list(s, f);
}

inline int
same_space_int (int x, int y)
{
	int i;
	int l[] = {1, 5, 21, 85};
	int mn1 = (x < y ? x : y);
	int mn2 = (x < y ? y : x);

	if (mn1 == mn2 || mn1 < 1 || mn2 < 1) {
		return 1;
	} else {
		for (i = 0; i < 3; i++) {
			if (mn1 < l[i + 1]) {
				return mn_class(mn1, i) == mn_class(mn2, i);
			}
		}

		return mn_class(mn1, i) == mn_class(mn2, i);
	}
}

inline int
same_space (Shape *s1, Shape *s2)
{
	return same_space_int(s1->mn, s2->mn);
}
