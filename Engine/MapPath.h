#ifndef __MAPPATH_H__
#define __MAPPATH_H__


#include <xtl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Main.h"
#include "XKUtils\\XKUtils.h"

extern "C" XBOXAPI LONG WINAPI XapiFormatFATVolume( IN PANSI_STRING pcVolume);

// Path Structure
struct pathconv_s {
	char * DriveLetter;
	char * FullPath;
};


// Path Variable
extern pathconv_s pathconv_table[];



// Prototypes
void	resetpaths();
char*	getpath(char * DriveLetter);
HRESULT mappath(char * Device, char * DriveLetter);
HRESULT formatdriveletter(WCHAR *DriveLetter);



#endif