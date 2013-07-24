#ifndef _VELOCITY_H_
#define _VELOCITY_H_

typedef struct velocity {
	int dx;
	int dy;
	int ax;
	int ay;

	int reflectable;
	int movable;

	void (*set_v) (struct velocity *, int, int);
	void (*set_a) (struct velocity *, int, int);

	void (*add_v) (struct velocity *, int, int);
	void (*add_a) (struct velocity *, int, int);

	void (*reflect_x) (struct velocity *);
	void (*reflect_y) (struct velocity *);
} Velocity;

extern inline void new_Velocity (Velocity *);

extern inline void set_velocity (Velocity *, int, int);
extern inline void set_accelerate (Velocity *, int, int);

extern inline void add_velocity (Velocity *, int, int);
extern inline void add_accelarate (Velocity *, int, int);

extern inline void velocity_reflect_x (Velocity *);
extern inline void velocity_reflect_y (Velocity *);

#endif
