#include "TFT.h"
#include "captureimage.h"
#include "config.h"
#include "twicam.h"
#include "camregdef.h"
#include <stdint.h>
#include <util/delay.h>
static uint8_t point=0;
inline void graph(uint8_t y,uint8_t x,uint16_t col)
{
	y=y*15/32;
	tft_setXY(239-y,319-x);
	tft_sendData(col);
}
uint16_t pickSel(uint8_t sel)
{
		return sel==point ? RED:WHITE;
}
void redrawGraph(void)
{
	graph(rdReg(0x7b),2,pickSel(0));
	graph(rdReg(0x7c),4,pickSel(1));
	graph(rdReg(0x7d),8,pickSel(2));
	graph(rdReg(0x7e),15,pickSel(3));
	graph(rdReg(0x7f),19,pickSel(4));
	graph(rdReg(0x80),23,pickSel(5));
	graph(rdReg(0x81),26,pickSel(6));
	graph(rdReg(0x82),30,pickSel(7));
	graph(rdReg(0x83),34,pickSel(8));
	graph(rdReg(0x84),38,pickSel(9));
	graph(rdReg(0x85),45,pickSel(10));
	graph(rdReg(0x86),53,pickSel(11));
	graph(rdReg(0x87),68,pickSel(12));
	graph(rdReg(0x88),83,pickSel(13));
	graph(rdReg(0x89),98,pickSel(14));
	graph(256-(rdReg(0x7A)*3/4),120,GREEN);
}
void gammaEdit(void)
{
	uint16_t x,y,z;
	setRes(qqvga);
	tft_paintScreenBlack();
	do{
		getPoint(&x,&y,&z);
		tft_setOrientation(1);
		wrReg(REG_COM13, COM13_UVSAT|COM13_RSVD);
		capImgqqvga(160);
		wrReg(REG_COM13, COM13_GAMMA|COM13_UVSAT|COM13_RSVD);
		capImgqqvga(0);
		tft_setDisplayDirect(DOWN2UP);
		redrawGraph();
		//tft_fillRectangle(120,320,80,160,BLACK);
		//tft_drawHorizontalLine(120,319,119,WHITE);
		//tft_drawVerticalLine(239,160,120,WHITE);
		
	}while(z<10);
	setRes(qvga);
	
}
