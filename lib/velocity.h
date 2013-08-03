#ifndef _VELOCITY_H_
#define _VELOCITY_H_

typedef struct velocity {
	int dx;
	int dy;
	int ax;
	int ay;

	int reflectable;
	int movable;

	void (*set_movable) (struct velocity *);

	void (*set_v) (struct velocity *, int, int);
	void (*set_a) (struct velocity *, int, int);

	void (*up_v) (struct velocity *, int, int);
	void (*up_a) (struct velocity *, int, int);

	void (*down_v) (struct velocity *, int, int);
	void (*down_a) (struct velocity *, int, int);

	void (*reflect_x) (struct velocity *);
	void (*reflect_y) (struct velocity *);
} Velocity;

extern inline void new_Velocity (Velocity *);

static inline void set_movable (Velocity *);

static inline void set_velocity (Velocity *, int, int);
static inline void set_accelerate (Velocity *, int, int);

static inline void up_velocity (Velocity *, int, int);
static inline void up_accelarate (Velocity *, int, int);

static inline void down_velocity (Velocity *, int, int);
static inline void down_accelarate (Velocity *, int, int);

static inline void velocity_reflect_x (Velocity *);
static inline void velocity_reflect_y (Velocity *);

#endif
