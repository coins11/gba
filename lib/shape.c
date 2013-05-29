#include "gba.h"
#include "libgba/gba_systemcalls.h"
#include <stdarg.h>
#include "point.h"
#include "vector.h"
#include "velocity.h"
#include "shape.h"
#include "circle.h"
#include "box.h"

#define COLOR_BLACK     0
#define COLOR_WHITE     BGR(31, 31, 31)

inline void
new_Shape (Shape *s)
{
	s->breakable = 0;

	s->pre_p = s->p;

	s->move   = move_shape;
	s->erase  = erase_shape;
	s->redraw = redraw_shape;

	s->draw_all   = draw_all_shapes;
	s->erase_all  = erase_all_shapes;
	s->redraw_all = redraw_all_shapes;

	s->run   = shape_run;

	s->touch = touch_shapes;
}

void
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

int
touch_c2b (Shape *c, Shape *b)
{
	Vector pq, pm, qm;
	int i, cross, d;
	int n[][4] = {
		{0, 1, 3, 2},
		{1, 3, 2, 0}
	};
	int r = c->as.circle.r;

	for (i = 0; i < 4; i++) {
		if ( b->as.box.apex[i].distance( &(b->as.box.apex[i]), &(c->p) ) <= r * r ) {
			return 1;
		}
	}

	new_Vector(&pq);
	new_Vector(&pm);
	new_Vector(&qm);

	for (i = 0; i < 4; i++) {
		pq.set( &pq, &(b->as.box.apex[n[0][i]]), &(b->as.box.apex[n[1][i]]) );
		pm.set( &pm, &(b->as.box.apex[n[0][i]]), &(c->p) );
		qm.set( &qm, &(b->as.box.apex[n[1][i]]), &(c->p) );

		cross = pm.outer(&pm, &pq);
		d     = Div( (cross * cross), pq.length2(&pq) );

		if (d <= r * r) {
			if (pm.inner(&pm, &pq) * qm.inner(&qm, &pq) <= 0) {
				return 1;
			}
		}
	}
	return 0;
}

int
touch_two_shapes(Shape *s1, Shape *s2)
{
	if (!s1->breakable && s2->breakable || s1 == s2) {
		return 0;
	}

	if (s1->type == 1 && s2->type == 1) {
		int r = s1->as.circle.r + s2->as.circle.r;

		return s1->p.distance( &(s1->p), &(s2->p) ) < r * r;
	} 
	else if (s1->type == 1 && s2->type == 2) {
		return touch_c2b(s1, s2);
	}
	else if (s1->type == 2 && s2->type == 1) {
		return touch_c2b(s2, s1);
	}
	else if (s1->type == 2 && s2->type == 2) {
		Box *b1 = &(s1->as.box),
			*b2 = &(s2->as.box);

		return b1->apex[0].x < b2->apex[3].x &&
			   b2->apex[0].x < b1->apex[3].x &&
			   b2->apex[0].y < b1->apex[3].y &&
			   b1->apex[0].y < b2->apex[3].y;
	} else {
		return 0;
	}
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

inline void
break_shape (Shape *s)
{
	s->color = COLOR_BLACK;

	s->prev->next = s->next;
	s->next->prev = s->prev;

	s->next = NULL;
	s->prev = NULL;

	s->draw(s);
	s->erase(s);
}

int
touch_shapes (Shape *s)
{
	Shape *n;

	inline int f (Shape *s1, Shape *s2) {
		return touch_two_shapes(s1, s2);
	}

	n = run_two_side_list(s, f);

	if (n != NULL && s->breakable) {
		break_shape(s);

		if (n->v.reflectable) {
			n->color = BGR(31, 10, 31);
			n->v.reflect_x(&(n->v));
			n->v.reflect_y(&(n->v));
		}
		else if (n->breakable) {
			break_shape(n);
		}

		n->color = COLOR_WHITE;
	}

	return n != NULL;
}

int
move_shape (Shape *s, int x, int y)
{
	s->p.set( &(s->p), s->p.x + x, s->p.y + y );
	
	if ( s->in_screen(s) && !s->touch(s) ) {
		if (s->type == 2) {
			s->as.box.update_apex(s);
		}

		return 1;
	} else {
		s->p.set( &(s->p), s->p.x - x, s->p.y - y );

		return 0;
	}
}

inline void
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

inline void
redraw_shape (Shape *s)
{
	if ( s->p.x != s->pre_p.x || s->p.y != s->pre_p.y ) {
		s->erase(s);
		s->draw(s);
	}
}

inline void
draw_all_shapes (Shape *s)
{
	inline int f (Shape *s1, Shape *s2) {
		s2->draw(s2);
		return 0;
	}

	run_two_side_list(s, f);
}

inline void
erase_all_shapes (Shape *s)
{
	inline int f (Shape *s1, Shape *s2) {
		s2->erase(s2);
		return 0;
	}

	run_two_side_list(s, f);
}

inline void
redraw_all_shapes (Shape *s)
{
	inline int f (Shape *s1, Shape *s2) {
		s2->redraw(s2);
		return 0;
	}

	run_two_side_list(s, f);
}

void
shape_run_body (Shape *s)
{
	int i, d, dx, dy;

	s->v.set_v( &(s->v), s->v.dx + s->v.ax, s->v.dy + s->v.ay );
	s->pre_p.set( &(s->pre_p), s->p.x, s->p.y );

	dx = s->v.dx;
	d  = 1;
	if (dx < 0) {
		d  =  -1;
		dx *= -1;
	}
	for (i = 0; i < dx; i++) {
		if ( !s->move(s, d, 0) ) {
			s->v.reflect_x( &(s->v) );
			d *= -1 * s->v.reflectable;
			s->move(s, d, 0);
		}
	}

	dy = s->v.dy;
	d  = 1;
	if (dy < 0) {
		d  =  -1;
		dy *= -1;
	}
	for (i = 0; i < dy; i++) {
		if ( !s->move(s, 0, d) ) {
			s->v.reflect_y( &(s->v) );
			d *= -1 * s->v.reflectable;
			s->move(s, 0, d);
		}
	}
}

void
shape_run (Shape *s)
{
	Shape *n = s->next;

	shape_run_body(s);

	while (s != n && n != NULL) {
		n->touch(n);
		
		if (n->v.movable) {
			shape_run_body(n);
		}

		n = n->next;
	}
}
