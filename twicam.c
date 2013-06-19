#include <util/twi.h>
#include <avr/pgmspace.h>
#include "config.h"
#include <util/delay.h>
#include "TFT.h"
#include "twicam.h"
#include "camregdef.h"
void errorD(uint8_t err)
{
	if (err !=0){
		uint16_t x=WHITE;
		while (1)
			tft_drawStringP(PSTR("TWI ERROR!"),120,300,3,x--);
	}
}
void twiStart(void)
{
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);//send start
	while (!(TWCR & (1<<TWINT))) {}//wait for start to be transmitted
	if ((TWSR & 0xF8) != TW_START)
		errorD(1);
}
void twiWriteByte(uint8_t DATA,uint8_t type)
{
	static uint16_t err=5;
	TWDR = DATA;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while (!(TWCR & (1<<TWINT))) {}
	if ((TWSR & 0xF8) != type)
		errorD(err);
	err++;
}
void twiAddr(uint8_t addr,uint8_t typeTWI)
{
	//This function does not do error checking
	TWDR = addr;//send address
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	if ((TWSR & 0xF8) != typeTWI)
		errorD(2);
}
uint8_t twiRd(uint8_t nack)
{
	if (nack){
		TWCR=_BV(TWINT) | _BV(TWEN);
		while ((TWCR & _BV(TWINT)) == 0); /* wait for transmission */
	if ((TWSR & 0xF8) != TW_MR_DATA_NACK)
		errorD(4);
		return TWDR;
	}else{
		TWCR=_BV(TWINT) | _BV(TWEN) | _BV(TWEA);
		while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
		if ((TWSR & 0xF8) != TW_MR_DATA_ACK)
			errorD(3);
		return TWDR;
	}
}
void wrReg(uint8_t reg,uint8_t dat)
{
	//send start condition
	PORTG|=1<<5;
	twiStart();
	twiAddr(camAddr_WR,TW_MT_SLA_ACK);
	twiWriteByte(reg,TW_MT_DATA_ACK);
	twiWriteByte(dat,TW_MT_DATA_ACK);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
	_delay_ms(1);
	PORTG&=~(1<<5);
}
uint8_t rdReg(uint8_t reg)
{
	PORTG|=1<<5;
	uint16_t dat;
	twiStart();
	twiAddr(camAddr_WR,TW_MT_SLA_ACK);
	twiWriteByte(reg,TW_MT_DATA_ACK);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
	_delay_ms(1);
	twiStart();
	twiAddr(camAddr_RD,TW_MR_SLA_ACK);
	dat=twiRd(1);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
	_delay_ms(1);
	PORTG&=~(1<<5);
    return dat;
}
#ifdef MT9D111
void wrReg16(uint8_t reg,uint16_t dat)
{
	//send start condition
	twiStart();
	twiAddr(camAddr_WR,TW_MT_SLA_ACK);
	twiWriteByte(reg,TW_MT_DATA_ACK);
	twiWriteByte(dat >> 8,TW_MT_DATA_ACK);
	twiWriteByte(dat&0x00FF,TW_MT_DATA_ACK);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
	_delay_ms(1);
}
uint16_t rdReg16(uint8_t reg)
{
	uint16_t dat;
	twiStart();
	twiAddr(camAddr_WR,TW_MT_SLA_ACK);
	twiWriteByte(reg,TW_MT_DATA_ACK);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
	_delay_ms(1);
	twiStart();
	twiAddr(camAddr_RD,TW_MR_SLA_ACK);
	dat=twiRd(0)<<8;
	dat|=twiRd(1);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
	return dat;
}
#endif
#ifndef MT9D111
const struct regval_list vga_ov7670[] PROGMEM = {
	{REG_HREF,0xF6},        // was B6  
    {0x17,0x13},        // HSTART
    {0x18,0x01},        // HSTOP
    {0x19,0x02},        // VSTART
    {0x1a,0x7a},        // VSTOP
    {REG_VREF,0x0a},        // VREF
	{ 0xff, 0xff },	/* END MARKER */
};
const struct regval_list qvga_ov7670[] PROGMEM = {
	{REG_COM14, 0x19},
	{0x72, 0x11},
	{0x73, 0xf1},
	{REG_HSTART,0x16},
	{REG_HSTOP,0x04},
	{REG_HREF,0x24},
	{REG_VSTART,0x02},
	{REG_VSTOP,0x7a},
	{REG_VREF,0x0a},
	{ 0xff, 0xff },	/* END MARKER */
};
const struct regval_list qqvga_ov7670[] PROGMEM = {
	{REG_COM14, 0x1a},          // divide by 4
	{0x72, 0x22},               // downsample by 4
	{0x73, 0xf2},               // divide by 4
	{REG_HSTART,0x16},
	{REG_HSTOP,0x04},
	{REG_HREF,0xa4},           
	{REG_VSTART,0x02},
	{REG_VSTOP,0x7a},
	{REG_VREF,0x0a},
	{ 0xff, 0xff },	/* END MARKER */
};
const struct regval_list yuv422_ov7670[] PROGMEM = {
	 { REG_COM7, 0x0 },  /* Selects YUV mode */
         { REG_RGB444, 0 },      /* No RGB444 please */
         { REG_COM1, 0 },
         { REG_COM15, COM15_R00FF },
         { REG_COM9, 0x6A }, /* 128x gain ceiling; 0x8 is reserved bit */
         { 0x4f, 0x80 },         /* "matrix coefficient 1" */
         { 0x50, 0x80 },         /* "matrix coefficient 2" */
         { 0x51, 0    },         /* vb */
         { 0x52, 0x22 },         /* "matrix coefficient 4" */
         { 0x53, 0x5e },         /* "matrix coefficient 5" */
         { 0x54, 0x80 },         /* "matrix coefficient 6" */
         { REG_COM13, /*COM13_GAMMA|*/COM13_UVSAT },
	{ 0xff, 0xff },	/* END MARKER */
};
const struct regval_list rgb565_ov7670[] PROGMEM = {
	 { REG_COM7, COM7_RGB }, /* Selects RGB mode */
         { REG_RGB444, 0 },      /* No RGB444 please */
         { REG_COM1, 0x0 },
         { REG_COM15, COM15_RGB565|COM15_R00FF},
         { REG_COM9, 0x6A },     /* 128x gain ceiling; 0x8 is reserved bit */
         { 0x4f, 0xb3 },         /* "matrix coefficient 1" */
         { 0x50, 0xb3 },         /* "matrix coefficient 2" */
         { 0x51, 0    },         /* vb */
         { 0x52, 0x3d },         /* "matrix coefficient 4" */
         { 0x53, 0xa7 },         /* "matrix coefficient 5" */
         { 0x54, 0xe4 },         /* "matrix coefficient 6" */
         { REG_COM13, /*COM13_GAMMA|*/COM13_UVSAT },
	{ 0xff, 0xff },	/* END MARKER */
};
const struct regval_list bayerRGB_ov7670[] PROGMEM = {
	 { REG_COM7, COM7_BAYER },
         { REG_COM13, 0x08 }, /* No gamma, magic rsvd bit */
         { REG_COM16, 0x3d }, /* Edge enhancement, denoise */
         { REG_REG76, 0xe1 }, /* Pix correction, magic rsvd */
	{ 0xff, 0xff },	/* END MARKER */
};
void setColor(uint8_t color)
{
	switch(color){
		case yuv422:
			wrSensorRegs8_8(yuv422_ov7670);
		break;
		case rgb565:
			wrSensorRegs8_8(rgb565_ov7670);
			{uint8_t temp=rdReg(0x11);
			_delay_ms(1);
			wrReg(0x11,temp);}//accorind to the linux kernel driver rgb565 PCLK needs re-writting
		break;
		case bayerRGB:
			wrSensorRegs8_8(bayerRGB_ov7670);
		break;
	}
}
void setRes(uint8_t res)
{
	switch(res){
		case vga:
			//wrReg(0x11,2);//divider
			wrReg(REG_COM3,0);	// REG_COM3
			wrSensorRegs8_8(vga_ov7670);
		break;
		case qvga:
			wrReg(0x11,1);//divider
			wrReg(REG_COM3,4);	// REG_COM3 enable scaling
			wrSensorRegs8_8(qvga_ov7670);
		break;
		case qqvga:
			wrReg(0x11,0);//divider
			wrReg(REG_COM3,4);	// REG_COM3 enable scaling
			wrSensorRegs8_8(qqvga_ov7670);
		break;
	}
}
void wrSensorRegs8_8(const struct regval_list reglist[])
{
	uint8_t reg_addr,reg_val;
	const struct regval_list *next = reglist;
	while ((reg_addr != 0xff) | (reg_val != 0xff)){
		reg_addr = pgm_read_byte(&next->reg_num);
		reg_val = pgm_read_byte(&next->value);
		wrReg(reg_addr, reg_val);
	   	next++;
	}
}
#endif
