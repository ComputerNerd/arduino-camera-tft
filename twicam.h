#pragma once
#include "config.h"
#include <avr/pgmspace.h>
void wrReg(uint8_t reg,uint8_t dat);
uint8_t rdReg(uint8_t reg);
void setRes(enum RESOLUTION res);
void setColor(enum COLORSPACE color);
#ifdef MT9D111
void MT9D111Refresh(void);
uint16_t rdReg16(uint8_t reg);
void wrReg16(uint8_t reg,uint16_t dat);
struct regval_list {
	uint8_t reg_num;
	uint16_t value;
};
#define MT9D111_DELAY 3
#define EndRegs_MT9D111 4
struct regval_listP {
	uint8_t page;
	uint8_t reg_num;
	uint16_t value;
};
#else
struct regval_list {
	uint8_t reg_num;
	uint8_t value;
};
void wrSensorRegs8_8(const struct regval_list reglist[]);
extern const struct regval_list ov7670_default_regs[];
#endif
#ifdef ov7670
void initCam(uint8_t bayerUse);
#else
void initCam(void);
#endif
