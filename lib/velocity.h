#ifndef _VELOCITY_H_
#define _VELOCITY_H_

typedef struct velocity {
	int dx;
	int dy;
	int ax;
	int ay;

	int reflectable;

	void (*set_v)(struct velocity *, int, int);
	void (*set_a)(struct velocity *, int, int);

	void (*reflect_x)(struct velocity *);
	void (*reflect_y)(struct velocity *);
} Velocity;

void new_Velocity (Velocity *);

void set_velocity (Velocity *, int, int);
void set_accelerate (Velocity *, int, int);

void velocity_reflect_x (Velocity *);
void velocity_reflect_y (Velocity *);

#endif
