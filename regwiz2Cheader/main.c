#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char ** argv){
	if(argc!=3){
		puts("input output not specifed");
		return 1;
	}
	FILE * fi=fopen(argv[1],"r");
	FILE * fo=fopen(argv[2],"w");
	fputs("const struct regval_listP MT9D111_init[] PROGMEM={\n",fo);
	while(1){
		int in=fgetc(fi);
		if(in==EOF)
			break;
		switch(in){
			case '[':
			case ';':
				fputs("//",fo);
			break;
			/*case ']':
				//do nothing
			break;*/
			case 'D':
				//replace "ELAY = " with {MT9D111_DELAY, 0x00,
				{char temp[10];
					fgets(temp,8,fi);
					if(strcmp("ELAY = ",temp)==0){
						fputs("\t{MT9D111_DELAY, 0x00,",fo);
						while(1){
							int ins=fgetc(fi);
							if(ins==EOF)
								break;
							else if(ins=='\n'){
								fseek(fo,-1,SEEK_CUR);
								fputs("},\n",fo);
								break;
							}else
								fputc(ins,fo);
						}
					}else{
						fputc(in,fo);
						fputs(temp,fo);
				}	}
			break;
			case 'R':
				//replace "EG = " with \t{ the letter R is already read
				{char temp[8];
				fgets(temp,6,fi);
				if(strcmp("EG = ",temp)==0){
					fputs("\t{",fo);
					while(1){
						int ins=fgetc(fi);
						if(ins==EOF)
							break;
						if(ins==' '){
							ins=fgetc(fi);
							if(ins==EOF)
								break;
							else if(ins==' '){
								fputs("},",fo);
								break;
							}else
								fputc(ins,fo);
						}else
							fputc(ins,fo);
					}
				}else{
					fputc(in,fo);
					fputs(temp,fo);
				}}
			break;
			default:
				fputc(in,fo);
			break;
		}
	}
	fputs("\t{EndRegs_MT9D111,0xFF,0xFFFF}\n};",fo);
	fclose(fi);
	fclose(fo);
	return 0;
}
