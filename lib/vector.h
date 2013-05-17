#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "point.h"

typedef struct vector {
	int x, y;

	void (*set)(struct vector *, Point *p1, Point *p2);

	int  (*length2)(struct vector *);
	int  (*inner)(struct vector *, struct vector *);
	int  (*outer)(struct vector *, struct vector *);
} Vector;

extern void new_Vector(Vector *);

extern void set_vector(Vector *, Point *, Point *);

extern int vector_length2(Vector *);
extern int inner_product(Vector *, Vector *);
extern int outer_product(Vector *, Vector *);
#endif
