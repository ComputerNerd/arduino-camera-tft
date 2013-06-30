#define F_CPU 16000000UL  // 16 MHz
#define MT9D111
//#define ov7670
//#define ov7740
#define vga 0
#define qvga 1
#ifndef MT9D111
#define qqvga 2
#else
#define SVGA 3
#define XGA 4
#define WXGA 5
#define QuVGA 6
#define SXGA 7
#define SXGAp 8
#define WSXGAp 9
#define UXGA 10
#define WUXGA 11
#define QXGA 12
#endif
#define yuv422 0
#ifndef ov7740
//the ov7740 does not support rgb565 output
#define rgb565 1
#endif
#define bayerRGB 2
#ifdef MT9D111
#define camAddr_WR 0xBA
#define camAddr_RD 0xBB
#else
#define camAddr_WR 0x42
#define camAddr_RD 0x43
#endif
