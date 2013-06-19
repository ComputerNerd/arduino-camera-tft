#define F_CPU 16000000UL //16mhz
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "TouchScreen.h" 
#include "TFT.h"
#include "config.h"
#include "twicam.h"
#include "captureimage.h"
#include "camregdef.h"
#include "gammaedit.h"
#include "selections.h"
void menu(void);
uint16_t leadingZeros(uint8_t x);
//undocumented rgister 1F is called LAEC this means exposures less than one line
void main(void)
{
	ADCSRA=(1<<ADEN)|(1<<ADPS2);//enable ADC
	DDRL|=8;
	ASSR &= ~(_BV(EXCLK) | _BV(AS2));
	//generate 8mhz clock
	TCCR5A =67;
	TCCR5B=17;
	OCR5A = 0;
	DDRC=0;
	DDRG|=1<<5;
	uint8_t error;
	tft_init();  //init TFT library
	tft_setDisplayDirect(DOWN2UP);
	#ifdef MT9D111
		tft_drawStringP(PSTR("MT9D111"),20,240,4,BLUE);
	#else
		tft_drawStringP(PSTR("OV7670"),20,240,4,BLUE);
	#endif
	tft_drawStringP(PSTR("Test"),60,220,4,RED);
	cli();
	TWSR&=~3;//disable prescaler for TWI
	TWBR=72;//set to 100khz
	initCam(0);
	tft_drawStringP(PSTR("Starting"),120,300,4,WHITE);
	menu();
}

#ifdef MT9D111
void redrawT(uint8_t z,uint16_t regD,uint8_t micro,uint8_t id,uint8_t bit8)
#else
void redrawT(uint8_t z,uint8_t regD)
#endif
{
	tft_fillRectangle(0,320,112,320,BLACK);
	#ifdef MT9D111
	if(micro)
		tft_drawStringP(PSTR("Reg,id:"),0,320,2,WHITE);
	else
	#endif
	tft_drawStringP(PSTR("Register:"),0,320,2,WHITE);
	utoa(z,(char *)buf,16);
	tft_drawString((char *)buf,0,176,2,WHITE);
	utoa(regD,(char *)buf,16);
	tft_drawString((char *)buf,16,320,2,WHITE);
	utoa(regD,(char *)buf,10);
	tft_drawString((char *)buf,32,320,2,WHITE);
	#ifdef MT9D111
		utoa(regD>>8,(char *)buf,2);
		tft_drawString((char *)buf,48,leadingZeros(48),4,WHITE);
		utoa(regD&255,(char *)buf,2);
		tft_drawString((char *)buf,80,leadingZeros(80),4,WHITE);  
	#else
		utoa(regD,(char *)buf,2);
		tft_drawString((char *)buf,80,leadingZeros(80),4,WHITE);
	#endif
}
#ifdef MT9D111
void editRegs(uint8_t microedit)
#else
void editRegs(void)
#endif
{
	uint16_t x,y,z;
	#ifdef MT9D111
		uint16_t val;
	#else
		uint8_t val;
	#endif
	uint8_t address=3;
	#ifdef MT9D111
		uint8_t bitm8=0;
		uint8_t maddr=0;
	#endif
	#ifdef MT9D111
		redrawGUI(microedit);
	#else
		redrawGUI();
	#endif
	#ifdef MT9D111
		if(microedit){
			wrReg16(0xC6,address|(1<<13)||(maddr<<8)|(bitm8<<15));
			val=rdReg16(0xC8);
		}else
			val=rdReg16(address);
	#else
		val=rdReg(address);
	#endif
  #ifdef MT9D111
  redrawT(address,val,microedit,maddr,bitm8);
  #else
  redrawT(address,val);
  #endif
	while (1){
		getPoint(&x,&y,&z);
		if (z > 10){
			if (x >= 188 && x <= 256 && y <= 128){
				tft_setOrientation(1);
				do {
					capImg();
					//p = ts.getPoint();//wait for screen to be pressed
					getPoint(&x,&y,&z);
				} while (z < 10);
				tft_setDisplayDirect(DOWN2UP);
				#ifdef MT9D111
					redrawGUI(microedit);
				#else
					redrawGUI();
				#endif
				val=rdReg(address);
				redrawT(address,val);
			}
			else if (x >= 188 && x <= 212 && y >= 224)
				return;
			else if (x >= 188 && x <= 212 && y <= 232 && y >= 136){
				#ifdef MT9D111
					uint16_t stp=0;
				#else
					uint8_t stp=0;
				#endif
				do {
					wrReg(address,stp);
					_delay_ms(1000);//give register time to settle
					tft_setOrientation(1);
					capImg();
					utoa(stp,(char *)buf,10);
					tft_drawString((char *)buf,0,320,2,WHITE);
					//p = ts.getPoint();//wait for screen to be pressed
					getPoint(&x,&y,&z);
					stp++;
				} while (z < 10);
				tft_setDisplayDirect(DOWN2UP);
				#ifdef MT9D111
					redrawGUI(microedit);
				#else
					redrawGUI();
				#endif
				val=rdReg(address);
				redrawT(address,val);
			}
    else if (x >= 80 && x <= 112 && y > 64){
      uint8_t off=(y-64)/32;
      off=1<<off;
      val^=off;
      wrReg(address,val);
      val=rdReg(address);
      redrawT(address,val);
    }
    else if (x >= 126 && x <= 156){
     //change register value
     if (y <= 256 && y >= 224){
        val++;
        wrReg(address,val);
        val=rdReg(address);
        redrawT(address,val);
      }
      else if (y <= 224 && y >= 192){
        val--;
        wrReg(address,val);
        val=rdReg(address);
        redrawT(address,val);
      }
      else if (y <= 160 && y >= 136){
        val+=16;
        wrReg(address,val);
        val=rdReg(address);
        redrawT(address,val);
      }
      else if (y <= 112 && y >= 88){
        val-=16;
        wrReg(address,val);
        val=rdReg(address);
        redrawT(address,val);
      }
    }
    else if (x >= 156 && x <= 188){
      if (y <= 256 && y >= 224){
        ++address;
        val=rdReg(address);
        redrawT(address,val);
      }
      else if (y <= 224 && y >= 192){
        --address;
        val=rdReg(address);
        redrawT(address,val);
      }
    }
  }
  uint8_t val2=rdReg(address);
  if (val2 != val){
   val=val2;
   redrawT(address,val);
  }
  }//end of loop
   //tft_paintScreenBlack();
}
uint16_t leadingZeros(uint8_t x)
{
	uint8_t len=strlen((const char *)buf);
	uint16_t len2=320;
	len=8-len;
	while (len--){
		tft_drawChar('0',x, len2, 4, WHITE);
		len2-=32;
	}
	return len2;
}
static uint16_t editer;//current value
static uint16_t minE;
static uint16_t maxE;
void update_edit(const char * text)
{
	
}
