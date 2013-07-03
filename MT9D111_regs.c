#include "config.h"
#include "twicam.h"
#ifdef MT9D111
//This is from the Arducam project not my work
//Also from http://www.beagleboard.org/~arago/esc-boston-2009/mvista/beaglekernel/drivers/media/video/omap/sensor_ex3691.c
#include <avr/pgmspace.h>

/*******************************************************************************************
 #  Display resolution standards #
        QCIF    : 176 x 144
        CIF     : 352 x 288
        QVGA    : 320 x 240
        VGA     : 640 x 480 
        SVGA    : 800 x 600 
        XGA     : 1024 x 768 
        WXGA    : 1280 x 800 
        QVGA    : 1280 x 960 
        SXGA    : 1280 x 1024 
        SXGA+   : 1400 x 1050 
        WSXGA+  : 1680 x 1050 
        UXGA    : 1600 x 1200 
        WUXGA   : 1920 x 1200 
        QXGA    : 2048 x 1536
********************************************************************************************/

const struct regval_listP MT9D111_init[] PROGMEM=
{
	{MT9D111_DELAY, 0x00, 1 },
	{0,0x65,0xA000},
	{1,0xC3,0x0501},
	{0,0x0D, 0x0021},
	{0,0x0D, 0x0000},
	{MT9D111_DELAY, 0x00, 1 },
	/*{1,0xC6, 0xA103}, //SEQ_CMD
	{1,0xC8, 0x0002}, //SEQ_CMD, Do capture
	{1,0xA4,(1<<5)},
	{2,0x0D,0},*/
	{EndRegs_MT9D111, 0xFF, 0xFFFF }
};

const struct regval_list default_size_a_list[] PROGMEM =
{
	{0xf0, 0x1},                                         
	{0xC6, 0x2703	}, //MODE_OUTPUT_WIDTH_A
	{0xC8, 0x00B0	}, //MODE_OUTPUT_WIDTH_A
	{0xC6, 0x2705	}, //MODE_OUTPUT_HEIGHT_A
	{0xC8, 0x0090	}, //MODE_OUTPUT_HEIGHT_A
	{0xC6, 0xA103	}, //SEQ_CMD
	{0xC8, 0x0005	}, //SEQ_CMD
	{0xff, 0xffff }
};
const struct regval_list default_size_b_list[] PROGMEM =
{
	{0xf0, 0x1},                             
	{0xC6, 0x2707	}, //MODE_OUTPUT_WIDTH_B
	{0xC8, 0x0140	}, //MODE_OUTPUT_WIDTH_B
	{0xC6, 0x2709	}, //MODE_OUTPUT_HEIGHT_B
	{0xC8, 0x00F0	}, //MODE_OUTPUT_HEIGHT_B
	{0xC6, 0xA103	}, //SEQ_CMD
	{0xC8, 0x0005	}, //SEQ_CMD
	{0xff, 0xffff }
};
// For QVGA ( 320 x 240) on 3BA module
const struct regval_list MT9D111_QVGA[] PROGMEM =
{
	{0xf0, 0x1},
	{0xC6, 0x2707	}, //MODE_OUTPUT_WIDTH_B
	{0xC8, 0x0140	}, //MODE_OUTPUT_WIDTH_B
	{0xC6, 0x2709	}, //MODE_OUTPUT_HEIGHT_B
	{0xC8, 0x00F0	}, //MODE_OUTPUT_HEIGHT_B
	{0xC6, 0xA103	}, //SEQ_CMD
	{0xC8, 0x0005	}, //SEQ_CMD
    {0xff, 0xffff }
};
const struct regval_list MT9D111_RGB565[] PROGMEM =
{
	{0xf0, 0x1},
	{0x09, 0x2	}, //Disable factory bypass
	{0xC6, 0xA77D	}, //MODE_OUTPUT_FORMAT_A
	{0xC8, 0x0020	}, //MODE_OUTPUT_FORMAT_A; RGB565
	{0xC6, 0xA103	}, //SEQ_CMD
	{0xC8, 0x0005	}, //SEQ_CMD, refresh
	{0x09, 0x2	}, //Disable factory bypass
	{0xC6, 0xA77E	}, //MODE_OUTPUT_FORMAT_B
	{0xC8, 0x0020	}, //MODE_OUTPUT_FORMAT_B; RGB565
	{0xC6, 0xA103	}, //SEQ_CMD
	{0xC8, 0x0005	}, //SEQ_CMD, refresh
	{0xff, 0xffff }
};
// For VGA ( 640 x 480) on 3BA module
const struct regval_list MT9D111_VGA[] PROGMEM =
{
	{0xf0, 0x0001    },
    {0xC6, 0x2703    }, //MODE_OUTPUT_WIDTH_A
    {0xC8, 0x0280    }, //MODE_OUTPUT_WIDTH_A
    {0xC6, 0x2705    }, //MODE_OUTPUT_HEIGHT_A
    {0xC8, 0x01E0    }, //MODE_OUTPUT_HEIGHT_A
    {0xC6, 0x2707    }, //MODE_OUTPUT_WIDTH_B
    {0xC8, 0x0280    }, //MODE_OUTPUT_WIDTH_B
    {0xC6, 0x2709    }, //MODE_OUTPUT_HEIGHT_B
    {0xC8, 0x01E0    }, //MODE_OUTPUT_HEIGHT_B
    {0xC6, 0x2779    }, //Spoof Frame Width
    {0xC8, 0x0280    }, //Spoof Frame Width
    {0xC6, 0x277B    }, //Spoof Frame Height
    {0xC8, 0x01E0    }, //Spoof Frame Height
    {0xC6, 0xA103    }, //SEQ_CMD
    {0xC8, 0x0005    }, //SEQ_CMD
    {0xff, 0xffff	 }
};
#endif
