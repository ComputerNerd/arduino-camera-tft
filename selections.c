#include "TFT.h"
#include "TouchScreen.h"
#include "twicam.h"
#include "camregdef.h"
#include "config.h"
#include "MT9D111_regs.h"
#include "captureimage.h"
#include "twicam.h"
#include "gammaedit.h"
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "mmc.h"
#include "filebrowser.h"
#include "exiticon.h"
#ifndef MT9D11
void setmatrix(uint8_t id){
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
const char menu4[] PROGMEM = "qqvga preview";
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
const char menup21[] PROGMEM = "Previous Page";
const char *const menu_tablep2[] PROGMEM = {
	menup20,menup21
};
#ifdef ov7670
const char maxtrix0[] PROGMEM = "Maxtrix yuv422";
const char maxtrix1[] PROGMEM = "Maxtrix 2";
const char maxtrix2[] PROGMEM = "Maxtrix rgb565";
const char *const maxtrix_table[] PROGMEM = {maxtrix0,maxtrix1,maxtrix2};
#elif defined ov7740
const char maxtrix0[] PROGMEM = "Maxtrix off";
const char maxtrix1[] PROGMEM = "OVT Maxtrix";
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
	for(item=0;item<240;++item){
		for(x=0;x<320;++x){
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
void menu(void){
	uint16_t x,y,z;
	while (1){
		switch (selection((const char**)menu_table,10)){
			case 0:
			#ifdef MT9D111
				setRes(qvga);
				setColor(rgb565);
				MT9D111Refresh();
				editRegs(0);
			#else
				setColor(rgb565);
				setRes(qvga);
				editRegs();
			#endif
		break;
		case 1:
			#ifdef MT9D111
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
			//compare matrixes
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
				setColor(rgb565);
				setRes(qqvga);
				#ifdef MT9D111
					MT9D111Refresh();
				#endif
				do{
					getPoint(&x,&y,&z);
					tft_setOrientation(1);
					capImgqqvga(160);
					tft_setDisplayDirect(DOWN2UP);
				}while(z<10);
				setRes(qvga);
			break;
			case 5:
				#ifdef ov7670
					setColor(rgb565);
				#endif
				gammaEdit();
			break;
			case 6:
				//File browser
				//start by listing files
				browserSD();
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
				setRes(qvga);
				setColor(rgb565);
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
					setColor(yuv422);
				#endif
				switch(reso){
					case 0:
						#ifdef ov7670
							wrReg(REG_COM7, COM7_BAYER); // BGBGBG... GRGRGR...
						#elif defined MT9D111
							//setupt jpeg
						#endif
					break;
					case 1:
						#ifdef MT9D111
							setRes(svga);
						#else
							setRes(qvga);
							setColor(yuv422);
						#endif
					break;
					#ifdef MT9D111
					case 2:
						setRes(qvga);
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
				#elif defined MT9D111
					MT9D111Refresh();
				#endif
				tft_setOrientation(1);
				do{
					#ifdef MT9D111
						switch(reso){
							case 0:
								capJpeg();
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
				switch(selection((const char**)menu_tablep2,2)){
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
						//previous page
					break;
				}
			break;
		}
	}
}
