#include "config.h"
#include "TFT.h"
#include <avr/pgmspace.h>
#ifdef MT9D111
void redrawGUI(uint8_t mico)
#else
void redrawGUI(void)
#endif
{
	tft_paintScreenBlack();
	tft_drawStringP(PSTR("val"),124,320,2,WHITE);
	tft_drawStringP(PSTR("+"),124,256,4,WHITE);
	tft_drawStringP(PSTR("-"),124,224,4,WHITE);
	tft_drawStringP(PSTR("+16"),124,192,3,WHITE);
	tft_drawStringP(PSTR("-16"),124,96,3,WHITE);
	tft_drawStringP(PSTR("reg"),156,320,2,WHITE);
	tft_drawStringP(PSTR("+"),156,256,4,WHITE);
	tft_drawStringP(PSTR("-"),156,224,4,WHITE);
	tft_drawStringP(PSTR("Exit"),188,320,3,WHITE);
	tft_drawStringP(PSTR("Step"),188,232,3,WHITE);
	tft_drawStringP(PSTR("Snap"),188,128,4,WHITE);
}
