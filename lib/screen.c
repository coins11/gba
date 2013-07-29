#include "gba.h"
#include "point.h"
#include "screen.h"

inline int
touch (Point *p1, Point *p2, int r) {
	return p1->distance(p1, p2) < r;
}

inline int
int_touch (int x0, int y0, int x1, int y1, int r)
{
	Point p1, p2;

	p1.x = x0;
	p1.y = y0;
	p2.x = x1;
	p2.y = y1;

	return touch(&p1, &p2, r);
}

inline int
in_screen (int x, int y)
{
	return x >= 0 && x <= LCD_WIDTH && y >= 0 && y <= LCD_HEIGHT;
}

inline int
on_side_area_border (int x)
{
	return x <= 0 || x >= LCD_WIDTH;
}

inline int
on_updown_area_border (int y)
{
	return y <= 0 || y >= LCD_HEIGHT;
}

inline int
on_area_border (int x, int y)
{
	return on_side_area_border(x) || on_updown_area_border(y);
}

inline int
x_overfull (int x)
{
	if ( on_side_area_border(x) ) {
		return 0;
	} else {
		return (x < 0 ? x : x - LCD_WIDTH);
	}
}

inline int
y_overfull (int y)
{
	if ( on_updown_area_border(y) ) {
		return 0;
	} else {
		return (y < 0 ? y : y - LCD_WIDTH);
	}
}
