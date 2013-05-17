#include "gba.h"
#include "string.h"
#include "point.h"
#include "screen.h"
#include "velocity.h"
#include "delay.h"
#include "shape.h"
#include "box.h"

#define COLOR_BLACK     0

void
new_Box (Shape *s)
{
	Box *b = &(s->as.box);

	new_Shape(s);
	new_Point( &(s->p) );
	new_Velocity( &(s->v) );

	new_Point( &(b->apex[0]) );
	new_Point( &(b->apex[1]) );
	new_Point( &(b->apex[2]) );
	new_Point( &(b->apex[3]) );

	s->type = 2;

	b->apex[0].set( &(b->apex[0]), s->p.x, s->p.y );
	b->apex[1].set( &(b->apex[1]), s->p.x + b->width, s->p.y );
	b->apex[2].set( &(b->apex[2]), s->p.x, s->p.y + b->height );
	b->apex[3].set( &(b->apex[3]), s->p.x + b->width, s->p.y + b->height );

	s->draw  = draw_box;
	s->erase = erase_box;

	b->update_apex = update_apex_of_box;

	s->in_screen      = box_in_screen;
	s->on_area_border = box_on_area_border;
}

void
draw_box (Shape *s)
{
	Box *b = &(s->as.box);
	hword *base = (hword *)VRAM;
	int x, y;

	wait_until_vblank();
	for (y = 1; y <= b->height; y++) {
		memset((base + s->p.x + (s->p.y + y) * LCD_WIDTH), s->color, b->width*2);
	}
	wait_while_vblank();
}

void
update_apex_of_box (Shape *s)
{
	Box *b = &(s->as.box);

	b->apex[0].set( &(b->apex[0]), s->p.x, s->p.y );
	b->apex[1].set( &(b->apex[1]), s->p.x + b->width, s->p.y );
	b->apex[2].set( &(b->apex[2]), s->p.x, s->p.y + b->height );
	b->apex[3].set( &(b->apex[3]), s->p.x + b->width, s->p.y + b->height );
}

void
erase_box (Shape *s)
{
	Box *b = &(s->as.box);
	Shape copy_b;

	copy_b = *s;
	copy_b.color = COLOR_BLACK;

	copy_b.draw( &copy_b );
}

int
box_in_screen (Shape *s)
{
	Box *b = &(s->as.box);

	return in_screen(s->p.x, s->p.y) && in_screen(s->p.x + b->width, s->p.y + b->height);
}

int
box_on_area_border (Shape *s)
{
	Box *b = &(s->as.box);

	return on_area_border(s->p.x, s->p.y) ||
		on_side_area_border(s->p.x + b->width) ||
		on_updown_area_border(s->p.y + b->height);
}

