/*

	XbFTP.h


	Simple Wrapper Functions For XbFileZilla.
	Makes Things A Little Easier To Get Basic FTP Up & Running!


	g0at3r

*/

#ifndef __XBFTP_H__
#define __XBFTP_H__


#include <xtl.h>
#include <tchar.h>
#include "XbFileZilla.h"
#include "..\\XKUtils\\XKUtils.h"
#include "..\\XKUtils\\XKExports.h"
#include "xbhttpdownloader.h"

#ifdef _DEBUG
#pragma	comment(linker, "/NODEFAULTLIB:LIBCD.lib")
#pragma comment(lib, "xbfz/XbFileZillaD.lib")
#pragma comment(lib, "xnetd.lib")
#else
#pragma	comment(linker, "/NODEFAULTLIB:LIBC.lib")
#pragma comment(lib, "xbfz/XbFileZilla.lib")
#pragma comment(lib, "xnet.lib")
#endif



// Structure
struct sFTPServer {
	int		Port;
	int		MaxLogins;
	char	WelcomeMessage[100];
};

// Generic network stuff
// Init Network
bool Network_Init( const char* IpAddress, const char* SubnetMask, const char* DefaultGateway,
					  const char* Dns1, const char* Dns2, bool StaticIP );
bool Network_Shutdown();

BOOL IsEthernetConnected();

//FTP Stuff

// Config FTP
void Network_FTP_Config( sFTPServer ServerSettings );
// Start & Stop FTP Server
bool Network_FTP_StartServer( void );
bool Network_FTP_StopServer( void );
// Add User Account
bool Network_FTP_AddUser( char* Name, char* Password );
#endif