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

	v->set_movable = set_movable;

	v->set_v  = set_velocity;
	v->set_a  = set_accelerate;

	v->up_v  = up_velocity;
	v->up_a  = up_accelarate;

	v->down_v = down_velocity;
	v->down_a = down_accelarate;

	v->reflect_x = velocity_reflect_x;
	v->reflect_y = velocity_reflect_y;
}

static inline void
set_movable (Velocity *v)
{
	if (v->dx || v->dy || v->ax || v->ay) {
		v->movable = 1;
	} else {
		v->movable = 0;
	}
}

static inline void
set_velocity (Velocity *v, int x, int y)
{
	v->dx = x;
	v->dy = y;

	v->set_movable(v);
}

static inline void
set_accelerate (Velocity *v, int x, int y)
{
	v->ax = x;
	v->ay = y;

	v->set_movable(v);
}

static inline void
up_velocity (Velocity *v, int x, int y)
{
	v->set_v(v, v->dx + x, v->dy + y);
}

static inline void
up_accelarate (Velocity *v, int x, int y)
{
	v->set_a(v, v->ax + x, v->ay + y);
}

static inline void
down_velocity (Velocity *v, int x, int y)
{
	v->up_v(v, x * (v->dx > 0 ? -1 : 1) * (v->dx ? 1 : 0), y * (v->dy > 0 ? -1 : 1) * (v->dy ? 1 : 0));
}

static inline void
down_accelarate (Velocity *v, int x, int y)
{
	v->up_a(v, x * (v->ax > 0 ? -1 : 1) * (v->ax ? 1 : 0), y * (v->ay > 0 ? -1 : 1) * (v->ay ? 1 : 0));
}

static inline void
velocity_reflect_x (Velocity *v)
{
	if (v->reflectable) {
		v->set_v(v, -(v->dx), v->dy );
	}
}

static inline void
velocity_reflect_y (Velocity *v)
{
	if (v->reflectable) {
		v->set_v(v, v->dx, -(v->dy) );
	}
}

