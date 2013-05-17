#include "gba.h"
#include "point.h"
#include "vector.h"
#include "velocity.h"
#include "shape.h"
#include "circle.h"
#include "box.h"

void
new_Shape (Shape *s)
{
	s->move = move_shape;

	s->run  = shape_run;

	s->touch = touch_shapes;
}

int
touch_c2b (Shape *c, Shape *b)
{
	Vector pq, pm, qm;
	int i, cross;
	double d;
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
		d     = (cross * cross) / pq.length2(&pq);

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

int
touch_shapes (Shape *s)
{
	Shape *n = s->next;
	int flag = 0;

	while (s != n && n != NULL) {
		flag = flag || touch_two_shapes(s, n);

		n = n->next;
	}

	return flag;
}

int
move_shape (Shape *s, int x, int y)
{
	Shape copy;

	copy = *s;
	copy.p.set( &(copy.p), copy.p.x + x, copy.p.y + y );
	
	if ( copy.in_screen(&copy) ) {
		s->p.set( &(s->p), s->p.x + x, s->p.y + y );

		if (s->type == 2) {
			s->as.box.update_apex(s);
		}

		return 1;
	} else {
		return 0;
	}
}

void
shape_run_body (Shape *s)
{
	int i, d, dx, dy;

	s->v.set_v( &(s->v), s->v.dx + s->v.ax, s->v.dy + s->v.ay);

	dx = s->v.dx;
	d  = 1;
	if (dx < 0) {
		d  =  -1;
		dx *= -1;
	}
	dx %= LCD_WIDTH; 
	for (i = 0; i < dx; i++) {
		if ( ! s->move(s, d, 0) ) {
			s->v.reflect_x( &(s->v) );
			d *= -1;
			s->move(s, d, 0);
		}
		else if ( s->touch(s) ) {
			s->v.reflect_x( &(s->v) );
			d *= -1;
			s->move(s, d, 0);
		}

	}

	dy = s->v.dy;
	d  = 1;
	if (dy < 0) {
		d  =  -1;
		dy *= -1;
	}
	dy %= LCD_HEIGHT;
	for (i = 0; i < dy; i++) {
		if ( ! s->move(s, 0, d) ) {
			s->v.reflect_y( &(s->v) );
			d *= -1;
			s->move(s, 0, d);
		}
		else if ( s->touch(s) ) {
			s->v.reflect_y( &(s->v) );
			d *= -1;
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
		shape_run_body(n);

		n = n->next;
	}
}
