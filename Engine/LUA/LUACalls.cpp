#include "LUACalls.h"
#include "..\\Threads.h"
#include "..\\Launcher.h"
#include <setjmp.h>

#include "..\\xboxinfo.h"

MenuList Menu[9]; // Number of Menu Items
CXboxInfo XBOXDriveStatus; // Xbox

// Globals
lua_State*	LUA_STATE;			// Main Secne State
string		LUA_SCENE_BUFFER;	// Scene Buffer


// Days
char szXboxDay[7][20] = {
	"Sunday", "Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday", "Saturday"
};
//Months
char szXboxMonth[12][20] = {
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"
};

// Variables For LUACalls.cpp Only!
bool g_bEnableXrayRendering = false;
bool g_bEnableAlphaRendering = false;

bool bCheckedNetStatus = false;

int CPUTempC;
int CPUTempF;
int SYSTempC;
int SYSTempF;
char Temperature[255];
char szFanSpeed[255];
DWORD dwFontColour = 0xFF000000;
DWORD NetStatus; // Network Connection Information
DWORD XnAddrStatus; // Network Address Information
XNADDR xnaddr;

// FTPClient
ftp* theFtpConnection=NULL;

int progress=0;
DWORD progressbar=0;
DWORD progressmax=0;
char progressmessage[128]="";
/////////////////////////////


// Register LUA Calls
void RegisterLUACalls( )
{
	// Set The State
	LUA_STATE = lua_open( );

      luaopen_base(LUA_STATE);             /* opens the basic library */
      luaopen_table(LUA_STATE);            /* opens the table library */
      luaopen_io(LUA_STATE);               /* opens the I/O library */
      luaopen_string(LUA_STATE);           /* opens the string lib. */
      luaopen_math(LUA_STATE);             /* opens the math lib. */

	// Register The Calls
	lua_register( LUA_STATE, "SceneLoad", lua_SceneLoad );

	lua_register( LUA_STATE, "TrayOpen", lua_TrayOpen );
	lua_register( LUA_STATE, "TrayClose", lua_TrayClose );
	lua_register( LUA_STATE, "XBOX_POWEROFF", lua_XboxPoweroff );
	lua_register( LUA_STATE, "XBOX_REBOOT", lua_XboxReboot );
	lua_register( LUA_STATE, "XBOX_POWERCYCLE", lua_XboxPowercycle );

	lua_register( LUA_STATE, "ClearScreen", lua_ClearScreen );

	lua_register( LUA_STATE, "Present", lua_Present );
	lua_register( LUA_STATE, "MeshLoad", lua_MeshLoad );
	lua_register( LUA_STATE, "MeshFree", lua_MeshFree );
	lua_register( LUA_STATE, "MeshPosition", lua_MeshPosition );
	lua_register( LUA_STATE, "MeshScale", lua_MeshScale );
	lua_register( LUA_STATE, "MeshRotate", lua_MeshRotate );
	lua_register( LUA_STATE, "MeshRender", lua_MeshRender );

	lua_register( LUA_STATE, "CameraSetFOV", lua_CameraSetFOV );
	lua_register( LUA_STATE, "CameraSetAspect", lua_CameraSetAspect ); //fixed typo
	lua_register( LUA_STATE, "CameraSetPosition", lua_CameraSetPosition );
	lua_register( LUA_STATE, "CameraSetLookAt", lua_CameraSetLookAt);
	lua_register( LUA_STATE, "CameraMoveForward", lua_CameraMoveForward);
	lua_register( LUA_STATE, "CameraMoveBackward", lua_CameraMoveBackward);
	lua_register( LUA_STATE, "CameraMoveLeft", lua_CameraMoveLeft);
	lua_register( LUA_STATE, "CameraMoveRight", lua_CameraMoveRight);

	lua_register( LUA_STATE, "FontLoad", lua_FontLoad );
	lua_register( LUA_STATE, "FontFree", lua_FontFree );
	lua_register( LUA_STATE, "FontColour", lua_FontColour );
	lua_register( LUA_STATE, "FontDrawText", lua_FontDrawText );
	lua_register( LUA_STATE, "FontPosition", lua_FontPosition );
	lua_register( LUA_STATE, "XrayEnable", lua_XrayEnable );
	lua_register( LUA_STATE, "XrayDisable", lua_XrayDisable );
	lua_register( LUA_STATE, "AlphaEnable", lua_AlphaEnable );
	lua_register( LUA_STATE, "AlphaDisable", lua_AlphaDisable );
	lua_register( LUA_STATE, "FogRender", lua_FogRender );
	lua_register( LUA_STATE, "FogColour", lua_FogColour );
	lua_register( LUA_STATE, "StringMerge", lua_StringMerge );
	lua_register( LUA_STATE, "GetMenuItems", lua_getmenuitems );
	lua_register( LUA_STATE, "GetMenuItemName", lua_getmenuitemname );
	lua_register( LUA_STATE, "GetItemRegion", lua_getitemregion );
	lua_register( LUA_STATE, "GetItemRating", lua_getitemrating );
	lua_register( LUA_STATE, "GetItemFilename", lua_getitemfilename );
	lua_register( LUA_STATE, "GetItemIconPath", lua_getitemiconpath );
	lua_register( LUA_STATE, "GetItemDblocks", lua_getitemdblocks );
	lua_register( LUA_STATE, "GetItemType", lua_getitemtype );

	lua_register( LUA_STATE, "TextureLoad", lua_TextureLoad );
	lua_register( LUA_STATE, "TextureFree", lua_TextureFree );
	lua_register( LUA_STATE, "DrawTexturedQuad", lua_DrawTexturedQuad );
	lua_register( LUA_STATE, "DrawTexturedQuadAlpha", lua_DrawTexturedQuadAlpha );
    

	lua_register( LUA_STATE, "FlakeLoad", lua_FlakeLoad );
	/*lua_register( LUA_STATE, "FlakeFree", lua_FlakeFree );*/
	lua_register( LUA_STATE, "FlakeBoundaries", lua_FlakeSetBoundaries );
	lua_register( LUA_STATE, "FlakeSpeed", lua_FlakeSpeed );
	lua_register( LUA_STATE, "FlakeTotal", lua_FlakeTotal );
	lua_register( LUA_STATE, "FlakeScale", lua_FlakeScale );
	lua_register( LUA_STATE, "FlakeRotateSpeed", lua_FlakeRotateSpeed );
	lua_register( LUA_STATE, "FlakeRender", lua_FlakeRender );

	lua_register( LUA_STATE, "SoundLoad", lua_WaveLoad );
	lua_register( LUA_STATE, "SoundFree", lua_WaveFree );
	lua_register( LUA_STATE, "SoundPlay", lua_WavePlay );

	lua_register( LUA_STATE, "LaunchXBE", lua_LaunchXBE );
	lua_register( LUA_STATE, "FormatDrive", lua_FormatDrive );
	lua_register( LUA_STATE, "ClearCache", lua_ClearCache );

	// FTP Functions
	lua_register(LUA_STATE, "ftpopen", lua_ftpopen);
	lua_register(LUA_STATE, "ftpclose", lua_ftpclose);
	lua_register(LUA_STATE, "ftplist", lua_ftplist);
	lua_register(LUA_STATE, "ftpcd", lua_ftpcd);
	lua_register(LUA_STATE, "ftpsetuser", lua_ftpsetuser);
	lua_register(LUA_STATE, "ftpsetpass", lua_ftpsetpass);
	lua_register(LUA_STATE, "ftplcd", lua_ftplcd);
	lua_register(LUA_STATE, "ftpput", lua_ftpput);
	lua_register(LUA_STATE, "ftpget", lua_ftpget);
	lua_register(LUA_STATE, "ftplls", lua_ftplls);
	lua_register(LUA_STATE, "ftpbinary", lua_ftpbinary);
	lua_register(LUA_STATE, "ftpascii", lua_ftpascii);
	lua_register(LUA_STATE, "ftppwd", lua_ftppwd);

	lua_register( LUA_STATE, "SetSkin", lua_SetSkin );
	lua_register( LUA_STATE, "SaveConfig", lua_SaveConfig );

//	lua_register( LUA_STATE, "LightPosition", lua_LightAmbient );
//	lua_register( LUA_STATE, "LightAmbient", lua_LightAmbient );
	//lua_register( LUA_STATE, "LightAmbient", lua_LightAmbient );

	lua_register( LUA_STATE, "XBOX_LOCK_HDD", lua_LockHDD );
	lua_register( LUA_STATE, "XBOX_UNLOCK_HDD", lua_UnlockHDD );

	lua_register( LUA_STATE, "XBOX_AUTO_UPDATE", lua_AutoUpdate );

	lua_register (LUA_STATE, "SetLCDText", lua_SetLCDText);


	// Open Math Library
	lua_mathlibopen( LUA_STATE );
	lua_pushnumber( LUA_STATE, (double)D3DX_PI );
	lua_setglobal( LUA_STATE, "PI" );

	// Set LUA_PATH Global

	lua_pushstring(LUA_STATE,"SKIN:\\?;SKIN:\\?.lua;SKINS:\\COMMON\\?;SKINS:\\COMMON\\?.lua");
	lua_setglobal(LUA_STATE,"LUA_PATH");

	// Strat FTPClient if net Connnected
	if (IsEthernetConnected())
	{
		theFtpConnection = new ftp();
	}

}

// Update The Variables In Script
void UpdateLUAVariables()
{
	// Font Styles
	lua_pushnumber( LUA_STATE, (int)1 );
	lua_setglobal( LUA_STATE, "XBOX_FONT_LEFT" );
	lua_pushnumber( LUA_STATE, (int)2 );
	lua_setglobal( LUA_STATE, "XBOX_FONT_CENTER" );
	lua_pushnumber( LUA_STATE, (int)3 );
	lua_setglobal( LUA_STATE, "XBOX_FONT_RIGHT" );

	// Fog Options
	lua_pushnumber( LUA_STATE, (int)1 );
	lua_setglobal( LUA_STATE, "D3DFOG_EXP" );
	lua_pushnumber( LUA_STATE, (int)2 );
	lua_setglobal( LUA_STATE, "D3DFOG_EXP2" );
	lua_pushnumber( LUA_STATE, (int)3 );
	lua_setglobal( LUA_STATE, "D3DFOG_LINEAR" );

	 // PI - 3.14***
	lua_pushnumber( LUA_STATE, (double)D3DX_PI );
	lua_setglobal( LUA_STATE, "PI" );

	// XBOX version & encoder
	lua_pushstring( LUA_STATE, g_szXboxVersion );
	lua_setglobal( LUA_STATE, "XBOX_VERSION" );
	lua_pushstring( LUA_STATE, g_szXboxEncoder );
	lua_setglobal( LUA_STATE, "XBOX_ENCODER" );

	// neXgen Version / Build Information
	lua_pushstring(LUA_STATE, "neXgen Version 2.0");
	lua_setglobal(LUA_STATE, "NEXGEN_VERSION");
	lua_pushstring(LUA_STATE, "Build: 1425");
	lua_setglobal(LUA_STATE, "NEXGEN_BUILD");

	// IP -- Moved to Updateable Variables and returning IP the propper way not fixed from XML.
	/*lua_pushstring( LUA_STATE, myDash.sIP.c_str() );
	lua_setglobal( LUA_STATE, "XBOX_IP_ADDRESS" );*/
 
	// XBOX Free Space (BYTES)
	MEMORYSTATUS pMemStat;
	GlobalMemoryStatus( &pMemStat );
	lua_pushnumber( LUA_STATE, pMemStat.dwAvailPhys );
	lua_setglobal( LUA_STATE, "XBOX_MEMORY" );

	// XBOX Time & Date
	SYSTEMTIME pTime;
	char szBuffer[1000];
	//GetSystemTime( &pTime ); // WorldTime
	GetLocalTime( &pTime ); // Local Time
	FILETIME  NewTime;
	SystemTimeToFileTime(&pTime,  &NewTime);
	// Hour
	if( pTime.wHour < 10 ) { sprintf( szBuffer, "%i%i", 0, pTime.wHour ); }
	else { sprintf( szBuffer, "%i", pTime.wHour ); }
	lua_pushstring( LUA_STATE, szBuffer );
	lua_setglobal( LUA_STATE, "XBOX_TIME_HOUR" );
	// Minutes
	if( pTime.wMinute < 10 ) { sprintf( szBuffer, "%i%i", 0, pTime.wMinute ); }
	else { sprintf( szBuffer, "%i", pTime.wMinute ); }
	lua_pushstring( LUA_STATE, szBuffer );
	lua_setglobal( LUA_STATE, "XBOX_TIME_MINUTE" );
	// Seconds
	if( pTime.wSecond < 10 ) { sprintf( szBuffer, "%i%i", 0, pTime.wSecond ); }
	else { sprintf( szBuffer, "%i", pTime.wSecond ); }
	lua_pushstring( LUA_STATE, szBuffer );
	lua_setglobal( LUA_STATE, "XBOX_TIME_SECOND" );
	// Milliseconds
	if( pTime.wMilliseconds < 10 ) { sprintf( szBuffer, "%i%i", 0, pTime.wMilliseconds ); }
	else { sprintf( szBuffer, "%i", pTime.wMilliseconds ); }
	lua_pushstring( LUA_STATE, szBuffer );
	lua_setglobal( LUA_STATE, "XBOX_TIME_MILLISECOND" );
	// Day Number
	if( pTime.wDay < 10 ) { sprintf( szBuffer, "%i%i", 0, pTime.wDay ); }
	else { sprintf( szBuffer, "%i", pTime.wDay ); }
	lua_pushstring( LUA_STATE, szBuffer );
	lua_setglobal( LUA_STATE, "XBOX_DATE_DAY" );
	// Day String
	lua_pushstring( LUA_STATE, szXboxDay[pTime.wDayOfWeek] );
	lua_setglobal( LUA_STATE, "XBOX_DATE_DAY_STRING" );
	// Month Number
	if( pTime.wMonth < 10 ) { sprintf( szBuffer, "%i%i", 0, pTime.wMonth ); }
	else { sprintf( szBuffer, "%i", pTime.wMonth ); }
	lua_pushstring( LUA_STATE, szBuffer );
	lua_setglobal( LUA_STATE, "XBOX_DATE_MONTH" );
	// Month String
	lua_pushstring( LUA_STATE, szXboxMonth[pTime.wMonth-1] );
	lua_setglobal( LUA_STATE, "XBOX_DATE_MONTH_STRING" );
	// Year
	sprintf( szBuffer, "%i", pTime.wYear );
	lua_pushstring( LUA_STATE, szBuffer );
	lua_setglobal( LUA_STATE, "XBOX_DATE_YEAR" );

	// Menu List Items
	lua_pushnumber( LUA_STATE, 0 );
	lua_setglobal( LUA_STATE, "APP_LIST" );
	lua_pushnumber( LUA_STATE, 1 );
	lua_setglobal( LUA_STATE, "EMU_LIST" );
	lua_pushnumber( LUA_STATE, 2 );
	lua_setglobal( LUA_STATE, "GAME_LIST" );
	lua_pushnumber( LUA_STATE, 3 );
	lua_setglobal( LUA_STATE, "SKIN_LIST" );

	// XBOX DriveStatus
	XBOXDriveStatus.GetDriveState();
	if ( XBOXDriveStatus.m_bTrayOpen ) { lua_pushstring(LUA_STATE, "Tray Open"); }
	else if( XBOXDriveStatus.m_iDVDType == XBI_DVD_INIT ) { lua_pushstring(LUA_STATE, "Init"); }
	else if( XBOXDriveStatus.m_iDVDType == XBI_DVD_EMPTY ) { lua_pushstring(LUA_STATE, "Empty"); }
	else if( XBOXDriveStatus.m_iDVDType == XBI_DVD_XBOX ) { lua_pushstring(LUA_STATE, "Game"); }
	else if( XBOXDriveStatus.m_iDVDType == XBI_DVD_MOVIE ) { lua_pushstring(LUA_STATE, "Movie"); }
	else { lua_pushstring(LUA_STATE, "Unknown"); }
	lua_setglobal(LUA_STATE, "XBOX_DVD_DRIVE_STATUS");

	// Hard Drive Space
	string sFree = XGetDriveFree( 'C' );
	lua_pushstring( LUA_STATE, sFree.c_str() );
	lua_setglobal( LUA_STATE, "XBOX_FREE_C" );
	sFree = XGetDriveFree( 'E' );
	lua_pushstring( LUA_STATE, sFree.c_str() );
	lua_setglobal( LUA_STATE, "XBOX_FREE_E" );
	sFree = XGetDriveFree( 'F' );
	lua_pushstring( LUA_STATE, sFree.c_str() );
	lua_setglobal( LUA_STATE, "XBOX_FREE_F" );
	sFree = XGetDriveFree( 'G' );
	lua_pushstring( LUA_STATE, sFree.c_str() );
	lua_setglobal( LUA_STATE, "XBOX_FREE_G" );
	sFree = XGetDriveSize( 'C' );
	lua_pushstring( LUA_STATE, sFree.c_str() );
	lua_setglobal( LUA_STATE, "XBOX_SIZE_C" );
	sFree = XGetDriveSize( 'E' );
	lua_pushstring( LUA_STATE, sFree.c_str() );
	lua_setglobal( LUA_STATE, "XBOX_SIZE_E" );
	sFree = XGetDriveSize( 'F' );
	lua_pushstring( LUA_STATE, sFree.c_str() );
	lua_setglobal( LUA_STATE, "XBOX_SIZE_F" );
	sFree = XGetDriveSize( 'G' );
	lua_pushstring( LUA_STATE, sFree.c_str() );
	lua_setglobal( LUA_STATE, "XBOX_SIZE_G" );

	// XBOX Name
	// Mem Leak! -- Fixed Mem Leak. :)
	// no, mem leak was still there. Moved to xbutils.cpp instead
	//lua_pushstring( LUA_STATE, GetXboxNick() );
	//lua_setglobal( LUA_STATE, "XBOX_NAME" );

	// XBOX Temperatures
	XKUtils::GetCPUTemp(CPUTempC, false);
	XKUtils::GetCPUTemp(CPUTempF, true);
	XKUtils::GetSYSTemp(SYSTempC, false);
	XKUtils::GetSYSTemp(SYSTempF, true);

	sprintf(Temperature, "%d 'C", CPUTempC);
	lua_pushstring( LUA_STATE, Temperature );
	lua_setglobal( LUA_STATE, "XBOX_TEMP_CPU_C" );
	sprintf(Temperature, "%d 'F", CPUTempF);
	lua_pushstring( LUA_STATE, Temperature );
	lua_setglobal( LUA_STATE, "XBOX_TEMP_CPU_F" );
	sprintf(Temperature, "%d 'C", SYSTempC);
	lua_pushstring( LUA_STATE, Temperature );
	lua_setglobal( LUA_STATE, "XBOX_TEMP_SYS_C" );
	sprintf(Temperature, "%d 'F", SYSTempF);
	lua_pushstring( LUA_STATE, Temperature );
	lua_setglobal( LUA_STATE, "XBOX_TEMP_SYS_F" );

	// XBOX Network Connection Status
	if (IsEthernetConnected())
	{
		// Cable Connected
		lua_pushstring( LUA_STATE, "Connected" );
		lua_setglobal( LUA_STATE, "XBOX_NET_CONNECTED" );

		if (!bCheckedNetStatus)
		{
			// Set check status to true so we dont keep checking unless we need too upon a settings change!
			// saves a crash as well :)
			// Note: We are setting too soon when using DHCP this will return NULL!
			// Need to move the connection to a thread or delay the lock for a few seconds after launch!.
			//bCheckedNetStatus = true;

			// XBOX Network Information
			// Connection Information
			char ipaddr[32];
			XnAddrStatus = XNetGetTitleXnAddr(&xnaddr);

			// XBOX Address Assign Type...
			if (XnAddrStatus != XNET_GET_XNADDR_PENDING )
			{
				// XBOX IP Address.
				sprintf(ipaddr, "%d.%d.%d.%d", xnaddr.ina.S_un.S_un_b.s_b1, xnaddr.ina.S_un.S_un_b.s_b2, xnaddr.ina.S_un.S_un_b.s_b3, xnaddr.ina.S_un.S_un_b.s_b4);
				lua_pushstring(LUA_STATE, ipaddr);
				lua_setglobal(LUA_STATE, "XBOX_IP_ADDRESS");

				bCheckedNetStatus = true; // Temp Fix for IP Null problem!

				if ( XnAddrStatus&XNET_GET_XNADDR_STATIC )
				{
					lua_pushstring( LUA_STATE, "Static IP" );
				} else if ( XnAddrStatus&XNET_GET_XNADDR_DHCP )
				{
					 lua_pushstring( LUA_STATE, "Dynamic IP" );
				}
				lua_setglobal( LUA_STATE, "XBOX_NET_ADDRESS_TYPE" );
			}
	

			// Get NetWork Status:
			NetStatus=XNetGetEthernetLinkStatus();
			if (NetStatus>0)
			{
				if (NetStatus&XNET_ETHERNET_LINK_100MBPS)
				{
					lua_pushstring(LUA_STATE, "100 Mbps");
				}
				else if (NetStatus&XNET_ETHERNET_LINK_10MBPS)
				{
					lua_pushstring(LUA_STATE, "10 Mbps");
				}	
				lua_setglobal(LUA_STATE, "XBOX_NET_CONNECTION_SPEED");

				// XBOX Duplex Mode
				if (NetStatus&XNET_ETHERNET_LINK_FULL_DUPLEX)
				{
					lua_pushstring(LUA_STATE, "Full Duplex");
				}
				else if (NetStatus&XNET_ETHERNET_LINK_HALF_DUPLEX)
				{
					lua_pushstring(LUA_STATE, "Half Duplex");
				}
				lua_setglobal(LUA_STATE, "XBOX_NET_DUPLEX_INFO");
			}
		}
	}
	else
	{
		bCheckedNetStatus = false; // Reset so we can pickup network stats again

		// No cable plugged in.
		lua_pushstring( LUA_STATE, "Not Connected" );
		lua_setglobal( LUA_STATE, "XBOX_NET_CONNECTED" );

		// Set Defaul N/A Values as we are not connected
		lua_pushstring(LUA_STATE, "0.0.0.0");
		lua_setglobal(LUA_STATE, "XBOX_IP_ADDRESS");

		lua_pushstring(LUA_STATE, "N/A");
		lua_setglobal( LUA_STATE, "XBOX_NET_ADDRESS_TYPE" );

		lua_pushstring(LUA_STATE, "N/A");
		lua_setglobal(LUA_STATE, "XBOX_NET_CONNECTION_SPEED");

		lua_pushstring(LUA_STATE, "N/A");
		lua_setglobal(LUA_STATE, "XBOX_NET_DUPLEX_INFO");
	}

	// XBOX Fan Speed
	// Set XBOX Fan Speed
	int iSetFanSpeed = myDash.iFanSpeed;
	/*if ( iSetFanSpeed < 5 ) { iSetFanSpeed = 5; }
	if ( iSetFanSpeed > 50 ) { iSetFanSpeed = 50; }*/
//	SetFanSpeed( iSetFanSpeed ); // Set XBOX Fan Speed
	XKUtils::SetFanSpeed( iSetFanSpeed ); // Set XBOX Fan Speed
	
	// Get FanSpeed
//	sprintf(szFanSpeed, "%d %%", GetFanSpeed());
	sprintf(szFanSpeed, "%d %%", XKUtils::GetFanSpeed());
	lua_pushstring( LUA_STATE, szFanSpeed );
	lua_setglobal( LUA_STATE, "XBOX_FAN_SPEED" );

	// Soundtracks
	lua_pushstring( LUA_STATE, mySndTrk.GetCurrentSoundTrackName() );
	lua_setglobal( LUA_STATE, "XBOX_SND_TRACK_ALBUM" );
	lua_pushstring( LUA_STATE, mySndTrk.GetCurrentSongName() );
	lua_setglobal( LUA_STATE, "XBOX_SND_TRACK_TITLE" );
	lua_pushstring( LUA_STATE, mySndTrk.GetCurrentSongTimePlayedString() );
	lua_setglobal( LUA_STATE, "XBOX_SND_TRACK_TIME_PLAYED" );
	lua_pushstring( LUA_STATE, mySndTrk.GetCurrentSongTimeTotalString() );
	lua_setglobal( LUA_STATE, "XBOX_SND_TRACK_TIME_TOTAL" );
	lua_pushstring( LUA_STATE, mySndTrk.GetCurrentSongTimeRemainsString() );
	lua_setglobal( LUA_STATE, "XBOX_SND_TRACK_TIME_REMAINS" );
}

// Change Scene
void SetLUABuffer( const char* szFilename )
{
	RegisterLUACalls();							// Reset Variables & Functions
	UpdateLUAVariables();						// Push Variables
	OutputDebugString ("Calling lua_dofile (\"");
	OutputDebugString (szFilename);
	OutputDebugString ("\")\n");

//	char filename[MAX_PATH];
//	sprintf(filename,"%s",szFilename);

	char thislocation[MAX_PATH];
	
	strcpy(thislocation,szFilename);
	
	// Get lua filename
	char* luafile =  FileFromFilePathA(thislocation);

	// see if we specify any mount point
	if (strcmp(luafile,thislocation) != 0 ) {

		char szStartPath[MAX_PATH];
		char szMountPath[MAX_PATH];
	
		// Yep so lets mount the drive as SKIN:
		
		// Find out where the Engine was loaded from
		XI_GetProgramPath(szStartPath);

		// Get folder located under skins
		char* mountpoint = MountpointFromFilePathA(thislocation);
		mountpoint = PathFromFilePathA(mountpoint);

		sprintf(szMountPath,"%s\\skins\\%s",szStartPath,mountpoint);
		mappath(szMountPath, "SKIN:");
	}

	char luaPath[MAX_PATH];
	sprintf(luaPath,"SKIN:\\%s",luafile);
	lua_dofile( LUA_STATE, luaPath);
}

// Render Scene
void RenderLUAScene( )
{
	lua_getglobal( LUA_STATE, "Render" ); // Find The Render() In *.lua
	lua_call( LUA_STATE, 0, 0 );		  // Now Call It!
}

// Move The Frame
void FrameMoveLUAScene( )
{
	lua_getglobal(LUA_STATE, "FrameMove");
	lua_call(LUA_STATE, 0, 0);
}


// Switch LUA Scene
int lua_SceneLoad( lua_State* L )
{
	// Remove Any Stored TitleImages until next Page load
	//ResetTitleImages(); // Needs fixing

	// Passed SKINS:\\Default\\main.lua

	char* filename = (char*) lua_tostring(L,1);
	char thislocation[MAX_PATH];
	
	strcpy(thislocation,filename);
	
	// Get lua filename
	char* luafile =  FileFromFilePathA(thislocation);

	// see if we specify any mount point
	if (strcmp(luafile,thislocation) != 0 ) {

		char szStartPath[MAX_PATH];
		char szMountPath[MAX_PATH];
	
		// Yep so lets mount the drive as SKIN:
		
		// Find out where the Engine was loaded from
		XI_GetProgramPath(szStartPath);

		// Get folder located under skins
		char* mountpoint = MountpointFromFilePathA(thislocation);
		mountpoint = PathFromFilePathA(mountpoint);

		sprintf(szMountPath,"%s\\skins\\%s",szStartPath,mountpoint);
		mappath(szMountPath, "SKIN:");
	}

	char luaPath[MAX_PATH];
	sprintf(luaPath,"SKIN:\\%s",luafile);
	lua_dofile( LUA_STATE, luaPath);
	return 1;
}

// Open DVD Tray
int lua_TrayOpen( lua_State* L )
{
	XKUtils::DVDEjectTray();
	return 1;
}

// Close DVD Tray
int lua_TrayClose( lua_State* L )
{
	XKUtils::DVDLoadTray();
	return 1;
}

// XBOX Power Off
int lua_XboxPoweroff( lua_State* L )
{
	XKUtils::XBOXPowerOff();
	return 1;
}

// XBOX Reboot
int lua_XboxReboot( lua_State* L )
{
	XKUtils::XBOXReset();
	return 1;
}

// XBOX Power Cycle
int lua_XboxPowercycle( lua_State* L )
{
	XKUtils::XBOXPowerCycle();
	return 1;
}

// Clear The Screen
int lua_ClearScreen( lua_State* L )
{
	// Get Number Of Arguments
	int iArgs = lua_gettop( L );

	// Make Sure Theres 3 - R,G,B
	// If Not Clear To Black Screen!
	if( iArgs != 3 )
	{
		my3D.Device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0 );
		return 0;
	}

	// Got 3!!
	// Get The Values
	int iR = (int)lua_tonumber( L, 1 );
	int iG = (int)lua_tonumber( L, 2 );
	int iB = (int)lua_tonumber( L, 3 );

	// Now Clear
	my3D.Device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(iR, iG, iB), 1.0f, 0 );

	return 1;
}

// Present Whats Been Rendered
int lua_Present( lua_State* L )
{
	my3D.Device()->Present( NULL, NULL, NULL, NULL );

	return 1;
}

// Load A New Mesh
int lua_MeshLoad( lua_State* L )
{
	char* Texture = "";
	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Must Be 2 Args! - Model Name & Model File
	//if( iArgs != 2 ) { return 0; }

	// Make Sure Model With Same Name Not Already Loaded!
	int iModel;
	bool bBreak = false;
	for( iModel = 0; iModel < MAX_OBJECTS; iModel++ )
	{
		// Check Name Isnt NULL
		while( myModels[iModel].szName == NULL )
		{
			if( iModel == MAX_OBJECTS-1 ) { bBreak = true; break; }
			iModel++;
		}

		if( bBreak ) { break; }

		// Name Match?
		if( XmlCmpNoCase( myModels[iModel].szName, lua_tostring( L, 1 ) ) )
		{
			return 0;
		}
	}

	// Get First Free Model
	int iFree;
	for( iFree = 0; iFree < MAX_OBJECTS; iFree++ )
	{
		// Empty?
		if( myModels[iFree].szName == NULL ) { break; }
	}

	// Make Sure They Really Are Empty
	delete [] myModels[iFree].szName;
	delete [] myModels[iFree].pMesh;
	myModels[iFree].szName = NULL;
	myModels[iFree].pMesh = NULL;

	// Set Name
	myModels[iFree].szName = (char*)malloc( (int)strlen( lua_tostring( L, 1 ) ) + 1 );
	sprintf( myModels[iFree].szName, "%s", lua_tostring( L, 1 ) );

	// Load Model


	Texture = (char*)lua_tostring(L, 3);

	myModels[iFree].pMesh = new CXBMesh;
	if (Texture > "")
	{
		char szMeshPath[MAX_PATH];
		char szTexturePath[MAX_PATH];
		char* sztmpFilePath = (char*) lua_tostring(L,2);
		sztmpFilePath = (char*) lua_tostring(L,2);
		if (strnicmp("SKIN",sztmpFilePath,4) == 0 ) {
			sprintf(szMeshPath,"%s",sztmpFilePath);
		} else {
			sprintf(szMeshPath,"SKIN:\\%s",sztmpFilePath);
		}

		sztmpFilePath = (char*) lua_tostring(L,2);
		sztmpFilePath = (char*) lua_tostring(L,2);
		if (strnicmp("SKIN",sztmpFilePath,4) == 0 ) {
			sprintf(szTexturePath,"%s",sztmpFilePath);
		} else {
			sprintf(szTexturePath,"SKIN:\\%s",sztmpFilePath);
		}

		// Creat model with Texture
		myModels[iFree].pMesh->Create( (CHAR*)szMeshPath, szTexturePath );
	} 
	else
	{
		// Create model without Texture
		char szFilePath[MAX_PATH];
		char* sztmpFilePath = (char*) lua_tostring(L,2);
		sztmpFilePath = (char*) lua_tostring(L,2);
		if (strnicmp("SKIN",sztmpFilePath,4) == 0 ) {
			sprintf(szFilePath,"%s",sztmpFilePath);
		} else {
			sprintf(szFilePath,"SKIN:\\%s",sztmpFilePath);
		}

		myModels[iFree].pMesh->Create( (CHAR*)szFilePath );
	}

	// All Done
	return 1;
}

// Free A Loaded Mesh
int lua_MeshFree( lua_State* L )
{
	// Found Model?
	bool bFoundModel = false;

	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only Need 1! - Model Name!
	if( iArgs != 1 ) { return 0; }

	// Find Model
	int iModel;
	for( iModel = 0; iModel < MAX_OBJECTS; iModel++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myModels[iModel].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundModel = true;
			break;
		}
	}

	if( bFoundModel )
	{
		delete [] myModels[iModel].szName;
		myModels[iModel].szName = NULL;

		delete [] myModels[iModel].pMesh;
		myModels[iModel].pMesh = NULL;

		return 1;
	}

	return 0;
}

// Set Position Of A Mesh
int lua_MeshPosition( lua_State* L )
{
	// Found Model?
	bool bFoundModel = false;

	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 1! - Model Name!
	if( iArgs != 4 ) { return 0; }

	// Get Model
	int iModel;
	for( iModel = 0; iModel < MAX_OBJECTS; iModel++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myModels[iModel].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundModel = true;
			break;
		}
	}

	if( bFoundModel )
	{
		float fX = (float)lua_tonumber( L, 2 );
		float fY = (float)lua_tonumber( L, 3 );
		float fZ = (float)lua_tonumber( L, 4 );

		myModels[iModel].pMesh->Move( fX, fY, fZ );
		return 1;
	}

	return 0;
}

// Set Scale Of A Mesh
int lua_MeshScale( lua_State* L )
{
	// Found Model?
	bool bFoundModel = false;

	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 1! - Model Name!
	if( iArgs != 4 ) { return 0; }

	// Get Model
	int iModel;
	for( iModel = 0; iModel < MAX_OBJECTS; iModel++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myModels[iModel].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundModel = true;
			break;
		}
	}

	if( bFoundModel )
	{
		float fX = (float)lua_tonumber( L, 2 );
		float fY = (float)lua_tonumber( L, 3 );
		float fZ = (float)lua_tonumber( L, 4 );

		myModels[iModel].pMesh->Scale( fX, fY, fZ );
		return 1;
	}

	return 0;
}

// Rotate A Mesh
int lua_MeshRotate( lua_State* L )
{
	// Found Model?
	bool bFoundModel = false;

	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 1! - Model Name!
	if( iArgs != 4 ) { return 0; }

	// Get Model
	int iModel;
	for( iModel = 0; iModel < MAX_OBJECTS; iModel++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myModels[iModel].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundModel = true;
			break;
		}
	}

	if( bFoundModel )
	{
		float fX = (float)lua_tonumber( L, 2 );
		float fY = (float)lua_tonumber( L, 3 );
		float fZ = (float)lua_tonumber( L, 4 );

		myModels[iModel].pMesh->Rotate( D3DXToRadian(fX), D3DXToRadian(fY), D3DXToRadian(fZ) );
		return 1;
	}

	return 0;
}

// Render The Mesh To Screen
int lua_MeshRender( lua_State* L )
{
	// Found Model?
	bool bFoundModel = false;

	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 1! - Model Name!
	if( iArgs != 1 ) { return 0; }

	// Get Model
	int iModel;
	for( iModel = 0; iModel < MAX_OBJECTS; iModel++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myModels[iModel].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundModel = true;
			break;
		}
	}

	if( bFoundModel )
	{
		if( g_bEnableXrayRendering )
		{
			myModels[iModel].pMesh->Render( XBMESH_XRAY );
		}
		else if ( g_bEnableAlphaRendering )
		{
			myModels[iModel].pMesh->Render( XBMESH_ALPHAON );
		}
		else
		{
			myModels[iModel].pMesh->Render();
		}

		return 1;
	}

	return 0;
}

// Sets Camera Field Of View
int lua_CameraSetFOV( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only 1! - FOV Float
	if( iArgs != 1 ) { return 0; }

	// Set Value
	float fField = (float)lua_tonumber( L, 1 );
	myCam.SetFieldOfView( fField );

	return 1;
}

// Set Cameras Aspect Ratio
int lua_CameraSetAspect( lua_State* L)
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only 1! - Aspect Float
	if( iArgs != 1 ) { return 0; }

	// Set Value
	float fRatio = (float)lua_tonumber( L, 1 );
	myCam.SetAspectRatio( fRatio );

	return 1;
}

// Set Position of Camera
int lua_CameraSetPosition( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only 3! - X Y Z
	if( iArgs != 3 ) { return 0; }

	// Set Value
	D3DXVECTOR3 myPos;
	myPos.x = (float)lua_tonumber( L, 1 );
	myPos.y = (float)lua_tonumber( L, 2 );
	myPos.z = (float)lua_tonumber( L, 3 );
	myCam.SetPosition( myPos );

	return 1;
}

// Set point Camera is looking at
int lua_CameraSetLookAt( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only 3! - X Y Z
	if( iArgs != 3 ) { return 0; }

	// Set Value
	D3DXVECTOR3 myPos;
	myPos.x = (float)lua_tonumber( L, 1 );
	myPos.y = (float)lua_tonumber( L, 2 );
	myPos.z = (float)lua_tonumber( L, 3 );
	myCam.SetLookAt( myPos );

	return 1;
}

int lua_CameraMoveForward( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only 3! - X Y Z
	if( iArgs != 1 ) { return 0; }

	myCam.MoveForward( (float)lua_tonumber( L, 1 ) );

	return 1;
}

int lua_CameraMoveBackward( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only 3! - X Y Z
	if( iArgs != 1 ) { return 0; }

	myCam.MoveBackward( (float)lua_tonumber( L, 1 ) );

	return 1;
}

int lua_CameraMoveLeft( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only 3! - X Y Z
	if( iArgs != 1 ) { return 0; }

	myCam.MoveLeft( (float)lua_tonumber( L, 1 ) );

	return 1;
}

int lua_CameraMoveRight( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only 3! - X Y Z
	if( iArgs != 1 ) { return 0; }

	myCam.MoveRight( (float)lua_tonumber( L, 1 ) );

	return 1;
}
// Load A New Font
int lua_FontLoad( lua_State* L )
{
	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Must Be 2 Args! - Font Name & Font File
	if( iArgs != 2 ) { return 0; }

	// Make Sure Font With Same Name Not Already Loaded!
	int iFont;
	bool bBreak = false;
	for( iFont = 0; iFont < MAX_OBJECTS; iFont++ )
	{
		// Check Name Isnt NULL
		while( myFonts[iFont].szName == NULL )
		{
			if( iFont == MAX_OBJECTS-1 ) { bBreak = true; break; }
			iFont++;
		}

		if( bBreak ) { break; }

		// Name Match?
		if( XmlCmpNoCase( myFonts[iFont].szName, lua_tostring( L, 1 ) ) )
		{
			return 0;
		}
	}

	// Get First Free Font
	int iFree;
	for( iFree = 0; iFree < MAX_OBJECTS; iFree++ )
	{
		// Empty?
		if( myFonts[iFree].szName == NULL ) { break; }
	}

	// Make Sure They Really Are Empty
	delete [] myFonts[iFree].szName;
	delete [] myFonts[iFree].pFont;
	myFonts[iFree].szName = NULL;
	myFonts[iFree].pFont = NULL;

	// Set Name
	myFonts[iFree].szName = (char*)malloc( (int)strlen( lua_tostring( L, 1 ) ) + 1 );
	sprintf( myFonts[iFree].szName, "%s", lua_tostring( L, 1 ) );


	// Load Font
	char szFilePath[MAX_PATH];
	char* sztmpFilePath = (char*) lua_tostring(L,2);
	sztmpFilePath = (char*) lua_tostring(L,2);
	if (strnicmp("SKIN",sztmpFilePath,4) == 0 ) {
		sprintf(szFilePath,"%s",sztmpFilePath);
	} else {
		sprintf(szFilePath,"SKIN:\\%s",sztmpFilePath);
	}
	myFonts[iFree].pFont = new CXBFont;
	myFonts[iFree].pFont->Create( (CHAR*)szFilePath );

	// All Done
	return 1;
}

// Free A Loaded Font
int lua_FontFree( lua_State* L )
{
	// Found font?
	bool bFoundFont = false;

	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only Need 1! - Font Name!
	if( iArgs != 1 ) { return 0; }

	// Find Font
	int iFont;
	for( iFont = 0; iFont < MAX_OBJECTS; iFont++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myFonts[iFont].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundFont = true;
			break;
		}
	}

	if( bFoundFont )
	{
		delete [] myFonts[iFont].szName;
		myFonts[iFont].szName = NULL;

		myFonts[iFont].pFont->Destroy();
		free( myFonts[iFont].pFont );
		myFonts[iFont].pFont = NULL;

		return 1;
	}

	return 0;
}

// Set FOnt Colour
int lua_FontColour( lua_State* L )
{
	// Set Text Colour
	int iA = (int)lua_tonumber( L, 1 );
	int iR = (int)lua_tonumber( L, 2 );
	int iG = (int)lua_tonumber( L, 3 );
	int iB = (int)lua_tonumber( L, 4 );

	dwFontColour = (iA << 24) | (iR << 16) | (iG << 8) | (iB);

	return 1;
}

// Render Text on Screen
int lua_FontDrawText( lua_State* L )
{
	// Found font?
	bool bFoundFont = false;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Find Font
	int iFont;
	for( iFont = 0; iFont < MAX_OBJECTS; iFont++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myFonts[iFont].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundFont = true;
			break;
		}
	}

	if( bFoundFont )
	{
		// Styled
		if( iArgs == 3 )
		{
			DWORD dwStyle;

			// Get Style
			if( lua_tonumber( L, 3 ) == 1 ) { dwStyle = XBFONT_LEFT; }
			else if( lua_tonumber( L, 3 ) == 2 ) { dwStyle = XBFONT_CENTER; }
			else if( lua_tonumber( L, 3 ) == 3 ) { dwStyle = XBFONT_RIGHT; }
			else { dwStyle = 0x00000000; }
			myFonts[iFont].pFont->DrawTextLine( lua_tostring( L, 2 ), dwFontColour, 0, dwStyle );	
			return 1;
		}

		// Normal
		else
		{
			myFonts[iFont].pFont->DrawText( lua_tostring( L, 2 ), dwFontColour );
			return 1;
		}
	}

	return 0;
}

// Set Position
int lua_FontPosition( lua_State* L )
{
	// Found font?
	bool bFoundFont = false;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Find Font
	int iFont;
	for( iFont = 0; iFont < MAX_OBJECTS; iFont++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myFonts[iFont].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundFont = true;
			break;
		}
	}

	if( bFoundFont )
	{
		float fX = (float)lua_tonumber( L, 2 );
		float fY = (float)lua_tonumber( L, 3 );

		myFonts[iFont].pFont->SetCursorPosition( fX, fY );

		return 1;
	}

	return 0;
}

// Load A New Texture
int lua_TextureLoad( lua_State* L )
{
	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Must Be 2 Args! - Texture Name & File
	if( iArgs != 2 ) { return 0; }

	// Make Sure Texture With Same Name Not Already Loaded!
	int iTexture;
	bool bBreak = false;
	for( iTexture = 0; iTexture < MAX_OBJECTS; iTexture++ )
	{
		// Check Name Isnt NULL
		while( myTextures[iTexture].szName == NULL )
		{
			if( iTexture == MAX_OBJECTS-1 ) { bBreak = true; break; }
			iTexture++;
		}

		if( bBreak ) { break; }

		// Name Match?
		if( XmlCmpNoCase( myTextures[iTexture].szName, lua_tostring( L, 1 ) ) )
		{
			return 0;
		}
	}

	// Get First Free Texture
	int iFree;
	for( iFree = 0; iFree < MAX_OBJECTS; iFree++ )
	{
		// Empty?
		if( myTextures[iFree].szName == NULL ) { break; }
	}

	// Set Name);


	myTextures[iFree].szName = (char*)malloc( (int)strlen( lua_tostring( L, 1 ) ) + 1 );
	sprintf( myTextures[iFree].szName, "%s", lua_tostring( L, 1 ) );

	// Set name again
	
	char szFilePath[MAX_PATH];
	char* sztmpFilePath = (char*) lua_tostring(L,2);
	sztmpFilePath = (char*) lua_tostring(L,2);

	// Handle TitleImage.xbx or *.xpr Files
	string szFn(sztmpFilePath);
	if (szFn.substr(szFn.length() - 4) == ".xpr"
		||	szFn.substr(szFn.length() - 4) == ".xbx"){
		myTextures[iFree].pTexture = (LPDIRECT3DTEXTURE8)LoadTextureFromXPR( sztmpFilePath );
		myTextures[iFree].iType = 1; // Texture was loaded from an XBX / XPR
		// All Done
		return 1;
	}

	if (strnicmp("SKIN",sztmpFilePath,4) == 0 ) {
		sprintf(szFilePath,"%s",sztmpFilePath);
	} else {
		sprintf(szFilePath,"SKIN:\\%s",sztmpFilePath);
	}

	OutputDebugString ("Loading ");
	OutputDebugString (szFilePath);
	OutputDebugString ("\n");

	// Load Texture
	D3DXCreateTextureFromFile( my3D.Device(), szFilePath, &myTextures[iFree].pTexture );
	myTextures[iFree].iType = 0; // Normal Texture

	// All Done
	return 1;
}

// Free A Loaded Font
int lua_TextureFree( lua_State* L )
{
	// Found font?
	bool bFoundTexture = false;

	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only Need 1! - Texture Name!
	if( iArgs != 1 ) { return 0; }

	// Find Font
	int iTexture;
	for( iTexture = 0; iTexture < MAX_OBJECTS; iTexture++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myTextures[iTexture].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundTexture = true;
			break;
		}
	}

	if( bFoundTexture )
	{
		/*delete [] myTextures[iTexture].szName;
		myTextures[iTexture].szName = NULL;

		free( myTextures[iTexture].pTexture );
		myTextures[iTexture].pTexture = NULL;*/

		delete [] myTextures[iTexture].szName;
		myTextures[iTexture].szName = NULL;
		if (myTextures[iTexture].iType == 0) myTextures[iTexture].pTexture->Release();
		if (myTextures[iTexture].iType == 1) {
			free( myTextures[iTexture].pTexture );
		}
		//free( myTextures[iTexture].pTexture );
		myTextures[iTexture].pTexture = NULL;

		return 1;
	}

	return 0;
}

// SnowFlakes
// Load A New Snow Flake
int lua_FlakeLoad( lua_State* L )
{
	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Must Be 3 Args! - 3 Flake Models
	if( iArgs != 3 ) { return 0; }

	/*char *Model1 = (char)lua_tostring( L, 1 );
	char *Model2 = (char)lua_tostring( L, 2 );
	char *Model3 = (char)lua_tostring( L, 3 );*/
	//"SKINS:\\FLAKE1.XBG", "SKINS:\\FLAKE2.XBG", "SKINS:\\FLAKE3.XBG"
	mySnow.LoadModels( (char*)lua_tostring( L, 1 ), (char*)lua_tostring( L, 2 ), (char*)lua_tostring( L, 3 ) );
	// All Done
	return 1;
}
/*
// Free A Loaded Flake
int lua_FlakeFree( lua_State* L )
{
	// Found Model?
	bool bFoundModel = false;

	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only Need 1! - Model Name!
	if( iArgs != 1 ) { return 0; }

	// Find Model
	int iModel;
	for( iModel = 0; iModel < MAX_OBJECTS; iModel++ )
	{
		// Name Match?
		if( XmlCmpNoCase( mySnowFlakes[iModel].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundModel = true;
			break;
		}
	}

	if( bFoundModel )
	{
		delete [] mySnowFlakes[iModel].szName;
		mySnowFlakes[iModel].szName = NULL;

//		delete [] mySnowFlakes[iModel].pSnow;
		free(mySnowFlakes[iModel].pSnow);
		mySnowFlakes[iModel].pSnow = NULL;

		return 1;
	}

	return 0;
}*/

// Set Boundaries Of A Flake
int lua_FlakeSetBoundaries( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 6! - XYZ Positions Start / End
	if( iArgs != 6 ) { return 0; }

	float fXStart = (float)lua_tonumber( L, 1 );
	float fXEnd = (float)lua_tonumber( L, 2 );
	float fYStart = (float)lua_tonumber( L, 3 );
	float fYEnd = (float)lua_tonumber( L, 4 );
	float fZStart = (float)lua_tonumber( L, 5 );
	float fZEnd = (float)lua_tonumber( L, 6 );

	mySnow.SetBoundaries( fXStart, fXEnd, fYStart, fYEnd, fZStart, fZEnd);
	return 1;
}

int lua_FlakeSpeed( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 2! - MinSpeed, MaxSpeed
	if( iArgs != 2 ) { return 0; }

	float fMinFlakeSpeed = (float)lua_tonumber( L, 1 );
	float fMaxFlakeSpeed = (float)lua_tonumber( L, 2 );

	mySnow.SetSpeed( fMinFlakeSpeed, fMaxFlakeSpeed );
	return 1;
}

int lua_FlakeTotal( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 1! - Total
	if( iArgs != 1 ) { return 0; }
		
	int iTotalFlakes = (int)lua_tonumber( L, 1 );

	mySnow.SetFlakes( iTotalFlakes );
	return 1;
}

int lua_FlakeScale( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 3! - X, Y, Z
	if( iArgs != 3 ) { return 0; }

	float fX = (float)lua_tonumber( L, 1 );
	float fY = (float)lua_tonumber( L, 2 );
	float fZ = (float)lua_tonumber( L, 3 );

	mySnow.SetScale( fX, fY, fZ );
	return 1;
}

int lua_FlakeRotateSpeed( lua_State* L )
{
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 3! - Flake1, Flake2, Flake3 Speed
	if( iArgs != 3 ) { return 0; }

	float fFlakeSpeed1 = (float)lua_tonumber( L, 1 );
	float fFlakeSpeed2 = (float)lua_tonumber( L, 2 );
	float fFlakeSpeed3 = (float)lua_tonumber( L, 3 );

	mySnow.SetRotationSpeed( fFlakeSpeed1, fFlakeSpeed2, fFlakeSpeed3 );
	return 1;
}

// Render The Flake To Screen
int lua_FlakeRender( lua_State* L )
{
	// Render Snow Flakes
	mySnow.Render();
	return 1;
}

// End Snow Flakes

// Load A New Wave
int lua_WaveLoad( lua_State* L )
{
	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Must Be 2 Args! - Wave Name & Wave File
	if( iArgs != 2 ) { return 0; }

	// Make Sure Wave With Same Name Not Already Loaded!
	int iWave;
	bool bBreak = false;
	for( iWave = 0; iWave < MAX_OBJECTS; iWave++ )
	{
		// Check Name Isnt NULL
		while( myWaveFiles[iWave].szName == NULL )
		{
			if( iWave == MAX_OBJECTS-1 ) { bBreak = true; break; }
			iWave++;
		}

		if( bBreak ) { break; }

		// Name Match?
		if( XmlCmpNoCase( myWaveFiles[iWave].szName, lua_tostring( L, 1 ) ) )
		{
			return 0;
		}
	}

	// Get First Free Wave
	int iFree;
	for( iFree = 0; iFree < MAX_OBJECTS; iFree++ )
	{
		// Empty?
		if( myWaveFiles[iFree].szName == NULL ) { break; }
	}

	// Make Sure They Really Are Empty
	delete [] myWaveFiles[iFree].szName;
	delete [] myWaveFiles[iFree].pSound;
	myWaveFiles[iFree].szName = NULL;
	myWaveFiles[iFree].pSound = NULL;

	// Set Name
	myWaveFiles[iFree].szName = (char*)malloc( (int)strlen( lua_tostring( L, 1 ) ) + 1 );
	sprintf( myWaveFiles[iFree].szName, "%s", lua_tostring( L, 1 ) );

	// Load Wave

	char szFilePath[MAX_PATH];
	char* sztmpFilePath = (char*) lua_tostring(L,2);
	sztmpFilePath = (char*) lua_tostring(L,2);
	if (strnicmp("SKIN",sztmpFilePath,4) == 0 ) {
		sprintf(szFilePath,"%s",sztmpFilePath);
	} else {
		sprintf(szFilePath,"SKIN:\\%s",sztmpFilePath);
	}
	myWaveFiles[iFree].pSound = new CXBSound;
	myWaveFiles[iFree].pSound->Create( (CHAR*)lua_tostring( L, 2 ) );

	// All Done
	return 1;
}

// Free A Loaded Wave File
int lua_WaveFree( lua_State* L )
{
	// Found font?
	bool bFoundWave = false;

	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Only Need 1! - Font Name!
	if( iArgs != 1 ) { return 0; }

	// Find Wave
	int iWave;
	for( iWave = 0; iWave < MAX_OBJECTS; iWave++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myWaveFiles[iWave].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundWave = true;
			break;
		}
	}

	if( bFoundWave )
	{
		delete [] myWaveFiles[iWave].szName;
		myWaveFiles[iWave].szName = NULL;

		myWaveFiles[iWave].pSound->Destroy();
		free( myWaveFiles[iWave].pSound );
		myWaveFiles[iWave].pSound = NULL;

		return 1;
	}

	return 0;
}

int lua_WavePlay( lua_State* L )
{
	// Found Model?
	bool bFoundWave = false;

	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 1! - Model Name!
	if( iArgs != 1 ) { return 0; }

	// Get Model
	int iWave;
	for( iWave = 0; iWave < MAX_OBJECTS; iWave++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myWaveFiles[iWave].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundWave = true;
			break;
		}
	}

	if( bFoundWave )
	{
		// Reset Track to Zero then Play Again
		//myWaveFiles[iWave].m_pDSoundBuffer->SetCurrentPosition(0);
		//myWaveFiles[iWave].pSound->m_pDSoundBuffer->SetCurrentPosition(0);
		myWaveFiles[iWave].pSound->Play();
	}

	return 1;
}

// Enable Xray
int lua_XrayEnable( lua_State* L )
{
	g_bEnableXrayRendering = true;
	return 1;
}

// Disable Xray
int lua_XrayDisable( lua_State* L )
{
	g_bEnableXrayRendering = false;
	return 1;
}

// Enable Alpha
int lua_AlphaEnable( lua_State* L )
{
	g_bEnableAlphaRendering = true;
	return 1;
}

// Disable Alpha
int lua_AlphaDisable( lua_State* L )
{
	g_bEnableAlphaRendering = false;
	return 1;
}

// Render Fog
int lua_FogRender( lua_State* L )
{
	myFog.Enable();
	myFog.Render();
	myFog.Disable();

	return 1;
}

// Fog Colour
int lua_FogColour( lua_State* L )
{
	int Mode;
	// Get Arg Count
	int iArgs = lua_gettop( L );

	// Should Be 8! - Model Name!
	if( iArgs != 8 ) { return 0; }

	Mode = (int)lua_tonumber(L, 1 ); // Fog Render Mode

	// Fog Options
	float fStart = (float)lua_tonumber(L, 2);
	float fEnd = (float)lua_tonumber(L, 3);
	float fDensity = (float)lua_tonumber(L, 4);

	DWORD dA = (DWORD)lua_tonumber(L, 5);
	DWORD dR = (DWORD)lua_tonumber(L, 6);
	DWORD dG = (DWORD)lua_tonumber(L, 7);
	DWORD dB = (DWORD)lua_tonumber(L, 8);

	// Mode Switch for Fog render type:
	switch ( Mode )
	{
		case 1:
			{
				myFog.ChangeFog(D3DFOG_EXP, fStart, fEnd, fDensity, D3DCOLOR_ARGB(dA, dR, dG, dB));
			}
			break;
		case 2:
			{
				myFog.ChangeFog(D3DFOG_EXP2, fStart, fEnd, fDensity, D3DCOLOR_ARGB(dA, dR, dG, dB));
			}
			break;
		case 3:
			{
				myFog.ChangeFog(D3DFOG_LINEAR, fStart, fEnd, fDensity, D3DCOLOR_ARGB(dA, dR, dG, dB));
			}
			break;
		default:
			{
				myFog.ChangeFog(D3DFOG_LINEAR, fStart, fEnd, fDensity, D3DCOLOR_ARGB(dA, dR, dG, dB));
			}
			break;
	}



	return 1;
}

// Merge Strings Into One
int lua_StringMerge( lua_State* L )
{
	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Get Size
	int iSize = 0;
	for( int i = 1; i <= iArgs; i++ )
	{
		iSize += strlen( lua_tostring( L, i ) );
	}

	// Create Buffer
	char* szBuffer = (char*)malloc(iSize+1);
	memset( szBuffer, 0, iSize+1 );

	// Fill Buffer
	for( int i = 1; i <= iArgs; i++ )
	{
		strcat( szBuffer, lua_tostring( L, i ) );
	}

	// Push Buffer
	lua_pushstring( LUA_STATE, szBuffer );

	//Cleanup
	delete [] szBuffer;
	szBuffer = NULL;
	return 1;
}

/* Menu Functions */

// Return Total number of Menu Items
int lua_getmenuitems(lua_State *L) 
{
	int mnu=-1;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Only Need 1! - Menu Number!
	if( iArgs != 1 ) { return 0; }

	mnu = (int)lua_tonumber(L, 1);

	if(mnu>=0) {
		lua_pushnumber(L, Menu[mnu].NodeCount());
		return 1;
	}

	lua_pushnumber(L, 0);
	return 1;

}

int lua_getmenuitemname(lua_State *L) 
{
	int mnu=-1;
	int itm=-1;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Only Need 2! - Menu Number, Item number!
	if( iArgs != 2 ) { return 0; }

	mnu = (int)lua_tonumber(L, 1);
	itm = (int)lua_tonumber(L, 2);

	if(mnu>=0 && itm>=0) {
		if (Menu[mnu].IndexedMenu[itm]->Title!=NULL) {
			lua_pushstring(L, Menu[mnu].IndexedMenu[itm]->Title);
			return 1;
		} else {
			char * name = NULL;
			sprintf (name,"IndexedMenu[%d] = NULL",itm);
			lua_pushstring(L, name);
			return 1;
		}
	}

	lua_pushstring(L, "-- NULL --");
	return 1;
}

int lua_getitemregion(lua_State *L)
{
	int mnu=-1;
	int itm=-1;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Only Need 2! - Menu Number, Item number!
	if( iArgs != 2 ) { return 0; }

	mnu = (int)lua_tonumber(L, 1);
	itm = (int)lua_tonumber(L, 2);

	if (mnu >=0 && itm >=0) 
	{
		if( Menu[mnu].IndexedMenu[itm]->Region & XBEIMAGE_GAME_REGION_NA )
		{
			lua_pushstring (L, "Region: NTSC");
		}
		else if( Menu[mnu].IndexedMenu[itm]->Region & XBEIMAGE_GAME_REGION_JAPAN )
		{
			lua_pushstring (L, "Region: NTSC-J");
		}
		else if( Menu[mnu].IndexedMenu[itm]->Region & XBEIMAGE_GAME_REGION_RESTOFWORLD )
		{
			lua_pushstring (L, "Region: PAL");
		}
		else if( Menu[mnu].IndexedMenu[itm]->Region & XBEIMAGE_GAME_REGION_MANUFACTURING )
		{
			lua_pushstring (L, "Region: DEBUG");
		}
		else
		{
			lua_pushstring (L, "Region: UNKNOWN");
		}

	}

	return 1;
}

int lua_getitemrating(lua_State *L)
{
	int mnu=-1;
	int itm=-1;	

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Only Need 2! - Menu Number, Item number!
	if( iArgs != 2 ) { return 0; }

	mnu = (int)lua_tonumber(L, 1);
	itm = (int)lua_tonumber(L, 2);

	if (mnu >=0 && itm >=0)
	{
		if( Menu[mnu].IndexedMenu[itm]->Rating & XC_PC_ESRB_ALL )
		{
			lua_pushstring (L, "Rating: ALL");
		}
		else if( Menu[mnu].IndexedMenu[itm]->Rating & XC_PC_ESRB_ADULT )
		{
			lua_pushstring (L, "Rating: ADULT");
		}
		else if( Menu[mnu].IndexedMenu[itm]->Rating & XC_PC_ESRB_MATURE )
		{
			lua_pushstring (L, "Rating: MATURE");
		}
		else if( Menu[mnu].IndexedMenu[itm]->Rating & XC_PC_ESRB_TEEN )
		{
			lua_pushstring (L, "Rating: TEEN");
		}
		else if( Menu[mnu].IndexedMenu[itm]->Rating & XC_PC_ESRB_KIDS_TO_ADULTS )
		{
			lua_pushstring (L, "Rating: KIDS TO ADULTS");
		}
		else if( Menu[mnu].IndexedMenu[itm]->Rating & XC_PC_ESRB_EARLY_CHILDHOOD)
		{
			lua_pushstring (L, "Rating: EARLY CHILDHOOD");
		}
		else
		{
			lua_pushstring (L, "Rating: UNKNOWN");
		}
	}
	return 1;
}

int lua_getitemfilename(lua_State *L)
{
	int mnu=-1;
	int itm=-1;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Only Need 2! - Menu Number, Item number!
	if( iArgs != 2 ) { return 0; }

	mnu = (int)lua_tonumber(L, 1);
	itm = (int)lua_tonumber(L, 2);

	if(mnu>=0 && itm>=0) {
		lua_pushstring(L, Menu[mnu].IndexedMenu[itm]->File);
		return 1;
	}

	lua_pushstring(L, "-- NULL --");
	return 1;
}

int lua_getitemiconpath(lua_State *L) 
{
	int mnu=-1;
	int itm=-1;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Only Need 2! - Menu Number, Item number!
	if( iArgs != 2 ) { return 0; }

	mnu = (int)lua_tonumber(L, 1);
	itm = (int)lua_tonumber(L, 2);

	if(mnu>=0 && itm>=0) {
		lua_pushstring(L, Menu[mnu].IndexedMenu[itm]->Icon);
		return 1;
	}

	lua_pushstring(L, "-- NULL --");
	return 1;
}

int lua_getitemdblocks(lua_State *L)
{
	int mnu=-1;
	int itm=-1;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Only Need 2! - Menu Number, Item number!
	if( iArgs != 2 ) { return 0; }

	mnu = (int)lua_tonumber(L, 1);
	itm = (int)lua_tonumber(L, 2);

	if(mnu>=0 && itm>=0) {
		lua_pushnumber(L, Menu[mnu].IndexedMenu[itm]->DBlocks);
		return 1;
	}

	lua_pushnumber(L, 0);
	return 1;
}

int lua_getitemtype(lua_State *L) 
{
	int mnu=-1;
	int itm=-1;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Only Need 2! - Menu Number, Item number!
	if( iArgs != 2 ) { return 0; }

	mnu = (int)lua_tonumber(L, 1);
	itm = (int)lua_tonumber(L, 2);

	if(mnu>=0 && itm>=0) {
		lua_pushnumber(L, Menu[mnu].IndexedMenu[itm]->Action);
		return 1;
	}

	lua_pushnumber(L, -1);
	return 1;
}


    
 // Draw Texured Quad with alpha level
 int lua_DrawTexturedQuadAlpha( lua_State* L )
 {
 	// Found Texture?
 	bool bFoundTexture = false;
 
 	// Get Arg Number
 	int iArgs = lua_gettop( L );
 
 	// Check Args
 	// Need.. NAME TOP BOTTOM LEFT RIGHT ALPHA
 	if( iArgs != 6 ) { return 0; }
 
 	// Find Font
 	int iTexture;
 	for( iTexture = 0; iTexture < MAX_OBJECTS; iTexture++ )
 	{
 		// Name Match?
 		if( XmlCmpNoCase( myTextures[iTexture].szName, lua_tostring( L, 1 ) ) )
 		{
 			bFoundTexture = true;
 			break;
 		}
 	}
 
 	if( bFoundTexture )
 	{
 		float fLeft   = (float)lua_tonumber( L, 2 );
 		float fRight  = (float)lua_tonumber( L, 3 );
 		float fTop    = (float)lua_tonumber( L, 4 );
 		float fBottom = (float)lua_tonumber( L, 5 );
 		float fAlpha =	(float)lua_tonumber(L,6);
 
 		stVertex myVerts[] = {
 			{ fLeft,  fTop,    0, 1, 0, 0 },
 			{ fRight, fTop,    0, 1, 1, 0 },
 			{ fLeft,  fBottom, 0, 1, 0, 1 },
 	
 			{ fLeft,  fBottom, 0, 1, 0, 1 },
 			{ fRight, fTop,    0, 1, 1, 0 },
 			{ fRight, fBottom, 0, 1, 1, 1 },
 		};
 
 		my3D.Device()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
 		my3D.Device()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 		my3D.Device()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
 		my3D.Device()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
 		my3D.Device()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
 		my3D.Device()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
 		my3D.Device()->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
 		my3D.Device()->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
 		my3D.Device()->SetRenderState( D3DRS_TEXTUREFACTOR, (DWORD)(255.0f*fAlpha)<<24L );
 		my3D.Device()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
 		my3D.Device()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
 		my3D.Device()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
 		my3D.Device()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
 		my3D.Device()->SetRenderState( D3DRS_ZENABLE, FALSE );
 		my3D.Device()->SetRenderState( D3DRS_FOGENABLE,    FALSE );
 		my3D.Device()->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
 		my3D.Device()->SetRenderState( D3DRS_CULLMODE,     D3DCULL_NONE );
 		my3D.Device()->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
 		my3D.Device()->SetTexture( 0, myTextures[iTexture].pTexture );
 		my3D.Device()->DrawVerticesUP( D3DPT_TRIANGLESTRIP, 6, &myVerts, sizeof(stVertex) );

		return 1;
	}

	return 0;
}

struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; };

// Draw Texured Quad
int lua_DrawTexturedQuad( lua_State* L )
{
	// Found Texture?
	bool bFoundTexture = false;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Check Args
	// Need.. NAME TOP BOTTOM LEFT RIGHT
	if( iArgs != 5 ) { return 0; }

	// Find Font
	int iTexture;
	for( iTexture = 0; iTexture < MAX_OBJECTS; iTexture++ )
	{
		// Name Match?
		if( XmlCmpNoCase( myTextures[iTexture].szName, lua_tostring( L, 1 ) ) )
		{
			bFoundTexture = true;
			break;
		}
	}

	if( bFoundTexture )
	{
		float fLeft   = (float)lua_tonumber( L, 2 );
		float fRight  = (float)lua_tonumber( L, 3 );
		float fTop    = (float)lua_tonumber( L, 4 );
		float fBottom = (float)lua_tonumber( L, 5 );

		stVertex myVerts[] = {
			{ fLeft,  fTop,    0, 1, 0, 0 },
			{ fRight, fTop,    0, 1, 1, 0 },
			{ fLeft,  fBottom, 0, 1, 0, 1 },
	
			{ fLeft,  fBottom, 0, 1, 0, 1 },
			{ fRight, fTop,    0, 1, 1, 0 },
			{ fRight, fBottom, 0, 1, 1, 1 },
		};

		my3D.Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		my3D.Device()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		//my3D.Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);
		my3D.Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		my3D.Device()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		my3D.Device()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		my3D.Device()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		my3D.Device()->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		my3D.Device()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		my3D.Device()->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		my3D.Device()->SetTexture( 0, myTextures[iTexture].pTexture );
		my3D.Device()->DrawVerticesUP( D3DPT_TRIANGLESTRIP, 6, &myVerts, sizeof(stVertex) );
		}

/*
    LPDIRECT3DVERTEXBUFFER8 m_pVB;

	float x, float y, float nw, float nh;

	float m_nWidth=640;
	float m_nHeight=480;
	my3D.Device()->CreateVertexBuffer( 4*6*sizeof(FLOAT), D3DUSAGE_WRITEONLY, 
		0L, D3DPOOL_DEFAULT, &m_pVB );



	x=fLeft;
	y=fTop;
	nw=fRight-fLeft;
	nh=fBottom-fTop;
	VERTEX* vertex;
	m_pVB->Lock( 0, 0, (BYTE**)&vertex, 0L );

	{
		vertex[0].p = D3DXVECTOR4( x - 0.5f,			y - 0.5f,			0, 0 );
		vertex[0].tu = 0;
		vertex[0].tv = 0;

		vertex[1].p = D3DXVECTOR4( x+nw - 0.5f,	y - 0.5f,			0, 0 );
		vertex[1].tu = m_nWidth;
		vertex[1].tv = 0;

		vertex[2].p = D3DXVECTOR4( x+nw - 0.5f,	y+nh - 0.5f,	0, 0 );
		vertex[2].tu = m_nWidth;
		vertex[2].tv = m_nHeight;

		vertex[3].p = D3DXVECTOR4( x - 0.5f,	y+nh - 0.5f,	0, 0 );
		vertex[3].tu = 0;
		vertex[3].tv = m_nHeight;  
	}


	m_pVB->Unlock();


	// Set state to render the image
	my3D.Device()->SetTexture( 0, myTextures[iTexture].pTexture );

	my3D.Device()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	my3D.Device()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	my3D.Device()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	my3D.Device()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	my3D.Device()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	my3D.Device()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	my3D.Device()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	my3D.Device()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	my3D.Device()->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
	my3D.Device()->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	my3D.Device()->SetTextureStageState(0, D3DTSS_MAGFILTER,D3DTEXF_POINT );
	my3D.Device()->SetTextureStageState(0, D3DTSS_MINFILTER,D3DTEXF_POINT );
	my3D.Device()->SetTextureStageState(0, D3DTSS_MIPFILTER,D3DTEXF_NONE);
	my3D.Device()->SetRenderState( D3DRS_ZENABLE,      FALSE );
	my3D.Device()->SetRenderState( D3DRS_FOGENABLE,    FALSE );
	my3D.Device()->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
	my3D.Device()->SetRenderState( D3DRS_FILLMODE,     D3DFILL_SOLID );
	my3D.Device()->SetRenderState( D3DRS_CULLMODE,     D3DCULL_CCW );
	my3D.Device()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	my3D.Device()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	my3D.Device()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	my3D.Device()->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );

	// Render the image
	my3D.Device()->SetStreamSource( 0, m_pVB, 6*sizeof(FLOAT) );
	my3D.Device()->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );

	if (m_pVB!=NULL)
	{
		m_pVB->Release();
		m_pVB=NULL;
	}

	}*/
	return 0;
}

// Launch XBE
int lua_LaunchXBE(lua_State *L)
{
	bool  EnablePatch = myDash.bRegionPatch;
	char *xbefile=NULL;

	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Check Args
	if( iArgs != 1 ) { return 0; }

	xbefile = (char*)lua_tostring(L, 1);

	// Launching DVD Title
	if (XmlCmpNoCase("dvd", xbefile))
	{
		myXBELauncher.Launch_XBOX_Game();
		myXBELauncher.Launch_XBOX_Movie();
	}
	else {
		myXBELauncher.Custom_Launch_XBOX_Game(EnablePatch, xbefile);
	}

	return 1;
}

// Format Drive Partition
int lua_FormatDrive( lua_State* L )
{
	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Check Args
	if( iArgs != 1 ) { return 0; }

	FormatDrivePartition((char*)lua_tostring(L, 1));

	return 1;
}

// Reset TitleImages
void ResetTitleImages()
{
	char szRemoveTitle[255];

	// W-I-P Remove Only TitleImages TXR?
	for(int i=0; i<MAX_OBJECTS; i++)
	{
		sprintf(szRemoveTitle, "txr%i", i);
		if( myTextures[i].szName == szRemoveTitle ) { 
		// Release All Loaded Textures
		delete [] myTextures[i].szName;
		myTextures[i].szName = NULL;
		if (myTextures[i].iType == 0) myTextures[i].pTexture->Release();
		if (myTextures[i].iType == 1) {
			free( myTextures[i].pTexture );
		}
		myTextures[i].pTexture = NULL;
		}
	}
}

// Reset Structures
void StructReset()
{
	// Need a Check on total ammount of Objects Loaded!!! -- Check Now in Place 16/11/04
	for(int i=0; i<MAX_OBJECTS; i++)
	{
		if( myModels[i].szName == NULL ) { 
		} else {
		// Release All Loaded Mesh Files
		delete [] myModels[i].szName;
		myModels[i].szName = NULL;
		delete [] myModels[i].pMesh;
		myModels[i].pMesh = NULL;
		}

		if( myFonts[i].szName == NULL ) { 
		} else {
		// Release All Loaded Font Files
		delete [] myFonts[i].szName;
		myFonts[i].szName = NULL;
		myFonts[i].pFont->Destroy();
		free( myFonts[i].pFont );
		myFonts[i].pFont = NULL;
		}

		if( myTextures[i].szName == NULL ) { 
		} else {
		// Release All Loaded Textures
		delete [] myTextures[i].szName;
		myTextures[i].szName = NULL;
		//myTextures[i].pTexture->Release();
		if (myTextures[i].iType == 0) myTextures[i].pTexture->Release();
		if (myTextures[i].iType == 1) {
			free( myTextures[i].pTexture );
		}
		myTextures[i].pTexture = NULL;
		}

		if( myWaveFiles[i].szName == NULL ) { 
		} else {
		// Release All Loaded Wave Files
		// Destroy Needs a Load Check!
		delete [] myWaveFiles[i].szName;
		myWaveFiles[i].szName = NULL;
		myWaveFiles[i].pSound->Destroy();
		free( myWaveFiles[i].pSound );
		myWaveFiles[i].pSound = NULL;
		}
	}
}

// Set Skin
int lua_SetSkin( lua_State* L )
{
	// Get Arg Number
	int iArgs = lua_gettop( L );

	// Check Args
	if( iArgs != 1 ) { return 0; }

	OutputDebugString(lua_tostring(L, 1));
	myDash.sSkin = lua_tostring(L, 1);

	StructReset(); // Clear All our Structurs ready for next Skin.

	return 1;
}

// Save Configuration
int lua_SaveConfig( lua_State* L )
{
	SaveConfig();

	return 1;
}

// Lock / Unlock HDD

void HDDLockThread()
{
	DWORD dwThreadId, dwThrdParam = 1; 
    HANDLE hThread; 

	    hThread = CreateThread( 
        NULL,                        // (this parameter is ignored)
        0,                           // use default stack size  
        LockHDDThread,                  // thread function 
        &dwThrdParam,                // argument to thread function 
        0,                           // use default creation flags 
        &dwThreadId); 

	CloseHandle( hThread );
}

int lua_LockHDD( lua_State* L )
{
	char *type;
	type = "1";
	//CreateThread(0, 0, &LockHDDThread, NULL, 0, 0);
	//CreateThread(NULL, 0, LockHDDThread, type, 0, NULL);
	//GetEEpromData();
	//LockHDD();
	HDDLockThread();

	return 1;
}

int lua_UnlockHDD( lua_State* L )
{
	char *type;
	type = "1";
	//CreateThread(0, 0, &UnLockHDDThread, NULL, 0, 0);
	CreateThread(NULL, 0, UnLockHDDThread, type, 0, NULL);
	//GetEEpromData();
	//UnlockHDD();

	return 1;
}

// AutoUpdate
int lua_AutoUpdate( lua_State* L )
{
	CreateThread(0, 0, &AutoUpdateThread, NULL, 0, 0);

	return 1;
}

 // LUA OutputDebug
int lua_DebugOutput(lua_State *L)
{
	int n=lua_gettop(L);
	if (n) 
	{
		char *DebugText = (char *)lua_tostring(L,1);
		OutputDebugString (DebugText);
	}
 	return 0;
 }

// Set LCD Text from LUA
int lua_SetLCDText (lua_State* L)
{
	char* pointer;
	pointer = (char *) malloc(255);
	int iArgs = lua_gettop( L );
	switch (iArgs) {
			case 1:
				if (myxLCD.GetRenderTextStatus()) {
					strcpy(pointer,lua_tostring(L,1));
					myxLCD.UpdateDisplayText(pointer);
				}
				break;
			case 2:
				if (myxLCD.GetRenderTextStatus()) {
					strcpy(pointer,lua_tostring(L,2));
										myxLCD.UpdateLine((int)lua_tonumber(L,1),pointer);
				}
				break;
			default:
				break;
	}
	delete [] pointer;
	pointer=NULL;
	return 1;
}

// Clear Cache
int lua_ClearCache(lua_State *L)
{
	DeleteFile("U:\\menu0.dat");
	DeleteFile("U:\\menu1.dat");
	DeleteFile("U:\\menu2.dat");
	DeleteFile("U:\\menu3.dat");
	DeleteFile("U:\\menu4.dat");
	DeleteFile("U:\\menu5.dat");
	DeleteFile("U:\\menu6.dat");
	DeleteFile("U:\\menu7.dat");
	DeleteFile("U:\\menu8.dat");
	DeleteFile("U:\\default.dat");

	return 1;
}

/*
	FTP Client LUA Commands
*/
// Overkill, if any idiot actually uses shit that overflows this then they deserve what they get.
char ftpclient_user[30];
char ftpclient_pass[30];

DWORD WINAPI T_DoOpen(LPVOID string) {
	progressmax=3;
	theFtpConnection->DoOpen((char *) string);

	char user_pass[64];
	//sprintf(user_pass, "%s:%s", "xbox", "xbox");
	sprintf(user_pass, "%s:%s", myDash.sRemoteFTPUser.c_str(), myDash.sRemoteFTPPass.c_str());
	//sprintf(user_pass, "%s:%s", ftpclient_user, ftpclient_pass);
	theFtpConnection->DoLogin(user_pass);
	// progress++ causes Black screen on Connect
	//progressbar++;
	theFtpConnection->DoList("dirFiltered");
	//progressbar++;
	theFtpConnection->DoLLS("*");

	return 0;
}

int lua_ftpopen(lua_State *L)
{
	/*int n=lua_gettop(L);
    int i;
	char *remote;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1: remote = (char *)lua_tostring(L, i); break;
		}
    }

	if(remote!=NULL) {
		if(CreateThread(NULL, 0, T_DoOpen, remote, 0, NULL)!=NULL) {
			lua_pushnumber(L, 1);
			return 1;
		}
	}*/
	char *remote;
	//char szHostIP[255];
	//sprintf(szHostIP, "%s", myDash.sRemoteFTPHost.c_str()); 
	remote = (char *) myDash.sRemoteFTPHost.c_str();
	//remote = "192.168.0.44";
	if(CreateThread(NULL, 0, T_DoOpen, remote, 0, NULL)!=NULL) {
			lua_pushnumber(L, 1);
			return 1;
		}

	lua_pushnumber(L, 0);
	return 1;
}

DWORD WINAPI T_DoList(LPVOID string) {
	theFtpConnection->DoList("dirFiltered");
	return 0;
}

int lua_ftplist(lua_State *L)
{
	if(CreateThread(NULL, 0, T_DoList, NULL, 0, NULL)!=NULL) {
		lua_pushnumber(L, 1);
		return 1;
	}

	lua_pushnumber(L, 0);
	return 1;
}

DWORD WINAPI T_DoLLS(LPVOID string) {
	theFtpConnection->DoLLS("*");
	return 0;
}

int lua_ftplls(lua_State *L)
{
	if(CreateThread(NULL, 0, T_DoLLS, NULL, 0, NULL)!=NULL) {
		lua_pushnumber(L, 1);
		return 1;
	}

	lua_pushnumber(L, 0);
	return 1;
}

DWORD WINAPI T_DoCD(LPVOID string) {
	theFtpConnection->DoCD((char *) string);
	// Causes Crash!: Very odd as this works ok for OnConnect
	theFtpConnection->DoList("dirFiltered");
	return 0;
}

int lua_ftpcd(lua_State *L)
{
	int n=lua_gettop(L);
    int i;
	char *type;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1: type = (char *)lua_tostring(L, i); break;
		}
    }

	if(type!=NULL) {
		if(CreateThread(NULL, 0, T_DoCD, type, 0, NULL)!=NULL) {
			lua_pushnumber(L, 1);
			return 1;
		}
	}
	lua_pushnumber(L, 0);
	return 1;
}

int lua_ftpsetuser(lua_State *L)
{
	int n=lua_gettop(L);
    int i;
	char *type;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1: type = (char *)lua_tostring(L, i); break;
		}
    }

	if(type!=NULL) {
		strcpy(ftpclient_user, type);
	}

	return 0;
}

int lua_ftpsetpass(lua_State *L)
{
	int n=lua_gettop(L);
    int i;
	char *type;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1: type = (char *)lua_tostring(L, i); break;
		}
    }

	if(type!=NULL) {
		strcpy(ftpclient_pass, type);
	}

	return 0;
}

int lua_ftpclose(lua_State *L)
{
	theFtpConnection->DoClose();
	return 0;
}

DWORD WINAPI T_DoLCD(LPVOID string) {
	Menu[5].Clear();
	theFtpConnection->DoLCD((char *) string);
	theFtpConnection->DoLLS("*");
	return 0;
}

int lua_ftplcd(lua_State *L)
{
	int n=lua_gettop(L);
    int i;
	char *type;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1: type = (char *)lua_tostring(L, i); break;
		}
    }

	if(type!=NULL) {
		if(CreateThread(NULL, 0, T_DoLCD, type, 0, NULL)!=NULL) {
			lua_pushnumber(L, 1);
			return 1;
		}
	}

	lua_pushnumber(L, 0);
	return 1;
}

DWORD WINAPI T_DoPut(LPVOID string) {
	theFtpConnection->DoPut((char *) string);
	return 0;
}

int lua_ftpput(lua_State *L)
{
	int n=lua_gettop(L);
    int i;
	char *type;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1: type = (char *)lua_tostring(L, i); break;
		}
    }

	if(type!=NULL) {
		if(CreateThread(NULL, 0, T_DoPut, type, 0, NULL)!=NULL) {
			lua_pushnumber(L, 1);
			return 1;
		}
	}

	lua_pushnumber(L, 0);
	return 1;
}

DWORD WINAPI T_DoGet(LPVOID string) {
	theFtpConnection->DoGet((char *) string);
	return 0;
}

int lua_ftpget(lua_State *L)
{
	int n=lua_gettop(L);
    int i;
	char *type;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1: type = (char *)lua_tostring(L, i); break;
		}
    }

	if(type!=NULL) {
		if(CreateThread(NULL, 0, T_DoGet, type, 0, NULL)!=NULL) {
			lua_pushnumber(L, 1);
			return 1;
		}
	}

	lua_pushnumber(L, 0);
	return 1;
}

int lua_ftpbinary(lua_State *L)
{
	theFtpConnection->DoBinary();
	return 0;
}

int lua_ftpascii(lua_State *L)
{
	theFtpConnection->DoAscii();
	return 0;
}

int lua_ftppwd(lua_State *L)
{
	theFtpConnection->DoPWD();
	return 0;
}