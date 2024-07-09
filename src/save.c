/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

//thanks to spicyjpeg for helping me understand this code

#include "save.h"

#include <libmcrd.h>
#include "stage.h"
				  
	        //HAS to be BASCUS-scusid,somename
#define savetitle "bu00:BASCUS-62123Grace"
#define savename  "PSXFunkin: Amazing Grace - Rotten Smoothie"

static const u8 saveIconPalette[32] = 
{
	0x4B, 0x9D, 0x6C, 0xA5, 0xAE, 0xA9, 0x42, 0x88, 0x7D, 0xAA, 0x1A, 0x9E,
	0xD8, 0x95, 0x51, 0x91, 0xB6, 0x95, 0xF8, 0xA1, 0x74, 0x91, 0x7F, 0xCF,
	0x84, 0x90, 0x30, 0x8D, 0x0F, 0x89, 0xD0, 0xB1
};

static const u8 saveIconImage[128] = 
{
 	0x00, 0x21, 0x22, 0x22, 0x22, 0x32, 0x11, 0x00, 0x10, 0x22, 0x42, 0x55,
	0x55, 0x34, 0x12, 0x00, 0x21, 0x42, 0x55, 0x66, 0x56, 0x55, 0x14, 0x11,
	0x27, 0x65, 0x36, 0x63, 0x38, 0x63, 0x46, 0x11, 0x22, 0x66, 0x66, 0x86,
	0x66, 0x86, 0x68, 0x11, 0x92, 0x88, 0x88, 0x66, 0x86, 0x86, 0x88, 0x19,
	0x82, 0x8A, 0x68, 0x88, 0x88, 0x88, 0xA8, 0x18, 0xA2, 0x3A, 0xB3, 0xBB,
	0xBB, 0x3B, 0xA3, 0x2A, 0x72, 0x3A, 0x3C, 0x33, 0x33, 0xC3, 0x7C, 0x17,
	0xD2, 0xCD, 0xCC, 0xCC, 0xCC, 0xCC, 0x73, 0x17, 0xD2, 0x3D, 0xCC, 0xCC,
	0xCC, 0xCC, 0xED, 0x2D, 0xE3, 0xDE, 0xC3, 0xCC, 0xCC, 0xD3, 0xEE, 0x3E,
	0xDF, 0xEE, 0x3E, 0xC3, 0x33, 0xED, 0xEE, 0xFD, 0xF2, 0xED, 0xEE, 0xEE,
	0xEE, 0xEE, 0xDE, 0x2F, 0x22, 0xDF, 0xEE, 0xEE, 0xEE, 0xEE, 0xFD, 0x2F,
	0x21, 0xF2, 0xEE, 0xEE, 0xEE, 0xEE, 0x2F, 0x12
};

static void toShiftJIS(u8 *buffer, const char *text)
{
    int pos = 0;
    for (u32 i = 0; i < strlen(text); i++) 
    {
        u8 c = text[i];
        if (c >= '0' && c <= '9') { buffer[pos++] = 0x82; buffer[pos++] = 0x4F + c - '0'; }
        else if (c >= 'A' && c <= 'Z') { buffer[pos++] = 0x82; buffer[pos++] = 0x60 + c - 'A'; }
        else if (c >= 'a' && c <= 'z') { buffer[pos++] = 0x82; buffer[pos++] = 0x81 + c - 'a'; }
        else if (c == '(') { buffer[pos++] = 0x81; buffer[pos++] = 0x69; }
        else if (c == ')') { buffer[pos++] = 0x81; buffer[pos++] = 0x6A; }
        else /* space */ { buffer[pos++] = 0x81; buffer[pos++] = 0x40; }
    }
}

static void initSaveFile(SaveFile *file, const char *name) 
{
	file->id = 0x4353;
 	file->iconDisplayFlag = 0x11;
 	file->iconBlockNum = 1;
  	toShiftJIS(file->title, name);
 	memcpy(file->iconPalette, saveIconPalette, 32);
 	memcpy(file->iconImage, saveIconImage, 128);
}

void defaultSettings()
{
	stage.prefs.songtimer = 1;

	for (StageId i = 0; i < StageId_Max; i++)
	{
		for (StageDiff j = 0; j < StageDiff_Max; j++)
			stage.prefs.savescore[i][j] = 0;
	}
}

boolean ReadSave()
{
	int fd = open(savetitle, 0x0001);
	if (fd < 0) // file doesnt exist 
		return false;

	SaveFile file;
	if (read(fd, (void *) &file, sizeof(SaveFile)) == sizeof(SaveFile)) 
		printf("ok\n");
	else {
		printf("read error\n");
		return false;
	}
	memcpy((void *) &stage.prefs, (const void *) file.saveData, sizeof(stage.prefs));
	close(fd);
	return true;
}

void WriteSave()
{	
	int fd = open(savetitle, 0x0002);

	if (fd < 0) // if save doesnt exist make one
		fd =  open(savetitle, 0x0202 | (1 << 16));

	SaveFile file;
	initSaveFile(&file, savename);
  	memcpy((void *) file.saveData, (const void *) &stage.prefs, sizeof(stage.prefs));
	
	if (fd >= 0) {
	  	if (write(fd, (void *) &file, sizeof(SaveFile)) == sizeof(SaveFile)) 
	  		printf("ok\n");
	 	else 
	 		printf("write error\n");  // if save doesnt exist do a error
		close(fd);
	} 
	else 
		printf("open error %d\n", fd);  // failed to save
}

//initiliaze memory card
void MCRD_Init(void)
{
  InitCARD(1);
	StartCARD();
	_bu_init();	
	ChangeClearPAD(0);
}