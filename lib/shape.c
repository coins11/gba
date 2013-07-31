#include "gba.h"
#include "libgba/gba_systemcalls.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "point.h"
#include "vector.h"
#include "velocity.h"
#include "shape.h"
#include "circle.h"
#include "box.h"
#include "touch.h"
#include "quadtree.h"
#include "debug.h"

inline void
new_Shape (Shape *s)
{
	int i;

	s->id        = -1;
	s->breakable = 0;
	s->mn        = 0;

	s->move_callback  = NULL;
	s->touch_callback = NULL;

	s->pre_p = s->p;

	s->move        = move_shape;
	s->direct_move = direct_move;
	s->erase       = erase_shape;
	s->clear       = clear_shape;
	s->redraw      = redraw_shape;

	s->draw_all   = draw_all_shapes;
	s->erase_all  = erase_all_shapes;
	s->clear_all  = clear_all_shapes;
	s->redraw_all = redraw_all_shapes;

	s->run   = shape_run;

	s->touch = touch_shapes;

	s->update_mn      = update_mn;
	s->update_mn_all  = update_mn_all;

	s->same_space = same_space;
}

inline void
chain_shapes (int n, ...)
{
	int i;
	Shape *s, *n1, *n2;
	va_list shapes;

	va_start(shapes, n);

	s  = va_arg(shapes, Shape *);
	n1 = va_arg(shapes, Shape *);

	s->next  = n1;
	n1->prev = s;

	for (i = 2; i < n; i++) {
		n2 = va_arg(shapes, Shape *);
		n1->next = n2;
		n2->prev = n1;

		n1 = n2;
	}

	n2->next = s;
	s->prev  = n2;

	va_end(shapes);
}

inline Shape *
run_two_side_list (Shape *s, int (*f)(Shape *, Shape *) )
{
	Shape *p   = s->prev,
		  *n   = s;
	int flag_n = 0,
		flag_p = 0;

	while (n != p && p != NULL && n != NULL) {
		flag_n = f(s, n);
		flag_p = f(s, p);

		if (p->prev == n || flag_n || flag_p) {
			break;
		}
		p = p->prev;
		n = n->next;
	}

	if (flag_n) {
		return n;
	}
	else if (flag_p) {
		return p;
	}
	else if ( f(s, p) ) {
		return p;
	}
	else {
		return NULL;
	}
}


static inline void
move_shape_without_check (Shape *s, int x, int y)
{
	s->p.set( &(s->p), s->p.x + x, s->p.y + y );
	
	if (s->type == 2) {
		s->as.box.update_apex(s);
	}

	if (s->move_callback != NULL) {
		s->move_callback(s, 1);
	}

	s->update_mn(s);
}

static inline int
move_shape (Shape *s, int x, int y)
{
	s->p.set( &(s->p), s->p.x + x, s->p.y + y );
	
	if ( s->in_screen(s) && !s->touch(s) && s->breakable != -1) {
		move_shape_without_check(s, x, y);

		return 1;
	} else {
		if (s->move_callback != NULL) {
			s->move_callback(s, 0);
		}

		s->p.set( &(s->p), s->p.x - x, s->p.y - y );

		return 0;
	}
}

static inline int
in_screen_after_move (Shape *s, int x, int y)
{
	s->p.set( &(s->p), s->p.x + x, s->p.y + y );
	
	if ( s->in_screen(s) ) {
		s->p.set( &(s->p), s->p.x - x, s->p.y - y );
		return 1;
	} else { 
		s->p.set( &(s->p), s->p.x - x, s->p.y - y );
		return 0;
	}
}

static inline int
move_if_in_screen (Shape *s, int x, int y, Shape **l)
{
	s->p.set( &(s->p), s->p.x + x, s->p.y + y );

	if ( !touch_shapes_of_list(s, l) && s->breakable != -1) {
		s->p.set( &(s->p), s->p.x - x, s->p.y - y );

		move_shape_without_check(s, x, y);
		return 1;
	} else {
		if (s->move_callback != NULL) {
			s->move_callback(s, 0);
		}

		s->p.set( &(s->p), s->p.x - x, s->p.y - y );

		return 0;
	}
}

static inline int
direct_move (Shape *s, int x, int y)
{
	s->p.set( &(s->p), s->p.x + x, s->p.y + y );
	if ( !s->in_screen(s) ) {
		s->p.set( &(s->p), s->p.x - x, s->p.y - y );
		return 0;
	}
	s->p.set( &(s->p), s->p.x - x, s->p.y - y );
	
	if ( (s->type == 2 ? move_touch_test_of_box(s, x, y) : move_touch_test_of_circle(s, x, y)) ) {
		s->p.set( &(s->p), s->p.x + x, s->p.y + y );

		if (s->type == 2) {
			s->as.box.update_apex(s);
		}

		if (s->move_callback != NULL) {
			s->move_callback(s, 1);
		}

		s->update_mn(s);

		return 1;
	} else {
		return 0;
	}
}

static inline void
erase_shape (Shape *s)
{
	hword c = s->color;
	int   x = s->p.x,
		  y = s->p.y;
	
	s->color = COLOR_BLACK;
	s->p     = s->pre_p;

	s->draw(s);

	s->color = c;
	s->p.x   = x;
	s->p.y   = y;
}

static inline void
redraw_shape (Shape *s)
{
	if ( s->p.x != s->pre_p.x || s->p.y != s->pre_p.y  ) {
		s->erase(s);
		s->draw(s);
	}
}

static inline void
clear_shape (Shape *s)
{
	hword c = s->color;

	s->color = COLOR_BLACK;
	s->draw(s);
	s->color = c;
}

static inline void
draw_all_shapes (Shape *s)
{
	inline int f (Shape *s1, Shape *s2) {
		s2->draw(s2);
		return 0;
	}

	run_two_side_list(s, f);
}

static inline void
erase_all_shapes (Shape *s)
{
	inline int f (Shape *s1, Shape *s2) {
		s2->erase(s2);
		return 0;
	}

	run_two_side_list(s, f);
}

static inline void
clear_all_shapes (Shape *s)
{
	inline int f (Shape *s1, Shape *s2) {
		s2->clear(s2);
		return 0;
	}

	run_two_side_list(s, f);
}

static inline void
redraw_all_shapes (Shape *s)
{
	inline int f (Shape *s1, Shape *s2) {
		s2->redraw(s2);
		return 0;
	}

	run_two_side_list(s, f);
}

inline int
abs (int n)
{
	return (n < 0 ? n * -1 : n);
}

static inline void
shape_run_body (Shape *s)
{
	int i, j, x, y, p, m, n;
	int k, d, dg, dl, xb, yb;
	int old_mn = s->mn;
	int in_screen_am;
	Shape *next;
	Shape *l[40];
	void (*fg)(Velocity *);
	void (*fl)(Velocity *);

	s->v.up_v( &(s->v), s->v.ax, s->v.ay );
	s->pre_p.set( &(s->pre_p), s->p.x, s->p.y );

	if ( s->direct_move(s, s->v.dx, s->v.dy) ) {
		return;
	}

	in_screen_am = in_screen_after_move(s, s->v.dx, s->v.dy);
	if ( in_screen_am ) {
		if (s->type == 2) {
			s->mn = get_move_box_mn(s, s->v.dx, s->v.dy);
		} else {
			s->mn = get_move_circle_mn(s, s->v.dx, s->v.dy);
		}

		i = 0;
		for (next = s->next; next != s; next = next->next) {
			if (s->same_space(s, next)) {
				l[i++] = next;
			}
		}
		s->mn = old_mn;
		l[i] = NULL;
	}

	x  = abs(s->v.dx);
	y  = abs(s->v.dy);

	xb = x > y;
	yb = y >= x;

	n  = (xb ? y : x);

	dg = ( xb ? (s->v.dx < 0 ? -1 : 1) : ( s->v.dy < 0 ? -1 : 1 ) );
	dl = ( yb ? (s->v.dx < 0 ? -1 : 1) : ( s->v.dy < 0 ? -1 : 1 ) );

	fg = ( xb ? s->v.reflect_x : s->v.reflect_y );
	fl = ( yb ? s->v.reflect_x : s->v.reflect_y );

	if (x == 0) {
		d = ( s->v.dy < 0 ? -1 : 1 ); 
		for (i = 0; i < y; i++) {
			if ( ! (in_screen_am ? move_if_in_screen(s, 0, d, l) : s->move(s, 0, d)) ) {
				s->v.reflect_y( &(s->v) );
				d *= -1 * s->v.reflectable;
			}
		}

		if (s->breakable == -1) {
			return;

		} 
	}
	else if (y == 0) {
		d = ( s->v.dx < 0 ? -1 : 1 );
		for (i = 0; i < x; i++) {
			if ( ! (in_screen_am ? move_if_in_screen(s, d, 0, l) : s->move(s, d, 0)) ) {
				s->v.reflect_x( &(s->v) );
				d *= -1 * s->v.reflectable;
			}
		}

		if (s->breakable == -1) {
			return;
		}
	}
	else {
		p = ( xb ? Div(x, y)    : Div(y, x) );
		m = ( xb ? DivMod(x, y) : DivMod(y, x) );

		for (i = 0; i < n; i++, m--) {
			if ( ! (in_screen_am ? move_if_in_screen(s, dl * !xb, dl * !yb, l) : s->move(s, dl * !xb, dl * !yb)) ) {
				fl( &(s->v) );
				dl *= -1 * s->v.reflectable;

				if ( ! (in_screen_am ? move_if_in_screen(s, dl * !xb, dl * !yb, l) : s->move(s, dl * !xb, dl * !yb)) ) {
					fl( &(s->v) );
					dl *= -1 * s->v.reflectable;
					fg( &(s->v) );
					dg *= -1 * s->v.reflectable;
				}
			}

			k = (m > 0 ? 1 : 0);
			for (j = 0; j < (p + k); j++) {
				if ( ! (in_screen_am ? move_if_in_screen(s, dg * xb, dg * yb, l) : s->move(s, dg * xb, dg * yb)) ) {
					fg( &(s->v) );
					dg *= -1 * s->v.reflectable;

					if ( ! (in_screen_am ? move_if_in_screen(s, dg * xb, dg * yb, l) : s->move(s, dg * xb, dg * yb)) ) {
						fg( &(s->v) );
						dg *= -1 * s->v.reflectable;
						fl( &(s->v) );
						dl *= -1 * s->v.reflectable;
					}
				}
			}

			if (s->breakable == -1) {
				return;
			}
		}
	}
}

static int
shape_run (Shape *s)
{
	int i = 1;
	Shape *n;

	if (s->v.movable) {
		shape_run_body(s);
	}

	for (n = s->next; s != n && n != NULL; n = n->next) {
		i++;

		if (n->v.movable && n->breakable != -1) {
			shape_run_body(n);
		}
	}
	
	return i;
}

