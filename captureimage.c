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
	while (!(PINE&32)){}//wait for high
	while (PINE&32){}//wait for low
	while (h--)
	{
		ww=w;
		while (ww--)
		{
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
	for (y=0;y<240;y++){
	uint16_t * bufPtr=(uint16_t *)buf;
	for (x=0;x<320;x++){
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
