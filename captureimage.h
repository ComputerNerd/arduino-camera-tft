#pragma once
#include "config.h"
void capImgqqvga(uint8_t offsetx);
void capImg(void);
void capImgPC(void);
void capImgPCqvga(void);
#ifdef MT9D111
uint32_t capJpeg(void);
#endif
