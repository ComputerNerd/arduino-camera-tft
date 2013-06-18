#include <util/twi.h>
#include <avr/pgmspace.h>
#include "config.h"
#include <util/delay.h>
#include "TFT.h"
#include "twicam.h"
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
	twiStart();
	twiAddr(camAddr_WR,TW_MT_SLA_ACK);
	twiWriteByte(reg,TW_MT_DATA_ACK);
	twiWriteByte(dat,TW_MT_DATA_ACK);
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);//send stop
	_delay_ms(1);
}
uint8_t rdReg(uint8_t reg)
{
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
