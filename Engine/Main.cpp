#include <xtl.h>
#include <conio.h>

#include "Main.h"

bool bUseMatrixScreensaver = false;

static int fade_out_splash =60;

HRESULT hr;

//XBOX_VERSION xver;
XKUtils m_xkutils;
XKEEPROM m_xkeeprom;


CXBDirect3D	my3D;
CXBCamera	myCam(my3D.Device());

CXBFog		myFog;
CXBSndTrk	mySndTrk;
CXBSnowFlake mySnow;
CXBScreenSaver myScreenSaver;
CXBFont		myFont;
CXBMatrix	myMatrix;

CXBRemoteCtrl myRemoteCtrl;
CXBHTTPServer myHTTPServer;
CXBXeniumOS myXenium;

xLCDSysInfo myxLCDSysInfo;
xLCD myxLCD;

CXBMD5 myMD5;

CXBScreenShot myScreenShot;

CXBLight    myLight;

stModel		myModels[MAX_OBJECTS];
stFont		myFonts[MAX_OBJECTS];
stTexture	myTextures[MAX_OBJECTS];
stWaveFiles	myWaveFiles[MAX_OBJECTS];
stSnowFlakes mySnowFlakes[MAX_OBJECTS];


char g_szXboxVersion[25];
char g_szXboxEncoder[25];

char szSystemPath[255];
char szSkinsPath[255];

char szDefaultSkin[255];


void main( void )
 {
	// Seed Random Number
	time_t t;
	srand( time(&t) );

	// Disable Reset on Eject.
	m_xkutils.DVDDisableEjectReset();

	// XBOX EEprom Handleing
	//GetEEpromData();
	//LockHDD();
	//CreateThread(0, 0, &LockHDDThread, NULL, 0, 0);

	// re-mount our Drives
	resetpaths();

	// Get Program LaunchPath
	XI_GetProgramPath(szSystemPath);

	PANSI_STRING pImageFileName = (PANSI_STRING)XeImageFileName;

	//Init input (Gamepads / iR Remote Control)
	Input_Init(); // GamePad
	myRemoteCtrl.Initialize(); // iR Remote Control

	// Set Skins Dir From LaunchPath
	sprintf(szSkinsPath, "%s\\skins", szSystemPath);

	// SYSTEM: -- Alternate D Mounting
	// SKINS: -- Skins Dir
	// Re-map our new Drives
	mappath(szSystemPath, "SYSTEM:");
	mappath(szSkinsPath, "SKINS:");

	// Initialize Dash
	LoadConfig(); // Must call First to set paths and other configuration options!

	// Chech AutoLaunch Status: 
	if ( myDash.bAutoLaunchDVD || myDash.bAutoLaunchGame )
	{
			myXBELauncher.Launch_XBOX_Game();
			myXBELauncher.Launch_XBOX_Movie();
	}

	// Init Network (FTP/HTTP).
	//CreateThread(0, 0, &Init_NetworkThread, NULL, 0, 0); // W-I-P
	/*Network_Init(myDash.sIP.c_str(), myDash.sSubnetMask.c_str(), myDash.sGateway.c_str(),
		myDash.sDNS1.c_str(), myDash.sDNS2.c_str(),!myDash.bDHCP );
*/
	InitializeNetwork(); // reverted to old FTP Server. More stable.

	// Init HTTP server
	myHTTPServer.Initialize(); // Testing new HTTPD fix up. W-I-P

	// Setup Matrix Screensaver -- ToDo: Add more checks + maybe redo font embedding
	MatrixFont_Create("z:\\matrixfont.xpr");
	myMatrix.Create("z:\\matrixfont.xpr");

	// Innit XBOX Info
	XI_GetVersion( g_szXboxVersion );
	XI_GetEncoder( g_szXboxEncoder );

	// Search and Index XBE's
	FindFiles(0, myDash.sAppsPath.c_str(), NULL, 0, 0);
	FindFiles(1, myDash.sEmusPath.c_str(), NULL, 0, 0);
	FindFiles(2, myDash.sGamesPath.c_str(), NULL, 0, 0);

	// Search and Index Skin Files
	FindFiles(3, "SKINS:", "*.skin", 1, 0);

	// Load Lua Skin Set
	sprintf(szDefaultSkin, "SKINS:\\%s", myDash.sSkin.c_str());

	if (!FileExists(szDefaultSkin))
	{
		// Custom Skin not set Load Default
		SetLUABuffer( "SKINS:\\Default\\Main.lua" );
	}
	else
	{
		// Found Custom skin file
		SetLUABuffer( szDefaultSkin );
	}

	// SoundTracks
	bool bAutoStarted = false;
	mySndTrk.Initialize();
	mySndTrk.SetRandomPlay( myDash.bRandomMusicPlay );

	//RSS feed
	// Create RSS News thread so we can keep polling untill we get a news.xml
	CreateThread(0, 0, &RSSNewsThread, NULL, 0, 0);	
	
	// Render Loop
	while (true)
	{
		my3D.Device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0 );

		// Frame Updates
		myCam.UpdateCamera();
		GetPadInput();
		
		// Check For Reboot
		// For DEV Purpose Only!
		// To Be Removed In Final Build!
		//		if( myPad.CheckReboot() ) { XLaunchNewImage( NULL, NULL ); }

		// Temporary Basic Calls
		// Will Be Added To LUA Soon!
		my3D.Device()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		my3D.Device()->SetRenderState(D3DRS_LIGHTING, false);
		my3D.Device()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		my3D.Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		my3D.Device()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		my3D.Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		my3D.Device()->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		my3D.Device()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		my3D.Device()->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		my3D.Device()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		myLight.Do_Light();
		// Hardcoded ScreenSaver
		if ( myDash.iScreenSaverMode ) myScreenSaver.CheckScreenSaver();

		// LUA Stuff
		UpdateLUAVariables();
		FrameMoveLUAScene();
		RenderLUAScene();
		render_rss();

		// Start Playing SoundTracks
		// Moved Below LUA Becasue Music Skips When Loading The
		// Resources From The LUA Above - Only Called Once.. Then Set To True!
		if( mySndTrk.HasSoundTracks() && myDash.bAutoStartMusic && !bAutoStarted )
		{
			mySndTrk.Play();
			bAutoStarted = true;
			CreateThread(0, 0, &MusicThread, NULL, 0, 0);
		}

		if (fade_out_splash)
		{
			draw_splash((1.0f/60)*fade_out_splash);
			fade_out_splash--;
		}
				
		// Moved Below LUA so we do see the Render ;)
		// Enable matrix screensaver
		if ( bUseMatrixScreensaver ) myMatrix.Render();

		// Present Our Render
		my3D.Device()->Present( NULL, NULL, NULL, NULL );
	}
}

void GetPadInput()
{
	Input_Frame();

	// iR Remote
	WORD wRemotes = myRemoteCtrl.m_DefaultIR_Remote.wButtons;
	DWORD				m_dwTimer;
	m_dwTimer = 0;
	myRemoteCtrl.UpdateInput();

	bool g_bPadUp = false;
	bool g_bPadDown = false;
	bool g_bPadLeft = false;
	bool g_bPadRight = false;
	bool g_bLeftStickUp = false;
	bool g_bLeftStickDown = false;
	bool g_bLeftStickLeft = false;
	bool g_bLeftStickRight = false;
	bool g_bRightStickUp = false;
	bool g_bRightStickDown = false;
	bool g_bRightStickLeft = false;
	bool g_bRightStickRight = false;
	bool g_bGreenButtonDown = false;
	bool g_bRedButtonDown = false;
	bool g_bBackButtonDown = false;
	bool g_bBlueButtonDown = false;
	bool g_bYellowButtonDown = false;
	bool g_bBlackButtonDown = false;
	bool g_bWhiteButtonDown = false;
	bool g_bStartButtonDown = false;
	bool g_bLeftTriggerDown = false;
	bool g_bRightTriggerDown = false;

	// iR Remote Control
	bool g_biR_Up = false;
	bool g_biR_Down = false;
	bool g_biR_Left = false;
	bool g_biR_Right = false;
	bool g_biR_Select = false;
	bool g_biR_0 = false;
	bool g_biR_1 = false;
	bool g_biR_2 = false;
	bool g_biR_3 = false;
	bool g_biR_4 = false;
	bool g_biR_5 = false;
	bool g_biR_6 = false;
	bool g_biR_7 = false;
	bool g_biR_8 = false;
	bool g_biR_9 = false;
	bool g_biR_Display = false;
	bool g_biR_Reverse = false;
	bool g_biR_Play = false;
	bool g_biR_Forward = false;
	bool g_biR_DSkip = false;
	bool g_biR_Stop = false;
	bool g_biR_Pause = false;
	bool g_biR_USkip = false;
	bool g_biR_Title = false;
	bool g_biR_Info = false;
	bool g_biR_Menu = false;
	bool g_biR_Back = false;



	// XBOX iR Remote Control
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_UP )			{ g_biR_Up = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_DOWN )			{ g_biR_Down = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_LEFT )			{ g_biR_Left = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_RIGHT )			{ g_biR_Right = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_SELECT )			{ g_biR_Select = true; myScreenSaver.ResetScreensaver();}

	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_DISPLAY )			{ g_biR_Display = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_REVERSE )			{ g_biR_Reverse = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_PLAY )			{ g_biR_Play = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_FORWARD )			{ g_biR_Forward = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_SKIP_MINUS )			{ g_biR_DSkip = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_STOP )			{ g_biR_Stop = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_PAUSE )			{ g_biR_Pause = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_SKIP_PLUS )			{ g_biR_USkip = true; myScreenSaver.ResetScreensaver();}

	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_TITLE )			{ g_biR_Title = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_INFO )			{ g_biR_Info = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_MENU )			{ g_biR_Menu = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_BACK )			{ g_biR_Back = true; myScreenSaver.ResetScreensaver();}

	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_0 )			{ g_biR_0 = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_1 )			{ g_biR_1 = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_2 )			{ g_biR_2 = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_3 )			{ g_biR_3 = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_4 )			{ g_biR_4 = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_5 )			{ g_biR_5 = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_6 )			{ g_biR_6 = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_7 )			{ g_biR_7 = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_8 )			{ g_biR_8 = true; myScreenSaver.ResetScreensaver();}
	if(myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_9 )			{ g_biR_9 = true; myScreenSaver.ResetScreensaver();}


	// XBOX GamePad
	if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )				{ g_bPadUp			= true; myScreenSaver.ResetScreensaver();}
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )				{ g_bPadDown		= true; myScreenSaver.ResetScreensaver();}
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )				{ g_bPadLeft		= true; myScreenSaver.ResetScreensaver();}
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )			{ g_bPadRight		= true; myScreenSaver.ResetScreensaver();}

	if(g_Gamepads[0].fY1 < 0  )				{ g_bLeftStickUp	= true; myScreenSaver.ResetScreensaver();}
	if(  g_Gamepads[0].fY1 > 0 )			{ g_bLeftStickDown	= true; myScreenSaver.ResetScreensaver();}
	if(g_Gamepads[0].fX1 > 0 )			{ g_bLeftStickLeft	= true; myScreenSaver.ResetScreensaver();}
	if( g_Gamepads[0].fX1 < 0  )			{ g_bLeftStickRight	= true; myScreenSaver.ResetScreensaver();}

	if(g_Gamepads[1].fY1 < 0  )				{ g_bRightStickUp	= true; myScreenSaver.ResetScreensaver();}
	if(  g_Gamepads[1].fY1 > 0 )			{ g_bRightStickDown	= true; myScreenSaver.ResetScreensaver();}
	if(g_Gamepads[1].fX1 > 0 )				{ g_bRightStickLeft	= true; myScreenSaver.ResetScreensaver();}
	if( g_Gamepads[1].fX1 < 0  )			{ g_bRightStickRight = true; myScreenSaver.ResetScreensaver();}

	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A])				{ g_bGreenButtonDown = true; myScreenSaver.ResetScreensaver();}
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B])				{ g_bRedButtonDown	 = true; myScreenSaver.ResetScreensaver();}
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )			{ g_bBlueButtonDown	 = true; myScreenSaver.ResetScreensaver();}
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y])			{ g_bYellowButtonDown = true; myScreenSaver.ResetScreensaver();}

	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] )			{ g_bLeftTriggerDown	 = true; myScreenSaver.ResetScreensaver();}
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] )			{ g_bRightTriggerDown	 = true; myScreenSaver.ResetScreensaver();}

	if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )			{ g_bBlackButtonDown	 = true; myScreenSaver.ResetScreensaver();}
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] )			{ g_bWhiteButtonDown	 = true; myScreenSaver.ResetScreensaver();}

	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK  )			{ g_bBackButtonDown	 = true; myScreenSaver.ResetScreensaver();}
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START  )			{ g_bStartButtonDown = true; myScreenSaver.ResetScreensaver();}

	// LUA Bindings for GamePad / iR Remote Buttons
	lua_pushboolean( LUA_STATE, g_bStartButtonDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_START_BUTTON" );

	lua_pushboolean( LUA_STATE, g_bBackButtonDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_BACK_BUTTON" );

	lua_pushboolean( LUA_STATE, g_bGreenButtonDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_A_BUTTON" );

	lua_pushboolean( LUA_STATE, g_bRedButtonDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_B_BUTTON" );

	lua_pushboolean( LUA_STATE, g_bBlueButtonDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_X_BUTTON" );

	lua_pushboolean( LUA_STATE, g_bYellowButtonDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_Y_BUTTON" );

	lua_pushboolean( LUA_STATE, g_bBlackButtonDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_BLACK_BUTTON" );

	lua_pushboolean( LUA_STATE, g_bWhiteButtonDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_WHITE_BUTTON" );

	lua_pushboolean( LUA_STATE, g_bLeftTriggerDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_LEFT_TRIGGER" );

	lua_pushboolean( LUA_STATE, g_bRightTriggerDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_RIGHT_TRIGGER" );

	lua_pushboolean( LUA_STATE, g_bPadUp );
	lua_setglobal( LUA_STATE, "XBOX_PAD_UP" );

	lua_pushboolean( LUA_STATE, g_bPadDown );
	lua_setglobal( LUA_STATE, "XBOX_PAD_DOWN" );

	lua_pushboolean( LUA_STATE, g_bPadLeft );
	lua_setglobal( LUA_STATE, "XBOX_PAD_LEFT" );

	lua_pushboolean( LUA_STATE, g_bPadRight );
	lua_setglobal( LUA_STATE, "XBOX_PAD_RIGHT" );

	lua_pushboolean( LUA_STATE, g_bLeftStickUp );
	lua_setglobal( LUA_STATE, "XBOX_LEFT_STICK_UP" );

	lua_pushboolean( LUA_STATE, g_bLeftStickDown );
	lua_setglobal( LUA_STATE, "XBOX_LEFT_STICK_DOWN" );

	lua_pushboolean( LUA_STATE, g_bLeftStickLeft );
	lua_setglobal( LUA_STATE, "XBOX_LEFT_STICK_LEFT" );

	lua_pushboolean( LUA_STATE, g_bLeftStickRight );
	lua_setglobal( LUA_STATE, "XBOX_LEFT_STICK_RIGHT" );

	lua_pushboolean( LUA_STATE, g_bRightStickUp );
	lua_setglobal( LUA_STATE, "XBOX_RIGHT_STICK_UP" );

	lua_pushboolean( LUA_STATE, g_bRightStickDown );
	lua_setglobal( LUA_STATE, "XBOX_RIGHT_STICK_DOWN" );

	lua_pushboolean( LUA_STATE, g_bRightStickLeft );
	lua_setglobal( LUA_STATE, "XBOX_RIGHT_STICK_LEFT" );

	lua_pushboolean( LUA_STATE, g_bRightStickRight );
	lua_setglobal( LUA_STATE, "XBOX_RIGHT_STICK_RIGHT" );


	lua_pushboolean( LUA_STATE, m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP );
	lua_setglobal( LUA_STATE, "XBOX_PAD_UP_HELD" );

	lua_pushboolean( LUA_STATE, m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN );
	lua_setglobal( LUA_STATE, "XBOX_PAD_DOWN_HELD" );

	lua_pushboolean( LUA_STATE, m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT );
	lua_setglobal( LUA_STATE, "XBOX_PAD_LEFT_HELD" );

	lua_pushboolean( LUA_STATE, m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT );
	lua_setglobal( LUA_STATE, "XBOX_PAD_RIGHT_HELD" );

	lua_pushboolean( LUA_STATE, m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_START  );
	lua_setglobal( LUA_STATE, "XBOX_PAD_START_BUTTON_HELD" );

	lua_pushboolean( LUA_STATE, m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_BACK  );
	lua_setglobal( LUA_STATE, "XBOX_PAD_BACK_BUTTON_HELD" );

	lua_pushboolean( LUA_STATE, m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_A]  );
	lua_setglobal( LUA_STATE, "XBOX_PAD_A_BUTTON_HELD" );

	lua_pushboolean( LUA_STATE,  m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_B]  );
	lua_setglobal( LUA_STATE, "XBOX_PAD_B_BUTTON_HELD" );

	lua_pushboolean( LUA_STATE,  m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_X]  );
	lua_setglobal( LUA_STATE, "XBOX_PAD_X_BUTTON_HELD" );

	lua_pushboolean( LUA_STATE,  m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_Y]  );
	lua_setglobal( LUA_STATE, "XBOX_PAD_Y_BUTTON_HELD" );

	lua_pushboolean( LUA_STATE,  m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK]  );
	lua_setglobal( LUA_STATE, "XBOX_PAD_BLACK_BUTTON_HELD" );

	lua_pushboolean( LUA_STATE,  m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE]  );
	lua_setglobal( LUA_STATE, "XBOX_PAD_WHITE_BUTTON_HELD" );

	lua_pushboolean( LUA_STATE,  m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  );
	lua_setglobal( LUA_STATE, "XBOX_PAD_LEFT_TRIGGER_HELD" );

	lua_pushboolean( LUA_STATE,  m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]  );
	lua_setglobal( LUA_STATE, "XBOX_PAD_RIGHT_TRIGGER_HELD" );

	// iR Remote Control LUA
	lua_pushboolean( LUA_STATE, g_biR_Up );
	lua_setglobal( LUA_STATE, "XBOX_IR_UP" );

	lua_pushboolean( LUA_STATE, g_biR_Down );
	lua_setglobal( LUA_STATE, "XBOX_IR_DOWN" );

	lua_pushboolean( LUA_STATE, g_biR_Left );
	lua_setglobal( LUA_STATE, "XBOX_IR_LEFT" );

	lua_pushboolean( LUA_STATE, g_biR_Right );
	lua_setglobal( LUA_STATE, "XBOX_IR_RIGHT" );

	lua_pushboolean( LUA_STATE, g_biR_Select );
	lua_setglobal( LUA_STATE, "XBOX_IR_SELECT" );

	lua_pushboolean( LUA_STATE, g_biR_Display );
	lua_setglobal( LUA_STATE, "XBOX_IR_DISPLAY" );

	lua_pushboolean( LUA_STATE, g_biR_Reverse );
	lua_setglobal( LUA_STATE, "XBOX_IR_REVERSE" );

	lua_pushboolean( LUA_STATE, g_biR_Play );
	lua_setglobal( LUA_STATE, "XBOX_IR_PLAY" );

	lua_pushboolean( LUA_STATE, g_biR_Forward );
	lua_setglobal( LUA_STATE, "XBOX_IR_FORWARD" );

	lua_pushboolean( LUA_STATE, g_biR_DSkip );
	lua_setglobal( LUA_STATE, "XBOX_IR_SKIP_MINUS" );

	lua_pushboolean( LUA_STATE, g_biR_Stop );
	lua_setglobal( LUA_STATE, "XBOX_IR_STOP" );

	lua_pushboolean( LUA_STATE, g_biR_Pause );
	lua_setglobal( LUA_STATE, "XBOX_IR_PAUSE" );

	lua_pushboolean( LUA_STATE, g_biR_USkip );
	lua_setglobal( LUA_STATE, "XBOX_IR_SKIP_PLUS" );

	lua_pushboolean( LUA_STATE, g_biR_Title );
	lua_setglobal( LUA_STATE, "XBOX_IR_TITLE" );

	lua_pushboolean( LUA_STATE, g_biR_Info );
	lua_setglobal( LUA_STATE, "XBOX_IR_INFO" );

	lua_pushboolean( LUA_STATE, g_biR_Menu );
	lua_setglobal( LUA_STATE, "XBOX_IR_MENU" );

	lua_pushboolean( LUA_STATE, g_biR_Back );
	lua_setglobal( LUA_STATE, "XBOX_IR_BACK" );

	lua_pushboolean( LUA_STATE, g_biR_0 );
	lua_setglobal( LUA_STATE, "XBOX_IR_0" );

	lua_pushboolean( LUA_STATE, g_biR_1 );
	lua_setglobal( LUA_STATE, "XBOX_IR_1" );

	lua_pushboolean( LUA_STATE, g_biR_2 );
	lua_setglobal( LUA_STATE, "XBOX_IR_2" );

	lua_pushboolean( LUA_STATE, g_biR_3 );
	lua_setglobal( LUA_STATE, "XBOX_IR_3" );

	lua_pushboolean( LUA_STATE, g_biR_4 );
	lua_setglobal( LUA_STATE, "XBOX_IR_4" );

	lua_pushboolean( LUA_STATE, g_biR_5 );
	lua_setglobal( LUA_STATE, "XBOX_IR_5" );

	lua_pushboolean( LUA_STATE, g_biR_6 );
	lua_setglobal( LUA_STATE, "XBOX_IR_6" );

	lua_pushboolean( LUA_STATE, g_biR_7 );
	lua_setglobal( LUA_STATE, "XBOX_IR_7" );

	lua_pushboolean( LUA_STATE, g_biR_8 );
	lua_setglobal( LUA_STATE, "XBOX_IR_8" );

	lua_pushboolean( LUA_STATE, g_biR_9 );
	lua_setglobal( LUA_STATE, "XBOX_IR_9" );

	if (m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_RIGHT_THUMB || myRemoteCtrl.m_DefaultIR_Remote.wButtons == XINPUT_IR_REMOTE_INFO)
	{
		//myScreenShot.Take();
		// X3 LCD
		myDash.x3Settings.xLCD.enabled = true;
		myDash.x3Settings.enabled = true;
		CreateThread(0, 0, &XLCDSysInfoThread, NULL, 0, 0);
	}

#ifdef _DEBUG // load calling dash, only for debug builds
	if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0 )
	{
		if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 0 )
		{
			if( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_START )
			{
				if( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_BACK )
				{
					XLaunchNewImage( NULL, NULL );
				}
			}
		}
	}
#endif
}