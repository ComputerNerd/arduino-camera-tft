#pragma once
#include "config.h"
#ifdef MT9D111
#define FORMAT_YCBCR422 0
#define FORMAT_YCBCR420 1
#define FORMAT_MONOCHROME 2
int CreateJpegHeader(char *header, int width, int height, int format, int restart_int, int qscale);
#endif
