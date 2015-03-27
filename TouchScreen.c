#define F_CPU 16000000UL //16mhz
#include <util/delay.h>
#include <avr/io.h>
//Measured ADC values for (0,0) and (210-1,320-1)
//TS_MINX corresponds to ADC value when X = 0
//TS_MINY corresponds to ADC value when Y = 0
//TS_MAXX corresponds to ADC value when X = 240 -1
//TS_MAXY corresponds to ADC value when Y = 320 -1
#define TS_MINX 140
#define TS_MAXX 900
#define TS_MINY 120
#define TS_MAXY 940
static inline uint16_t readADC(uint8_t mux){
	ADMUX=mux|(1<<REFS0);
	//_delay_ms(1);
	ADCSRA|=(1<<ADSC);
	while((ADCSRA & (1<<ADSC)));
	uint8_t temp=ADCL;
	uint16_t silly=temp|(ADCH<<8);
	//ADCSRA=0;//disabe ADC
	return silly;
}
void getPoint(uint16_t * x,uint16_t * y,uint16_t * pressure){
	//YM A0 XM A1 YP A2 XP A3
	DDRF&=~(1|(1<<2));//set YP and YM to input
	PORTF&=~(1|(1<<2));//set YP and YM to low
	DDRF|=(1<<1)|(1<<3);//set XM and XP to output
	PORTF|=(1<<3);//XP set to high
	PORTF&=~(1<<1);//XM set to low
	*x=readADC(2);//read yp
	if(*x!=readADC(2)){
		*pressure=0;
		return;
	}
	*x=1023-*x;
	DDRF&=~((1<<1)|(1<<3));//set both xm and xp to input
	PORTF&=~(1<<3);//set XP to low
	DDRF|=(1<<2);//set YP as output
	PORTF|=(1<<2);//set YP to high
	DDRF|=1;//set YM as output
	*y=readADC(1);//read xm
	if(*y!=readADC(1)){
		*pressure=0;
		return;
	}
	*y=1023-*y;
	// Set X+ to ground
	//pinMode(_xp, OUTPUT);
	DDRF|=(1<<3);
	//*portOutputRegister(xp_port) &= ~xp_pin;
	PORTF&=~(1<<3);
	// Set Y- to VCC
	//*portOutputRegister(ym_port) |= ym_pin;
	PORTF|=1;
	// Hi-Z X- and Y+
	//*portOutputRegister(yp_port) &= ~yp_pin;
	PORTF&=~(1<<2);
	//pinMode(_yp, INPUT);
	DDRF&=~(1<<2);
	uint16_t z1 = readADC(1);//read XM 
	uint16_t z2 = readADC(2);//read YP
	/*float rtouch;
	rtouch = z2;
	rtouch /= z1;
	rtouch -= 1;
	rtouch *= x;
	rtouch *= _rxplate;
	rtouch /= 1024;
	z = rtouch;*/
	float rtouch=(float)z2;
	rtouch/=(float)z1;
	rtouch-=1.0f;
	rtouch*=(float)*x;
	rtouch*=(float)75.0f;//gcf of 300 and 1024 is 4 so 75 and 256
	//rtouch>>=10;//divide by 1024
	//rtouch>>=6;//convert to integer
	rtouch/=256.0f;
	//_rxplate=300
	*pressure=rtouch;
	*x-=TS_MINX;//this simple range scaling formula is x*desiredmax/originalmax unlike many others it avoids floats
	*x=*x*6/19;//240/760
	*y-=TS_MINY;
	*y=*y*16/41;//320/820
	*x=240-*x;
	*y=320-*y;
}
