#include "gba.h"
#include "point.h"
#include "velocity.h"
#include "shape.h"
#include "vector.h"
#include "touch.h"
	
inline int
touch_c2b (Shape *c, Shape *b)
{
	Vector pq, pm, qm;
	int i, cross, d;
	int n[][4] = {
		{0, 1, 3, 2},
		{1, 3, 2, 0}
	};
	int r = c->as.circle.r;

	if ( c->p.x < b->as.box.apex[0].x - r || 
		 c->p.x > b->as.box.apex[1].x + r ||
		 c->p.y < b->as.box.apex[0].y - r ||
		 c->p.y > b->as.box.apex[2].y + r ) {
		return 0;
	}

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

inline int
touch_two_shapes(Shape *s1, Shape *s2)
{
	if ( s1 == s2 || !s1->same_space(s1, s2) ) {
		return 0;
	}
	else if (s1->type == 1 && s2->type == 1) {
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
	}
	else {
		return 0;
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

inline int
touch_shapes (Shape *s)
{
	Shape *n;

	n = run_two_side_list(s, touch_two_shapes);

	if (n != NULL) {
		if ( s->touch_callback != NULL && s->touch_callback(s, n) ) {
			return 0;
		}

		if (n->touch_callback != NULL) {
			n->touch_callback(n, s);
		}

		if (s->breakable) {
			break_shape(s);
		}

		if (n->breakable) {
			break_shape(n);
		}

		return 1;
	} else {
		return 0;
	}
}

inline int
move_touch_test_of_box (Shape *s, int x, int y)
{
	Box *b     = &(s->as.box);
	int old_mn = s->mn;
	int x1, y1, x2, y2;

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

	s->mn = mn_index(x1, y1, x2, y2);

	inline int f (Shape *s1, Shape *s2) {
		return s1 != s2 && s1->same_space(s1, s2);
	}

	s->mn = old_mn;

	if (run_two_side_list(s, f) == NULL) {
		return 1;
	} else {
		return 0;
	}
}

inline int
move_touch_test_of_circle (Shape *s, int x, int y)
{
	Circle *c  = &(s->as.circle);
	int old_mn = s->mn;
	int x1, y1, x2, y2;

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

	s->mn = mn_index(x1, y1, x2, y2);

	int f (Shape *s1, Shape *s2) {
		return s1 != s2 && s1->same_space(s1, s2);
	}

	s->mn = old_mn;

	if (run_two_side_list(s, f) == NULL) {
		return 1;
	} else {
		return 0;
	}
}

