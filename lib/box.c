#include "gba.h"
#include <string.h>
#include "point.h"
#include "screen.h"
#include "velocity.h"
#include "delay.h"
#include "shape.h"
#include "box.h"

inline void
new_Box (Shape *s)
{
	Box *b = &(s->as.box);

	new_Shape(s);
	new_Point( &(s->p) );
	new_Point( &(s->pre_p) );
	new_Velocity( &(s->v) );

	new_Point( &(b->apex[0]) );
	new_Point( &(b->apex[1]) );
	new_Point( &(b->apex[2]) );
	new_Point( &(b->apex[3]) );

	s->type = 2;

	b->set         = set_data_of_box;
	b->update_apex = update_apex_of_box;
	b->resize      = resize_box;

	s->draw  = draw_box;

	s->in_screen        = box_in_screen;
	s->on_area_border   = box_on_area_border;
	s->on_bottom_border = box_on_bottom_border;
}

inline void
set_data_of_box (Shape *s, int x, int y, int w, int h) {
	Box *b = &(s->as.box);

	s->p.set(&(s->p), x, y);
	b->width  = w;
	b->height = h;

	b->update_apex(s);
}

inline void
update_apex_of_box (Shape *s)
{
	Box *b = &(s->as.box);

	b->apex[0].set( &(b->apex[0]), s->p.x, s->p.y );
	b->apex[1].set( &(b->apex[1]), s->p.x + b->width, s->p.y );
	b->apex[2].set( &(b->apex[2]), s->p.x, s->p.y + b->height );
	b->apex[3].set( &(b->apex[3]), s->p.x + b->width, s->p.y + b->height );
}

inline int
resize_box (Shape *s, int dw, int dh) {
	Box *b = &(s->as.box);

	if ( (b->width + dw) > 0 && (b->height + dh) > 0 ) {
		s->erase(s);

		b->width  += dw;
		b->height += dh;

		b->update_apex(s);
		s->draw(s);

		return 1;
	} else {
		return 0;
	}
}

inline void
draw_box (Shape *s)
{
	Box *b = &(s->as.box);
	hword *base = (hword *)VRAM;
	int x, y;

	for (y = 1; y <= b->height; y++) {
		memset((base + s->p.x + (s->p.y + y) * LCD_WIDTH), s->color, b->width*2);
	}
}

inline int
box_in_screen (Shape *s)
{
	Box *b = &(s->as.box);

	return in_screen(s->p.x, s->p.y) && in_screen(s->p.x + b->width, s->p.y + b->height);
}

inline int
box_on_area_border (Shape *s)
{
	Box *b = &(s->as.box);

	return on_area_border(s->p.x, s->p.y) ||
		on_side_area_border(s->p.x + b->width) ||
		on_updown_area_border(s->p.y + b->height);
}

inline int
box_on_bottom_border (Shape *s)
{
	Box *b = &(s->as.box);

	return on_updown_area_border(s->p.y + b->height);
}
