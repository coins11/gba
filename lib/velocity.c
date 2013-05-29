#include "gba.h"
#include "point.h"
#include "velocity.h"

inline void
new_Velocity (Velocity *v)
{
	v->dx = 0;
	v->dy = 0;
	v->ax = 0;
	v->ay = 0;

	v->reflectable = 1;
	v->movable     = 0;

	v->set_v  = set_velocity;
	v->set_a  = set_accelerate;

	v->reflect_x = velocity_reflect_x;
	v->reflect_y = velocity_reflect_y;
}

inline void
set_velocity (Velocity *v, int x, int y)
{
	v->dx = x;
	v->dy = y;

	if (x && y) {
		v->movable = 1;
	}
}

inline void
set_accelerate (Velocity *v, int x, int y) {
	v->ax = x;
	v->ay = y;

	if (x && y) {
		v->movable = 1;
	}
}

inline void
velocity_reflect_x (Velocity *v)
{
	if (v->reflectable) {
		v->set_v(v, -(v->dx), v->dy );
	}
}

inline void
velocity_reflect_y (Velocity *v)
{
	if (v->reflectable) {
		v->set_v(v, v->dx, -(v->dy) );
	}
}

