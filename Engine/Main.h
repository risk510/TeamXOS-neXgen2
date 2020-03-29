#ifndef __MAIN_H__
#define __MAIN_H__


#include <xtl.h>
#include <string>
using namespace std;
#include "XbDirect3D.h"
#include "XbCamera.h"
#include "XbMesh.h"
#include "XbFont.h"
#include "XbFog.h"
#include "xblight.h"

#include "MapPath.h"
#include "XbGamePad.h"
#include "RemoteCtrl\\InputIR.h"
#include "RemoteCtrl\\DelayController.h"
#include "xbsound.h"
#include "LoadConfig.h"
#include "SaveConfig.h"
#include "XboxInfo.h"
#include "Threads.h"

#include "Lock_Unlock.h"

#include "Xenium\\Xenium.h"

#include "FTP\\Network.h"
#include "FTPClient\ftpclient.h"
//#include "xbfz\xbftp.h"
#include "xbfz\XbHTTPDownloader.h"
#include "xbfz\xbrssfeed.h"

#include "AutoUpdate.h"

#include "HTTPServer\\HTTPServer.h"

#include "XKUtils\\xkutils.h"
#include "XKUtils\\xkeeprom.h"
#include "XKUtils\\xkexports.h"

#include "Screenshot\\ScreenShot.h"

#include "SoundTracks\\XBSndTrk.h"

#include "SnowFlakes\\SnowFlake.h"

#include "Screensaver.h"
#include "Matrix\\xbMatrix.h"
#include "matrixfont.h"

#include "MD5\\XBMD5.h"

#include "Launcher.h"

#include "splash.h"

#include "LUA\\LUACalls.h"

#include "zlib\\unzip.h"
#include "zlib\\zlib.h"
#include "xbUnzip.h"

#include "X3\\xLCD.h"

#include "xbeheader.h"

// XLCD Structure
typedef struct XLCDOptions {
	bool CDrive;
	bool DDrive;
	bool EDrive;
	bool FDrive;
	bool GDrive;
	bool XDrive;
	bool YDrive;
	bool ZDrive;
	bool CPUTemp;
	bool GPUTemp;
	bool Music;

} XLCDOptions;

typedef struct XLCD {
	bool enabled;
	int rows;
	int cols;
	int UpdateSpeed;
	XLCDOptions Options;
} XLCD;

typedef struct X3Data {
	bool enabled;
	XLCD xLCD;
} X3Data;

// Config Structure
struct stDashConfig {
	// General
	string	sXboxName;
	string	sSkin;
	bool	bAutoLaunchDVD;
	bool	bAutoLaunchGame;
	bool	bAutoLaunchUnknown;
	string	sDVDPlayer;
	string	sDashboard;
	string	sUnknownPlayer;

	int		iScreenSaverTime;
	int		iScreenSaverFadeLevel;
	int		iScreenSaverMode;

	// Network
	bool	bDHCP;
	string	sIP;
	string	sSubnetMask;
	string	sGateway;
	string	sDNS1;
	string	sDNS2;
	string	sFTPUser;
	string	sFTPPass;
	string	sHTTPUser;
	string	sHTTPPass;
	string	sRemoteFTPHost;
	string	sRemoteFTPPort;
	string	sRemoteFTPUser;
	string	sRemoteFTPPass;

	// Search Paths
	string sAppsPath;
	string sGamesPath;
	string sEmusPath;
	string sSoundsPath;

	// Music
	bool	bAutoStartMusic;
	bool	bRandomMusicPlay;

	// Advanced
	bool	bUseCache;
	bool	bRegionPatch;
	int		iFanSpeed;
	int		iAutoPowerOff;

	// XBOX LED Colours
	int		iLEDComplete;
	int		iLEDTransfer;
	int		iLEDError;
	int		iLEDFTPConnect;
	int		iLEDStartup;

	// Xenium Support
	bool	bXeniumEnabled;
	int		iXeniumMode;

	// Xecuter3
	bool	bX3Enabled;
	X3Data 	x3Settings;
};

// Vertex Structure
struct stVertex {
	FLOAT x, y, z, rhw;
	FLOAT tu, tv;
};

// Model Structure
struct stModel {
	char*	 szName;
	CXBMesh* pMesh;
};

// Font Structure
struct stFont {
	char*	 szName;
	CXBFont* pFont;
};

// Textures Structure
struct stTexture {
	char* szName;
	LPDIRECT3DTEXTURE8 pTexture;
	int iType;
};

// WaveBank Structure
struct stWaveFiles {
	char* szName;
	CXBSound* pSound;
	LPDIRECTSOUNDBUFFER  m_pDSoundBuffer;
};

// SnowFlake Structure
struct stSnowFlakes {
	char* szName;
	CXBSnowFlake* pSnow;
};


#define MAX_OBJECTS			1024
#define DASHBOARD_CONFIG	"SYSTEM:\\nexgen.xml"
#define DEFAULT_SKIN		"SKINS:\\Default.skin"



extern CXBCamera	myCam;	// Global Camera
extern CXBDirect3D	my3D;	// Global DirectX Device

extern CXBFog		myFog;	// Global Fog Instance
extern CXBSndTrk	mySndTrk; // Sound Tracks
extern CXBSnowFlake mySnow;	// Snow Flakes
//extern CXBScreenSaver myScreenSaver;
extern CXBFont myFont;	// Global Font
extern CXBMatrix	myMatrix; // Global Matrix

// X3 XLCD
extern xLCDSysInfo myxLCDSysInfo;
extern xLCD myxLCD;

extern CXBXeniumOS myXenium;  // Global XeniumOS
extern CXBScreenShot myScreenShot; // Global Screenshots
extern CXBRemoteCtrl myRemoteCtrl; // Global iR Remote Control
extern CXBHTTPServer myHTTPServer;

extern stDashConfig	myDash;	// Global Dash Configuration
extern stModel		myModels[MAX_OBJECTS];	// Global Models Instance
extern stFont		myFonts[MAX_OBJECTS];	// Global Fonts Instance
extern stTexture	myTextures[MAX_OBJECTS];// Global Textures Instance
extern stWaveFiles	myWaveFiles[MAX_OBJECTS];// Global WaveBank Instance
extern stSnowFlakes mySnowFlakes[MAX_OBJECTS];	// Global Snow Flakes

extern CXBMD5 myMD5;							// Global MD5

extern bool bUseMatrixScreensaver;

extern char			g_szXboxVersion[25]; // XBOX Version
extern char			g_szXboxEncoder[25]; // XBOX Encoder

extern char			szSystemPath[255]; // System Path
extern char			szSkinsPath[255]; // Skins Path

extern void			GetPadInput();

#endif