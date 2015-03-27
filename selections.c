#include <stdlib.h>
#include <avr/pgmspace.h>
#include "config.h"
#include <util/delay.h>
#include "TFT.h"
#include "TouchScreen.h"
#include "twicam.h"
#include "camregdef.h"
#include "MT9D111_regs.h"
#include "captureimage.h"
#include "twicam.h"
#include "gammaedit.h"
#include "regedit.h"
#ifdef haveSDcard
#include "mmc.h"
#endif
#include "filebrowser.h"
#include "exiticon.h"
#ifndef MT9D111
static void setmatrix(uint8_t id){
	switch (id) {
		case 0:
			#ifdef ov7670
				wrReg(MTX1,0x80);        
				wrReg(MTX2,0x80);      
				wrReg(MTX3,0x00);        
				wrReg(MTX4,0x22);        
				wrReg(MTX5,0x5e);        
				wrReg(MTX6,0x80);        
				wrReg(MTXS,0x9e); 
			#elif defined ov7740
				wrReg(ISP_CTRL01,rdReg(ISP_CTRL01)&(~ISP_CTRL01_CMX_enable));
			#endif
		break;
		case 1:
			#ifdef ov7670
				wrReg(MTX1,0x40);        
				wrReg(MTX2,0x34);      
				wrReg(MTX3,0x0c);        
				wrReg(MTX4,0x17);
				wrReg(MTX5,0x29);        
				wrReg(MTX6,0x40);
				wrReg(MTXS,0x1A);
			#elif defined ov7740
				wrReg(ISP_CTRL01,rdReg(ISP_CTRL01)|ISP_CTRL01_CMX_enable);
			#endif
		break;
		#ifdef ov7670
		case 2:
			wrReg(MTX1,0xB3);        
			wrReg(MTX2,0xB3);      
			wrReg(MTX3,0x0);        
			wrReg(MTX4,0x3D);        
			wrReg(MTX5,0xA7);        
			wrReg(MTX6,0xE4);
			wrReg(MTXS,0x9E);
		break;
		#endif
	}
}
#endif
const char menu0[] PROGMEM = "Register edit";
#ifdef MT9D111
const char menu1[] PROGMEM = "Reg edit micro";
#else
const char menu1[] PROGMEM = "Pick a matrix";
#endif
const char menu2[] PROGMEM = "Reset Reg";
const char menu3[] PROGMEM = "Compare Matrix";
const char menu4[] PROGMEM = "QQVGA preview";
#ifndef MT9D111
const char menu5[] PROGMEM = "gamma edit";
#else
const char menu5[] PROGMEM = "Fixme";
#endif
const char menu6[] PROGMEM = "File Browser";
#ifdef ov7670
const char menu7[] PROGMEM = "White balance";
#else
const char menu7[] PROGMEM = "FIXME!";
#endif
const char menu8[] PROGMEM = "Image to PC";
const char menu9[] PROGMEM = "Next Page";

const char *const menu_table[] PROGMEM = {
	menu0,menu1,menu2,menu3,
	menu4,menu5,menu6,menu7,
	menu8,menu9
};
const char menup20[] PROGMEM = "Touch Test";
const char menup21[] PROGMEM = "Time Lapse";
const char menup22[] PROGMEM = "Previous Page";
const char *const menu_tablep2[] PROGMEM = {
	menup20,menup21,menup22
};
#ifdef ov7670
const char maxtrix0[] PROGMEM = "Matrix YUV422";
const char maxtrix1[] PROGMEM = "Matrix 2";
const char maxtrix2[] PROGMEM = "Matrix RGB565";
const char *const maxtrix_table[] PROGMEM = {maxtrix0,maxtrix1,maxtrix2};
#elif defined ov7740
const char maxtrix0[] PROGMEM = "Matrix off";
const char maxtrix1[] PROGMEM = "OVT Matrix";
const char *const maxtrix_table[] PROGMEM = {maxtrix0,maxtrix1};
#endif
const char wb0[] PROGMEM="No AWB";
const char wb1[] PROGMEM="Advanced AWB";
const char wb2[] PROGMEM="Simple AWB";
const char wb3[] PROGMEM="Sunny";
const char wb4[] PROGMEM="Cloudy";
const char wb5[] PROGMEM="Office";
const char wb6[] PROGMEM="Home";
const char *const wb_table[] PROGMEM={wb0,wb1,wb2,wb3,wb4,wb5,wb6};
#ifdef MT9D111
const char res0[] PROGMEM="UXGA jpeg";
const char res1[] PROGMEM="SVGA";
const char res2[] PROGMEM="QVGA";
const char res3[] PROGMEM="Main Menu";
const char *const res_tab[] PROGMEM={res0,res1,res2,res3};
#else
const char res0[] PROGMEM="VGA";
const char res1[] PROGMEM="QVGA";
const char res2[] PROGMEM="Main Menu";
const char *const res_tab[] PROGMEM={res0,res1,res2};
#endif
uint8_t selection(const char ** table,uint8_t maxitems){
	uint8_t item;
	uint16_t x,y,z;
	tft_setOrientation(1);
	tft_setXY(0,0);
	CS_LOW;
    RS_HIGH;
    RD_HIGH;
    DDRA=0xFF;
    z=31<<8;
	item=240;
	while(item--){
		x=320;
		while(x--){
			WR_HIGH;
			PORTA=0;
			WR_LOW;
			WR_HIGH;
			PORTA=z>>8;
			WR_LOW;
		}
		z-=33;
	}
	tft_setDisplayDirect(DOWN2UP);
	item=0;
	for(x=0;x<maxitems*24;x+=24){
		tft_drawStringP((PGM_P)pgm_read_word(&(table[item])),x,320,3,WHITE);
		++item;
	}
	while(1){
		do{
			getPoint(&x,&y,&z);
		}while(z<10);
		item=x/24;
		if(item<maxitems)
			break;
	}
	tft_drawStringP((PGM_P)pgm_read_word(&(table[item])),item*24,320,3,RED);
	_delay_ms(100);
	tft_paintScreenBlack();
	return item;
}
#ifdef ov7670
const char config0[] PROGMEM = "Linux driver";
const char config1[] PROGMEM = "suwa-koubou";
const char config2[] PROGMEM = "Arducam";
const char *const config_tab[] PROGMEM = {config0,config1,config2};
void configSel(void){
	initCam(selection((const char**)config_tab,3));
}
#endif

#ifdef MT9D111
static const uint8_t jpegHeader[] PROGMEM ={//619 bytes
255,216,255,224,0,16,74,70,73,70,0,1,2,0,0,1,0,1,0,0,255,219,0,132,0,28,19,21,25,21,18,28,
25,23,25,32,30,28,33,42,70,46,42,39,39,42,86,61,65,51,70,102,89,107,105,100,89,98,96,112,126,161,137,112,
119,152,121,96,98,140,191,142,152,166,172,180,182,180,109,135,198,212,196,175,210,161,177,180,173,1,30,32,32,42,37,42,
82,46,46,82,173,116,98,116,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,
173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,255,192,0,17,8,4,
176,6,64,3,0,33,0,1,17,1,2,17,1,255,196,0,31,0,0,1,5,1,1,1,1,1,1,0,0,0,0,0,
0,0,0,1,2,3,4,5,6,7,8,9,10,11,255,196,0,181,16,0,2,1,3,3,2,4,3,5,5,4,4,0,
0,1,125,1,2,3,0,4,17,5,18,33,49,65,6,19,81,97,7,34,113,20,50,129,145,161,8,35,66,177,193,21,
82,209,240,36,51,98,114,130,9,10,22,23,24,25,26,37,38,39,40,41,42,52,53,54,55,56,57,58,67,68,69,70,
71,72,73,74,83,84,85,86,87,88,89,90,99,100,101,102,103,104,105,106,115,116,117,118,119,120,121,122,131,132,133,134,
135,136,137,138,146,147,148,149,150,151,152,153,154,162,163,164,165,166,167,168,169,170,178,179,180,181,182,183,184,185,186,194,
195,196,197,198,199,200,201,202,210,211,212,213,214,215,216,217,218,225,226,227,228,229,230,231,232,233,234,241,242,243,244,245,
246,247,248,249,250,255,196,0,31,1,0,3,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,2,3,4,5,
6,7,8,9,10,11,255,196,0,181,17,0,2,1,2,4,4,3,4,7,5,4,4,0,1,2,119,0,1,2,3,17,
4,5,33,49,6,18,65,81,7,97,113,19,34,50,129,8,20,66,145,161,177,193,9,35,51,82,240,21,98,114,209,10,
22,36,52,225,37,241,23,24,25,26,38,39,40,41,42,53,54,55,56,57,58,67,68,69,70,71,72,73,74,83,84,85,
86,87,88,89,90,99,100,101,102,103,104,105,106,115,116,117,118,119,120,121,122,130,131,132,133,134,135,136,137,138,146,147,
148,149,150,151,152,153,154,162,163,164,165,166,167,168,169,170,178,179,180,181,182,183,184,185,186,194,195,196,197,198,199,200,
201,202,210,211,212,213,214,215,216,217,218,226,227,228,229,230,231,232,233,234,242,243,244,245,246,247,248,249,250,255,218,0,
12,3,0,0,1,17,2,17,0,63,0};
#endif
static inline void serialWrB(uint8_t dat){
	UDR0=dat;
	while (!( UCSR0A & (1<<UDRE0))); //wait for byte to transmit
}
static void BSend(void){
	uint16_t res=tft_readRegister(0x22);
	serialWrB(res>>8);
	serialWrB(res&255);
}
void menu(void){
	uint16_t x,y,z;
	while (1){
		switch (selection((const char**)menu_table,10)){
			case 0:
			#ifdef MT9D111
				setRes(QVGA);
				setColor(RGB565);
				MT9D111Refresh();
				editRegs(0);
			#else
				setColor(RGB565);
				setRes(QVGA);
				editRegs();
			#endif
		break;
		case 1:
			#ifdef MT9D111
				setRes(QVGA);
				setColor(RGB565);
				MT9D111Refresh();
				editRegs(1);
			#else
				#ifdef ov7740
					setmatrix(selection((const char**)maxtrix_table,2));
				#else
					setmatrix(selection((const char**)maxtrix_table,3));
				#endif
				tft_setOrientation(1);
				capImg();
				tft_setDisplayDirect(DOWN2UP);
			#endif
		break;
		case 2:
			#ifdef ov7670
				initCam(0);
			#else
				initCam();
			#endif
		break;
		#ifndef MT9D111
		case 3:
			//compare matrices
			tft_setOrientation(1);
			do{
				getPoint(&x,&y,&z);
				uint8_t a;
				#ifdef ov7670
					for (a=0;a<3;a++){
				#elif defined ov7740
					for (a=0;a<2;a++){
				#endif
					setmatrix(a);
					capImg();
				}
			}while (z < 10);
			tft_setDisplayDirect(DOWN2UP);
		break;
		#endif
			case 4:
				setColor(RGB565);
				setRes(QQVGA);
				#ifdef MT9D111
					MT9D111Refresh();
				#endif
				do{
					getPoint(&x,&y,&z);
					tft_setOrientation(1);
					capImgqqvga(160);
					tft_setDisplayDirect(DOWN2UP);
				}while(z<10);
				setRes(QVGA);
			break;
			case 5:
				#ifdef ov7670
					setColor(RGB565);
				#endif
				gammaEdit();
			break;
			case 6:
				//File browser
				//start by listing files
				#ifdef haveSDcard
					browserSD();
				#else
					tft_drawStringP(PSTR("No SD card"),16,320,3,WHITE);
					_delay_ms(666);
				#endif
			break;
			case 7:
				#ifdef ov7670
				{uint8_t pick=selection((const char**)wb_table,7);//registers from http://thinksmallthings.wordpress.com/2012/10/25/white-balance-control-with-ov7670/
				if(pick==1||pick==2){
					wrReg(0x13, 0xE7);
					wrReg(0x6F, 0x9E|(pick&1));
				}else{
					wrReg(0x13, 0xE5);
					switch(pick){
						case 2:
							wrReg(0x01, 0x5A);
							wrReg(0x02, 0x5C);
						break;
						case 3:
							wrReg(0x01, 0x58);
							wrReg(0x02, 0x60);
						break;
						case 4:
							wrReg(0x01, 0x84);
							wrReg(0x02, 0x4C);
						break;
						case 5:
							wrReg(0x01, 0x96);
							wrReg(0x02, 0x40);
						break;
					}
				}
				tft_setOrientation(1);
				setRes(QVGA);
				setColor(RGB565);
				capImg();
				tft_setDisplayDirect(DOWN2UP);}
				#endif
			break;
			case 8:
				#ifdef ov7670
				wrReg(0x1e,rdReg(0x1e)&(~(1<<5))&(~(1<<4)));
				#endif
				{
				#ifdef MT9D111
					uint8_t reso=selection((const char**)res_tab,4);
				#else
					uint8_t reso=selection((const char**)res_tab,3);
				#endif
				#ifdef MT9D111
					wrReg16(0xF0,2);//page 2
					wrReg16(0x0D,0);
					setColor(YUV422);
				#endif
				switch(reso){
					case 0:
						#ifdef ov7670
							wrReg(REG_COM7, COM7_BAYER); // BGBGBG... GRGRGR...
						#elif defined MT9D111
							//setup jpeg
							MT9D111JPegCapture();
						#endif
					break;
					case 1:
						#ifdef MT9D111
							setRes(SVGA);
						#else
							setRes(QVGA);
							setColor(YUV422);
						#endif
					break;
					#ifdef MT9D111
					case 2:
						setRes(QVGA);
					break;
					#endif
					default:
						goto theEnd;
					break;
				}
				#ifdef ov7670
					_delay_ms(200);
					if(reso)
						wrReg(0x11,1);
					else
						wrReg(0x11,2);
				#endif
				tft_setOrientation(1);
				do{
					#ifdef MT9D111
						switch(reso){
							case 0:
								{uint32_t jpgSize=capJpeg();
								serialWrB('R');
								serialWrB('D');
								serialWrB('Y');
								uint16_t w;
								uint8_t h=0;
								serialWrB(jpgSize&255);
								serialWrB(jpgSize>>8);
								serialWrB(jpgSize>>16);
								serialWrB(jpgSize>>24);
								while(jpgSize){
									if(jpgSize>=640){
										for (w=0;w<320;++w){
											tft_setXY(h,w);
											BSend();
										}
										++h;
										jpgSize-=640;
									}else{
										for(w=0;w<jpgSize/2;++w){
											tft_setXY(h,w);
											BSend();
										}
										if(jpgSize&1){
											tft_setXY(h,w);
											uint16_t res=tft_readRegister(0x22);
											serialWrB(res>>8);
										}
										jpgSize=0;
									}
								}
								}
							break;
							case 1:
								capImgPC();
							break;
							case 2:
								capImgPCqvga();
							break;
						}
					#else
						if(reso)
							capImgPCqvga();
						else
							capImgPC();
					#endif
					getPoint(&x,&y,&z);
				}while(z<10);
theEnd:
				tft_setDisplayDirect(DOWN2UP);
				}
			break;
			case 9:
				switch(selection((const char**)menu_tablep2,3)){
					case 0:
						{
							tft_drawImage_P(exit_icon,32,32,0,0);
							uint16_t x1,y1;
							do{
								getPoint(&x,&y,&z);
							}while(z<10);
							if((y<=32)&&(x<=32))
									break;
							tft_fillCircle(x,y,4,WHITE);
							while(1){
								x1=x;
								y1=y;
								do{
									getPoint(&x,&y,&z);
								}while(z<10);
								tft_fillRectangle(224,320,16,36,BLACK);
								tft_fillCircle(x1,y1,4,BLACK);
								tft_fillCircle(x,y,4,WHITE);
								if((y<=32)&&(x<=32))
									break;
								char temp[6];
								utoa(x,temp,10);
								tft_drawString(temp,224,320,1,WHITE);
								utoa(y,temp,10);
								tft_drawString(temp,232,320,1,WHITE);
							}
						}
					break;
					case 1:
						//time lapse
						#ifdef MT9D111
							//MT9D111Refresh();
							//Since This is a time lapse we want to be in "video" mode
							MT9D111JPegCapture();
							/*do{
								_delay_ms(10);
								wrReg16(0xC6,(1<<15)|(1<<13)|(1<<8)|4);
							}while(rdReg16(0xC8)<4);
							waitStateMT9D111(3);*/
						#endif
						#ifdef haveSDcard
						{
							char buf[24];
							uint16_t imgc=0;
							tft_setOrientation(1);
							do{
								FIL Fo;
								#ifdef MT9D111
									uint32_t jpgSize=capJpeg();
								#else
									capImg();
								#endif
								utoa(imgc,buf,10);
								#ifdef MT9D111
								strcat(buf,".JPG");
								#else
								strcat(buf,".RAW");
								#endif
								f_open(&Fo,buf,FA_WRITE|FA_CREATE_ALWAYS);
								++imgc;
								UINT written;
								uint16_t w;
								uint8_t h;
								uint16_t cpybuf[320];
								#ifdef MT9D111
									h=0;
									uint8_t * cpyptr=cpybuf;
									for(w=0;w<619;++w)
										*cpyptr++=pgm_read_byte_near(jpegHeader+w);
									f_write(&Fo,cpybuf,619,&written);
									while(jpgSize){
										if(jpgSize>=640){
											for (w=0;w<320;++w){
												tft_setXY(h,w);
												cpybuf[w]=__builtin_bswap16(tft_readRegister(0x22));//Either bytes need to be swapped or a byte is being missed
											}
											f_write(&Fo,cpybuf,640,&written);
											++h;
											jpgSize-=640;
										}else{
											for(w=0;w<jpgSize/2;++w){
												tft_setXY(h,w);
												cpybuf[w]=__builtin_bswap16(tft_readRegister(0x22));
											}
											f_write(&Fo,cpybuf,jpgSize,&written);
											if(jpgSize&1){
												tft_setXY(h,w);
												cpybuf[w]=tft_readRegister(0x22);
												f_write(&Fo,&cpybuf[w],1,&written);
											}
											jpgSize=0;
										}
									}
									cpybuf[0]=0xFFD9;
									f_write(&Fo,cpybuf,2,&written);
								#else
								for (h=0;h<240;++h){
									for (w=0;w<320;++w){
										tft_setXY(h,w);
										cpybuf[w]=tft_readRegister(0x22);
									}
									f_write(&Fo,cpybuf,640,&written);
								}
								#endif
								f_close(&Fo);
								getPoint(&x,&y,&z);
							}while(z<10);
							tft_setDisplayDirect(DOWN2UP);
							#ifdef MT9D111
								MT9D111DoPreview();
							#endif
						}
						#else
							tft_drawStringP(PSTR("No SD card"),16,320,3,WHITE);
							_delay_ms(666);
						#endif
					break;
					case 2:
						//previous page
					break;
				}
			break;
		}
	}
}
