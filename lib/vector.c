#include "gba.h"
#include "point.h"
#include "vector.h"

void
new_Vector (Vector *v) {
	v->set = set_vector;

	v->length2 = vector_length2;

	v->inner = inner_product;
	v->outer = outer_product;
}

inline void
set_vector (Vector *v, Point *p1, Point *p2)
{
	v->x = p2->x - p1->x;
	v->y = p2->y - p1->y;
}

inline int
vector_length2 (Vector *v)
{
	return v->inner(v, v);
}

inline int
inner_product(Vector *v1, Vector *v2){
	return (v1->x * v2->x + v1->y * v2->y);
}

inline int
outer_product(Vector *v1, Vector *v2){
	return (v1->x * v2->y - v1->y * v2->x);
}
