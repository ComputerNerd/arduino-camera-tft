#pragma once
#define F_CPU 16000000UL  // 16 MHz
//#define MT9D111
#define ov7670
//#define ov7740
//#define haveSDcard
enum RESOLUTION{VGA,QVGA,QQVGA,
#ifdef MT9D111
SVGA,XGA,WXGA,QUVGA,SXGA,UXGA,WUXGA,QXGA
#endif
};
enum COLORSPACE{YUV422,
#ifndef ov7740
//The OV7740 does not support RGB565 output.
RGB565,
#endif
BAYER_RGB};
#ifdef MT9D111
#define camAddr_WR 0xBA
#define camAddr_RD 0xBB
#else
#define camAddr_WR 0x42
#define camAddr_RD 0x43
#endif
