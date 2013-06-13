#include <stdint.h>
#include <TouchScreen.h> 
#include <TFT.h>
#include <Wire.h>
#include <util/twi.h>
#include <util/delay.h>
#define camAddr_WR 0x42
#define camAddr_RD 0x43
#define REG_COM1	0x04	/* Control 1 */
#define REG_COM7	0x12	/* Control 7 */
#define   COM7_RESET	  0x80	  /* Register reset */
#define   COM7_FMT_MASK	  0x38
#define   COM7_FMT_VGA	  0x00
#define	  COM7_FMT_CIF	  0x20	  /* CIF format */
#define   COM7_FMT_QVGA	  0x10	  /* QVGA format */
#define   COM7_FMT_QCIF	  0x08	  /* QCIF format */
#define	  COM7_RGB	  0x04	  /* bits 0 and 2 - RGB format */
#define	  COM7_YUV	  0x00	  /* YUV */
#define	  COM7_BAYER	  0x01	  /* Bayer format */
#define	  COM7_PBAYER	  0x05	  /* "Processed bayer" */
#define REG_RGB444	0x8c	/* RGB 444 control */
#define   R444_ENABLE	  0x02	  /* Turn on RGB444, overrides 5x5 */
#define   R444_RGBX	  0x01	  /* Empty nibble at end */
#define REG_COM9	0x14	/* Control 9  - gain ceiling */
#define REG_COM10	0x15	/* Control 10 */
#define REG_COM13	0x3d	/* Control 13 */
#define   COM13_GAMMA	  0x80	  /* Gamma enable */
#define	  COM13_UVSAT	  0x40	  /* UV saturation auto adjustment */
#define   COM13_UVSWAP	  0x01	  /* V before U - w/TSLB */
#define REG_COM15	0x40	/* Control 15 */
#define   COM15_R10F0	  0x00	  /* Data range 10 to F0 */
#define	  COM15_R01FE	  0x80	  /*            01 to FE */
#define   COM15_R00FF	  0xc0	  /*            00 to FF */
#define   COM15_RGB565	  0x10	  /* RGB565 output */
#define   COM15_RGB555	  0x30	  /* RGB555 output */
#define REG_COM11	0x3b	/* Control 11 */
#define   COM11_NIGHT	  0x80	  /* NIght mode enable */
#define   COM11_NMFR	  0x60	  /* Two bit NM frame rate */
#define   COM11_HZAUTO	  0x10	  /* Auto detect 50/60 Hz */
#define	  COM11_50HZ	  0x08	  /* Manual 50Hz select */
#define   COM11_EXP	  0x02
#define REG_COM17       0x42    /* Control 17 */
#define COM17_AECWIN    0xc0    /* AEC window - must match COM4 */
#define COM17_CBAR      0x08    /* DSP Color bar */
#define REG_TSLB	0x3a	/* lots of stuff */
#define   TSLB_YLAST	  0x04	  /* UYVY or VYUY - see com13 */
#define MTX1            0x4f    /* Matrix Coefficient 1 */
#define MTX2            0x50    /* Matrix Coefficient 2 */
#define MTX3            0x51    /* Matrix Coefficient 3 */
#define MTX4            0x52    /* Matrix Coefficient 4 */
#define MTX5            0x53    /* Matrix Coefficient 5 */
#define MTX6            0x54    /* Matrix Coefficient 6 */
#define REG_CONTRAS     0x56    /* Contrast control */
#define MTXS            0x58    /* Matrix Coefficient Sign */
#define AWBC7           0x59    /* AWB Control 7 */
#define AWBC8           0x5a    /* AWB Control 8 */
#define AWBC9           0x5b    /* AWB Control 9 */
#define AWBC10          0x5c    /* AWB Control 10 */
#define AWBC11          0x5d    /* AWB Control 11 */
#define AWBC12          0x5e    /* AWB Control 12 */
#define REG_GFIX        0x69    /* Fix gain control */
#define GGAIN           0x6a    /* G Channel AWB Gain */
#define DBLV            0x6b    
#define AWBCTR3         0x6c    /* AWB Control 3 */
#define AWBCTR2         0x6d    /* AWB Control 2 */
#define AWBCTR1         0x6e    /* AWB Control 1 */
#define AWBCTR0         0x6f    /* AWB Control 0 */
#define REG_COM8	0x13	/* Control 8 */
#define   COM8_FASTAEC	  0x80	  /* Enable fast AGC/AEC */
#define   COM8_AECSTEP	  0x40	  /* Unlimited AEC step size */
#define   COM8_BFILT	  0x20	  /* Band filter enable */
#define   COM8_AGC	  0x04	  /* Auto gain enable */
#define   COM8_AWB	  0x02	  /* White balance enable */
#define   COM8_AEC	  0x01	  /* Auto exposure enable */
#define REG_COM3        0x0c    /* Control 3 */
#define COM3_SWAP       0x40    /* Byte swap */
#define COM3_SCALEEN    0x08    /* Enable scaling */
#define COM3_DCWEN      0x04    /* Enable downsamp/crop/window */
#define REG_BRIGHT      0x55    /* Brightness */
#define REG_COM14	0x3e	/* Control 14 */
#define   COM14_DCWEN	  0x10	  /* DCW/PCLK-scale enable */
#define REG_HSTART      0x17    /* Horiz start high bits */
#define REG_HSTOP       0x18    /* Horiz stop high bits */
#define REG_VSTART      0x19    /* Vert start high bits */
#define REG_VSTOP       0x1a    /* Vert stop high bits */
#define REG_HREF        0x32    /* HREF pieces */
#define REG_VREF	0x03	/* Pieces of GAIN, VSTART, VSTOP */
#define REG_CLKRC	0x11	/* Clocl control */
#define   CLK_EXT	  0x40	  /* Use external clock directly */
#define   CLK_SCALE	  0x3f	  /* Mask for internal clock scale */
/* Registers */
#define REG_GAIN	0x00	/* Gain lower 8 bits (rest in vref) */
#define REG_BLUE	0x01	/* blue gain */
#define REG_RED		0x02	/* red gain */
#define REG_VREF	0x03	/* Pieces of GAIN, VSTART, VSTOP */
#define REG_COM1	0x04	/* Control 1 */
#define  COM1_CCIR656	  0x40  /* CCIR656 enable */
#define REG_BAVE	0x05	/* U/B Average level */
#define REG_GbAVE	0x06	/* Y/Gb Average level */
#define REG_AECHH	0x07	/* AEC MS 5 bits */
#define REG_RAVE	0x08	/* V/R Average level */
#define REG_COM2	0x09	/* Control 2 */
#define  COM2_SSLEEP	  0x10	/* Soft sleep mode */
#define REG_PID		0x0a	/* Product ID MSB */
#define REG_VER		0x0b	/* Product ID LSB */
#define REG_COM3	0x0c	/* Control 3 */
#define  COM3_SWAP	  0x40	  /* Byte swap */
#define  COM3_SCALEEN	  0x08	  /* Enable scaling */
#define  COM3_DCWEN	  0x04	  /* Enable downsamp/crop/window */
#define REG_COM4	0x0d	/* Control 4 */
#define REG_COM5	0x0e	/* All "reserved" */
#define REG_COM6	0x0f	/* Control 6 */
#define REG_AECH	0x10	/* More bits of AEC value */
#define REG_CLKRC	0x11	/* Clocl control */
#define   CLK_EXT	  0x40	  /* Use external clock directly */
#define   CLK_SCALE	  0x3f	  /* Mask for internal clock scale */
#define REG_COM7	0x12	/* Control 7 */
#define   COM7_RESET	  0x80	  /* Register reset */
#define   COM7_FMT_MASK	  0x38
#define   COM7_FMT_VGA	  0x00
#define	  COM7_FMT_CIF	  0x20	  /* CIF format */
#define   COM7_FMT_QVGA	  0x10	  /* QVGA format */
#define   COM7_FMT_QCIF	  0x08	  /* QCIF format */
#define	  COM7_RGB	  0x04	  /* bits 0 and 2 - RGB format */
#define	  COM7_YUV	  0x00	  /* YUV */
#define	  COM7_BAYER	  0x01	  /* Bayer format */
#define	  COM7_PBAYER	  0x05	  /* "Processed bayer" */
#define REG_COM8	0x13	/* Control 8 */
#define   COM8_FASTAEC	  0x80	  /* Enable fast AGC/AEC */
#define   COM8_AECSTEP	  0x40	  /* Unlimited AEC step size */
#define   COM8_BFILT	  0x20	  /* Band filter enable */
#define   COM8_AGC	  0x04	  /* Auto gain enable */
#define   COM8_AWB	  0x02	  /* White balance enable */
#define   COM8_AEC	  0x01	  /* Auto exposure enable */
#define REG_COM9	0x14	/* Control 9  - gain ceiling */
#define REG_COM10	0x15	/* Control 10 */
#define   COM10_HSYNC	  0x40	  /* HSYNC instead of HREF */
#define   COM10_PCLK_HB	  0x20	  /* Suppress PCLK on horiz blank */
#define   COM10_HREF_REV  0x08	  /* Reverse HREF */
#define   COM10_VS_LEAD	  0x04	  /* VSYNC on clock leading edge */
#define   COM10_VS_NEG	  0x02	  /* VSYNC negative */
#define   COM10_HS_NEG	  0x01	  /* HSYNC negative */
#define REG_HSTART	0x17	/* Horiz start high bits */
#define REG_HSTOP	0x18	/* Horiz stop high bits */
#define REG_VSTART	0x19	/* Vert start high bits */
#define REG_VSTOP	0x1a	/* Vert stop high bits */
#define REG_PSHFT	0x1b	/* Pixel delay after HREF */
#define REG_MIDH	0x1c	/* Manuf. ID high */
#define REG_MIDL	0x1d	/* Manuf. ID low */
#define REG_MVFP	0x1e	/* Mirror / vflip */
#define   MVFP_MIRROR	  0x20	  /* Mirror image */
#define   MVFP_FLIP	  0x10	  /* Vertical flip */

#define REG_AEW		0x24	/* AGC upper limit */
#define REG_AEB		0x25	/* AGC lower limit */
#define REG_VPT		0x26	/* AGC/AEC fast mode op region */
#define REG_HSYST	0x30	/* HSYNC rising edge delay */
#define REG_HSYEN	0x31	/* HSYNC falling edge delay */
#define REG_HREF	0x32	/* HREF pieces */
#define REG_TSLB	0x3a	/* lots of stuff */
#define   TSLB_YLAST	  0x04	  /* UYVY or VYUY - see com13 */
#define REG_COM11	0x3b	/* Control 11 */
#define   COM11_NIGHT	  0x80	  /* NIght mode enable */
#define   COM11_NMFR	  0x60	  /* Two bit NM frame rate */
#define   COM11_HZAUTO	  0x10	  /* Auto detect 50/60 Hz */
#define	  COM11_50HZ	  0x08	  /* Manual 50Hz select */
#define   COM11_EXP	  0x02
#define REG_COM12	0x3c	/* Control 12 */
#define   COM12_HREF	  0x80	  /* HREF always */
#define REG_COM13	0x3d	/* Control 13 */
#define   COM13_GAMMA	  0x80	  /* Gamma enable */
#define	  COM13_UVSAT	  0x40	  /* UV saturation auto adjustment */
#define   COM13_UVSWAP	  0x01	  /* V before U - w/TSLB */
#define REG_COM14	0x3e	/* Control 14 */
#define   COM14_DCWEN	  0x10	  /* DCW/PCLK-scale enable */
#define REG_EDGE	0x3f	/* Edge enhancement factor */
#define REG_COM15	0x40	/* Control 15 */
#define   COM15_R10F0	  0x00	  /* Data range 10 to F0 */
#define	  COM15_R01FE	  0x80	  /*            01 to FE */
#define   COM15_R00FF	  0xc0	  /*            00 to FF */
#define   COM15_RGB565	  0x10	  /* RGB565 output */
#define   COM15_RGB555	  0x30	  /* RGB555 output */
#define REG_COM16	0x41	/* Control 16 */
#define   COM16_AWBGAIN   0x08	  /* AWB gain enable */
#define REG_COM17	0x42	/* Control 17 */
#define   COM17_AECWIN	  0xc0	  /* AEC window - must match COM4 */
#define   COM17_CBAR	  0x08	  /* DSP Color bar */
/*
 * This matrix defines how the colors are generated, must be
 * tweaked to adjust hue and saturation.
 *
 * Order: v-red, v-green, v-blue, u-red, u-green, u-blue
 *
 * They are nine-bit signed quantities, with the sign bit
 * stored in 0x58.  Sign for v-red is bit 0, and up from there.
 */
#define	REG_CMATRIX_BASE 0x4f
#define   CMATRIX_LEN 6
#define REG_CMATRIX_SIGN 0x58


#define REG_BRIGHT	0x55	/* Brightness */
#define REG_CONTRAS	0x56	/* Contrast control */

#define REG_GFIX	0x69	/* Fix gain control */

#define REG_REG76	0x76	/* OV's name */
#define   R76_BLKPCOR	  0x80	  /* Black pixel correction enable */
#define   R76_WHTPCOR	  0x40	  /* White pixel correction enable */

#define REG_RGB444	0x8c	/* RGB 444 control */
#define   R444_ENABLE	  0x02	  /* Turn on RGB444, overrides 5x5 */
#define   R444_RGBX	  0x01	  /* Empty nibble at end */

#define REG_HAECC1	0x9f	/* Hist AEC/AGC control 1 */
#define REG_HAECC2	0xa0	/* Hist AEC/AGC control 2 */

#define REG_BD50MAX	0xa5	/* 50hz banding step limit */
#define REG_HAECC3	0xa6	/* Hist AEC/AGC control 3 */
#define REG_HAECC4	0xa7	/* Hist AEC/AGC control 4 */
#define REG_HAECC5	0xa8	/* Hist AEC/AGC control 5 */
#define REG_HAECC6	0xa9	/* Hist AEC/AGC control 6 */
#define REG_HAECC7	0xaa	/* Hist AEC/AGC control 7 */
#define REG_BD60MAX	0xab	/* 60hz banding step limit */
#ifdef SEEEDUINO
  #define YP A2   // must be an analog pin, use "An" notation!
  #define XM A1   // must be an analog pin, use "An" notation!
  #define YM 14   // can be a digital pin, this is A0
  #define XP 17   // can be a digital pin, this is A3 
#endif

#ifdef MEGA
  #define YP A2   // must be an analog pin, use "An" notation!
  #define XM A1   // must be an analog pin, use "An" notation!
  #define YM 54   // can be a digital pin, this is A0
  #define XP 57   // can be a digital pin, this is A3 
#endif
//Measured ADC values for (0,0) and (210-1,320-1)
//TS_MINX corresponds to ADC value when X = 0
//TS_MINY corresponds to ADC value when Y = 0
//TS_MAXX corresponds to ADC value when X = 240 -1
//TS_MAXY corresponds to ADC value when Y = 320 -1

#define TS_MINX 140
#define TS_MAXX 900
#define qvga
#define rgb565
//#define yuv422
#define TS_MINY 120
#define TS_MAXY 940
#define sensor_addr 0x42
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
uint8_t buf[640];
void errorD(uint8_t err)
{
  if (err !=0)
  {
    uint16_t x=WHITE;
    sprintf((char *)buf,"Error code %d",err);
     while (1)
     {
    Tft.drawString((char *)buf,120,300,3,x--);
     }
  }
}
Point p;
void setmatrix(uint8_t id)
{
 switch (id) {
 case 0:
    wrReg(MTX1,0x80);        
    wrReg(MTX2,0x80);      
    wrReg(MTX3,0x00);        
    wrReg(MTX4,0x22);        
    wrReg(MTX5,0x5e);        
    wrReg(MTX6,0x80);        
    wrReg(MTXS,0x9e); 
    break;
  case 1:
      wrReg(MTX1,0x40);        
    wrReg(MTX2,0x34);      
    wrReg(MTX3,0x0c);        
    wrReg(MTX4,0x17);
    wrReg(MTX5,0x29);        
    wrReg(MTX6,0x40);
    wrReg(MTXS,0x1A);
  break;
  case 2:
    wrReg(MTX1,0xB3);        
    wrReg(MTX2,0xB3);      
    wrReg(MTX3,0x0);        
    wrReg(MTX4,0x3D);        
    wrReg(MTX5,0xA7);        
    wrReg(MTX6,0xE4);
    wrReg(MTXS,0x9E);
  break;
 }
  
}
struct regval_list {
	uint8_t reg_num;
	uint8_t value;
};

const struct regval_list ov7670_default_regs[] PROGMEM = {
	{ REG_COM7, COM7_RESET },
/*
 * Clock scale: 3 = 15fps
 *              2 = 20fps
 *              1 = 30fps
 */
	//{ REG_CLKRC, 0x2 },	/* OV: clock scale (30 fps) */
	{ REG_TSLB,  0x04 },	/* OV */
	{ REG_COM7, 0 },	/* VGA */
	/*
	 * Set the hardware window.  These values from OV don't entirely
	 * make sense - hstop is less than hstart.  But they work...
	 */
	{ REG_HSTART, 0x13 },	{ REG_HSTOP, 0x01 },
	{ REG_HREF, 0xb6 },	{ REG_VSTART, 0x02 },
	{ REG_VSTOP, 0x7a },	{ REG_VREF, 0x0a },

	{ REG_COM3, 0 },	{ REG_COM14, 0 },
	/* Mystery scaling numbers */
	{ 0x70, 0x3a },		{ 0x71, 0x35 },
	{ 0x72, 0x11 },		{ 0x73, 0xf0 },
	{ 0xa2,/* 0x02 changed to 1*/1},{ REG_COM10, 0x0 },

	/* Gamma curve values */
	{ 0x7a, 0x20 },		{ 0x7b, 0x10 },
	{ 0x7c, 0x1e },		{ 0x7d, 0x35 },
	{ 0x7e, 0x5a },		{ 0x7f, 0x69 },
	{ 0x80, 0x76 },		{ 0x81, 0x80 },
	{ 0x82, 0x88 },		{ 0x83, 0x8f },
	{ 0x84, 0x96 },		{ 0x85, 0xa3 },
	{ 0x86, 0xaf },		{ 0x87, 0xc4 },
	{ 0x88, 0xd7 },		{ 0x89, 0xe8 },

	/* AGC and AEC parameters.  Note we start by disabling those features,
	   then turn them only after tweaking the values. */
	{ REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT },
	{ REG_GAIN, 0 },	{ REG_AECH, 0 },
	{ REG_COM4, 0x40 }, /* magic reserved bit */
	{ REG_COM9, 0x18 }, /* 4x gain + magic rsvd bit */
	{ REG_BD50MAX, 0x05 },	{ REG_BD60MAX, 0x07 },
	{ REG_AEW, 0x95 },	{ REG_AEB, 0x33 },
	{ REG_VPT, 0xe3 },	{ REG_HAECC1, 0x78 },
	{ REG_HAECC2, 0x68 },	{ 0xa1, 0x03 }, /* magic */
	{ REG_HAECC3, 0xd8 },	{ REG_HAECC4, 0xd8 },
	{ REG_HAECC5, 0xf0 },	{ REG_HAECC6, 0x90 },
	{ REG_HAECC7, 0x94 },
	{ REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC },

        {0x30,0},{0x31,0},//disable some delays

	/* Almost all of these are magic "reserved" values.  */
	{ REG_COM5, 0x61 },	{ REG_COM6, 0x4b },
	{ 0x16, 0x02 },		{ REG_MVFP, 0x07 },
	{ 0x21, 0x02 },		{ 0x22, 0x91 },
	{ 0x29, 0x07 },		{ 0x33, 0x0b },
	{ 0x35, 0x0b },		{ 0x37, 0x1d },
	{ 0x38, 0x71 },		{ 0x39, 0x2a },
	{ REG_COM12, 0x78 },	{ 0x4d, 0x40 },
	{ 0x4e, 0x20 },		{ REG_GFIX, 0 },
	/*{ 0x6b, 0x4a },*/		{ 0x74, 0x10 },
	{ 0x8d, 0x4f },		{ 0x8e, 0 },
	{ 0x8f, 0 },		{ 0x90, 0 },
	{ 0x91, 0 },		{ 0x96, 0 },
	{ 0x9a, 0 },		{ 0xb0, 0x84 },
	{ 0xb1, 0x0c },		{ 0xb2, 0x0e },
	{ 0xb3, 0x82 },		{ 0xb8, 0x0a },

	/* More reserved magic, some of which tweaks white balance */
	{ 0x43, 0x0a },		{ 0x44, 0xf0 },
	{ 0x45, 0x34 },		{ 0x46, 0x58 },
	{ 0x47, 0x28 },		{ 0x48, 0x3a },
	{ 0x59, 0x88 },		{ 0x5a, 0x88 },
	{ 0x5b, 0x44 },		{ 0x5c, 0x67 },
	{ 0x5d, 0x49 },		{ 0x5e, 0x0e },
	{ 0x6c, 0x0a },		{ 0x6d, 0x55 },
	{ 0x6e, 0x11 },		{ 0x6f, 0x9f }, /* "9e for advance AWB" */
	{ 0x6a, 0x40 },		{ REG_BLUE, 0x40 },
	{ REG_RED, 0x60 },
	{ REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC|COM8_AWB },

	/* Matrix coefficients */
	{ 0x4f, 0x80 },		{ 0x50, 0x80 },
	{ 0x51, 0 },		{ 0x52, 0x22 },
	{ 0x53, 0x5e },		{ 0x54, 0x80 },
	{ 0x58, 0x9e },

	{ REG_COM16, COM16_AWBGAIN },	{ REG_EDGE, 0 },
	{ 0x75, 0x05 },		{ 0x76, 0xe1 },
	{ 0x4c, 0 },		{ 0x77, 0x01 },
	{ REG_COM13, 0xc3 },	{ 0x4b, 0x09 },
	{ 0xc9, 0x60 },		/*{ REG_COM16, 0x38 },*/
	{ 0x56, 0x40 },

	{ 0x34, 0x11 },		{ REG_COM11, COM11_EXP|COM11_HZAUTO },
	{ 0xa4, 0x82/*Wax0x88*/ },		{ 0x96, 0 },
	{ 0x97, 0x30 },		{ 0x98, 0x20 },
	{ 0x99, 0x30 },		{ 0x9a, 0x84 },
	{ 0x9b, 0x29 },		{ 0x9c, 0x03 },
	{ 0x9d, 0x4c },		{ 0x9e, 0x3f },
	{ 0x78, 0x04 },

	/* Extra-weird stuff.  Some sort of multiplexor register */
	{ 0x79, 0x01 },		{ 0xc8, 0xf0 },
	{ 0x79, 0x0f },		{ 0xc8, 0x00 },
	{ 0x79, 0x10 },		{ 0xc8, 0x7e },
	{ 0x79, 0x0a },		{ 0xc8, 0x80 },
	{ 0x79, 0x0b },		{ 0xc8, 0x01 },
	{ 0x79, 0x0c },		{ 0xc8, 0x0f },
	{ 0x79, 0x0d },		{ 0xc8, 0x20 },
	{ 0x79, 0x09 },		{ 0xc8, 0x80 },
	{ 0x79, 0x02 },		{ 0xc8, 0xc0 },
	{ 0x79, 0x03 },		{ 0xc8, 0x40 },
	{ 0x79, 0x05 },		{ 0xc8, 0x30 },
	{ 0x79, 0x26 },

	{ 0xff, 0xff },	/* END MARKER */
};
//undocumented rgister 1F is called LAEC this means exposures less than one line
void initCam(void)
{
    wrReg(0x12, 0x80);
  _delay_ms(100);

  wrSensorRegs8_8(ov7670_default_regs);
 // _delay_ms(1000);
	wrReg(REG_COM10,32);//pclk does not toggle on HBLANK
        //wrReg(0x6B,74);
	//wrReg(0x11,32);//using scaler for divider
	wrReg(REG_RGB444, 0x00);             // Disable RGB444
	wrReg(REG_COM11,226);//enable night mode 1/8 frame rate COM11
	//wrReg(0x2E,63);
	wrReg(REG_TSLB,0x04);                // 0D = UYVY  04 = YUYV     
 	wrReg(REG_COM13,0xC8);               // connect to REG_TSLB
	//wrReg(REG_COM13,0x8);               // connect to REG_TSLB disable gamma
	#ifdef rgb565
	wrReg(REG_COM7, 0x04);           // RGB
        wrReg(REG_COM15, 0xD0);          // Set rgb565 with Full range    0xD0
	#elif defined rawRGB
	wrReg(REG_COM7,1);//raw rgb bayer
	wrReg(REG_COM15, 0xC0);          //Full range
	#else
	wrReg(REG_COM7, 0x00);           // YUV
        //wrReg(REG_COM17, 0x00);          // color bar disable
	wrReg(REG_COM15, 0xC0);          //Full range
	#endif
        //wrReg(REG_COM3, 0x04);
	#if defined qqvga || defined qvga
	wrReg(REG_COM3,4);    // REG_COM3 enable scaling
	#else
	wrReg(REG_COM3,0);    // REG_COM3
	#endif
        //wrReg(0x3e,0x00);        //  REG_COM14
     //   wrReg(0x72,0x11);        //
       // wrReg(0x73,0xf0);        //
	//wrReg(REG_COM8,0x8F);        // AGC AWB AEC Unlimited step size
	wrReg(REG_COM8,0xCF);//disable AGC disable AEC
      //  wrReg(REG_COM1, 3);//manual exposure
       // wrReg(0x07, 0xFF);//manual exposure
        //wrReg(0x10, 0xFF);//manual exposure*/
	#ifdef qqvga
	wrReg(REG_COM14, 0x1a);          // divide by 4
        wrReg(0x72, 0x22);               // downsample by 4
        wrReg(0x73, 0xf2);               // divide by 4
        wrReg(REG_HSTART,0x16);
        wrReg(REG_HSTOP,0x04);
        wrReg(REG_HREF,0xa4);           
        wrReg(REG_VSTART,0x02);
        wrReg(REG_VSTOP,0x7a);
        wrReg(REG_VREF,0x0a);
	#endif
	#ifdef qvga
	wrReg(REG_COM14, 0x19);//0x19
        wrReg(0x72, 0x11);
        wrReg(0x73, 0xf1);//f1
        wrReg(REG_HSTART,0x16);
        wrReg(REG_HSTOP,0x04);
        wrReg(REG_HREF,0x24);
        wrReg(REG_VSTART,0x02);
        wrReg(REG_VSTOP,0x7a);
        wrReg(REG_VREF,0x0a);
	#else
        wrReg(REG_HREF,0xF6);        // was B6  
        wrReg(0x17,0x13);        // HSTART
        wrReg(0x18,0x01);        // HSTOP
        wrReg(0x19,0x02);        // VSTART
        wrReg(0x1a,0x7a);        // VSTOP
        wrReg(REG_VREF,0x0a);        // VREF
	//wrReg(REG_VREF,0xCA);//set 2 high GAIN MSB
	#endif
       // wrReg(0x70, 0x3a);       // Scaling Xsc
        //wrReg(0x71, 0x35);       // Scaling Ysc
        //wrReg(0xA2, 0x02);       // pixel clock delay
 
  // COLOR SETTING
//	wrReg(0,0xFF);//set gain to maxium possile
    wrReg(0xAA,0x94);            // Average-based AEC algorithm
    wrReg(REG_BRIGHT,0x00);      // 0x00(Brightness 0) - 0x18(Brightness +1) - 0x98(Brightness -1)
    wrReg(REG_CONTRAS,0x40);     // 0x40(Contrast 0) - 0x50(Contrast +1) - 0x38(Contrast -1)
    //wrReg(0xB1,0xB1);            // Automatic Black level Calibration
    wrReg(0xb1,4);//really enable ABLC
  setmatrix(3);  
/*wrReg(0x4f, 0xb3); 	// "matrix coefficient 1" 
wrReg(0x50, 0xb3); 	// "matrix coefficient 2"
wrReg( 0x51, 0);		// vb
wrReg(0x52, 0x3d ); 	// "matrix coefficient 4"
wrReg(0x53, 0xa7 ); 	// "matrix coefficient 5" 
wrReg(0x54, 0xe4 ); 	// "matrix coefficient 6"*/
    wrReg(AWBC7,0x88);
    wrReg(AWBC8,0x88);
    wrReg(AWBC9,0x44);
    wrReg(AWBC10,0x67);
    wrReg(AWBC11,0x49);
    wrReg(AWBC12,0x0e);
	wrReg(REG_GFIX,0x00);
	//wrReg(GGAIN,0);
	wrReg(AWBCTR3,0x0a);
	wrReg(AWBCTR2,0x55);
	wrReg(AWBCTR1,0x11);
	wrReg(AWBCTR0,0x9f);
	wrReg(0xb0,0x84);//Why does this improve the quality so much???
	wrReg(REG_COM16,COM16_AWBGAIN);//disable auto denoise and edge enhancment
	//wrReg(REG_COM16,0);
	wrReg(0x4C,0);//disable denoise
	wrReg(0x76,0);//disable denoise
	wrReg(0x77,0);//disable denoise
	wrReg(0x7B,4);//brighten up shadows a bit end point 4
	wrReg(0x7C,8);//brighten up shadows a bit end point 8
	//wrReg(0x88,238);//darken highligts end point 176
	//wrReg(0x89,211);//try to get more highlight detail
	//wrReg(0x7A,60);//slope
	//wrReg(0x26,0xB4);//lower maxium stable operating range for AEC
	//hueSatMatrix(0,100);
	//ov7670_store_cmatrix();
	//wrReg(0x20,12);//set ADC range to 1.5x*/
	wrReg(REG_COM9,0x6A);//max gain to 128x
	wrReg(0x74,16);//disable digital gain
	//wrReg(0x93,15);//dummy line MSB*/
        wrReg(0x1e,33);//hflip
	wrReg(0x11,2);//divider
	//wrReg(0x2a,5);//href delay
}
#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)

// RGB -> YUV
#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)

// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)
void setup()
{
 //pinMode(46,OUTPUT);

 DDRL|=8;
 ASSR &= ~(_BV(EXCLK) | _BV(AS2));
 //generate 8mhz clock
TCCR5A =67;
TCCR5B=17;
        OCR5A = 0;
        DDRC=0;
       //DDRE|=48;
      byte error;
      Serial.begin(9600);
Tft.init();  //init TFT library

/*  Demo of 
    drawRectangle(unsigned int poX, unsigned int poY, unsigned int length,unsigned int width,unsigned int color);
    fillRectangle(unsigned int poX, unsigned int poY, unsigned int length, unsigned int width, unsigned int color);
*/
 Tft.setDisplayDirect(DOWN2UP);
  Tft.drawStringP(PSTR("OV7670"),20,240,4,BLUE);
  Tft.drawStringP(PSTR("Test"),60,220,4,RED);
   cli();
   	TWSR&=~3;//disable prescaler for TWI
	TWBR=72;//set to 100khz
  initCam();
   Tft.drawStringP(PSTR("Starting!"),120,300,4,WHITE);
  // editRegs();
  menu();
   sei();
}
void menu(void)
{
  uint8_t item;
  while (1)
  {
    Tft.paintScreenBlack();
           sprintf_P((char *)buf,PSTR("x: %d y: %d item: %d"),p.x,p.y,item);
   Tft.drawString((char *)buf,210,160,1,WHITE);
     //  Tft.setDisplayDirect(DOWN2UP);
 Tft.drawStringP(PSTR("Register edit"),0,320,3,WHITE);
 Tft.drawStringP(PSTR("Use Matrix 1"),24,320,3,WHITE);
 Tft.drawStringP(PSTR("Use Matrix 2"),48,320,3,WHITE);
 Tft.drawStringP(PSTR("Use Matrix 3"),72,320,3,WHITE);
 Tft.drawStringP(PSTR("Reset Reg"),96,320,3,WHITE);
  Tft.drawStringP(PSTR("Compare Matrix"),120,320,3,WHITE);
 do {
        p = ts.getPoint();//wait for screen to be pressed
 }while (p.z < ts.pressureThreshhold);
   p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);
      item=p.x/24;
 switch (item) {
  case 0:
   editRegs();
   break;
   case 1:
  setmatrix(0);
    Tft.setOrientation(1);
    capImg();
    Tft.setDisplayDirect(DOWN2UP);
   break;
   case 2:
   //from ov7675 datasheet
  setmatrix(1);
    Tft.setOrientation(1);
    capImg();
    Tft.setDisplayDirect(DOWN2UP);
   break;
      case 3:
  setmatrix(2);
    Tft.setOrientation(1);
    capImg();
    Tft.setDisplayDirect(DOWN2UP);
   break;
   case 4:
   initCam();
   break;
   case 5:
   //compare matrixes
   do {
        p = ts.getPoint();//wait for screen to be pressed
   for (uint8_t a=0;a<3;a++)
   {
      setmatrix(a);
    Tft.setOrientation(1);
    capImg();
    Tft.setDisplayDirect(DOWN2UP);
   }
    }while (p.z < ts.pressureThreshhold);
   break;
 }
 
  }
  
}
void redrawGUI(void)
{
    Tft.paintScreenBlack();
  Tft.drawStringP(PSTR("val"),124,320,2,WHITE);
  Tft.drawStringP(PSTR("+"),124,256,4,WHITE);
  Tft.drawStringP(PSTR("-"),124,224,4,WHITE);
  Tft.drawStringP(PSTR("+16"),124,192,3,WHITE);
  Tft.drawStringP(PSTR("-16"),124,96,3,WHITE);
  Tft.drawStringP(PSTR("reg"),156,320,2,WHITE);
  Tft.drawStringP(PSTR("+"),156,256,4,WHITE);
  Tft.drawStringP(PSTR("-"),156,224,4,WHITE);
  Tft.drawStringP(PSTR("Snap"),188,128,4,WHITE);
  Tft.drawStringP(PSTR("Exit"),188,320,3,WHITE);
  Tft.drawStringP(PSTR("Step"),188,232,3,WHITE);
}
uint16_t leadingZeros(uint8_t x)
{
 uint8_t len=strlen((const char *)buf);
 uint16_t len2=320;
 len=8-len;
 while (len--)
 {
   Tft.drawChar('0',x, len2, 4, WHITE);
   len2-=32;
 }
 return len2;
}
void redrawT(uint8_t z,uint8_t regD)
{
    Tft.fillRectangle(0,320,112,320,BLACK);
   Tft.drawStringP(PSTR("Register:"),0,320,2,WHITE);
   utoa(z,(char *)buf,16);
   Tft.drawString((char *)buf,0,176,2,WHITE);
   utoa(regD,(char *)buf,16);
   Tft.drawString((char *)buf,16,320,2,WHITE);
   utoa(regD,(char *)buf,10);
   Tft.drawString((char *)buf,32,320,2,WHITE);
    utoa(regD,(char *)buf,2);
   Tft.drawString((char *)buf,80,leadingZeros(80),4,WHITE);  
}
void editRegs()
{
  uint8_t val;
  uint8_t z=1;
  redrawGUI();
   val=rdReg(z);
    redrawT(z,val);
  while (1)
  {
  p = ts.getPoint();
  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);
  
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > ts.pressureThreshhold) {
    if (p.x >= 188 && p.x <= 256 && p.y <= 128)
    {
      Tft.setOrientation(1);
      do {
        capImg();
        p = ts.getPoint();//wait for screen to be pressed
      } while (p.z < ts.pressureThreshhold);
      Tft.setDisplayDirect(DOWN2UP);
      redrawGUI();
      val=rdReg(z);
      redrawT(z,val);
    }
    else if (p.x >= 188 && p.x <= 212 && p.y >= 224)
    {
     return;
    }
    else if (p.x >= 188 && p.x <= 212 && p.y <= 232 && p.y >= 136)
    {
      uint8_t stp=0;
      do {
        wrReg(z,stp);
        _delay_ms(1000);//give register time to settle
        Tft.setOrientation(1);
        capImg();
         utoa(stp,(char *)buf,10);
         Tft.drawString((char *)buf,0,320,2,WHITE);
        p = ts.getPoint();//wait for screen to be pressed
        stp++;
      } while (p.z < ts.pressureThreshhold);
      Tft.setDisplayDirect(DOWN2UP);
      redrawGUI();
      val=rdReg(z);
      redrawT(z,val);
      
    }
    else if (p.x >= 80 && p.x <= 112 && p.y > 64)
    {
      uint8_t off=(p.y-64)/32;
      off=1<<off;
      val^=off;
      wrReg(z,val);
      val=rdReg(z);
      redrawT(z,val);
    }
    else if (p.x >= 126 && p.x <= 156)
    {
     //change register value
     if (p.y <= 256 && p.y >= 224)
      {
        val++;
        wrReg(z,val);
        val=rdReg(z);
        redrawT(z,val);
      }
      else if (p.y <= 224 && p.y >= 192)
      {
        val--;
        wrReg(z,val);
        val=rdReg(z);
        redrawT(z,val);
      }
      else if (p.y <= 160 && p.y >= 136)
      {
        val+=16;
        wrReg(z,val);
        val=rdReg(z);
        redrawT(z,val);
      }
      else if (p.y <= 112 && p.y >= 88)
      {
        val-=16;
        wrReg(z,val);
        val=rdReg(z);
        redrawT(z,val);
      }
    }
    else if (p.x >= 156 && p.x <= 188)
    {
      if (p.y <= 256 && p.y >= 224)
      {
        z++;
        val=rdReg(z);
        redrawT(z,val);
      }
      else if (p.y <= 224 && p.y >= 192)
      {
        z--;
        val=rdReg(z);
        redrawT(z,val);
      }
    }
  }
  uint8_t val2=rdReg(z);
  if (val2 != val)
  {
   val=val2;
   redrawT(z,val);
  }
  }//end of loop
   //Tft.paintScreenBlack();
 
}
void loop()
{
  capImg();
}
static uint16_t editer;//current vale
static uint16_t minE;
static uint16_t maxE;
void update_edit(const char * text)
{
  
  
}
void capImg(void)
{
    cli();
 uint16_t w,ww;
 uint8_t h;
 w=640;
 h=240;
 Tft.setXY(0,0);
     CS_LOW;
    RS_HIGH;
    RD_HIGH;
    DDRA=0xFF;
    DDRC=0;
    
 while (!(PINE&32)){}//wait for high
 while (PINE&32){}//wait for low
 while (h--)
 {
  ww=w;
  while (ww--)
  {
   WR_LOW;
   while (PINE&16){}//wait for low
   PORTA=PINC;
   WR_HIGH;
   while (!(PINE&16)){}//wait for high
  }
 }
 #ifndef rgb565
 sei();
 //convert yuv422 to rgb565 this may take awhile
  uint16_t x,y;
  for (y=0;y<240;y++)
  {
   uint16_t * bufPtr=(uint16_t *)buf;
   for (x=0;x<320;x++)
   {
   //  DDRA=0xFF;
    Tft.setXY(y,x);
  //  DDRA=0;
    //CS_LOW;
    //RD_LOW;
    //RS_HIGH;
    //Serial.println(*bufPtr);
    *bufPtr++=Tft.readRegister(0x22);
   }
  // 
   DDRA=0xFF;
   Tft.setXY(y,0);
    CS_LOW;
  RS_HIGH;
  RD_HIGH;
   for (x=0;x<640;x+=4)
   {
   uint16_t h1,h2;
   uint8_t rgb[6];
  rgb[0]=YUV2R((int32_t)buf[x],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[1]=YUV2G((int32_t)buf[x],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[2]=YUV2B((int32_t)buf[x],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[3]=YUV2R((int32_t)buf[x+2],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[4]=YUV2G((int32_t)buf[x+2],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[5]=YUV2B((int32_t)buf[x+2],(int32_t)buf[x+1],(int32_t)buf[x+3]);
  rgb[0]>>=3;
  rgb[1]>>=2;
  rgb[2]>>=3;
  rgb[3]>>=3;
  rgb[4]>>=2;
  rgb[5]>>=3;
  h1= ((uint16_t)rgb[0] << 11) | ((uint16_t)rgb[1] << 5) | (uint16_t)rgb[2];
  h2=((uint16_t)rgb[3] << 11) | ((uint16_t)rgb[4] << 5) | (uint16_t)rgb[5];
      // Tft.setPixel(y,x/2,h1);
     //    Tft.setPixel(y,x/2+1,h2);
      WR_LOW;
      PORTA=h1>>8;
      WR_HIGH;
      WR_LOW;
      PORTA=h1&255;
      WR_HIGH;
      WR_LOW;
      PORTA=h2>>8;
      WR_HIGH;
      WR_LOW;
      PORTA=h2&255;
      WR_HIGH;
   }
  }
 #endif
 CS_HIGH;
 sei();
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
  if (nack)
  {
       TWCR=_BV(TWINT) | _BV(TWEN);
   while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  if ((TWSR & 0xF8) != TW_MR_DATA_NACK)
      errorD(4);
    return TWDR;
  }
  else
  {
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
void wrSensorRegs8_8(const struct regval_list reglist[])
{

	int err = 0;

	unsigned int reg_addr,reg_val;

	const struct regval_list *next = reglist;
	while ((reg_addr != 0xff) | (reg_val != 0xff))
	{
		reg_addr = pgm_read_byte(&next->reg_num);
		reg_val = pgm_read_byte(&next->value);
		wrReg(reg_addr, reg_val);
	   	next++;
	} 

}
