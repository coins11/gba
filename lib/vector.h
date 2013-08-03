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

extern inline void new_Vector(Vector *);

static inline void set_vector (Vector *, Point *, Point *);

static inline void add_vector (Vector *, Vector *);
static inline void sub_vector (Vector *, Vector *);
static inline void mul_vector (Vector *, int);

static inline int vector_length2 (Vector *);
static inline int inner_product (Vector *, Vector *);
static inline int outer_product (Vector *, Vector *);
#endif
