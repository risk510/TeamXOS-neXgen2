#ifndef __LUACALLS_H__
#define __LUACALLS_H__



#include <xtl.h>
#include <string>
using namespace std;
#include "..\\Main.h"
#include "..\\Utils.h"
#include "..\\Menu.h"
#include "..\\XKUtils\\XKUtils.h"
#include "..\\TinyXml\\XMLLoader.h"
#include "..\\Drivestatus\\drivestatus.h"
#include "..\\FTPClient\ftpclient.h"


// LUA Lib
extern "C" {
	#include "liblua\\lualib.h"
	#include "liblua\\lauxlib.h"
};


// Globals
extern lua_State*	LUA_STATE;			// Main Secne State
extern string		LUA_SCENE_BUFFER;		// Scene Buffer

extern MenuList Menu[9]; // Number of Menu Items
//extern class CXboxInfo Xbox;
extern CXboxInfo XBOXDriveStatus;



void RegisterLUACalls( );						// Registers LUA Calls
void UpdateLUAVariables();						// Update The Variables In Script
void SetLUABuffer( const char* szFilename );	// Sets Current Scene
void RenderLUAScene( );							// Render Current Scene
void FrameMoveLUAScene( );						// FrameMove Current Scene
void StructReset();								// Structure Reset
void ResetTitleImages();						// Reset TitleImages Only


// The LUA Calls
int lua_SceneLoad( lua_State* L );		// Load New Scene

int lua_TrayOpen( lua_State* L );		// Open DVD Tray
int lua_TrayClose( lua_State* L );		// Close DVD Tray
int lua_XboxPoweroff( lua_State* L );	// XBOX Power Off
int lua_XboxReboot( lua_State* L );		// XBOX Reboot
int lua_XboxPowercycle( lua_State* L );	// XBOX Power Cycle

int lua_ClearScreen( lua_State* L );	// Clear The Screen
int lua_Present( lua_State* L );		// Present Whats Been Rendered

int lua_MeshLoad( lua_State* L );		// Load A New Mesh
int lua_MeshFree( lua_State* L );		// Free A Loaded Mesh
int lua_MeshPosition( lua_State* L );	// Set Position Of A Mesh
int lua_MeshScale( lua_State* L );		// Set Scale Of A Mesh
int lua_MeshRotate( lua_State* L );		// Rotate A Mesh
int lua_MeshRender( lua_State* L );		// Render The Mesh To Screen

int lua_CameraSetFOV( lua_State* L );		// Sets Camera Field Of View
int lua_CameraSetAspect( lua_State* L);		// Set Cameras Aspect Ratio
int lua_CameraSetPosition( lua_State* L );	// Set Position of Camera
int lua_CameraSetLookAt( lua_State* L ); // Set point camera is looking at
int lua_CameraMoveForward( lua_State* L );
int lua_CameraMoveBackward( lua_State* L );
int lua_CameraMoveLeft( lua_State* L );
int lua_CameraMoveRight( lua_State* L );

int lua_FontLoad( lua_State* L );		// Load A Font
int lua_FontFree( lua_State* L );		// Free A Loaded Font
int lua_FontColour( lua_State* L );		// Set Font Colour
int lua_FontDrawText( lua_State* L );	// Draw Text
int lua_FontPosition( lua_State* L );	// Set Text Position

int lua_TextureLoad( lua_State* L );	// Load A New Texture
int lua_TextureFree( lua_State* L );	// Free A Loaded Texture


int lua_FlakeLoad( lua_State* L );		// Load A Snow Flake
/*int lua_FlakeFree( lua_State* L );		// Free A Loaded Snow Flake*/
int lua_FlakeSetBoundaries( lua_State* L );	// SetBoundaries for Snow Flake
int lua_FlakeSpeed( lua_State* L );		// Speed of falling Snow Flake
int lua_FlakeTotal( lua_State* L );		// Total Number of falling Snow Flakes
int lua_FlakeScale( lua_State* L );		// Scale Snow Flakes
int lua_FlakeRotateSpeed( lua_State* L );	// Snow Flakes Rotation*/
int lua_FlakeRender( lua_State* L );	// Render Snow Flakes


int lua_WaveLoad( lua_State* L );		// Load A New Wave File
int lua_WaveFree( lua_State* L );		// Free A Loaded Wave File
int lua_WavePlay( lua_State* L );		// Play Selected Wave File

int lua_XrayEnable( lua_State* L );		// Enable Xray
int lua_XrayDisable( lua_State* L );	// Disable Xray

int lua_AlphaEnable( lua_State* L );		// Enable Alpha
int lua_AlphaDisable( lua_State* L );	// Disable Alpha

int lua_FogRender( lua_State* L );		// Render Fog
int lua_FogColour( lua_State* L );		// Fog Colour

int lua_StringMerge( lua_State* L );	// Merge Strings Into One

int lua_getmenuitems( lua_State* L );		// Get Total Number of Menu Items
int lua_getmenuitemname( lua_State* L );	// Get Name of Menu Items
int lua_getitemrating(lua_State *L);		// Get Item Rating
int lua_getitemregion(lua_State *L);		// Get Item Region
int lua_getitemfilename(lua_State *L);		// Get Item Filename
int lua_getitemiconpath(lua_State *L);		// Get Item Icon Path
int lua_getitemdblocks(lua_State *L);		// Get Item DBlock Size
int lua_getitemtype(lua_State *L);			// Get Item Type.. (Action) etc..

int lua_DrawTexturedQuad( lua_State* L );	// Draw A Textured Quad
int lua_DrawTexturedQuadAlpha( lua_State* L ); // Draw A textured quad with a alpha level

int lua_LaunchXBE(lua_State *L);			// Launche XBE
int lua_FormatDrive(lua_State *L);			// Format Drive Partition

int lua_SetSkin( lua_State* L );			// Set Default Skin
int lua_SaveConfig( lua_State* L );			// Save Configuration Changes

int lua_LockHDD( lua_State* L );			// Lock HDD
int lua_UnlockHDD( lua_State* L );			// UnlockHDD

int lua_AutoUpdate( lua_State* L );			// AutoUpdate
int lua_ClearCache( lua_State* L );			// ClearCache

// Xecuter 3 Support
int lua_SetLCDText (lua_State* L);			// X3 XLCD SetLCDText

// FTPClient
int lua_ftpopen (lua_State* L);
int lua_ftpclose (lua_State* L);
int lua_ftplist (lua_State* L);
int lua_ftpcd (lua_State* L);
int lua_ftpsetuser (lua_State* L);
int lua_ftpsetpass (lua_State* L);
int lua_ftplcd (lua_State* L);
int lua_ftpput (lua_State* L);
int lua_ftpget (lua_State* L);
int lua_ftplls (lua_State* L);
int lua_ftpbinary (lua_State* L);
int lua_ftpascii (lua_State* L);
int lua_ftppwd (lua_State* L);


#endif