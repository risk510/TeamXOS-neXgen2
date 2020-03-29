// IoSupport.cpp: implementation of the CIoSupport class.
//
//////////////////////////////////////////////////////////////////////

#include "IoSupport.h"
#include "Undocumented.h"

#include <stdio.h>

#define CTLCODE(DeviceType, Function, Method, Access) ( ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method)  ) 
#define FSCTL_DISMOUNT_VOLUME  CTLCODE( FILE_DEVICE_FILE_SYSTEM, 0x08, METHOD_BUFFERED, FILE_ANY_ACCESS )

typedef struct 
{
	char	szDriveLetter;
	char* szDevice;
	int   iPartition;
} stDriveMapping;

stDriveMapping driveMapping[]=
{
	{ 'C', "Harddisk0\\Partition2",2},
	{ 'D', "Cdrom0",-1},
	{ 'E', "Harddisk0\\Partition1",1},
	{ 'F', "Harddisk0\\Partition6",6},
	{ 'X', "Harddisk0\\Partition3",3},
	{ 'Y', "Harddisk0\\Partition4",4},
	{ 'Z', "Harddisk0\\Partition5",5},
	{ 'G', "Harddisk0\\Partition7",7},
};
#define NUM_OF_DRIVES ( sizeof( driveMapping) / sizeof( driveMapping[0] ) )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIoSupport::CIoSupport()
{
	m_dwLastTrayState=0;
}

CIoSupport::~CIoSupport()
{

}

// szDrive e.g. "D:"
// szDevice e.g. "Cdrom0" or "Harddisk0\Partition6"

HRESULT CIoSupport::Mount(CHAR* szDrive, CHAR* szDevice)
{
	CHAR szSourceDevice[48];
	CHAR szDestinationDrive[16];

	sprintf(szSourceDevice,"\\Device\\%s",szDevice);
	sprintf(szDestinationDrive,"\\??\\%s",szDrive);

	STRING DeviceName =
	{
		strlen(szSourceDevice),
		strlen(szSourceDevice) + 1,
		szSourceDevice
	};

	STRING LinkName =
	{
		strlen(szDestinationDrive),
		strlen(szDestinationDrive) + 1,
		szDestinationDrive
	};

	IoCreateSymbolicLink(&LinkName, &DeviceName);

	return S_OK;
}



// szDrive e.g. "D:"

HRESULT CIoSupport::Unmount(CHAR* szDrive)
{
	char szDestinationDrive[16];
	sprintf(szDestinationDrive,"\\??\\%s",szDrive);

	STRING LinkName =
	{
		strlen(szDestinationDrive),
		strlen(szDestinationDrive) + 1,
		szDestinationDrive
	};

	IoDeleteSymbolicLink(&LinkName);
	
	return S_OK;
}


VOID CIoSupport::GetPartition(LPCSTR strFilename, LPSTR strPartition)
{
	strcpy(strPartition,"");
	for (int i=0; i < NUM_OF_DRIVES; i++)
	{
		if ( toupper(strFilename[0]) == driveMapping[i].szDriveLetter)
		{
			strcpy(strPartition, driveMapping[i].szDevice);
			return;
		}
	}
}


HRESULT CIoSupport::Remount(CHAR* szDrive, CHAR* szDevice)
{
	CHAR szSourceDevice[48];
	sprintf(szSourceDevice,"\\Device\\%s",szDevice);

	Unmount(szDrive);
	
	ANSI_STRING filename;
	OBJECT_ATTRIBUTES attributes;
	IO_STATUS_BLOCK status;
	HANDLE hDevice;
	NTSTATUS error;
	DWORD dummy;

	RtlInitAnsiString(&filename, szSourceDevice);
	InitializeObjectAttributes(&attributes, &filename, OBJ_CASE_INSENSITIVE, NULL);

	if (!NT_SUCCESS(error = NtCreateFile(&hDevice, GENERIC_READ |
		SYNCHRONIZE | FILE_READ_ATTRIBUTES, &attributes, &status, NULL, 0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT)))
	{
		return E_FAIL;
	}

	if (!DeviceIoControl(hDevice, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &dummy, NULL))
	{
		CloseHandle(hDevice);
		return E_FAIL;
	}

	CloseHandle(hDevice);
	Mount(szDrive,szDevice);

	return S_OK;
}

HRESULT CIoSupport::Remap(CHAR* szMapping)
{
	char szMap[32];
	strcpy(szMap, szMapping );

	char* pComma = strstr(szMap,",");
	if (pComma)
	{
		*pComma = 0;
		
		// map device to drive letter
		Unmount(szMap);
		Mount(szMap,&pComma[1]);
		return S_OK;
	}

	return E_FAIL;
}


HRESULT CIoSupport::EjectTray()
{
	HalWriteSMBusValue(0x20, 0x0C, FALSE, 0);  // eject tray
	return S_OK;
}

HRESULT CIoSupport::CloseTray()
{
	HalWriteSMBusValue(0x20, 0x0C, FALSE, 1);  // close tray
	return S_OK;
}

DWORD CIoSupport::GetTrayState()
{
	HalReadSMCTrayState(&m_dwTrayState,&m_dwTrayCount);

	if(m_dwTrayState == TRAY_CLOSED_MEDIA_PRESENT) 
	{
		if (m_dwLastTrayState != TRAY_CLOSED_MEDIA_PRESENT)
		{
			m_dwLastTrayState = m_dwTrayState;
			return DRIVE_CLOSED_MEDIA_PRESENT;
		}
		else
		{
			return DRIVE_READY;
		}
	}
	else if(m_dwTrayState == TRAY_CLOSED_NO_MEDIA)
	{
		m_dwLastTrayState = m_dwTrayState;
		return DRIVE_CLOSED_NO_MEDIA;
	}
	else if(m_dwTrayState == TRAY_OPEN)
	{
		m_dwLastTrayState = m_dwTrayState;
		return DRIVE_OPEN;
	}
	else
	{
		m_dwLastTrayState = m_dwTrayState;
	}

	return DRIVE_NOT_READY;
}

HRESULT CIoSupport::Shutdown()
{
	HalInitiateShutdown();
	return S_OK;
}

HANDLE CIoSupport::CreateFile()
{
	ANSI_STRING filename;
	OBJECT_ATTRIBUTES attributes;
	IO_STATUS_BLOCK status;
	HANDLE hDevice;
	NTSTATUS error;

	RtlInitAnsiString(&filename,"\\Device\\Cdrom0");
	InitializeObjectAttributes(&attributes, &filename, OBJ_CASE_INSENSITIVE, NULL);

	if (!NT_SUCCESS(error = NtCreateFile(&hDevice, GENERIC_READ |
		SYNCHRONIZE | FILE_READ_ATTRIBUTES, &attributes, &status, NULL, 0,
		FILE_SHARE_READ, FILE_OPEN,	FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT)))
	{
		return NULL;
	}

	return hDevice;
}

BOOL CIoSupport::GetFirstFile(CHAR* szFilename)
{
	ANSI_STRING filename;
	OBJECT_ATTRIBUTES attributes;
	IO_STATUS_BLOCK status;
	HANDLE hDevice;
	NTSTATUS error;

	RtlInitAnsiString(&filename,"\\Device\\Cdrom0");
	InitializeObjectAttributes(&attributes, &filename, OBJ_CASE_INSENSITIVE, NULL);

	if (!NT_SUCCESS(error = NtCreateFile(&hDevice, GENERIC_READ |
		SYNCHRONIZE | FILE_READ_ATTRIBUTES, &attributes, &status, NULL, 0,
		FILE_SHARE_READ, FILE_OPEN,	FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT)))
	{
		OutputDebugString("Unable to open Cdrom0.\n");
		return FALSE;
	}


	CHAR* szBuffer = new CHAR[2048];
	DWORD dwRead = 0;

	SetFilePointer(hDevice, 19*2048, NULL, FILE_BEGIN);
	if (!ReadFile(hDevice,szBuffer,2048,&dwRead,NULL))
	{
		OutputDebugString("Unable to read ISO9660 root directory.\n");
		CloseHandle(hDevice);
		return FALSE;
	}

	CloseHandle(hDevice);
	szBuffer[2047] = 0;

	int offset = 0;
	while (szBuffer[offset]==0x22) offset+=0x22;
	offset+=33; // jump to start of filename

	strcpy(szFilename,"#");
	strcat(szFilename,&szBuffer[offset]);

	if (szBuffer)
		delete [] szBuffer;

	return TRUE;
}

