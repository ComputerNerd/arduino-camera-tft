#define F_CPU 16000000UL  // 16 MHz
#define vga 0
#define qvga 1
#define qqvga 2
#define yuv422 0
#define rgb565 1
#define bayerRGB 2
//#define yuv422
//#define MT9D111
#ifdef MT9D111
#define sensor_addr 0xBA
#else
#define sensor_addr 0x42
#endif
#ifdef MT9D111
#define camAddr_WR 0xBA
#define camAddr_RD 0xBB
#else
#define camAddr_WR 0x42
#define camAddr_RD 0x43
#endif
