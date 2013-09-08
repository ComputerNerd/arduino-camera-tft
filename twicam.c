#include <util/twi.h>
#include <avr/pgmspace.h>
#include "config.h"
#include <util/delay.h>
#include "TFT.h"
#include "twicam.h"
#include "camregdef.h"
#include "ov7670_regs.h"
#include "MT9D111_regs.h"
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
	PORTG|=1<<5;
	twiStart();
	twiAddr(camAddr_WR,TW_MT_SLA_ACK);
	twiWriteByte(reg,TW_MT_DATA_ACK);
	twiWriteByte(dat >> 8,TW_MT_DATA_ACK);
	twiWriteByte(dat&0x00FF,TW_MT_DATA_ACK);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
	_delay_ms(1);
	PORTG&=~(1<<5);
}
uint16_t rdReg16(uint8_t reg)
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
	dat=twiRd(0)<<8;
	dat|=twiRd(1);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
	_delay_ms(1);
	PORTG&=~(1<<5);
	return dat;
}
void wrSensorRegs8_16(const struct regval_list reglist[])
{
	uint8_t reg_addr;
	uint16_t reg_val;
	const struct regval_list *next = reglist;
	while ((reg_addr != 0xFF) | (reg_val != 0xFFFF)){
		reg_addr = pgm_read_byte(&next->reg_num);
		reg_val = pgm_read_word(&next->value);
		wrReg16(reg_addr, reg_val);
	   	next++;
	}
}
void wrSensorRegs8_16P(const struct regval_listP reglist[])
{
	uint8_t reg_addr;
	uint16_t reg_val;
	uint8_t page;
	const struct regval_listP *next = reglist;
	while ((reg_addr != 0xFF) | (reg_val != 0xFFFF)){
		page=pgm_read_byte(&next->page);
		reg_addr = pgm_read_byte(&next->reg_num);
		reg_val = pgm_read_word(&next->value);
		if(page==MT9D111_DELAY){
			uint16_t l;
			for(l=0;l<reg_val;++l)
				_delay_ms(1);
		}else if(page==EndRegs_MT9D111)
			break;
		else{
			wrReg16(0xF0,page);
			wrReg16(reg_addr, reg_val);
		}
	   	++next;
	}
}
#endif
#ifndef MT9D111
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
#endif
#ifdef ov7740
void scalingToggle(uint8_t use)
{
	if(use)
		wrReg(ISP_CTRL02,rdReg(ISP_CTRL02)|(1<<4)|(1<<5));
	else
		wrReg(ISP_CTRL02,rdReg(ISP_CTRL02)&(~((1<<4)|(1<<5))));
}
#endif
void setRes(uint8_t res)
{
	switch(res){
		case vga:
			//wrReg(0x11,2);//divider
			#ifdef ov7740
				scalingToggle(0);
			#elif defined MT9D111
				wrSensorRegs8_16(MT9D111_VGA);
			#else
				wrReg(REG_COM3,0);	// REG_COM3
				wrSensorRegs8_8(vga_ov7670);
			#endif
		break;
		case qvga:
			#ifdef ov7740
				scalingToggle(1);
			#elif defined MT9D111
				wrSensorRegs8_16(MT9D111_QVGA);
			#else
				wrReg(0x11,1);//divider
				wrReg(REG_COM3,4);	// REG_COM3 enable scaling
				wrSensorRegs8_8(qvga_ov7670);
			#endif
		break;
		#ifndef MT9D111
		case qqvga:
			#ifdef ov7740
				scalingToggle(1);
			#else
				wrReg(0x11,0);//divider
				wrReg(REG_COM3,4);	// REG_COM3 enable scaling
				wrSensorRegs8_8(qqvga_ov7670);
			#endif
		break;
		#endif
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
#ifdef ov7670
void initCam(uint8_t bayerUse)
#else
void initCam(void)
#endif
{
	#ifdef MT9D111
		_delay_ms(1000);
		wrSensorRegs8_16P(MT9D111_init);
		//_delay_ms(1000);
		//wrSensorRegs8_16(MT9D111_QVGA);
		//wrSensorRegs8_16(MT9D111_RGB565);
		//wrSensorRegs8_16(default_size_a_list);
		wrReg16(0xF0,1);//page 1
		wrReg16(0xC6, 0xA103); //SEQ_CMD
		wrReg16(0xC8, 0x0002); //SEQ_CMD, Do capture
		//wrReg16(0xA4,(1<<5));//dithering
		wrReg16(0xF0,2);//pagevv 2
		wrReg16(0x0D,0);
		wrReg16(0xF0,0);//page 0
		//wrReg16(0x0A,4|(1<<4));//give each pixel N cyles or 2N cyles if 1 ADC 
		wrSensorRegs8_16(default_size_a_list);
		//wrSensorRegs8_16P(MT9D111_refresh);
	#elif defined ov7740
		wrReg(0x12,rdReg(0x12)|1);//RGB mode
		wrReg(0x11,16);//divider
		wrReg(0x55,0);//disable double
		wrReg(0x83,rdReg(0x83)|(1<<2));//RAW 8
	#elif defined ov7670
		wrReg(0x12, 0x80);
		_delay_ms(100);
		if(bayerUse==2){
			wrSensorRegs8_8(OV7670_QVGA);
		}
		else if(bayerUse==1){
			uint16_t n;
			for(n = 0; n < sizeof(reg_init_data);n+=2)
				wrReg(pgm_read_byte_near(reg_init_data+n), pgm_read_byte_near(reg_init_data+n+1));
		}
		else
			wrSensorRegs8_8(ov7670_default_regs);
		if(bayerUse!=2)
			wrReg(0x1e,rdReg(0x1e)|(1<<5));//hflip
		if(bayerUse==1)
			wrReg(REG_COM10,48);
		else
			wrReg(REG_COM10,32);//pclk does not toggle on HBLANK
		wrReg(REG_COM11,98);
	#else
		#error "No sensor selected"
	#endif
}
