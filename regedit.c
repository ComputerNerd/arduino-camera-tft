#define F_CPU 16000000UL //16mhz
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/io.h>
#include "regedit.h"
#include "config.h"
#include "TFT.h"
#include "captureimage.h"
#include "twicam.h"
#include "TouchScreen.h" 
static uint16_t leadingZeros(char * buf,uint8_t x){
	uint8_t len=strlen((const char *)buf);
	uint16_t len2=320;
	len=8-len;
	while (len--){
		tft_drawChar('0',x, len2, 4, WHITE);
		len2-=32;
	}
	return len2;
}
#ifdef MT9D111
static void redrawT(uint8_t z,uint16_t regD,uint8_t micro,uint8_t id,uint8_t bitm8)
#else
static void redrawT(uint8_t z,uint8_t regD)
#endif
{
	char buf[16];
	tft_fillRectangle(0,320,112,320,BLACK);
	#ifdef MT9D111
	if(micro)
		tft_drawStringP(PSTR("Reg,id:"),0,320,2,WHITE);
	else
	#endif
	tft_drawStringP(PSTR("Register:"),0,320,2,WHITE);
	utoa(z,buf,16);
	#ifdef MT9D111
		if(micro)
			tft_drawString(buf,0,208,2,WHITE);
		else
	#endif
	tft_drawString(buf,0,176,2,WHITE);
	utoa(regD,buf,16);
	tft_drawString(buf,16,320,2,WHITE);
	utoa(regD,buf,10);
	tft_drawString(buf,32,320,2,WHITE);
	#ifdef MT9D111
		if(micro){
			utoa(id,buf,16);
			tft_drawChar(',',0,176,2,WHITE);
			tft_drawString(buf,0,160,2,WHITE);
		}
		utoa(regD>>8,buf,2);
		tft_drawString(buf,48,leadingZeros(buf,48),4,WHITE);
		utoa(regD&255,buf,2);
		tft_drawString(buf,80,leadingZeros(buf,80),4,WHITE);  
	#else
		utoa(regD,buf,2);
		tft_drawString(buf,80,leadingZeros(buf,80),4,WHITE);
	#endif
}
#ifdef MT9D111
static uint16_t rdRegE(uint8_t address,uint8_t microEdit,uint8_t maddr,uint8_t bitm8){
	if(microEdit){
		wrReg16(0xC6,address|(1<<13)|(maddr<<8)|(bitm8<<15));
		return rdReg16(0xC8);
	}
	else
		return rdReg16(address);
}
static uint16_t updateReg(uint8_t address,uint8_t microEdit,uint8_t write,uint16_t newVal,uint8_t maddr,uint8_t bitm8){
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
static uint8_t changeId(int oldid,int8_t change){
	oldid+=change;
	if(oldid<0)
		oldid=26;
	if((oldid>=12)&&(oldid<=16))
		oldid=16;
	if(oldid>26)
		oldid=0;
	return oldid;
}
#endif
#ifdef MT9D111
static void redrawGUI(uint8_t mico)
#else
static void redrawGUI(void)
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
#ifdef MT9D111
void editRegs(uint8_t microedit)
#else
void editRegs(void)
#endif
{
	uint16_t x,y,z;
	#ifdef MT9D111
		uint16_t val;
		wrReg16(0xF0,2);
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
					char buf[8];
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
					#ifdef MT9D111
						if(address==255)
							maddr=changeId(maddr,1);
					#endif
					++address;
					#ifdef MT9D111
						if((address==0xF1)&&(microedit==0))
							++address;
						val=updateReg(address,microedit,0,0,maddr,bitm8);
					#else
						val=rdReg(address);
						redrawT(address,val);
					#endif
				}else if (y <= 224 && y >= 192){
					#ifdef MT9D111
						if((address==0)&&microedit)
							maddr=changeId(maddr,-1);
						else
					#endif
					--address;
					#ifdef MT9D111
						if((address==0xF1)&&(microedit==0))
							--address;
						val=updateReg(address,microedit,0,0,maddr,bitm8);
					#else
						val=rdReg(address);
						redrawT(address,val);
					#endif
				}
			}
		}
		#ifdef MT9D111
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
		}
	}//end of loop
}
