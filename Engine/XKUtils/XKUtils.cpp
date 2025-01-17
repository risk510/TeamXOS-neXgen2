/*
**********************************
**********************************
**      BROUGHT TO YOU BY:		**
**********************************
**********************************
**								**
**		  [TEAM ASSEMBLY]		**
**								**
**		www.team-assembly.com	**
**								**
******************************************************************************************************
* This is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
******************************************************************************************************


********************************************************************************************************
**	     XKUTILS.CPP - XBOX Utility Class' Implementation      
********************************************************************************************************
**
**	This Class encapsulates some XBOX utility functions and are mostly self explanatory
**	not a lot of commenting or documentation needed here...
**
********************************************************************************************************

********************************************************************************************************
**	CREDITS:
********************************************************************************************************
**	XBOX-LINUX TEAM:
**  ---------------
**		Wow, you guys are awsome !!  I bow down to your greatness !!  
**		REFERENCE URL:  http://xbox-linux.sourceforge.net
**
********************************************************************************************************

UPDATE LOG:
--------------------------------------------------------------------------------------------------------
Date: 02/18/2003
By: UNDEAD [team-assembly]
Reason: Prepared 0.2 for Public Release
--------------------------------------------------------------------------------------------------------

*/
#pragma once
#if defined (_XBOX)
	//This complete file is only supported for XBOX..


#include "XKUtils.h"

XKUtils::XKUtils()
{
}

XKUtils::~XKUtils()
{

}

LONG XKUtils::MountDevice(LPSTR sSymbolicLinkName, LPSTR sDeviceName)
{

	UNICODE_STRING 	deviceName;
	deviceName.Buffer  = sDeviceName;
	deviceName.Length = (USHORT)strlen(sDeviceName);
	deviceName.MaximumLength = (USHORT)strlen(sDeviceName) + 1;

	UNICODE_STRING 	symbolicLinkName;
	symbolicLinkName.Buffer  = sSymbolicLinkName;
	symbolicLinkName.Length = (USHORT)strlen(sSymbolicLinkName);
	symbolicLinkName.MaximumLength = (USHORT)strlen(sSymbolicLinkName) + 1;

	return IoCreateSymbolicLink(&symbolicLinkName, &deviceName);

}

LONG XKUtils::UnMountDevice(LPSTR sSymbolicLinkName)
{
	UNICODE_STRING 	symbolicLinkName;
	symbolicLinkName.Buffer  = sSymbolicLinkName;
	symbolicLinkName.Length = (USHORT)strlen(sSymbolicLinkName);
	symbolicLinkName.MaximumLength = (USHORT)strlen(sSymbolicLinkName) + 1;

	return IoDeleteSymbolicLink(&symbolicLinkName);

}

LONG XKUtils::MountDiskC()
{
	return MountDevice(DriveC, DeviceC);
}

LONG XKUtils::MountDiskD()
{

	return MountDevice(DriveD, CdRom);
}

LONG XKUtils::MountDiskE()
{
	return MountDevice(DriveE, DeviceE);
}

LONG XKUtils::MountDiskF()
{
	return MountDevice(DriveF, DeviceF);
}

LONG XKUtils::MountDiskG()
{
	return MountDevice(DriveG, DeviceG);
}

void XKUtils::MountAllDisks()
{
	UnMountAllDisks();

	MountDiskC();
	MountDiskD();
	MountDiskE();
	MountDiskF();
	MountDiskG();
}

LONG XKUtils::UnMountDiskC()
{
	return UnMountDevice(DriveC);
}

LONG XKUtils::UnMountDiskD()
{
	return UnMountDevice(DriveD);
}

LONG XKUtils::UnMountDiskE()
{
	return UnMountDevice(DriveE);
}

LONG XKUtils::UnMountDiskF()
{
	return UnMountDevice(DriveF);
}

LONG XKUtils::UnMountDiskG()
{
	return UnMountDevice(DriveG);
}

void XKUtils::UnMountAllDisks()
{
	UnMountDiskC();
	UnMountDiskD();
	UnMountDiskE();
	UnMountDiskF();
	UnMountDiskG();
}

void XKUtils::XBOXRebootToDash()
{
	XLaunchNewImage( NULL, NULL );
}

void XKUtils::XBOXReset()
{
	OUTPUT_DEBUG_STRING("XKUtils: Reset...\n");
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_POWER, 0, POWER_SUBCMD_RESET);
}

void XKUtils::SetXBOXLEDStatus(UCHAR LEDStatus)
{
	OUTPUT_DEBUG_STRING("XKUtils: Setting LED Status Registers...\n");
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_LED_REGISTER, 0, LEDStatus);
	Sleep(10);
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_LED_MODE, 0, 1);
}

void XKUtils::XBOXPowerOff()
{
	//Console Shutdown...
	OUTPUT_DEBUG_STRING("XKUtils: Shutdown...\n");
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_POWER, 0, POWER_SUBCMD_POWER_OFF);

}

void XKUtils::XBOXPowerCycle()
{
	//Console Shutdown...
	OUTPUT_DEBUG_STRING("XKUtils: Cycle...\n");
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_POWER, 0, POWER_SUBCMD_CYCLE);

}


/*  StartPos and EndPos are both ZERO offset based */
void XKUtils::WriteEEPROMToXBOX(LPBYTE EEPROMDATA, UCHAR STARTPOS, UCHAR ENDPOS)
{

	OUTPUT_DEBUG_STRING( "XKUtils: Writing EEPROM to XBOX...\n" );
 	for (UCHAR i=STARTPOS;i<ENDPOS;i++)
	{

		HalWriteSMBusValue(SMBDEV_EEPROM , i, 0, EEPROMDATA[i]);
		Sleep(5);
	}

}

/*  StartPos and EndPos are both ZERO offset based */
void XKUtils::ReadEEPROMFromXBOX(LPBYTE EEPROMDATA, UCHAR STARTPOS, UCHAR ENDPOS)
{
	ZeroMemory(EEPROMDATA, 256);
	
	OUTPUT_DEBUG_STRING( "XKUtils: Reading EEPROM from XBOX...\n" );
	for (UCHAR i=STARTPOS;i<ENDPOS;i++)
	{
		HalReadSMBusValue(SMBDEV_EEPROM, i, 0, EEPROMDATA+i);
		Sleep(1);

	}
	
}

void XKUtils::DVDDisableEjectReset()
{

	OUTPUT_DEBUG_STRING("XKUtils: Disable Reset on DVD Tray Eject...\n");
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_RESET_ON_EJECT, 0, RESET_ON_EJECT_SUBCMD_DISABLE);
	Sleep(1);
}

void XKUtils::DVDEnableEjectReset()
{

	OUTPUT_DEBUG_STRING("XKUtils: Enable Reset on DVD Tray Eject...\n");
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_RESET_ON_EJECT, 0, RESET_ON_EJECT_SUBCMD_ENABLE);
	Sleep(1);
}

void XKUtils::DVDEjectTray()
{
	OUTPUT_DEBUG_STRING("XKUtils: Ejecting DVD Tray...\n");
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_EJECT, 0, EJECT_SUBCMD_EJECT);
	Sleep(1);
}

void XKUtils::DVDLoadTray()
{
	OUTPUT_DEBUG_STRING("XKUtils: Loading DVD Tray...\n");
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_EJECT, 0, EJECT_SUBCMD_LOAD);
	Sleep(1);
}

// New Modifications SpOoK
int XKUtils::GetFanSpeed( void )
{
	int iFanSpeed = 0;
	int iTempSpeed;

	// read it
	HalReadSMBusValue( SMBDEV_PIC16L, 0x10, 0, (LPBYTE)&iTempSpeed );

	iFanSpeed = (int)iTempSpeed;
	return iFanSpeed;
}

void XKUtils::SetFanSpeed( int iFanSpeed )
{
	int Speed;
	Speed = iFanSpeed;

	// Safety check: so we never stop the fan or over run it ;)
	if (Speed < 5)
	{
		Speed = 5;
	}
	if (Speed > 80)
	{
		Speed = 80;
	}

	// write it
	HalWriteSMBusValue( SMBDEV_PIC16L, 0x06, 0, Speed);
	Sleep( 10 );
	// enable user specified speed
	HalWriteSMBusValue( SMBDEV_PIC16L, 0x05, 0, 1 );
}

bool XKUtils::GetCPUTemp(int &iTemperature, bool bFarenhight)
{
	HalReadSMBusValue(SMBDEV_PIC16L, 0x09, 0, (LPBYTE)&iTemperature);
	if (bFarenhight)
	{
		float fConvert = (float)iTemperature;
		fConvert *= 1.8f;
		fConvert += 32.5f;
		iTemperature = (int)fConvert;
	}
	return true;
}

bool XKUtils::GetSYSTemp(int &iTemperature, bool bFarenhight)
{
	HalReadSMBusValue(SMBDEV_PIC16L, 0x0a, 0, (LPBYTE)&iTemperature);
	if (bFarenhight)
	{
		float fConvert = (float)iTemperature;
		fConvert *= 1.8f;
		fConvert += 32.5f;
		iTemperature = (int)fConvert;
	}
	return true;
}

#endif