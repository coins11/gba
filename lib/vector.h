#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "point.h"

typedef struct vector {
	int x, y;

	void (*set)(struct vector *, Point *p1, Point *p2);

	int  (*length2)(struct vector *);
	int  (*inner)(struct vector *, struct vector *);
	int  (*outer)(struct vector *, struct vector *);

	void (*add) (struct vector *, struct vector *);
	void (*sub) (struct vector *, struct vector *);
	void (*mul) (struct vector *, int);
} Vector;

extern void new_Vector(Vector *);

extern inline void set_vector (Vector *, Point *, Point *);

extern inline void add_vector (Vector *, Vector *);
extern inline void sub_vector (Vector *, Vector *);
extern inline void mul_vector (Vector *, int);

extern inline int vector_length2 (Vector *);
extern inline int inner_product (Vector *, Vector *);
extern inline int outer_product (Vector *, Vector *);
#endif
