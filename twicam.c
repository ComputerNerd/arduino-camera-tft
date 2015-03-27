#include <util/twi.h>
#include <avr/pgmspace.h>
#include "config.h"
#include <util/delay.h>
#include "TFT.h"
#include "twicam.h"
#include "camregdef.h"
#include "ov7670_regs.h"
#include "MT9D111_regs.h"
void errorD(uint8_t err){
	if (err !=0){
		uint16_t x=WHITE;
		while (1)
			tft_drawStringP(PSTR("TWI ERROR!"),120,300,3,x--);
	}
}
void twiStart(void){
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);//send start
	while (!(TWCR & (1<<TWINT))) {}//wait for start to be transmitted
	if ((TWSR & 0xF8) != TW_START)
		errorD(1);
}
void twiWriteByte(uint8_t DATA,uint8_t type){
	static uint16_t err=5;
	TWDR = DATA;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while (!(TWCR & (1<<TWINT))) {}
	if ((TWSR & 0xF8) != type)
		errorD(err);
	++err;
}
void twiAddr(uint8_t addr,uint8_t typeTWI){
	TWDR = addr;//send address
	TWCR = _BV(TWINT) | _BV(TWEN);		/* clear interrupt to start transmission */
	while ((TWCR & _BV(TWINT)) == 0);	/* wait for transmission */
	if ((TWSR & 0xF8) != typeTWI)
		errorD(2);
}
uint8_t twiRd(uint8_t nack){
	if (nack){
		TWCR=_BV(TWINT) | _BV(TWEN);
		while ((TWCR & _BV(TWINT)) == 0);	/* wait for transmission */
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
void wrReg(uint8_t reg,uint8_t dat){
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
uint8_t rdReg(uint8_t reg){
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
void wrReg16(uint8_t reg,uint16_t dat){
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
uint16_t rdReg16(uint8_t reg){
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
void wrSensorRegs8_16(const struct regval_list reglist[]){
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
void wrSensorRegs8_16P(const struct regval_listP reglist[]){
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
void setColor(enum COLORSPACE color){
	#ifdef MT9D111
		wrReg16(0xF0,1);
	#endif
	switch(color){
		case YUV422:
			#ifdef MT9D111
				wrReg16(0xC6,(1<<15)|(1<<13)|(7<<8)|125);
				wrReg16(0xC8,0);
				wrReg16(0xC6,(1<<15)|(1<<13)|(7<<8)|126);
				wrReg16(0xC8,0);
			#else
				wrSensorRegs8_8(yuv422_ov7670);
			#endif
		break;
		case RGB565:
			#ifdef MT9D111
				wrReg16(0xC6,(1<<15)|(1<<13)|(7<<8)|125);
				wrReg16(0xC8,(1<<5));
				wrReg16(0xC6,(1<<15)|(1<<13)|(7<<8)|126);
				wrReg16(0xC8,(1<<5));
			#else
				wrSensorRegs8_8(rgb565_ov7670);
				{uint8_t temp=rdReg(0x11);
				_delay_ms(1);
				wrReg(0x11,temp);}//According to the Linux kernel driver RGB565 PCLK needs rewriting.
			#endif
		break;
		#ifndef MT9D111
		case BAYER_RGB:
			wrSensorRegs8_8(bayerRGB_ov7670);
		break;
		#endif
	}
}
#ifdef ov7740
void scalingToggle(uint8_t use){
	if(use)
		wrReg(ISP_CTRL02,rdReg(ISP_CTRL02)|(1<<4)|(1<<5));
	else
		wrReg(ISP_CTRL02,rdReg(ISP_CTRL02)&(~((1<<4)|(1<<5))));
}
#endif
#ifdef MT9D111
void MT9D111Refresh(void){
	wrReg16(0xC6,0xA103);
	wrReg16(0xC8,5);//refresh
}
void MT9D111DoPreview(void){
	wrReg16(0xC6,0xA103);
	wrReg16(0xC8,1);
}
void setMT9D111res(uint16_t w,uint16_t h){
	wrReg16(0xF0,1);
	wrReg16(0xC6,(1<<13)|(7<<8)|3);
	wrReg16(0xC8,w);
	wrReg16(0xC6,(1<<13)|(7<<8)|5);
	wrReg16(0xC8,h);
}
#endif
void setRes(enum RESOLUTION res){
	switch(res){
		#ifdef MT9D111
		case SVGA:
			setMT9D111res(800,600);
		break;
		#endif
		case VGA:
			//wrReg(0x11,2);//divider
			#ifdef ov7740
				scalingToggle(0);
			#elif defined MT9D111
				//wrSensorRegs8_16(MT9D111_VGA);
				setMT9D111res(640,480);
			#else
				wrReg(REG_COM3,0);	// REG_COM3
				wrSensorRegs8_8(vga_ov7670);
			#endif
		break;
		case QVGA:
			#ifdef ov7740
				scalingToggle(1);
			#elif defined MT9D111
				//wrSensorRegs8_16(MT9D111_QVGA);
				setMT9D111res(320,240);
			#else
				wrReg(0x11,1);//divider
				wrReg(REG_COM3,4);	// REG_COM3 enable scaling
				wrSensorRegs8_8(qvga_ov7670);
			#endif
		break;
		case QQVGA:
			#ifdef ov7740
				scalingToggle(1);
			#elif defined MT9D111
				setMT9D111res(160,120);
			#else
				wrReg(0x11,0);//divider
				wrReg(REG_COM3,4);	// REG_COM3 enable scaling
				wrSensorRegs8_8(qqvga_ov7670);
			#endif
	}
}
void wrSensorRegs8_8(const struct regval_list reglist[]){
	uint8_t reg_addr,reg_val;
	const struct regval_list *next = reglist;
	while ((reg_addr != 0xff) | (reg_val != 0xff)){
		reg_addr = pgm_read_byte(&next->reg_num);
		reg_val = pgm_read_byte(&next->value);
		wrReg(reg_addr, reg_val);
	   	next++;
	}
}

#ifdef MT9D111
/*
Bits 7:0 of address for physical access; driver variable offset for logical access.
Bits 12:8 of address for physical access; driver ID for logical access.
Bits 14:13 of address for physical access; R0xC6:1[14:13] = 01 select logical access.
Bit  15 1 = 8-bit access; 0 = 16-bit access
*/
void waitStateMT9D111(uint8_t state){
	wrReg16(0xF0,1);//Set to page 1
	do{
		_delay_ms(25);
		wrReg16(0xC6,(1<<15)|(1<<13)|(1<<8)|4);
	}while(rdReg16(0xC8)!=state);
}
void MT9D111JPegCapture(void){
	wrReg16(0xF0,1);
	
	wrReg16(0xC6,(1<<15)|(1<<13)|(1<<8)|32);
	wrReg16(0xc8,2);//Enable video

	
	wrReg16(0xC6,(1<<15)|(1<<13)|(9<<8)|7);
	wrReg16(0xC8,(1<<4)|1);//Use scaled table video
	wrReg16(0xC6,(1<<15)|(1<<13)|(9<<8)|10);
	wrReg16(0xC8,56|(1<<7));//Qscale
	
	wrReg16(0xC6,(1<<15)|(1<<13)|(9<<8)|11);
	wrReg16(0xC8,56|(1<<7));//Qscale
	
	wrReg16(0xC6,(1<<15)|(1<<13)|(9<<8)|12);
	wrReg16(0xC8,56|(1<<7));//Qscale

	wrReg16(0xC6,(1<<13)|(7<<8)|11);
	wrReg16(0xC8,0);//Enable JPEG

	wrReg16(0xC6,(1<<15)|(1<<13)|(1<<8)|3);
	wrReg16(0xc8,2);//Actully run the capture
}
#endif

#ifdef ov7670
void initCam(uint8_t bayerUse)
#else
void initCam(void)
#endif
{
	#ifdef MT9D111
		//_delay_ms(1000);
		//wrSensorRegs8_16P(MT9D111_init);
		//_delay_ms(1000);
		//wrSensorRegs8_16(MT9D111_QVGA);
		//wrSensorRegs8_16(MT9D111_RGB565);
		//wrSensorRegs8_16(default_size_a_list);
		
		//Start off with a soft reset

		
		wrReg16(0xF0,1);//Set to page 1
		wrReg16(0xC3,0x0501);
		wrReg16(0xF0,0);
		wrReg16(0x0D,0x0021);
		wrReg16(0x0D,0);
		_delay_ms(100);//Cannot use i2c for 24 camera cylces this should be way over that.
		waitStateMT9D111(3);
		//wrReg16(0xF0,0);//Set to page 0
		//wrReg16(0x15,(1<<7)|3);
		wrReg16(0xF0,1);//Set to page 1
		//Poll camera until it is ready
		/*wrReg16(0xC6,(1<<13)|(7<<8)|25);//Row speed
		wrReg16(0xC8,3);*/
		wrReg16(0xC6,(1<<15)|(1<<13)|(2<<8)|14);//increase maximum intergration time
		wrReg16(0xc8,128);
		/*wrReg16(0xC6,(1<<15)|(1<<13)|(2<<8)|16);//increase maximum virtual gain
		wrReg16(0xc8,232);
		wrReg16(0xC6,(1<<15)|(1<<13)|(2<<8)|24);//increase maximum gain
		wrReg16(0xc8,224);*/
		wrReg16(0xC6,(1<<13)|(2<<8)|20);//increase maximum pre-lc digital gain
		wrReg16(0xc8,256);
		/*wrReg16(0xC6,(1<<15)|(1<<13)|(7<<8)|67);//gamma contex A
		wrReg16(0xC8,2);
		wrReg16(0xC6,(1<<15)|(1<<13)|(7<<8)|68);//gamma B
		wrReg16(0xC8,2);*/
		//MT9D111Refresh();
		//_delay_ms(1000);
		wrReg16(0xC6,(1<<13)|(7<<8)|107);//Fifo context A
		wrReg16(0xC8,0);
		wrReg16(0xC6,(1<<13)|(7<<8)|114);//Fifo context B
		wrReg16(0xC8,0);
		MT9D111Refresh();
		//_delay_ms(1000);
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
