#pragma once

#include <xtl.h>
#include <string.h>
#include "Main.h"

typedef struct folders {
	const char * foldername;
} folders;

const char* GamePathFromFindFiles (const char *gamepath);
const char* RemanderpathFromFindFiles (const char *gamepath);
char* GamePathFromFindFilesC (char *gamepath);
char* RemanderpathFromFindFilesC (char *gamepath);
char* FileFromFilePathA(char* filepath);
char* FolderNameFromFilePathA(char* filepath);
WCHAR* FileFromFilePathW(WCHAR* filepath);
char* PathFromFilePathA(char* filepath);
WCHAR* PathFromFilePathW(WCHAR* filepath);
char* MountpointFromFilePathA(char* filepath);

BOOL  FileExists( LPCTSTR szFilename );
DWORD FileLength( LPCTSTR szFilename );

WCHAR * ConvertStr(char * szIn);
char * ConvertStr(WCHAR * szIn);

/* Menu Cache */
HRESULT CreateCache (int menunum);
HRESULT LoadCache(int menunum);
HRESULT ProcessMenuCacheFile (int mnu, const char* where);
char* CacheFilename (int num);
int FindFiles(int mnu, const char *where, const char *filter, int norecursion, int type);
void SetXBOXLEDColour(int colourval);

void FormatDrivePartition(char *Drive);