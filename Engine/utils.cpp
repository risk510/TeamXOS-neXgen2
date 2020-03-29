#include "utils.h"
#include "Menu.h"
#include "MapPath.h"

extern MenuList Menu[9]; // Number of Menu Items
 
char* GamePathFromFindFilesC (char* gamepath)
{
	char * path = strrchr (gamepath,';');
	if (path) {
		return path+1;
	}
	return gamepath;
}

char* RemanderpathFromFindFilesC(char* gamepath) {
	char * path = strrchr(gamepath, ';');
	if(path)
		path[0]=0;
	return gamepath;
}

const char * GamePathFromFindFiles (const char* gamepath)
{
	char * path = strrchr (gamepath,';');
	if (path) {
		return path+1;
	}
	return gamepath;
}

const char* RemanderpathFromFindFiles(const char* gamepath) {
	char * path = strrchr(gamepath, ';');
	if(path)
		path[0]=0;
	return gamepath;
}

char* FileFromFilePathA(char* filepath) {
	char * filename = strrchr(filepath, '\\');
	if(filename)
		return filename+1;
	return filepath;
}

//Get FolderPath from FileName (Swolsten)
char* FolderNameFromFilePathA (char* filepath) {
	char *folder = PathFromFilePathA(filepath);
	return FileFromFilePathA(folder);
}

WCHAR* FileFromFilePathW(WCHAR* filepath) {
	WCHAR * filename = wcsrchr(filepath, '\\');
	if(filename)
		return filename+1;
	return filepath;
}

char* PathFromFilePathA(char* filepath) {
	char * filename = strrchr(filepath, '\\');
	if(filename)
		filename[0]=0;
	return filepath;
}

WCHAR* PathFromFilePathW(WCHAR* filepath) {
	WCHAR * filename = wcsrchr(filepath, '\\');
	if(filename)
		filename[0]=0;
	return filepath;
}

char* MountpointFromFilePathA(char* filepath)
{
	char * mountpoint = strrchr(filepath,':');
	if (mountpoint)
		return mountpoint+1;
	return mountpoint;
}


//Borrowed from MXM
BOOL  FileExists( LPCTSTR szFilename )
{
	BOOL bReturn = FALSE;
	

	DWORD dwAttributes;

	if ( szFilename  )
	{

		dwAttributes = GetFileAttributes( szFilename );
		if ( dwAttributes != (DWORD)-1 )
		{
			bReturn = TRUE;
		}
	}

	return bReturn;
}

DWORD FileLength( LPCTSTR szFilename )
{
	DWORD dwReturn = 0;

	if ( FileExists(szFilename) )
	{
		HANDLE hTemp;
		hTemp = CreateFile( szFilename, 
					   GENERIC_READ, 
					   FILE_SHARE_READ, 
					   NULL,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL );
		if ( hTemp != INVALID_HANDLE_VALUE )
		{
			dwReturn = GetFileSize( hTemp, NULL );
		}
		CloseHandle( hTemp );
	}
	return dwReturn;
}

/* Remeber to free() after use*/
WCHAR * ConvertStr(char * szIn)
{
 DWORD dwSize = strlen(szIn);
 DWORD count = mbstowcs(NULL, szIn, dwSize);
 WCHAR * szOut = (WCHAR*) malloc ((count + 1) * sizeof(WCHAR));
 mbstowcs(szOut, szIn, dwSize);
 return szOut;
}

char * ConvertStr(WCHAR * szIn)
{
 DWORD dwSize = wcslen(szIn) + 1;
 DWORD count = wcstombs(NULL, szIn, dwSize);
 char * szOut = (char *) malloc ((count + 1) * sizeof(char));
 wcstombs(szOut, szIn, dwSize);
 return szOut;
}

// Menu Cacheing and Loading.........
char* CacheFilename (int num)
{
	char *filename;
	switch (num) {
		case 0: filename="U:\\menu0.dat"; break;
		case 1: filename="U:\\menu1.dat"; break;
		case 2: filename="U:\\menu2.dat"; break;
		case 3: filename="U:\\menu3.dat"; break;
		case 4: filename="U:\\menu4.dat"; break;
		case 5: filename="U:\\menu5.dat"; break;
		case 6: filename="U:\\menu6.dat"; break;
		case 7: filename="U:\\menu7.dat"; break;
		case 8: filename="U:\\menu8.dat"; break;
		default: filename="U:\\default.dat"; break;
	}
		return filename;
}

HRESULT CreateCache(int menunum)
{
	char *filename;
	char *blank[0x600];
	ZeroMemory(blank,0x600);
	filename = CacheFilename(menunum);
	
	OutputDebugString ("Going to open file\n");
	HANDLE hFile=CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	DWORD written;
	DWORD savesize= 0x0600;
	
	for (int i = 0; i < Menu[menunum].NodeCount(); i++) {
		SetFilePointer (hFile, savesize * i, NULL, FILE_BEGIN);
		WriteFile (hFile,blank,0x600,&written,NULL);
		SetFilePointer (hFile, savesize * i + 0x100, NULL, FILE_BEGIN);
		WriteFile (hFile,Menu[menunum].IndexedMenu[i]->Title,0x100,&written,NULL);
		SetFilePointer (hFile, savesize * i + 0x200, NULL, FILE_BEGIN);
		WriteFile (hFile,Menu[menunum].IndexedMenu[i]->File,0x100,&written,NULL);
		SetFilePointer (hFile, savesize * i + 0x300, NULL, FILE_BEGIN);
		WriteFile (hFile,Menu[menunum].IndexedMenu[i]->Icon,0x100,&written,NULL);
		SetFilePointer (hFile, savesize * i + 0x400, NULL, FILE_BEGIN);
		WriteFile (hFile,Menu[menunum].IndexedMenu[i]->Cmdline,0x100,&written,NULL);
		SetFilePointer (hFile, savesize * i + 0x500, NULL, FILE_BEGIN);
		WriteFile (hFile,&Menu[menunum].IndexedMenu[i]->Action,0x04,&written,NULL);
		SetFilePointer (hFile, savesize * i + 0x504, NULL, FILE_BEGIN);
		WriteFile (hFile,&Menu[menunum].IndexedMenu[i]->TitleID,0x04,&written,NULL);
		SetFilePointer (hFile, savesize * i + 0x508, NULL, FILE_BEGIN);
		WriteFile (hFile,&Menu[menunum].IndexedMenu[i]->Region,0x04,&written,NULL);
		SetFilePointer (hFile, savesize * i + 0x50c, NULL, FILE_BEGIN);
		WriteFile (hFile,&Menu[menunum].IndexedMenu[i]->Rating,0x04,&written,NULL);
		SetFilePointer (hFile, savesize * i + 0x510, NULL, FILE_BEGIN);
		WriteFile (hFile,&Menu[menunum].IndexedMenu[i]->DBlocks,0x04,&written,NULL);
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT LoadCache(int menunum)
{
	char* filename;

	char Title[256]="";
	char File[256]="";
	char Icon[256]="";
	char Cmdline[256]="";
	DWORD Action=0;
	DWORD TitleID=0;
	DWORD Region=0;
	DWORD Rating=0;
	DWORD DBlocks=0;
	filename = CacheFilename(menunum);
	HANDLE hFile=CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if(hFile!=INVALID_HANDLE_VALUE) {
		DWORD readin;	
		DWORD savesize= 0x0600;
		DWORD dwSize = GetFileSize (hFile,NULL);
		int menucount = (int) dwSize / 0x600;
//		DbgPrintf ("Total saves in menu %d = %d\n",menunum,menucount);
		for (int i = 0; i < menucount; i++) 
		{
			ZeroMemory(Title,256);
			ZeroMemory(File,256);
			ZeroMemory(Icon,256);
			ZeroMemory(Cmdline,256);
			DWORD Action=0;
			DWORD TitleID=0;
			DWORD Region=0;
			DWORD Rating=0;
			DWORD DBlocks=0;
			SetFilePointer (hFile, savesize * i + 0x100, NULL, FILE_BEGIN);
			ReadFile (hFile,&Title,0x100,&readin,NULL);
			SetFilePointer (hFile, savesize * i + 0x200, NULL, FILE_BEGIN);
			ReadFile (hFile,&File,0x100,&readin,NULL);
			SetFilePointer (hFile, savesize * i + 0x300, NULL, FILE_BEGIN);
			ReadFile (hFile,&Icon,0x100,&readin,NULL);
			SetFilePointer (hFile, savesize * i + 0x400, NULL, FILE_BEGIN);
			ReadFile (hFile,&Cmdline,0x100,&readin,NULL);
			SetFilePointer (hFile, savesize * i + 0x500, NULL, FILE_BEGIN);
			ReadFile (hFile,&Action,0x04,&readin,NULL);
			SetFilePointer (hFile, savesize * i + 0x504, NULL, FILE_BEGIN);
			ReadFile (hFile,&TitleID,0x04,&readin,NULL);
			SetFilePointer (hFile, savesize * i + 0x508, NULL, FILE_BEGIN);
			ReadFile (hFile,&Region,0x04,&readin,NULL);
			SetFilePointer (hFile, savesize * i + 0x50c, NULL, FILE_BEGIN);
			ReadFile (hFile,&Rating,0x04,&readin,NULL);
			SetFilePointer (hFile, savesize * i + 0x510, NULL, FILE_BEGIN);
			ReadFile (hFile,&DBlocks,0x04,&readin,NULL);
			if (FileExists(File)) {
				Menu[menunum].AddFromCache(Title,File,Icon,Cmdline,Action,TitleID,Region,Rating,DBlocks);
			} 
	
		}
		CloseHandle(hFile);
		Menu[menunum].IndexList();
		return S_OK;
	} else {
		CloseHandle(hFile);
		return S_OK;
	}
}

HRESULT ProcessMenuCacheFile (int mnu, const char* where) 
{
	folders folder[10];
	int totalpaths=0;
	int a=0;
	const char *temppath=NULL;
	bool lastpath=false;
	char * filename = CacheFilename(mnu);

	int u,w,z=0;
	WIN32_FIND_DATA fd;
	HANDLE hFD;
	char *tmpc = new char[MAX_PATH];
	char *tmpb = new char[MAX_PATH];
	char **paths = new char*[128];
	char * filter=NULL;

	char foldername[MAX_PATH] = "";

	bool foundmatch;
	lastpath=false;

	while (!(lastpath))
	{
		if (strcmp(GamePathFromFindFiles (where),where) == 0)
			lastpath=true;
		folder[totalpaths].foldername =GamePathFromFindFiles(where);
		if (!(lastpath)) {
			totalpaths++;
			temppath=RemanderpathFromFindFiles(where);
			where=temppath;
		}
	}
	Menu[mnu].Clear();

	HRESULT CacheLoad = LoadCache(mnu);
	OutputDebugString ("CacheLoaded\n");

	a=0; 

	while (a <= totalpaths) 
	{
		sprintf(foldername,folder[a].foldername);
		u,w,z=0;
		for(z=0; z<128; z++) paths[z] = new char[MAX_PATH];
		strcpy(tmpb, foldername);
		sprintf(tmpc, "%s\\*", foldername);
		if((hFD=FindFirstFile(tmpc, &fd))!=INVALID_HANDLE_VALUE) {
			for(z=0;z<128;) {
				if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					sprintf(paths[z], "%s\\%s", tmpb, fd.cFileName);
					z++;
				}
							
				if(!FindNextFile(hFD, &fd)) {
					FindClose(hFD);
					break;
				}
			}
		
		
		w=0;
		if(z>0) for(u=0; u<z; u++) {
			if(w==0) {
				if(filter!=NULL)
					sprintf(tmpc, "%s\\%s", paths[u], filter);
				else
					sprintf(tmpc, "%s\\default.xbe", paths[u]);
				
				if((hFD=FindFirstFile(tmpc, &fd))!=INVALID_HANDLE_VALUE)
					w=1;
			}
			while(w==1) {
				sprintf(tmpb, "%s\\%s", paths[u], fd.cFileName);
				foundmatch=false;
				if (Menu[mnu].NodeCount() == 0) {
					OutputDebugString ("Didnt find any cache entires\n");
					foundmatch=false;
				} else {
					for (int x=0; x < Menu[mnu].NodeCount(); x++) {
						if (strcmp(Menu[mnu].IndexedMenu[x]->File,tmpb) == 0) {
							foundmatch=true;
							break;
						}
					}
				}
	
				if (!(foundmatch)) {
					Menu[mnu].AddXBE(tmpb);
					Menu[mnu].IndexList();
				}

				if(!FindNextFile(hFD, &fd))
					w=2;
			}

			if(w==2) {
				FindClose(hFD);
				w=0;
			}
		}
		a++;
		} else {
			a++;
		}
	}
	Menu[mnu].IndexList();
	CreateCache(mnu);

	return S_OK;
}

int FindFiles(int mnu, const char *where, const char *filter, int norecursion, int type)
{
	folders folder[10];
	const char *temppath=NULL;
	int itm=0;
	bool lastpath=false;
	bool usecache=myDash.bUseCache;

	// Strat Processing....
	if (mnu >=0 && !where) {
		OutputDebugString ("Loading search values from xml\n");
		switch (mnu) {
			case 0:	where = myDash.sAppsPath.c_str(); break;
			case 1: where = myDash.sEmusPath.c_str(); break;
			case 2: where = myDash.sGamesPath.c_str(); break;
			default: return 0; break;
		}
	}

	if (filter || norecursion) {
		usecache=false;
	}

		if(mnu>=0 && where) 
	{
		if (usecache) {
			OutputDebugString ("Going to ProcessMenuCacheFile\n");
			ProcessMenuCacheFile (mnu,where);
			OutputDebugString ("Returned OK\n");
		} else {
			OutputDebugString ("Not going to use cache file\n");
			while (!(lastpath))
			{
				if (strcmp(GamePathFromFindFiles (where),where) == 0)
					lastpath=true;
				folder[itm].foldername = GamePathFromFindFiles(where);
				if (!(lastpath)) {
					itm++;
					temppath=RemanderpathFromFindFiles(where);
					where=temppath;
				}

			}
		
			int FoldersToIndex = itm;
			char *cachefile=CacheFilename(mnu);
			for (int i=0; i <= FoldersToIndex; i++) 
			{
				if (FileExists(folder[i].foldername)) {
					Menu[mnu].AddFiles(folder[i].foldername, filter, norecursion, type);
					Menu[mnu].IndexList();
					HRESULT tmp = CreateCache(mnu);
				}
			}
		}
	}

	Menu[mnu].NodeCount();

	return 1;
}

void SetXBOXLEDColour(int colourval)
{
	switch ( colourval )
	{
		case 1:
			{
				//Turn LED Steady Red...
				XKUtils::SetXBOXLEDStatus(XKUtils::LED_REGISTER_CYCLE0_RED|XKUtils::LED_REGISTER_CYCLE2_RED|
				XKUtils::LED_REGISTER_CYCLE1_RED|XKUtils::LED_REGISTER_CYCLE3_RED);
			}
			break;
		case 2:
			{
				//Turn LED Steady Orange...
				XKUtils::SetXBOXLEDStatus(
				XKUtils::LED_REGISTER_CYCLE0_GREEN|XKUtils::LED_REGISTER_CYCLE0_RED|
				XKUtils::LED_REGISTER_CYCLE1_GREEN|XKUtils::LED_REGISTER_CYCLE1_RED|
				XKUtils::LED_REGISTER_CYCLE2_GREEN|XKUtils::LED_REGISTER_CYCLE2_RED|
				XKUtils::LED_REGISTER_CYCLE3_GREEN|XKUtils::LED_REGISTER_CYCLE3_RED);
			}
			break;
		case 3:
			{
				//Turn LED Steady Green..
				XKUtils::SetXBOXLEDStatus(XKUtils::LED_REGISTER_CYCLE0_GREEN|XKUtils::LED_REGISTER_CYCLE2_GREEN|
				XKUtils::LED_REGISTER_CYCLE1_GREEN|XKUtils::LED_REGISTER_CYCLE3_GREEN);
			}
			break;
		case 4:
			{
				//Fast Flash Red..
				XKUtils::SetXBOXLEDStatus(XKUtils::LED_REGISTER_CYCLE0_RED|XKUtils::LED_REGISTER_CYCLE2_RED);
			}
			break;
		case 5:
			{
				//Fast Flash Green / Red..
				XKUtils::SetXBOXLEDStatus(XKUtils::LED_REGISTER_CYCLE0_GREEN|XKUtils::LED_REGISTER_CYCLE3_RED);
			}
			break;
		case 6:
			{
				//Fast Flash Green..
				XKUtils::SetXBOXLEDStatus(XKUtils::LED_REGISTER_CYCLE0_GREEN|XKUtils::LED_REGISTER_CYCLE2_GREEN);
			}
			break;
		case 7:
			{
				//Fast Flash Orange..
				XKUtils::SetXBOXLEDStatus(
				XKUtils::LED_REGISTER_CYCLE0_GREEN|XKUtils::LED_REGISTER_CYCLE0_RED|
				XKUtils::LED_REGISTER_CYCLE2_GREEN|XKUtils::LED_REGISTER_CYCLE2_RED);
			}
			break;
		case 8:
			{
				//Green, Orange, Red Flash
				XKUtils::SetXBOXLEDStatus(
				XKUtils::LED_REGISTER_CYCLE0_GREEN|XKUtils::LED_REGISTER_CYCLE0_GREEN|
				XKUtils::LED_REGISTER_CYCLE1_GREEN|XKUtils::LED_REGISTER_CYCLE1_RED|
				XKUtils::LED_REGISTER_CYCLE2_RED|XKUtils::LED_REGISTER_CYCLE2_RED);
			}
			break;
		default:
			//Turn LED Steady Green..
			XKUtils::SetXBOXLEDStatus(XKUtils::LED_REGISTER_CYCLE0_GREEN|XKUtils::LED_REGISTER_CYCLE2_GREEN|
			XKUtils::LED_REGISTER_CYCLE1_GREEN|XKUtils::LED_REGISTER_CYCLE3_GREEN);
		break;
	}

}

void FormatDrivePartition(char *Drive)
{
	WCHAR* partition;
	partition = ConvertStr(Drive); // Char to WCHAR Conversion
	formatdriveletter((WCHAR *)partition); // Format Drive
	free(partition); // Free Mem of specified Partition Drive 
}