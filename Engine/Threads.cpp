#include "Threads.h"

// Thread For Music Updates
DWORD __stdcall MusicThread( void* pData )
{
	// Keep On Updating!
	while( true )
	{
		// Update
		mySndTrk.Update();

		// Wait To Save CPU Power
		Sleep(10);
	}
}

// Thread For Lock HDD
//DWORD __stdcall LockHDDThread( void* pData )
//DWORD WINAPI LockHDDThread(LPVOID string)
DWORD WINAPI LockHDDThread( LPVOID lpParam )
{
	//GetEEpromData();
	LockHDD();
	Sleep(10);
	return 0;
}

//DWORD __stdcall UnLockHDDThread( void* pData )
DWORD WINAPI UnLockHDDThread(LPVOID string)
{
	GetEEpromData();
	UnlockHDD();
	Sleep(10);
	return 0;
}

// Thread For RSS News Feed
DWORD __stdcall RSSNewsThread( void* pData )
{
	// Clear out any old cached news.xml file
	if (FileExists("z:\\news.xml"))
	{
		DeleteFile("z:\\news.xml");
	}

	// Keep On Updating!
	while( true )
	{
		if (IsEthernetConnected())
		{
			if (!FileExists("z:\\news.xml"))
			{
				get_rss();
			}
		}

		// Wait To Save CPU Power
		Sleep(10);
	}
}

// Thread For AutoUpdate
DWORD __stdcall AutoUpdateThread( void* pData )
{
	if (IsEthernetConnected())
	{
		get_updates();
	}

	return 0;
}

DWORD __stdcall Init_NetworkThread( void* pData )
{
	bool bInitNetwork = false;
	// Keep On Updating!
	while( true )
	{
		if (IsEthernetConnected())
		{
			if (!bInitNetwork)
			{
				bInitNetwork = true;
				Sleep(10);

				/* Removed as no longer using XBFZ. fixup with current FTP Server.*/
				// Init Network (FTP/HTTP).
				/*Network_Init(myDash.sIP.c_str(), myDash.sSubnetMask.c_str(), myDash.sGateway.c_str(),
					myDash.sDNS1.c_str(), myDash.sDNS2.c_str(),!myDash.bDHCP );
				Network_FTP_StartServer();
				sFTPServer nexgenServerConfig;
				nexgenServerConfig.MaxLogins=2;
				nexgenServerConfig.Port=21;
				strcpy(nexgenServerConfig.WelcomeMessage,"Welcome to Nexgen");
				Network_FTP_Config(nexgenServerConfig);
				Network_FTP_AddUser((char*)myDash.sFTPUser.c_str(),(char*)myDash.sFTPPass.c_str() );*/
			}
		}
		else
		{
			bInitNetwork = false;
			//Network_Shutdown();
		}
		// Wait To Save CPU Power
		Sleep(10);
	}
}

// Xecuter 3 XLCD
DWORD __stdcall XLCDThread( void* pData)
{
	if (myDash.x3Settings.xLCD.enabled)
		myxLCD.Init();
    	
	return 0;
}

DWORD __stdcall XLCDSysInfoThread( void* pData )
{
	if (myDash.x3Settings.xLCD.enabled)
		myxLCDSysInfo.Init();
		OutputDebugString("Starting Sysinfo thread\n");
		return 0;
}