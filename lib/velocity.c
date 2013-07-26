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

	v->up_v  = up_velocity;
	v->up_a  = up_accelarate;

	v->down_v = down_velocity;
	v->down_a = down_accelarate;

	v->reflect_x = velocity_reflect_x;
	v->reflect_y = velocity_reflect_y;
}

inline void
set_velocity (Velocity *v, int x, int y)
{
	v->dx = x;
	v->dy = y;

	if (x || y) {
		v->movable = 1;
	}
}

inline void
set_accelerate (Velocity *v, int x, int y)
{
	v->ax = x;
	v->ay = y;

	if (x || y) {
		v->movable = 1;
	}
}

inline void
up_velocity (Velocity *v, int x, int y)
{
	v->set_v(v, v->dx + x, v->dy + y);
}

inline void
up_accelarate (Velocity *v, int x, int y)
{
	v->set_a(v, v->ax + x, v->ay + y);
}

inline void
down_velocity (Velocity *v, int x, int y)
{
	v->up_v(v, x * (v->dx > 0 ? -1 : 1) * (v->dx ? 1 : 0), y * (v->dy > 0 ? -1 : 1) * (v->dy ? 1 : 0));
}

inline void
down_accelarate (Velocity *v, int x, int y)
{
	v->up_a(v, x * (v->ax > 0 ? -1 : 1) * (v->ax ? 1 : 0), y * (v->ay > 0 ? -1 : 1) * (v->ay ? 1 : 0));
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

