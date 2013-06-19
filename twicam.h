#pragma once
#include "config.h"
void wrReg(uint8_t reg,uint8_t dat);
uint8_t rdReg(uint8_t reg);
#ifdef MT9D111
uint16_t rdReg16(uint8_t reg);
void wrReg16(uint8_t reg,uint16_t dat);
#else
struct regval_list {
	uint8_t reg_num;
	uint8_t value;
};
void setRes(uint8_t res);
void wrSensorRegs8_8(const struct regval_list reglist[]);
#endif
