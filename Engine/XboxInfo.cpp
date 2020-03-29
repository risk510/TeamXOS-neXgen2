#include "XboxInfo.h"

// XBOX Version
void XI_GetEncoder( char* szBuffer )
{
	int iType;

	// Connexant?
	if( HalReadSMBusValue( 0x8a, 0x00, 0, (LPBYTE)&iType ) == 0 )
	{
		sprintf( szBuffer, "Connexant" );
	}

	// Focus?
	else if( HalReadSMBusValue( 0xd4, 0x00, 0, (LPBYTE)&iType ) == 0 )
	{
		sprintf( szBuffer, "Focus" );
	}

	// XCalibur?
	else if( HalReadSMBusValue( 0xe0, 0x00, 0, (LPBYTE)&iType ) == 0 )
	{
		sprintf( szBuffer, "Xcalibur" );
	}
	
	// Unknown?
	else
	{
		sprintf( szBuffer, "Unknown" );
	}
}

// XBOX Encoder
void XI_GetVersion( char* szBuffer )
{
	char szVer[50];
	HalReadSMBusValue( 0x20, 0x01, 0, (LPBYTE)&szVer[0]);
	HalReadSMBusValue( 0x20, 0x01, 0, (LPBYTE)&szVer[1]);
	HalReadSMBusValue( 0x20, 0x01, 0, (LPBYTE)&szVer[2]);
	szVer[3] = '\0';

	// Green Debug Kit
	if( strcmp( szVer, "DBG" ) == NULL )
	{
		sprintf( szBuffer, "Debug Green" );
	}

	// Devkit
	else if( strcmp( szVer, "01D" ) == NULL )
	{
		sprintf( szBuffer, "Devkit" );
	}
	else if( strcmp( szVer, "D01" ) == NULL )
	{
		sprintf( szBuffer, "Devkit" );
	}
	else if( strcmp( szVer, "1D0" ) == NULL )
	{
		sprintf( szBuffer, "Devkit" );
	}
	else if( strcmp( szVer, "0D1" ) == NULL )
	{
		sprintf( szBuffer, "Devkit" );
	}

	// 1.0
	else if( strcmp( szVer, "P01" ) == NULL )
	{
		sprintf( szBuffer, "1.0" );
	}

	// 1.1
	else if( strcmp( szVer, "P05" ) == NULL )
	{
		sprintf( szBuffer, "1.1" );
	}

	// 1.3 & 1.4
	else if( strcmp( szVer, "P11" ) == NULL )
	{
		// 1.4
		sprintf( szBuffer, "1.4" );

		// 1.3
		char szEncoder[25];
		XI_GetEncoder( szEncoder );
		if( strcmp( szEncoder, "Connexant" ) == NULL )
		{
			sprintf( szBuffer, "1.2 / 1.3" );
		}

	}

	// 1.6
	else if( strcmp( szVer, "P2L" ) == NULL )
	{
		sprintf( szBuffer, "1.6" );
	}

	// Unknown
	else
	{
		sprintf( szBuffer, "Unknown" );
	}
}

// Get Full Path Of Running App
void XI_GetProgramFullPath( char* szBuffer )
{
	// Vars
	PANSI_STRING pTemp;

	// Get Info
	pTemp = (PANSI_STRING)XeImageFileName;
	
	// Full Path
	sprintf( szBuffer, pTemp->Buffer );
	szBuffer[ pTemp->Length ] = '\0';
}

// Get Program Path - No .xbe
void XI_GetProgramPath( char* szBuffer )
{
	// Vars
	char *p;
	PANSI_STRING pTemp;

	// Get Info
	pTemp = (PANSI_STRING)XeImageFileName;
	
	// Full Path
	sprintf( szBuffer, pTemp->Buffer );
	szBuffer[ pTemp->Length ] = '\0';

	// Remove XBE
	if( (p = strrchr( szBuffer, '\\' )) != NULL )
	{ 
		*p = '\0';
	}
}

// Get Device of Running App
void XI_GetProgramDevice( char* szBuffer )
{
	// Vars
	PANSI_STRING pTemp;

	// Get Info
	pTemp = (PANSI_STRING)XeImageFileName;
	
	// Full Path
	sprintf( szBuffer, pTemp->Buffer );
	szBuffer[ pTemp->Length ] = '\0';

	// Device & Folder
	if(		( szBuffer[0] == '\\' ) &&
			( ( szBuffer[1] == 'd' ) || ( szBuffer[1] == 'D' ) ) &&
			( ( szBuffer[2] == 'e' ) || ( szBuffer[2] == 'E' ) ) &&
			( ( szBuffer[3] == 'v' ) || ( szBuffer[3] == 'V' ) ) &&
			( ( szBuffer[4] == 'i' ) || ( szBuffer[4] == 'I' ) ) &&
			( ( szBuffer[5] == 'c' ) || ( szBuffer[5] == 'C' ) ) &&
			( ( szBuffer[6] == 'e' ) || ( szBuffer[6] == 'E' ) ) &&
			( szBuffer[7] == '\\' ) &&
			( ( ( szBuffer[8] == 'c' ) || ( szBuffer[8] == 'C' ) ) ||
			( ( szBuffer[8] == 'h' ) || ( szBuffer[8] == 'H' ) ) ) )
	{
		// HD ?
		if( ( szBuffer[8] == 'h' ) || ( szBuffer[8] == 'H' ) )
		{
			// Device
			szBuffer[28] = '\0';
		}

		// DVD
		else if( ( szBuffer[8] == 'c' ) || ( szBuffer[8] == 'C' ) )
		{
			// Device
			szBuffer[14] = '\0';
		}
	}
}

// Get Folder of Running App
void XI_GetProgramFolder( char* szBuffer )
{
	// Vars
	char *p;
	PANSI_STRING pTemp;

	// Get Info
	pTemp = (PANSI_STRING)XeImageFileName;
	
	// Full Path
	sprintf( szBuffer, pTemp->Buffer );
	szBuffer[ pTemp->Length ] = '\0';

	// Remove XBE
	if( (p = strrchr( szBuffer, '\\' )) != NULL )
	{ 
		*p = '\0';
	}

	// Device & Folder
	if(		( szBuffer[0] == '\\' ) &&
			( ( szBuffer[1] == 'd' ) || ( szBuffer[1] == 'D' ) ) &&
			( ( szBuffer[2] == 'e' ) || ( szBuffer[2] == 'E' ) ) &&
			( ( szBuffer[3] == 'v' ) || ( szBuffer[3] == 'V' ) ) &&
			( ( szBuffer[4] == 'i' ) || ( szBuffer[4] == 'I' ) ) &&
			( ( szBuffer[5] == 'c' ) || ( szBuffer[5] == 'C' ) ) &&
			( ( szBuffer[6] == 'e' ) || ( szBuffer[6] == 'E' ) ) &&
			( szBuffer[7] == '\\' ) &&
			( ( ( szBuffer[8] == 'c' ) || ( szBuffer[8] == 'C' ) ) ||
			( ( szBuffer[8] == 'h' ) || ( szBuffer[8] == 'H' ) ) ) )
	{
		// HD ?
		if( ( szBuffer[8] == 'h' ) || ( szBuffer[8] == 'H' ) )
		{
			//Folder
			sprintf( szBuffer, &szBuffer[28] );
		}

		// DVD
		else if( ( szBuffer[8] == 'c' ) || ( szBuffer[8] == 'C' ) )
		{
			// Folder
			sprintf( szBuffer, &szBuffer[14] );
		}
	}
}

// Get XBE of Running App
void XI_GetProgramXBE( char* szBuffer )
{
	// Vars
	char *p;
	PANSI_STRING pTemp;

	// Get Info
	pTemp = (PANSI_STRING)XeImageFileName;
	
	// Full Path
	sprintf( szBuffer, pTemp->Buffer );
	szBuffer[ pTemp->Length ] = '\0';

	// XBE
	if( (p = strrchr( szBuffer, '\\' )) != NULL )
	{
		sprintf( szBuffer, &p[1] );
	}
}