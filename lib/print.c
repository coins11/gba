#include "gba.h"
#include "8x8.til"
#include "print.h"

#define FONT_SIZE       8

inline void
draw_char (hword *ptr, int code, hword color)
{
	hword *p;
	int i, j;
	unsigned char *font = char8x8[code];

	for (i = 0; i < FONT_SIZE; i++) {
		p = ptr + LCD_WIDTH * i;
		for (j = FONT_SIZE - 1; j >= 0; j--, p++) {
			if (font[i] & (1 << j))
				*p = color;
		}
	}
}

inline void
draw_str (char *str, int x, int y, hword color) {
	hword *base = (hword *)VRAM;
	int i;

	for (i = 0; str[i] != '\0'; i++) {
		draw_char( (base + x + (y * LCD_WIDTH) + (i * FONT_SIZE)), (int)str[i], color );
	}
}

