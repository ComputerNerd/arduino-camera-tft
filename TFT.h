/*
This is a library for the seeedstudio tft shield 1.0
This is heavily based on seeed's library but is ported to avr-gcc
*/
#ifndef TFT_H
#define TFT_h
#include <stdint.h>
#include <avr/io.h>

//#define SEEEDUINO // arduino uno
#define MEGA
//Basic Colors
#define RED		0xf800
#define GREEN	0x07e0
#define BLUE	0x001f
#define BLACK	0x0000
#define YELLOW	0xffe0
#define WHITE	0xffff

//Other Colors
#define CYAN		0x07ff	
#define BRIGHT_RED	0xf810	
#define GRAY1		0x8410  
#define GRAY2		0x4208  

//TFT resolution 240*320
#define MIN_X	0
#define MIN_Y	0
#define MAX_X	240
#define MAX_Y	320

#ifdef SEEEDUINO

//========================================
#define DDR_CS	  DDRB
#define PORT_CS	 PORTB
#define CS_BIT	  0x04
#define CS_OUTPUT   {DDR_CS|=CS_BIT;}
#define CS_HIGH	 {PORT_CS|=CS_BIT;}
#define CS_LOW	  {PORT_CS&=~CS_BIT;}

//-----------------------------------------

#define DDR_RS	  DDRB
#define PORT_RS	 PORTB
#define RS_BIT	  0x08
#define RS_OUTPUT   {DDR_RS|=RS_BIT;}
#define RS_HIGH	 {PORT_RS|=RS_BIT;}
#define RS_LOW	  {PORT_RS&=~RS_BIT;}

//-----------------------------------------

#define DDR_WR	  DDRB
#define PORT_WR	 PORTB
#define WR_BIT	  0x10
#define WR_OUTPUT   {DDR_WR|=WR_BIT;}
#define WR_HIGH	 {PORT_WR|=WR_BIT;}
#define WR_LOW	  {PORT_WR&=~WR_BIT;}
#define WR_RISING   {PORT_WR|=WR_BIT;PORT_WR&=~WR_BIT;}

//-----------------------------------------

#define DDR_RD	  DDRB
#define PORT_RD	 PORTB
#define RD_BIT	  0x20
#define RD_OUTPUT   {DDR_RD|=RD_BIT;}
#define RD_HIGH	 {PORT_RD|=RD_BIT;}
#define RD_LOW	  {PORT_RD&=~RD_BIT;}
#define RD_RISING   {PORT_RD|=RD_BIT;PORT_RD&=~RD_BIT;}
//========================================

#endif

#ifdef MEGA
//==================/CS=====================
#define DDR_CS	  DDRB
#define PORT_CS	 PORTB
#define CS_BIT	  0x10
#define CS_OUTPUT   {DDR_CS|=CS_BIT;}
#define CS_HIGH	 {PORT_CS|=CS_BIT;}
#define CS_LOW	  {PORT_CS&=~CS_BIT;}

//------------------RS----------------------

#define DDR_RS	  DDRB
#define PORT_RS	 PORTB
#define RS_BIT	  0x20
#define RS_OUTPUT   {DDR_RS|=RS_BIT;}
#define RS_HIGH	 {PORT_RS|=RS_BIT;}
#define RS_LOW	  {PORT_RS&=~RS_BIT;}

//------------------WR----------------------

#define DDR_WR	  DDRB
#define PORT_WR	 PORTB
#define WR_BIT	  0x40
#define WR_OUTPUT   {DDR_WR|=WR_BIT;}
#define WR_HIGH	 {PORT_WR|=WR_BIT;}
#define WR_LOW	  {PORT_WR&=~WR_BIT;}
#define WR_RISING   {PORT_WR|=WR_BIT;PORT_WR&=~WR_BIT;}

//------------------RD---------------------

#define DDR_RD	  DDRB
#define PORT_RD	 PORTB
#define RD_BIT	  0x80
#define RD_OUTPUT   {DDR_RD|=RD_BIT;}
#define RD_HIGH	 {PORT_RD|=RD_BIT;}
#define RD_LOW	  {PORT_RD&=~RD_BIT;}
#define RD_RISING   {PORT_RD|=RD_BIT;PORT_RD&=~RD_BIT;}

#define RS_RD_HIGH	{DDR_RD|=RD_BIT|RS_BIT;}

//========================================

#endif
#define LEFT2RIGHT 0
#define DOWN2UP	1
#define RIGHT2LEFT 2
#define UP2DOWN	3
extern unsigned char simpleFont[][8];
uint16_t tft_readRegister(uint8_t index);
void tft_setOrientation(uint8_t HV);
void tft_sendData(unsigned int data);
void tft_setXY(uint16_t poX, uint16_t poY);
void tft_setDisplayDirect(unsigned char Direction);
void tft_drawLine(int16_t x0,int16_t y0,int16_t x1,int16_t y1,uint16_t color);
void tft_fillRectangle(unsigned int poX, unsigned int poY, unsigned int length, unsigned int width, unsigned int color);
void tft_drawVerticalLine(unsigned int poX, unsigned int poY,unsigned int length,unsigned int color);
void tft_drawHorizontalLine(unsigned int poX, unsigned int poY,unsigned int length,unsigned int color);
void tft_drawChar(unsigned char ascii,uint16_t poX, uint16_t poY,uint8_t size, uint16_t fgcolor);
void tft_drawString(const char *string,unsigned int poX, unsigned int poY,unsigned int size,unsigned int fgcolor);
void tft_drawStringP(const char *string,unsigned int poX, unsigned int poY,unsigned int size,unsigned int fgcolor);
void tft_paintScreenBlack(void);
void tft_drawImage(uint8_t * dat,uint16_t w,uint16_t h,uint16_t x,uint16_t y);
void tft_drawImage_P(const uint8_t * dat,uint16_t w,uint16_t h,uint16_t x,uint16_t y);
void tft_drawImageVf_P(const uint8_t * dat,uint16_t w,uint16_t h,uint16_t x,int16_t y);
void tft_drawCircle(int poX, int poY, int r,uint16_t color);
void tft_fillCircle(int poX, int poY, int r,uint16_t color);
void tft_init(void);
#endif
