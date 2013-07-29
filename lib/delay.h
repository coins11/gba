#ifndef _DELAY_H_
#define _DELAY_H_

static inline void
delay_init (void) {
	gba_register(TMR_COUNT0) = 0;
	gba_register(TMR_CTRL0)  = TMR_ENABLE + TMR_64CLOCK;
}

static inline void
wait_until (hword time)
{
	while (time != gba_register(TMR_COUNT0));
}

static inline void
delay (hword val)
{
	wait_until(gba_register(TMR_COUNT0) + val);
}

static inline void 
wait_until_vblank (void)
{
	while ((gba_register(LCD_STATUS) & 1) == 0);
}

static inline void 
wait_while_vblank (void)
{
	while ((gba_register(LCD_STATUS) & 1));
}



#endif
