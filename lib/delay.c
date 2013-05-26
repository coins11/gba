#include "gba.h"
#include "delay.h"

inline void
delay(hword val)
{
	val += gba_register(TMR_COUNT0);
	while (val != gba_register(TMR_COUNT0));
}

inline void
wait_until_vblank(void) {
	while ((gba_register(LCD_STATUS) & 1) == 0);
}

inline void
wait_while_vblank(void) {
	while ((gba_register(LCD_STATUS) & 1));
}

