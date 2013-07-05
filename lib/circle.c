#include "gba.h"
#include "point.h"
#include "screen.h"
#include "velocity.h"
#include "shape.h"
#include "delay.h"
#include "circle.h"

inline void
new_Circle (Shape *s)
{
	Circle *c = &(s->as.circle);

	new_Shape(s);
	new_Point( &(s->p) );
	new_Point( &(s->pre_p) );
	new_Velocity( &(s->v) );

	s->type = 1;

	c->set = set_data_of_circle;

	s->draw  = draw_circle;

	c->all_border  = all_circle_border;
	c->some_border = some_circle_border;
	c->do_border   = do_circle_border;

	s->in_screen        = circle_in_screen;
	s->on_area_border   = circle_on_area_border;
	s->on_bottom_border = circle_on_bottom_border;
}

inline void
set_data_of_circle (Shape *s, int x, int y, int r)
{
	Circle *c = &(s->as.circle);

	s->p.set( &(s->p), x, y);
	c->r = r;
}

inline int
all_on_circle ( int (*f)(int, int), Shape *c, int x, int y )
{
	return (*f)(y + c->p.x, x + c->p.y) &&
		(*f)(x + c->p.x, y + c->p.y) &&
		(*f)(-x + c->p.x, y + c->p.y) &&
		(*f)(-y + c->p.x, x + c->p.y) &&
		(*f)(-y + c->p.x, -x + c->p.y) &&
		(*f)(-x + c->p.x, -y + c->p.y) &&
		(*f)(x + c->p.x, -y + c->p.y) &&
		(*f)(y + c->p.x, -x + c->p.y); 
}

inline int
some_on_circle ( int (*f)(int, int), Shape *c, int x, int y )
{
	return (*f)(y + c->p.x, x + c->p.y) ||
		(*f)(x + c->p.x, y + c->p.y) ||
		(*f)(-x + c->p.x, y + c->p.y) ||
		(*f)(-y + c->p.x, x + c->p.y) ||
		(*f)(-y + c->p.x, -x + c->p.y) ||
		(*f)(-x + c->p.x, -y + c->p.y) ||
		(*f)(x + c->p.x, -y + c->p.y) ||
		(*f)(y + c->p.x, -x + c->p.y); 
}

inline void
do_on_circle ( void (*f)(int, int, hword), Shape *c, int x, int y) 
{
	(*f)(y + c->p.x, x + c->p.y, c->color);
	(*f)(x + c->p.x, y + c->p.y, c->color);
	(*f)(-x + c->p.x, y + c->p.y, c->color);
	(*f)(-y + c->p.x, x + c->p.y, c->color);
	(*f)(-y + c->p.x, -x + c->p.y, c->color);
	(*f)(-x + c->p.x, -y + c->p.y, c->color);
	(*f)(x + c->p.x, -y + c->p.y, c->color);
	(*f)(y + c->p.x, -x + c->p.y, c->color); 
}

int
circle_border ( Shape *s, int mode, void (*f)(void) )
{
	Circle *c = &(s->as.circle);
	int cx, cy, d, dH, dD;
	int flag;

	d     = 1 - c->r;
	dH    = 3;
	dD    = 5 - 2 * c->r;
	cy    = c->r;

	if (mode == 1) {
		flag = 1;
	} else {
		flag = 0;
	}

	for (cx = 0; cx <= cy; cx++) {
		if (d < 0) {
			d   += dH;
			dH  += 2;
			dD  += 2;
		} else {
			d   += dD;
			dH  += 2;
			dD  += 4;
			--cy;
		}

		if (mode == 0) {
			do_on_circle( (void (*)(int, int, hword) )f, s, cy, cx );
		} 
		else if (mode == 1) {
			flag = flag && all_on_circle(  ( int (*)(int, int) )f, s, cy, cx );
		}
		else if (mode == 2) {
			flag = flag || some_on_circle( ( int (*)(int, int) )f, s, cy, cx );
		}
	}

	return flag;
}

inline int
all_circle_border ( Shape *c, int (*f)(int, int) )
{
	return circle_border( c, 1, (void (*)(void))f );
}

inline int
some_circle_border ( Shape *c, int (*f)(int, int) )
{
	return circle_border( c, 2, (void (*)(void))f );
}

inline void
do_circle_border ( Shape *c, void (*f)(int, int, hword) )
{
	circle_border( c, 0, (void (*)(void))f );
}

inline void
draw_circle (Shape *s){
	void (*f)(int, int, hword);

	f = draw_point;

	s->as.circle.do_border(s, f);
}

inline int
circle_in_screen (Shape *s)
{
	Circle *c = &(s->as.circle);

	return in_screen(s->p.x + c->r, s->p.y + c->r) &&
		   in_screen(s->p.x - c->r, s->p.y - c->r);
}

inline int
circle_on_area_border (Shape *s)
{
	Circle *c = &(s->as.circle);

	return on_area_border(s->p.x + c->r, s->p.y + c->r) ||
		   on_area_border(s->p.x - c->r, s->p.y - c->r);
}

inline int
circle_on_bottom_border (Shape *s)
{
	Circle *c = &(s->as.circle);

	return on_area_border(s->p.x + c->r, s->p.y + c->r);
}
