#ifndef __THREADS_H__
#define __THREADS_H__



#include <xtl.h>
#include "Main.h"
#include "Lock_Unlock.h"



// Thread For Music Updates
DWORD __stdcall MusicThread( void* pData );

// HDD Lock / Unlock Threads
//DWORD __stdcall LockHDDThread( void* pData );
//DWORD __stdcall UnLockHDDThread( void* pData );
//DWORD WINAPI LockHDDThread(LPVOID string);
DWORD WINAPI LockHDDThread( LPVOID lpParam );
DWORD WINAPI UnLockHDDThread(LPVOID string);

DWORD __stdcall RSSNewsThread( void* pData );

DWORD __stdcall AutoUpdateThread( void* pData );

DWORD __stdcall Init_NetworkThread( void* pData );

// X3 XLCD
DWORD __stdcall XLCDThread( void* pData);
DWORD __stdcall XLCDSysInfoThread( void* pData );



#endif