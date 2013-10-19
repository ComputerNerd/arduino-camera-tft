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
//OV7670 undocumented rgister 1F is called LAEC this means exposures less than one line
void main(void){
	ADCSRA=(1<<ADEN)|(1<<ADPS2);//enable ADC
	DDRL|=8;
	ASSR &= ~(_BV(EXCLK) | _BV(AS2));
	//generate 8mhz clock
	TCCR5A=67;
	TCCR5B=17;
	#ifdef MT9D111
	OCR5A=2;// F_CPU/(2*(x+1))
	#else
	OCR5A=0;
	#endif
	DDRC=0;
	DDRG|=1<<5;
	UBRR0H=0;
	UCSR0A|=2;//double speed aysnc
	UBRR0L=3;//0 = 2m 1= 1m 3 = 0.5M 2M baud rate = 0 7 = 250k 207 is 9600 baud rate
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);//Enable receiver and transmitter
	UCSR0C=6;//async 1 stop bit 8bit char no parity bits
	uint8_t error;
	tft_init();//init TFT library
	tft_setDisplayDirect(DOWN2UP);
	cli();
	TWSR&=~3;//disable prescaler for TWI
	TWBR=72;//set to 100khz
	#ifdef ov7670
		configSel();
	#else
		initCam();
	#endif
	menu();
}
#ifdef MT9D111
void redrawT(uint8_t z,uint16_t regD,uint8_t micro,uint8_t id,uint8_t bitm8)
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
	#ifdef MT9D111
		if(micro)
			tft_drawString((char *)buf,0,208,2,WHITE);
		else
	#endif
	tft_drawString((char *)buf,0,176,2,WHITE);
	utoa(regD,(char *)buf,16);
	tft_drawString((char *)buf,16,320,2,WHITE);
	utoa(regD,(char *)buf,10);
	tft_drawString((char *)buf,32,320,2,WHITE);
	#ifdef MT9D111
		if(micro){
			utoa(id,(char*)buf,16);
			tft_drawChar(',',0,176,2,WHITE);
		}
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
uint16_t rdRegE(uint8_t address,uint8_t microEdit,uint8_t maddr,uint8_t bitm8){
	if(microEdit){
		wrReg16(0xC6,address|(1<<13)|(maddr<<8)|(bitm8<<15));
		return rdReg16(0xC8);
	}
	else
		return rdReg16(address);
}
uint16_t updateReg(uint8_t address,uint8_t microEdit,uint8_t write,uint16_t newVal,uint8_t maddr,uint8_t bitm8){
	if(write){
		if(microEdit){
			wrReg16(0xC6,address|(1<<13)|(maddr<<8)|(bitm8<<15));
			wrReg16(0xC8,newVal);
		}else
			wrReg16(address,newVal);
	}
	newVal=rdRegE(address,microEdit,maddr,bitm8);
	redrawT(address,newVal,microEdit,maddr,bitm8);
	return newVal;
}
void editRegs(uint8_t microedit)
#else
void editRegs(void)
#endif
{
	uint16_t x,y,z;
	#ifdef MT9D111
		uint16_t val;
		wrReg16(0xF0,2);
		if(rdReg16(0x0D)!=0){
			tft_fillRectangle(0,320,112,320,BLACK);
			tft_drawStringP(PSTR("Warning 0x0D"),0,320,1,WHITE);
			_delay_ms(1000);
			wrReg16(0x0D,0);
		}
	#else
		uint8_t val;
	#endif
	uint8_t address=0x0D;
	#ifdef MT9D111
		uint8_t bitm8=0;
		uint8_t maddr=0;
		redrawGUI(microedit);
	#else
		redrawGUI();
	#endif
	#ifdef MT9D111
		val=updateReg(address,microedit,0,0,maddr,bitm8);
	#else
		val=rdReg(address);
	#endif
	#ifndef MT9D111
		redrawT(address,val);
	#endif
	while (1){
		getPoint(&x,&y,&z);
		if (z > 10){
			if (x >= 188 && x <= 256 && y <= 128){
				tft_setOrientation(1);
				do {
					capImg();
					getPoint(&x,&y,&z);
				} while (z < 10);
				tft_setDisplayDirect(DOWN2UP);
				#ifdef MT9D111
					redrawGUI(microedit);
				#else
					redrawGUI();
				#endif
				#ifdef MT9D111
					val=updateReg(address,microedit,0,0,maddr,bitm8);
				#else
					val=rdReg(address);
				#endif
				#ifndef MT9D111
					redrawT(address,val);
				#endif
			}else if (x >= 188 && x <= 212 && y >= 224)
				return;
			else if (x >= 188 && x <= 212 && y <= 232 && y >= 136){
				#ifdef MT9D111
					uint16_t stp=0;
				#else
					uint8_t stp=0;
				#endif
				do {
					wrReg(address,stp);
					_delay_ms(100);//give register time to settle
					tft_setOrientation(1);
					capImg();
					utoa(stp,(char *)buf,10);
					tft_drawString((char *)buf,0,320,2,WHITE);
					getPoint(&x,&y,&z);
					++stp;
				} while (z < 10);
				tft_setDisplayDirect(DOWN2UP);
				#ifdef MT9D111
					redrawGUI(microedit);
				#else
					redrawGUI();
				#endif
				#ifdef MT9D111
					val=updateReg(address,microedit,0,0,maddr,bitm8);
				#else
					val=rdReg(address);
				#endif
				#ifndef MT9D111
					redrawT(address,val);
				#endif
			}else if (x >= 80 && x <= 112 && y > 64){
				uint8_t off=(y-64)/32;
				off=1<<off;
				val^=off;
				#ifdef MT9D111
					val=updateReg(address,microedit,1,val,maddr,bitm8);
				#else
					wrReg(address,val);
					val=rdReg(address);
				#endif
				#ifndef MT9D111
					redrawT(address,val);
				#endif
			#ifdef MT9D111
			}else if (x >= 48 && x <= 80 && y > 64){
				uint16_t off=(y-64)/32;
				off=1<<off;
				off<<=8;
				val^=off;
				val=updateReg(address,microedit,1,val,maddr,bitm8);
			#endif
			}else if (x >= 126 && x <= 156){
				//change register value
				if (y <= 256 && y >= 224){
					++val;
					#ifdef MT9D111
						val=updateReg(address,microedit,1,val,maddr,bitm8);
					#else
						wrReg(address,val);
						val=rdReg(address);
					#endif
					#ifndef MT9D111
						redrawT(address,val);
					#endif
				}else if (y <= 224 && y >= 192){
					--val;
					#ifdef MT9D111
						val=updateReg(address,microedit,1,val,maddr,bitm8);
					#else
						wrReg(address,val);
						val=rdReg(address);
					#endif
					#ifndef MT9D111
						redrawT(address,val);
					#endif
				}else if (y <= 160 && y >= 136){
					val+=16;
					#ifdef MT9D111
						val=updateReg(address,microedit,1,val,maddr,bitm8);
					#else
						wrReg(address,val);
						val=rdReg(address);
					#endif
					#ifndef MT9D111
						redrawT(address,val);
					#endif
				}else if (y <= 112 && y >= 88){
					val-=16;
					#ifdef MT9D111
						val=updateReg(address,microedit,1,val,maddr,bitm8);
					#else
						wrReg(address,val);
						val=rdReg(address);
					#endif
					#ifndef MT9D111
						redrawT(address,val);
					#endif
				}
			}else if (x >= 156 && x <= 188){
				if (y <= 256 && y >= 224){
					++address;
					#ifdef MT9D111
						if(address==0xF1)
							++address;
						val=updateReg(address,microedit,0,0,maddr,bitm8);
					#else
						val=rdReg(address);
					#endif
					#ifndef MT9D111
						redrawT(address,val);
					#endif
				}else if (y <= 224 && y >= 192){
					--address;
					#ifdef MT9D111
						if(address==0xF1)
							--address;
						val=updateReg(address,microedit,0,0,maddr,bitm8);
					#else
						val=rdReg(address);
					#endif
					#ifndef MT9D111
						redrawT(address,val);
					#endif
				}
			}
		}
		/*#ifdef MT9D111
			uint16_t val2=rdRegE(address,microedit,maddr,bitm8);
		#else
			uint8_t val2=rdReg(address);
		#endif
		if (val2!=val){
			val=val2;
			#ifdef MT9D111
				redrawT(address,val,microedit,maddr,bitm8);
			#else
				redrawT(address,val);
			#endif
		}*/
	}//end of loop
}
uint16_t leadingZeros(uint8_t x){
	uint8_t len=strlen((const char *)buf);
	uint16_t len2=320;
	len=8-len;
	while (len--){
		tft_drawChar('0',x, len2, 4, WHITE);
		len2-=32;
	}
	return len2;
}
