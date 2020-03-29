#ifndef __XBOXINFO_H__
#define __XBOXINFO_H__


#include <xtl.h>
#include <stdio.h>
#include "XKUtils\\XKExports.h"




void XI_GetVersion( char* szBuffer );
void XI_GetEncoder( char* szBuffer );


void XI_GetProgramFullPath( char* szBuffer ); // Returns - \Device\Folder\Prog.xbe
void XI_GetProgramPath( char* szBuffer );	  // Returns - \Device\Folder
void XI_GetProgramDevice( char* szBuffer );	  // Returns - \Device
void XI_GetProgramFolder( char* szBuffer );	  // Returns - \Folder
void XI_GetProgramXBE( char* szBuffer );	  // Returns - Prog.xbe



#endif