#include "gba.h"
#include "point.h"
#include "velocity.h"
#include "shape.h"
#include "quadtree.h"
#include "debug.h"

inline byte
separate_bit (byte n)
{
	n = ((n << 2) | n) & 0x33; // 0x33 = 00 11 00 11
	n = ((n << 1) | n) & 0x55; // 0x55 = 01 01 01 01

	return n;
}

inline byte
morton_number (int x, int y)
{
	byte mx, my;
	int i, d;

	d = Div(LCD_WIDTH, SPACE_NUM);
	for (i = 1; i <= SPACE_NUM; i++) {
		if (x < (d * i)) {
			mx = i - 1;
			break;
		}
	}

	d = Div(LCD_HEIGHT, SPACE_NUM);
	for (i = 1; i <= SPACE_NUM; i++) {
		if (y < (d * i)) {
			my = i - 1;
			break;
		}
	}

	return separate_bit(mx) | (separate_bit(my) << 1);
}

inline int
mn_index (int x1, int y1, int x2, int y2)
{
	byte ul = morton_number(x1, y1);
	byte br = morton_number(x2, y2);

	byte sm = ul ^ br;

	if ( sm >> 6 ) {
		return 0;
	}
	else if ( (sm >> 4) & 0x3 ) { // 0x3 = 00 00 00 11
		return (int)((sm >> 4) & 0x3) + 1;
	}
	else if ( (sm >> 2) & 0x3 ) {
		return (int)((sm >> 2) & 0x3) + 5;
	}
	else {
		return (int)(ul) + 21;
	}
}

inline int
box_mn (Shape *s)
{
	Box *b  = &(s->as.box);

	return mn_index(b->apex[0].x, b->apex[0].y, b->apex[3].x, b->apex[3].y);
}

inline int
circle_mn (Shape *s)
{
	Circle *c = &(s->as.circle);

	return mn_index( s->p.x - c->r, s->p.y - c->r, s->p.x + c->r, s->p.y + c->r );
}


inline void
update_mn (Shape *s)
{
	s->mn = ( s->type == 2 ? box_mn(s) : circle_mn(s) );
}

inline int
mn_class (int mn, int c)
{
	if (c == 0) { // parent
		if (mn < 1) {
			return -1;
		} else if (mn < 5) {
			return (mn - 1);
		} else if (mn < 21) {
			return (mn - 5) >> 2;
		} else {
			return (mn - 21) >> 4;
		}
	}
	else if (c == 1) { // child
		if (mn < 5) {
			return -1;
		} else if (mn < 21) {
			return (mn - 5) & 0x3;
		} else {
			return ((mn - 21) >> 2) & 0x3;
		}
	}
	else if (c == 2) { // grandchild
		if (mn < 21) {
			return -1;
		} else {
			return (mn - 21) & 0x3;
		}
	}
}

inline int
same_space (Shape *s1, Shape *s2)
{
	int mn1 = (s1->mn < s2->mn ? s1->mn : s2->mn);
	int mn2 = (s1->mn < s2->mn ? s2->mn : s1->mn);

	if (mn1 == mn2 || mn1 < 1 || mn2 < 1) {
		return 1;
	}
	else if (mn1 < 5) {
		return mn_class(mn1, 0) == mn_class(mn2, 0);
	}
	else if (mn1 < 21) {
		return mn_class(mn1, 1) == mn_class(mn2, 1);
	}
	else {
		return mn_class(mn1, 2) == mn_class(mn2, 2);
	}
}

