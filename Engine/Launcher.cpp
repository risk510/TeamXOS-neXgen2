// Launcher .cpp
#include <xtl.h>
//#include "DriveStatus\\undocumented.h"
#include "Launcher.h"

extern "C" 
{
	//extern void WINAPI HalInitiateShutdown(void);
	XBOXAPI DWORD WINAPI XWriteTitleInfoAndRebootA(IN LPSTR pszLaunchPath,IN LPSTR pszDDrivePath, IN DWORD dwLaunchDataType, IN DWORD dwTitleId, IN PCUSTOM_LAUNCH_DATA pLaunchData);
};

CXBLaunchXBE myXBELauncher;

// Custom Launch
CUSTOM_LAUNCH_DATA g_launchData;
int g_autoLaunchGame;
int g_launchReturnXBE;

// Constructor
CXBLaunchXBE::CXBLaunchXBE()
{
}

// Destructor
CXBLaunchXBE::~CXBLaunchXBE()
{
}

void CXBLaunchXBE::Launch_XBOX_Game()
{
	XBOXDriveStatus.GetDriveState();
	if( XBOXDriveStatus.m_iDVDType == XBI_DVD_XBOX ) { LaunchMyXBE("D:\\default.xbe",NULL); }
}

void CXBLaunchXBE::Launch_XBOX_Movie()
{
	XBOXDriveStatus.GetDriveState();

	if( XBOXDriveStatus.m_iDVDType == XBI_DVD_MOVIE ) { 
		if ( FileExists(myDash.sDVDPlayer.c_str()) )
		{
			char sAppPath[255];
			sprintf(sAppPath, "%s", myDash.sDVDPlayer.c_str()); 
			LaunchMyXBE(sAppPath,NULL);
		} else {
			LaunchMyXBE("C:\\xboxdash.xbe", NULL);
		}
	}
}

void CXBLaunchXBE::Custom_Launch_XBOX_Game(bool bPatch, char * XBEFile)
{
	if ( FileExists(XBEFile) )
	{
		if (bPatch)
		{
			F_COUNTRY NewCountry = GetAutoPatchCountry(XBEFile);
			F_VIDEO NewVideo = GetAutoPatchVideo(XBEFile);
			if (NewCountry != COUNTRY_NULL)
			if (NewVideo != VIDEO_NULL)
				PatchCountryVideo(NewCountry,NewVideo);
				LaunchMyXBE(XBEFile,NULL);
		}
		else
		{
			LaunchMyXBE(XBEFile,NULL);
		}
	}
}



// Launch XBE
HRESULT CXBLaunchXBE::LaunchMyXBE(char * XBEFile, PLAUNCH_DATA LDATA)
{
	HRESULT r;
	char umFilename[5000];
	char mFilename[5000];
	char mDrivePath[5000];
	char mDriveLetter[5000];
	char mDevicePath[5000];
	char mFullPath[5000];
	char asspath[5000];
	draw_splash(1.0f);
	my3D.Device()->PersistDisplay();
	strcpy(umFilename, XBEFile);
	strcpy(mFilename,FileFromFilePathA(umFilename));
	strcpy(mDrivePath,PathFromFilePathA(umFilename));

	int tm=0;
	while(mDrivePath[tm]!=':' && mDrivePath[tm]!=0) 
		tm++;
	lstrcpyn(mDriveLetter,mDrivePath, tm+2);
	mDriveLetter[tm+3]=0;
	strcpy(mDevicePath,getpath(mDriveLetter));
	sprintf(mFullPath, "%s%s", mDevicePath, (char *)mDrivePath+2);
	if(mFullPath[(strlen(mFullPath)-1)]=='\\')
		mFullPath[(strlen(mFullPath)-1)]=0;
	OutputDebugStringA("Mapping ");
	OutputDebugStringA(mFullPath);
	OutputDebugStringA(" to D:\n");
	mappath(mFullPath, "\\??\\D:");
	OutputDebugStringA("Launching ");
	OutputDebugStringA(mFullPath);
	OutputDebugStringA("\\");
	OutputDebugStringA(mFilename);
	OutputDebugStringA("\n");
	sprintf(asspath, "D:\\%s", mFilename);
	r=XLaunchNewImage(asspath, NULL);

	return r;
}

// Custom Launch
//GetXbeID Borrowed from MXM
DWORD CXBLaunchXBE::GetXbeID( LPCTSTR szFilePath )
{
	DWORD dwReturn = 0;
	HANDLE hFile;
	DWORD dwCertificateLocation;
	DWORD dwLoadAddress;
	DWORD dwRead;
	
	hFile = CreateFile( szFilePath, 
						GENERIC_READ, 
						FILE_SHARE_READ, 
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL );
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		if ( SetFilePointer(	hFile,  0x104, NULL, FILE_BEGIN ) == 0x104 )
		{
			if ( ReadFile( hFile, &dwLoadAddress, 4, &dwRead, NULL ) )
			{
				if ( SetFilePointer(	hFile,  0x118, NULL, FILE_BEGIN ) == 0x118 )
				{
					if ( ReadFile( hFile, &dwCertificateLocation, 4, &dwRead, NULL ) )
					{
						dwCertificateLocation -= dwLoadAddress;
						// Add offset into file
						dwCertificateLocation += 8;
						if ( SetFilePointer(	hFile,  dwCertificateLocation, NULL, FILE_BEGIN ) == dwCertificateLocation )
						{
							dwReturn = 0;
							ReadFile( hFile, &dwReturn, sizeof(DWORD), &dwRead, NULL );
							if ( dwRead != sizeof(DWORD) )
							{
								dwReturn = 0;
							}
						}

					}
				}
			}
		}
		CloseHandle(hFile);
	}
	return dwReturn;
}

//XGetCustomLaunchData
//
//If there is a valid filename to load, then g_autoLaunchGame will be set to 1
//If there is a valid return XBE, then g_launchReturnXBE will be set to 1
int CXBLaunchXBE::XGetCustomLaunchData()
{
	DWORD launchType;

	g_autoLaunchGame = 0 ;
	g_launchReturnXBE = 0 ;

	memset( &g_launchData, 0, sizeof( CUSTOM_LAUNCH_DATA ) ) ;


	if ( ( XGetLaunchInfo( &launchType,(PLAUNCH_DATA)&g_launchData) == ERROR_SUCCESS ) )
	{
		if  ( ( launchType == LDT_TITLE ) &&  ( g_launchData.magic == CUSTOM_LAUNCH_MAGIC ) )
		{
			if ( g_launchData.szFilename[0] ) 
				g_autoLaunchGame = 1 ;

			if ( g_launchData.szLaunchXBEOnExit[0] ) 
				g_launchReturnXBE = 1 ;

			return 1 ;
		}
		else
		{
		}


	}

	return 0 ;
}

void CXBLaunchXBE::XReturnToLaunchingXBE( )
{
	if ( g_launchReturnXBE )
	{
		LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
//		XWriteTitleInfoAndRebootA( g_launchData.szLaunchXBEOnExit, g_launchData.szRemap_D_As, LDT_TITLE, 0, &LaunchData);
	}
	else
	{
		LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
		XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
	}
}

void CXBLaunchXBE::XLaunchNewImageWithParams( char *szXBEName, char *szMap_D_As, PCUSTOM_LAUNCH_DATA pLaunchData )
{
	char szDevice[MAX_PATH];
	DWORD dwTitleID;

	dwTitleID = (DWORD)xbeReader.GetXboxTitleID(szXBEName);

	sprintf( szDevice, "%s\\%s", szMap_D_As, szXBEName ) ;

	XWriteTitleInfoAndRebootA( szXBEName, szMap_D_As, LDT_TITLE, dwTitleID, pLaunchData); 
	
}