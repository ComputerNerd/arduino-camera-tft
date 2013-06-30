#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "TFT.h"
#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)
// RGB -> YUV
#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)
// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )
#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)
uint8_t buf[1280];
void capImgqqvga(uint8_t offsetx)
{
	cli();
	uint8_t w,ww;
	uint8_t h;
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
	while (h--){
		tft_setXY(y,offsetx);
		++y;
		CS_LOW;
		RS_HIGH;
		RD_HIGH;
		ww=w;
		while (ww--){
			WR_LOW;
			while (PINE&16){}//wait for low
			PORTA=PINC;
			WR_HIGH;
			//while (!(PINE&16)){}//wait for high
			WR_LOW;
			while (PINE&16){}//wait for low
			PORTA=PINC;
			WR_HIGH;
			//while (!(PINE&16)){}//wait for high
		}
	}
	CS_HIGH;
	sei();
}
void capImghq(void)
{
	cli();
	uint16_t w,ww;
	uint8_t h;
	w=320;
	h=240;
	tft_setXY(0,0);
	CS_LOW;
    RS_HIGH;
    RD_HIGH;
    DDRA=0xFF;
    DDRC=0;
   	uint8_t *line,*line2;
	#ifdef MT9D111
		while (PINE&32){}//wait for low
		while (!(PINE&32)){}//wait for high
	#else
		while (!(PINE&32)){}//wait for high
		while (PINE&32){}//wait for low
	#endif
	while (h--){
		ww=w;
		line=buf;
		while (ww--){
			while (PINE&16){}//wait for low
			*line++=(PINC>>3)&31;//blue
			while (!(PINE&16)){}//wait for high
			while (PINE&16){}//wait for low
			*line++=PINC;//green
			while (!(PINE&16)){}//wait for high
		}
		line2=buf+1;
		ww=w;
		while (ww--){
			while (PINE&16){}//wait for low
			*line++=(PINC+*line2)>>3;//green
			line2+=2;
			while (!(PINE&16)){}//wait for high
			while (PINE&16){}//wait for low
			*line++=PINC&248;//red
			while (!(PINE&16)){}//wait for high
		}
		line=buf;
		line2=buf+640;
		/* B G
		   G R*/
		ww=w;
		while (ww--){
			//r r r r r g g g   g g g b b b b b
			// 0 0 g g g g g g
			WR_LOW;
			PORTA=line2[1]|((*line2)>>3);
			WR_HIGH;
			WR_LOW;
			PORTA=*line|((*line2)<<5);
			WR_HIGH;
			line+=2;   
			line2+=2;
		}
	}
	CS_HIGH;
	sei();
}
inline void serialWrB(uint8_t dat)
{
	UDR0=dat;
	while ( !( UCSR0A & (1<<UDRE0)) ) {} //wait for byte to transmit
}
void capImgOff(uint16_t off)
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
				while (PINE&16){}//wait for low
				while (!(PINE&16)){}//wait for high
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
void transBuffer(void)
{
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
void capImgPCqvga(void)
{
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
void capImgPC(void)
{//sends raw bayer data to serial
	//this must be divided into two transfers due to lack of ram
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
void capImg(void)
{
	cli();
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
	#ifndef rgb565
	sei();
	//convert yuv422 to rgb565 this may take awhile
	uint16_t x,y;
	for (y=0;y<240;++y){
	uint16_t * bufPtr=(uint16_t *)buf;
	for (x=0;x<320;++x){
		tft_setXY(y,x);
		*bufPtr++=tft_readRegister(0x22);
	}
	DDRA=0xFF;
	tft_setXY(y,0);
	CS_LOW;
	RS_HIGH;
	RD_HIGH;
   for (x=0;x<640;x+=4)
   {
   uint16_t h1,h2;
   uint8_t rgb[6];
  rgb[0]=YUV2R((int32_t)buf[x],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[1]=YUV2G((int32_t)buf[x],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[2]=YUV2B((int32_t)buf[x],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[3]=YUV2R((int32_t)buf[x+2],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[4]=YUV2G((int32_t)buf[x+2],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[5]=YUV2B((int32_t)buf[x+2],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[0]>>=3;
  rgb[1]>>=2;
  rgb[2]>>=3;
  rgb[3]>>=3;
  rgb[4]>>=2;
  rgb[5]>>=3;
  h1= ((uint16_t)rgb[0] << 11) | ((uint16_t)rgb[1] << 5) | (uint16_t)rgb[2];
  h2=((uint16_t)rgb[3] << 11) | ((uint16_t)rgb[4] << 5) | (uint16_t)rgb[5];
      // tft_setPixel(y,x/2,h1);
     //    tft_setPixel(y,x/2+1,h2);
      WR_LOW;
      PORTA=h1>>8;
      WR_HIGH;
      WR_LOW;
      PORTA=h1&255;
      WR_HIGH;
      WR_LOW;
      PORTA=h2>>8;
      WR_HIGH;
      WR_LOW;
      PORTA=h2&255;
      WR_HIGH;
   }
  }
 #endif
 CS_HIGH;
 sei();
}
