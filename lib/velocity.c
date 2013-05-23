#include "gba.h"
#include "point.h"
#include "velocity.h"

void
new_Velocity (Velocity *v)
{
	v->dx = 0;
	v->dy = 0;
	v->ax = 0;
	v->ay = 0;

	v->reflectable = 1;

	v->set_v  = set_velocity;
	v->set_a  = set_accelerate;

	v->reflect_x = velocity_reflect_x;
	v->reflect_y = velocity_reflect_y;
}

void
set_velocity (Velocity *v, int x, int y)
{
	v->dx = x;
	v->dy = y;
}

void
set_accelerate (Velocity *v, int x, int y) {
	v->ax = x;
	v->ay = y;
}

void
velocity_reflect_x (Velocity *v)
{
	if (v->reflectable) {
		v->set_v(v, -(v->dx), v->dy );
	}
}

void
velocity_reflect_y (Velocity *v)
{
	if (v->reflectable) {
		v->set_v(v, v->dx, -(v->dy) );
	}
}

