#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "TFT.h"
void capImgqqvga(uint8_t offsetx){
	cli();
	uint8_t w,ww,h;
	uint8_t y=0;
	w=160;
	h=120;
	DDRA=0xFF;
    DDRC=0;
	#ifdef MT9D111
		while (PINE&32){}//wait for low
		while (!(PINE&32)){}//wait for high
	#else
		while (!(PINE&32)){}//wait for high
		while (PINE&32){}//wait for low
	#endif
	do{
		tft_setXY(y,offsetx);
		++y;
		CS_LOW;
		RS_HIGH;
		RD_HIGH;
		ww=w;
		do{
			WR_LOW;
			while (PINE&16){}//wait for low
			PORTA=PINC;
			WR_HIGH;
			#ifndef ov7670
				while (!(PINE&16)){}//wait for high
			#endif
			WR_LOW;
			while (PINE&16){}//wait for low
			PORTA=PINC;
			WR_HIGH;
			#ifndef ov7670
				while (!(PINE&16)){}//wait for high
			#endif
		}while (--ww);
	}while (--h);
	CS_HIGH;
	sei();
}
inline void serialWrB(uint8_t dat){
	UDR0=dat;
	while (!( UCSR0A & (1<<UDRE0))){} //wait for byte to transmit
}
#ifdef MT9D111
void capImgOff(uint16_t off)
#else
void capImgOff(uint8_t off)
#endif
{
	tft_setXY(0,0);
	CS_LOW;
	RS_HIGH;
	RD_HIGH;
	DDRA=0xFF;
	DDRC=0;
	uint16_t w,ww;
	uint8_t h;
	#ifdef MT9D111
		w=1600;
		h=96;
		while (PINE&32){}//wait for low
		while (!(PINE&32)){}//wait for high
	#else
		w=640;
		h=240;
		while (!(PINE&32)){}//wait for high
		while (PINE&32){}//wait for low
	#endif
	if(off!=0){
			while (off--){
			ww=w;
			while(ww--){
				#ifdef MT9D111
				while (!(PINE&16)){}//wait for high
				while (PINE&16){}//wait for low
				#else
				while (PINE&16){}//wait for low
				while (!(PINE&16)){}//wait for high
				#endif
				
			}
		}
	}
	while (h--){
		ww=w;
		while(ww--){
			WR_LOW;
			while (PINE&16){}//wait for low
			PORTA=PINC;
			WR_HIGH;
			while (!(PINE&16)){}//wait for high
		}
	}
}
void transBuffer(void){
	uint16_t w;
	uint8_t h;
	for (h=0;h<240;++h){
		for (w=0;w<320;++w){
			tft_setXY(h,w);
			uint16_t res=tft_readRegister(0x22);
			serialWrB(res>>8);
			serialWrB(res&255);
		}
	}
}
void capImgPCqvga(void){
	cli();
	serialWrB('R');
	serialWrB('D');
	serialWrB('Y');
	uint16_t w,ww;
	uint8_t h;
	w=640;
	h=240;
	tft_setXY(0,0);
	CS_LOW;
	RS_HIGH;
	RD_HIGH;
	DDRA=0xFF;
	DDRC=0;
	#ifdef MT9D111
		while (PINE&32){}//wait for low
		while (!(PINE&32)){}//wait for high
	#else
		while (!(PINE&32)){}//wait for high
		while (PINE&32){}//wait for low
	#endif
	while (h--){
		ww=w;
		while (ww--){
			WR_LOW;
			while (PINE&16){}//wait for low
			PORTA=PINC;
			WR_HIGH;
			while (!(PINE&16)){}//wait for high
		}

	}
	transBuffer();
}
void capImgPC(void){//sends image data to serial
	//this must be divided into multiple transfers due to lack of ram
	cli();
	serialWrB('R');
	serialWrB('D');
	serialWrB('Y');
	#ifdef MT9D111
		uint16_t o;
		for(o=0;o<576;o+=96){
			capImgOff(o);
			transBuffer();
		}
	#else
		capImgOff(0);
		transBuffer();
		capImgOff(240);
		transBuffer();
	#endif
	sei();
}
#ifdef MT9D111
uint32_t capJpeg(void){
	cli();
	tft_setXY(0,0);
	CS_LOW;
	RS_HIGH;
	RD_HIGH;
	DDRA=0xFF;
	DDRC=0;
	while (PINE&32){}//wait for low
	while (!(PINE&32)){}//wait for high
	while (PINE&32){//keep reading until no vaild jpeg data is output
		WR_LOW;
		while (PINE&16){}//wait for low
		PORTA=PINC;
		WR_HIGH;
		while (!(PINE&16)){}//wait for high
	}
	sei();
}
#endif
void capImg(void){
	cli();
	uint8_t w,ww;
	uint8_t h;
	w=160;
	h=240;
	tft_setXY(0,0);
	CS_LOW;
	RS_HIGH;
	RD_HIGH;
	DDRA=0xFF;
	DDRC=0;
	#ifdef MT9D111
		while (PINE&32){}//wait for low
		while (!(PINE&32)){}//wait for high
	#else
		while (!(PINE&32)){}//wait for high
		while (PINE&32){}//wait for low
	#endif
	while (h--){
		ww=w;
		while (ww--){
			WR_LOW;
			while (PINE&16){}//wait for low
			PORTA=PINC;
			WR_HIGH;
			while (!(PINE&16)){}//wait for high
			WR_LOW;
			while (PINE&16){}//wait for low
			PORTA=PINC;
			WR_HIGH;
			while (!(PINE&16)){}//wait for high
			WR_LOW;
			while (PINE&16){}//wait for low
			PORTA=PINC;
			WR_HIGH;
			while (!(PINE&16)){}//wait for high
			WR_LOW;
			while (PINE&16){}//wait for low
			PORTA=PINC;
			WR_HIGH;
			while (!(PINE&16)){}//wait for high
		}

	}
	CS_HIGH;
	sei();
}
