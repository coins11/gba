#include "gba.h"
#include "point.h"
#include "screen.h"

int
touch (Point *p1, Point *p2, int r) {
	return p1->distance(p1, p2) < r;
}

int
int_touch (int x0, int y0, int x1, int y1, int r)
{
	Point p1, p2;

	p1.x = x0;
	p1.y = y0;
	p2.x = x1;
	p2.y = y1;

	return touch(&p1, &p2, r);
}

int
in_screen (int x, int y)
{
	return x >= 0 && x <= LCD_WIDTH && y >= 0 && y <= LCD_HEIGHT;
}

int
on_side_area_border (int x)
{
	return x <= 0 || x >= LCD_WIDTH;
}

int
on_updown_area_border (int y)
{
	return y <= 0 || y >= LCD_HEIGHT;
}

int
on_area_border (int x, int y)
{
	return on_side_area_border(x) || on_updown_area_border(y);
}

