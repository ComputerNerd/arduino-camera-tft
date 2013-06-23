#include "TFT.h"
#include "TouchScreen.h"
#include "twicam.h"
#include "camregdef.h"
#include "config.h"
#include <avr/pgmspace.h>
#include <util/delay.h>
#ifndef MT9D11
void setmatrix(uint8_t id)
{
	switch (id) {
		case 0:
			wrReg(MTX1,0x80);        
			wrReg(MTX2,0x80);      
			wrReg(MTX3,0x00);        
			wrReg(MTX4,0x22);        
			wrReg(MTX5,0x5e);        
			wrReg(MTX6,0x80);        
			wrReg(MTXS,0x9e); 
		break;
		case 1:
			wrReg(MTX1,0x40);        
			wrReg(MTX2,0x34);      
			wrReg(MTX3,0x0c);        
			wrReg(MTX4,0x17);
			wrReg(MTX5,0x29);        
			wrReg(MTX6,0x40);
			wrReg(MTXS,0x1A);
		break;
		case 2:
			wrReg(MTX1,0xB3);        
			wrReg(MTX2,0xB3);      
			wrReg(MTX3,0x0);        
			wrReg(MTX4,0x3D);        
			wrReg(MTX5,0xA7);        
			wrReg(MTX6,0xE4);
			wrReg(MTXS,0x9E);
		break;
	}
}
#endif
const char menu0[] PROGMEM = "Register edit";
#ifdef MT9D111
const char menu1[] PROGMEM = "Reg edit micro";
#else
const char menu1[] PROGMEM = "Pick a matrix";
#endif
const char menu2[] PROGMEM = "Reset Reg";
const char menu3[] PROGMEM = "Compare Matrix";
const char menu4[] PROGMEM = "qqvga preview";
const char menu5[] PROGMEM = "gamma edit";
const char menu6[] PROGMEM = "hq capture";
const char menu7[] PROGMEM = "White balance";
const char menu8[] PROGMEM = "Image to PC";
const char *const menu_table[] PROGMEM = {
	menu0,menu1,menu2,menu3,
	menu4,menu5,menu6,menu7,
	menu8
};
const char maxtrix0[] PROGMEM = "Maxtrix yuv422";
const char maxtrix1[] PROGMEM = "Maxtrix 2";
const char maxtrix2[] PROGMEM = "Maxtrix rgb565";
const char *const maxtrix_table[] PROGMEM = {maxtrix0,maxtrix1,maxtrix2};
const char wb0[] PROGMEM="No AWB";
const char wb1[] PROGMEM="Advanced AWB";
const char wb2[] PROGMEM="Simple AWB";
const char wb3[] PROGMEM="Sunny";
const char wb4[] PROGMEM="Cloudy";
const char wb5[] PROGMEM="Office";
const char wb6[] PROGMEM="Home";
const char *const wb_table[] PROGMEM={wb0,wb1,wb2,wb3,wb4,wb5,wb6};
const char res0[] PROGMEM="VGA";
const char res1[] PROGMEM="QVGA";
const char *const res_tab[] PROGMEM={res0,res1};
uint8_t selection(const char ** table,uint8_t maxitems)
{
	uint8_t item;
	uint16_t x,y,z;
	z=31<<8;
	tft_setOrientation(1);
	tft_setXY(0,0);
	CS_LOW;
    RS_HIGH;
    RD_HIGH;
    DDRA=0xFF;
	for(item=0;item<240;++item){
		for(x=0;x<320;++x){
			WR_HIGH;
			PORTA=0;
			WR_LOW;
			WR_HIGH;
			PORTA=z>>8;
			WR_LOW;
		}
		z-=31;
	}
	tft_setDisplayDirect(DOWN2UP);
	item=0;
	for(x=0;x<maxitems*24;x+=24){
		tft_drawStringP((PGM_P)pgm_read_word(&(table[item])),x,320,3,WHITE);
		++item;
	}
	while(1){
		do{
			getPoint(&x,&y,&z);
		}while(z<10);
		item=x/24;
		if(item<maxitems)
			break;
	}
	tft_paintScreenBlack();
	return item;
}
void menu(void)
{
	uint16_t x,y,z;
	while (1){
		switch (selection((const char**)menu_table,9)) {
			case 0:
			#ifdef MT9D111
				editRegs(0);
			#else
				setColor(rgb565);
				setRes(qvga);
				editRegs();
			#endif
		break;
		case 1:
			#ifdef MT9D111
				editRegs(1);
			#else
				setmatrix(selection((const char**)maxtrix_table,3));
				tft_setOrientation(1);
				capImg();
				tft_setDisplayDirect(DOWN2UP);
			#endif
		break;
		case 2:
			initCam(0);
		break;
		#ifndef MT9D111
		case 3:
			//compare matrixes
			tft_setOrientation(1);
			do {
				getPoint(&x,&y,&z);
				uint8_t a;
				for (a=0;a<3;a++){
					setmatrix(a);
					capImg();
				}
			}while (z < 10);
			tft_setDisplayDirect(DOWN2UP);
		break;
		#endif
			case 4:
				setColor(rgb565);
				setRes(qqvga);
				do{
					getPoint(&x,&y,&z);
					tft_setOrientation(1);
					capImgqqvga(160);
					tft_setDisplayDirect(DOWN2UP);
				}while(z<10);
				setRes(qvga);
			break;
			case 5:
				setColor(rgb565);
				gammaEdit();
			break;
			case 6:
				initCam(1);
				setRes(vga);
				_delay_ms(200);
				wrReg(0x11,14);
				do{
					getPoint(&x,&y,&z);
					tft_setOrientation(1);
					capImghq();
					tft_setDisplayDirect(DOWN2UP);
				}while(z<10);
				initCam(0);
				setRes(qvga);
				setColor(rgb565);
			break;
			case 7:
				{uint8_t pick=selection((const char**)wb_table,7);//registers from http://thinksmallthings.wordpress.com/2012/10/25/white-balance-control-with-ov7670/
				if(pick==1||pick==2){
					wrReg(0x13, 0xE7);
					wrReg(0x6F, 0x9E|(pick&1));
				}else{
					wrReg(0x13, 0xE5);
					switch(pick){
						case 2:
							wrReg(0x01, 0x5A);
							wrReg(0x02, 0x5C);
						break;
						case 3:
							wrReg(0x01, 0x58);
							wrReg(0x02, 0x60);
						break;
						case 4:
							wrReg(0x01, 0x84);
							wrReg(0x02, 0x4C);
						break;
						case 5:
							wrReg(0x01, 0x96);
							wrReg(0x02, 0x40);
						break;
					}
				}
				tft_setOrientation(1);
				setRes(qvga);
				setColor(rgb565);
				capImg();
				tft_setDisplayDirect(DOWN2UP);}
			break;
			case 8:
				/*initCam(0);
				_delay_ms(200);
				wrReg(0x11,2);*/
				{uint8_t reso=selection((const char**)res_tab,2);
				if(reso)
					setRes(qvga);
				else
					setRes(vga);
				setColor(yuv422);
				_delay_ms(200);
				if(reso)
					wrReg(0x11,1);
				else
					wrReg(0x11,3);
				tft_setOrientation(1);
				do{
					if(reso)
						capImgPCqvga();
					else
						capImgPC();
					getPoint(&x,&y,&z);
				}while(z<10);
				tft_setDisplayDirect(DOWN2UP);}
			break;
		}
	}
}
