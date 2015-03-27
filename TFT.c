#define F_CPU 16000000UL //16mhz
#include "TFT.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include <math.h>
#include <stdlib.h>
static uint8_t DisplayDirect;

static inline void tft_all_pin_input(void){
#ifdef MEGA
	DDRA=0;
#endif
#ifdef SEEEDUINO
	DDRD &=~ 0xfc;
	DDRB &=~ 0x03;
#endif
}
static inline void tft_all_pin_output(void){
#ifdef MEGA
	DDRA=0xff;
#endif
#ifdef SEEEDUINO
	DDRD |= 0xfc;
	DDRB |= 0x03;
#endif
}
static inline void tft_all_pin_low(void){
#ifdef MEGA	
	PORTA=0;
#endif
#ifdef SEEEDUINO
	PORTD &=~ 0xfc;
	PORTB &=~ 0x03;
#endif
}
static inline void tft_pushData(unsigned char data){
#ifdef SEEEDUINO
	tft_all_pin_low();
	PORTD |= (data<<2);
	PORTB |= (data>>6);
#endif
#ifdef MEGA
	PORTA=data;
#endif
}
static void tft_sendCommand(uint8_t index){
	#ifdef MEGA
		DDRA=0xFF;
		PORTA=0;
	#else
		tft_all_pin_output();
		tft_all_pin_low();
	#endif
	CS_LOW;
	RS_LOW;
	RD_HIGH;
	WR_HIGH;
	WR_LOW;
	#ifndef MEGA
		tft_pushData(0);//PORTA is already 0 for mega
	#endif
	WR_HIGH;
	WR_LOW;
	#ifdef MEGA
		PORTA=index;
	#else
		tft_pushData(index);	
	#endif
	WR_HIGH;
	CS_HIGH;
}
static void tft_sendCommandf(uint8_t index){
	#ifdef MEGA
		DDRA=0xFF;
		PORTA=0;
	#else
		tft_all_pin_output();
		tft_all_pin_low();
	#endif
	//CS_LOW;
	RS_LOW;
	//RD_HIGH;
	WR_HIGH;
	WR_LOW;
	#ifndef MEGA
		tft_pushData(0);//PORTA is already 0 for mega
	#endif
	WR_HIGH;
	WR_LOW;
	#ifdef MEGA
		PORTA=index;
	#else
		tft_pushData(index);	
	#endif
	WR_HIGH;
	//CS_HIGH;
}
void tft_sendData(uint16_t data){
	CS_LOW;
	RS_HIGH;
	RD_HIGH;
	WR_LOW;
	#ifdef MEGA
		PORTA=data>>8;
	#else
		tft_pushData(data>>8);
	#endif
	WR_HIGH;
	WR_LOW;
	#ifdef MEGA
		PORTA=data&255;
	#else
		tft_pushData(data&255);
	#endif
	WR_HIGH;
	CS_HIGH;
}
static void tft_sendDataf(uint16_t data){
	//CS_LOW;
	RS_HIGH;
	//RD_HIGH;
	WR_LOW;
	#ifdef MEGA
		PORTA=data>>8;
	#else
		tft_pushData(data>>8);
	#endif
	WR_HIGH;
	WR_LOW;
	#ifdef MEGA
		PORTA=data&255;
	#else
		tft_pushData(data&255);
	#endif
	WR_HIGH;
	//CS_HIGH;
}
static void tft_exitStandBy(void){
	tft_sendCommand(0x0010);
	tft_sendData(0x14E0);
	//tft_sendData(0x1480);
	_delay_ms(100);
	tft_sendCommand(0x0007);
	tft_sendData(0x133);
}
void tft_setOrientation(uint8_t HV){//horizontal or vertical
	tft_sendCommand(0x03);
	if(HV==1)//vertical
		tft_sendData(0x5038);
	else//horizontal
		tft_sendData(0x5030);
	tft_sendCommand(0x0022); //Start to write to display RAM
}
static uint8_t tft_getData(void){
	#ifdef MEGA
		return PINA;
	#else
		unsigned char data=0;
		_delay_ms(1);
		data |= ((PIND&0xfc)>>2);
		data |= ((PINB&0x03)<<6);
		return data;
	#endif
}
uint16_t tft_readRegister(uint8_t index){
	uint16_t data;

	CS_LOW;
	RS_LOW;
	RD_HIGH;

	tft_all_pin_output();

	WR_LOW;
	tft_pushData(0);
	WR_HIGH;

	WR_LOW;
	tft_pushData(index);
	WR_HIGH;

	tft_all_pin_input();
	tft_all_pin_low();
	RS_HIGH;

	RD_LOW;
	RD_HIGH;
	data = tft_getData()<<8;

	RD_LOW;
	RD_HIGH;
	data |= tft_getData();

	CS_HIGH;
	tft_all_pin_output();
	return data;
}
void tft_setXY(uint16_t poX, uint16_t poY){
	CS_LOW;
	RD_HIGH;
	tft_sendCommandf(0x0020);//X
	tft_sendDataf(poX);
	tft_sendCommandf(0x0021);//Y
	tft_sendDataf(poY);
	tft_sendCommand(0x0022);//Start to write to display RAM
	CS_HIGH;
}
void tft_setDisplayDirect(unsigned char Direction){
  DisplayDirect = Direction;
}
void tft_drawVerticalLine(unsigned int poX, unsigned int poY,unsigned int length,unsigned int color){
	tft_setXY(poX,poY);
	tft_setOrientation(1);
	if(length+poY>MAX_Y)
		length=MAX_Y-poY;
	CS_LOW;
	RD_HIGH;
	while(length--)
		tft_sendDataf(color);
	CS_HIGH;
}
void tft_drawHorizontalLine(uint16_t poX, uint16_t poY,uint16_t length,uint16_t color){
	tft_setXY(poX,poY);
	tft_setOrientation(0);
	if(length+poX>MAX_X)
		length=MAX_X-poX;
	uint16_t i;
	#ifdef MEGA
		CS_LOW;
		RS_HIGH;
		RD_HIGH;
		DDRA=0xFF;
		uint8_t col1,col2;
		col1=color>>8;
		col2=color&255;
	#endif
	for(i=0;i<length;++i){
		#ifdef SEEEDUINO
		tft_sendData(color);
		#endif
		#ifdef MEGA
			WR_LOW;
			PORTA=col1;
			WR_HIGH;
			WR_LOW;
			PORTA=col2;
			WR_HIGH;
		#endif
	}
	#ifdef MEGA
		CS_HIGH;
	#endif
}
static inline void tft_setPixel(uint16_t poX, uint16_t poY,uint16_t color){
	tft_setXY(poX,poY);
	tft_sendData(color);
}
void tft_drawLine(int16_t x0,int16_t y0,int16_t x1,int16_t y1,uint16_t color){
	int16_t x = x1-x0;
	int16_t y = y1-y0;
	int16_t dx = abs(x), sx = x0<x1 ? 1 : -1;
	int16_t dy = -abs(y), sy = y0<y1 ? 1 : -1;
	int16_t err = dx+dy, e2; /* error value e_xy */
	while (1){ /* loop */
		tft_setPixel(x0,y0,color);
		e2 = 2*err;
		if (e2 >= dy){ /* e_xy+e_x > 0 */
			if (x0 == x1) break;
			err += dy; x0 += sx;
		}
		if (e2 <= dx){ /* e_xy+e_y < 0 */
			if (y0 == y1) break;
			err += dx; y0 += sy;
		}
	}
}
void tft_fillRectangle(uint16_t poX, uint16_t poY, uint16_t length, uint16_t width, uint16_t color){
	uint16_t i;
	for(i=0;i<width;i++){
		if(DisplayDirect == LEFT2RIGHT)
			tft_drawHorizontalLine(poX, poY+i, length, color);
		else if (DisplayDirect ==  DOWN2UP)
			tft_drawHorizontalLine(poX, poY-i, length, color);
		else if(DisplayDirect == RIGHT2LEFT)
			tft_drawHorizontalLine(poX, poY-i, length, color);
		else if(DisplayDirect == UP2DOWN)
			tft_drawHorizontalLine(poX, poY+i, length, color);	
	}
}
void tft_drawChar(unsigned char ascii,uint16_t poX, uint16_t poY,uint8_t size, uint16_t fgcolor){
	tft_setXY(poX,poY);   
	if((ascii < 0x20)||(ascii > 0x7e))//Unsupported char.
		ascii = '?';
	uint8_t i,f;
	for(i=0;i<8;i++){
		uint8_t temp = pgm_read_byte(&simpleFont[ascii-0x20][i]);
		for(f=0;f<8;++f){
			if((temp>>f)&0x01){
				if(DisplayDirect == LEFT2RIGHT)
					tft_fillRectangle(poX+i*size, poY+f*size, size, size, fgcolor);
				else if(DisplayDirect == DOWN2UP)
					tft_fillRectangle(poX+f*size, poY-i*size, size, size, fgcolor);
				else if(DisplayDirect == RIGHT2LEFT)
					tft_fillRectangle(poX-i*size, poY-f*size, size, size, fgcolor);
				else if(DisplayDirect == UP2DOWN)
					tft_fillRectangle(poX-f*size, poY+i*size, size, size, fgcolor);
			}
		}
	}
}
void tft_drawString(const char *string,unsigned int poX, unsigned int poY,unsigned int size,unsigned int fgcolor){
	while(*string){
		tft_drawChar(*string++, poX, poY, size, fgcolor);
		if(DisplayDirect == LEFT2RIGHT){
			if(poX < MAX_X)
				poX+=8*size; // Move cursor right
		}
		else if(DisplayDirect == DOWN2UP){
			if(poY > 0)
				poY-=8*size; // Move cursor right
		}
		else if(DisplayDirect == RIGHT2LEFT){
			if(poX > 0)
				poX-=8*size; // Move cursor right
		}
		else if(DisplayDirect == UP2DOWN){
			if(poY < MAX_Y)
				poY+=8*size; // Move cursor right
		}
	}
}
void tft_drawStringP(const char *string,unsigned int poX, unsigned int poY,unsigned int size,unsigned int fgcolor){
	while(pgm_read_byte_near(string)){
		tft_drawChar(pgm_read_byte_near(string), poX, poY, size, fgcolor);
		string++;
		if(DisplayDirect == LEFT2RIGHT){
			if(poX < MAX_X)
				poX+=8*size; // Move cursor right
		}else if(DisplayDirect == DOWN2UP){
			if(poY > 8*size)
				poY-=8*size; // Move cursor right
			else{
				poY=320;
				poX+=8*size;
			}
		}else if(DisplayDirect == RIGHT2LEFT){
			if(poX > 0)
				poX-=8*size; // Move cursor right
		}else if(DisplayDirect == UP2DOWN){
			if(poY < MAX_Y)
				poY+=8*size; // Move cursor right
		}
	}
}
void tft_drawCircle(int poX, int poY, int r,uint16_t color){
    int x = -r, y = 0, err = 2-2*r, e2; 
    do {
        tft_setPixel(poX-x, poY+y,color); 
        tft_setPixel(poX+x, poY+y,color); 
        tft_setPixel(poX+x, poY-y,color); 
        tft_setPixel(poX-x, poY-y,color); 
        e2 = err;
        if (e2 <= y) { 
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0; 
        }
        if (e2 > x) err += ++x*2+1; 
    } while (x <= 0);
}
void tft_fillCircle(int poX, int poY, int r,uint16_t color){
    int x = -r, y = 0, err = 2-2*r, e2;
    do {

        tft_drawVerticalLine(poX-x,poY-y,2*y,color);
        tft_drawVerticalLine(poX+x,poY-y,2*y,color);

        e2 = err;
        if (e2 <= y) {
            err += ++y*2+1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x*2+1;
    } while (x <= 0);

}
void tft_paintScreenBlack(void){
	CS_LOW;
	RD_HIGH;
	uint16_t f;
	for(f=0;f<38400;++f){
		tft_sendDataf(BLACK);
		tft_sendDataf(BLACK);
	}
	CS_HIGH;
}
void tft_drawImage_P(const uint8_t * dat,uint16_t w,uint16_t h,uint16_t x,uint16_t y){
	uint16_t a,b;
	#ifdef MEGA
		DDRA=0xFF;
	#elif defined(SEEEDUINO)
		tft_all_pin_output();
	#endif
	tft_setOrientation(1);
	for(b=y;b<h+y;++b){
		tft_setXY(b,x);
		CS_LOW;
		RS_HIGH;
		RD_HIGH;
		dat+=(w-1)*2;
		for(a=0;a<w;++a){
			#ifdef MEGA
				WR_LOW;
				PORTA=pgm_read_byte(dat+1);
				WR_HIGH;
				WR_LOW;
				PORTA=pgm_read_byte(dat);
				WR_HIGH;
			#elif defined(SEEEDUINO)
				WR_LOW;
				tft_pushData(dat+1);
				WR_HIGH;
				WR_LOW;
				tft_pushData(dat);
				WR_HIGH;
			#endif
			dat-=2;
		}
		dat+=(w+1)*2;
	}
	tft_setDisplayDirect(DOWN2UP);
}
void tft_drawImage(uint8_t * dat,uint16_t w,uint16_t h,uint16_t x,uint16_t y){
	uint16_t a,b;
	#ifdef MEGA
		DDRA=0xFF;
	#elif defined(SEEEDUINO)
		tft_all_pin_output();
	#endif
	tft_setOrientation(1);
	for(b=y;b<h+y;++b){
		tft_setXY(b,x);
		CS_LOW;
		RS_HIGH;
		RD_HIGH;
		dat+=(w-1)*2;
		for(a=0;a<w;++a){
			WR_LOW;
			#ifdef MEGA
				PORTA=dat[1];
			#elif defined(SEEEDUINO)
				tft_pushData(dat[1]);
			#endif
			WR_HIGH;
			WR_LOW;
			#ifdef MEGA
				PORTA=*dat;
			#elif defined(SEEEDUINO)
				tft_pushData(*dat);
			#endif
			WR_HIGH;
			dat-=2;
		}
		dat+=(w+1)*2;
	}
	tft_setDisplayDirect(DOWN2UP);
}
void tft_drawImageVf_P(const uint8_t * dat,uint16_t w,uint16_t h,uint16_t x,int16_t y){
	uint16_t a;
	int16_t b;
	#ifdef MEGA
		DDRA=0xFF;
	#elif defined(SEEEDUINO)
		tft_all_pin_output();
	#endif
	tft_setOrientation(1);
	for(b=y+h-1;b>=y;--b){
		tft_setXY(b,x);
		CS_LOW;
		RS_HIGH;
		RD_HIGH;
		dat+=(w-1)*2;
		for(a=0;a<w;++a){
			WR_LOW;
			#ifdef MEGA
				PORTA=pgm_read_byte(dat+1);
			#elif defined(SEEEDUINO)
				tft_pushData(pgm_read_byte(dat+1));
			#endif
			WR_HIGH;
			WR_LOW;
			#ifdef MEGA
				PORTA=pgm_read_byte(dat);
			#elif defined(SEEEDUINO)
				tft_pushData(pgm_read_byte(dat));
			#endif
			WR_HIGH;
			dat-=2;
		}
		dat+=(w+1)*2;
	}
	tft_setDisplayDirect(DOWN2UP);
}
void tft_init(void){
	CS_OUTPUT;
	RD_OUTPUT;
	WR_OUTPUT;
	RS_OUTPUT;
	#ifdef MEGA
		DDRA=0xFF;
		PORTA=0;
	#else
		tft_all_pin_output();
		tft_all_pin_low();
	#endif
	_delay_ms(100);
	//register names and information from http://www.micro4you.com/files/lcd/SPFD5408A.pdf
	tft_sendCommand(0x0001);//"Driver Output Control Register"
	tft_sendData((1<<8));//This in effect makes the tft display backwards it changes shift direction
	tft_sendCommand(0x0002);
	tft_sendData((1<<8)|(1<<9)|(1<<10));
	tft_sendCommand(0x0003);
	tft_sendData(0x1030);
	tft_sendCommand(0x0004);//Scaling
	tft_sendData(0x0000);//Disable
	tft_sendCommand(0x0008);
	tft_sendData(0x0302);
	tft_sendCommand(0x000A);
	tft_sendData(0x0000);
	tft_sendCommand(0x000C);
	tft_sendData(0x0000);
	tft_sendCommand(0x000D);
	tft_sendData(0x0000);
	tft_sendCommand(0x000F);
	tft_sendData(0x0000);

	_delay_ms(100);

	tft_sendCommand(0x0030);
	tft_sendData(0x0000);
	tft_sendCommand(0x0031);
	tft_sendData(0x0405);
	tft_sendCommand(0x0032);
	tft_sendData(0x0203);
	tft_sendCommand(0x0035);
	tft_sendData(0x0004);
	tft_sendCommand(0x0036);
	tft_sendData(0x0B07);
	tft_sendCommand(0x0037);
	tft_sendData(0x0000);
	tft_sendCommand(0x0038);
	tft_sendData(0x0405);
	tft_sendCommand(0x0039);
	tft_sendData(0x0203);
	tft_sendCommand(0x003c);
	tft_sendData(0x0004);
	tft_sendCommand(0x003d);
	tft_sendData(0x0B07);
	tft_sendCommand(0x0020);
	tft_sendData(0x0000);
	tft_sendCommand(0x0021);
	tft_sendData(0x0000);
	tft_sendCommand(0x0050);
	tft_sendData(0x0000);
	tft_sendCommand(0x0051);
	tft_sendData(0x00ef);
	tft_sendCommand(0x0052);
	tft_sendData(0x0000);
	tft_sendCommand(0x0053);
	tft_sendData(0x013f);

	_delay_ms(100);

	tft_sendCommand(0x0060);
	tft_sendData(0xa700);
	tft_sendCommand(0x0061);
	tft_sendData(0x0001);
	tft_sendCommand(0x0090);
	tft_sendData(0x003A);
	tft_sendCommand(0x0095);
	tft_sendData(0x021E);
	tft_sendCommand(0x0080);
	tft_sendData(0x0000);
	tft_sendCommand(0x0081);
	tft_sendData(0x0000);
	tft_sendCommand(0x0082);
	tft_sendData(0x0000);
	tft_sendCommand(0x0083);
	tft_sendData(0x0000);
	tft_sendCommand(0x0084);
	tft_sendData(0x0000);
	tft_sendCommand(0x0085);
	tft_sendData(0x0000);
	tft_sendCommand(0x00FF);
	tft_sendData(0x0001);
	tft_sendCommand(0x00B0);
	tft_sendData(0x140D);
	tft_sendCommand(0x00FF);
	tft_sendData(0x0000);
	_delay_ms(100);
	tft_sendCommand(0x0007);
	tft_sendData(0x0133);
	_delay_ms(50);
	tft_exitStandBy();
	tft_sendCommand(0x0022);
	//paint screen black
	tft_paintScreenBlack();
}
