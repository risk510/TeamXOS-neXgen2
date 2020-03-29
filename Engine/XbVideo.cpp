#include "XbVideo.h"

F_COUNTRY GetAutoPatchCountry(char* xbefile) {

	DWORD baseaddr, certaddr ,region;
	HANDLE xbefd=CreateFile(xbefile, GENERIC_READ, FILE_SHARE_READ, NULL,
OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if(xbefd!=INVALID_HANDLE_VALUE) {
		DWORD readin;
		
		SetFilePointer(xbefd, 0x104, NULL, FILE_BEGIN);
		ReadFile(xbefd, &baseaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, 0x118, NULL, FILE_BEGIN);
		ReadFile(xbefd, &certaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0xa0, NULL, FILE_BEGIN);
		ReadFile(xbefd, &region, 0x4, &readin, NULL);

		CloseHandle(xbefd);

		
		switch (region) {
			case 0: 
				return COUNTRY_NULL;
				break;
			case 1: 
				return COUNTRY_USA; 
				break;
			case 2: 
				return COUNTRY_JAP; 
				break;
			case 4: 
				return COUNTRY_EUR; 
				break;
			default: 
				return COUNTRY_NULL; 
				break;
		}
	}
	return COUNTRY_NULL;
}

F_VIDEO GetAutoPatchVideo(char* xbefile) {

	DWORD baseaddr, certaddr ,region;
	HANDLE xbefd=CreateFile(xbefile, GENERIC_READ, FILE_SHARE_READ, NULL,
OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if(xbefd!=INVALID_HANDLE_VALUE) {
		DWORD readin;
		
		SetFilePointer(xbefd, 0x104, NULL, FILE_BEGIN);
		ReadFile(xbefd, &baseaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, 0x118, NULL, FILE_BEGIN);
		ReadFile(xbefd, &certaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0xa0, NULL, FILE_BEGIN);
		ReadFile(xbefd, &region, 0x4, &readin, NULL);

		CloseHandle(xbefd);

		
		switch (region) {
			case 0: 
				return VIDEO_NULL;
				break;
			case 1: 
				return VIDEO_NTSCM; 
				break;
			case 2: 
				return VIDEO_NTSCJ; 
				break;
			case 4: 
				return VIDEO_PAL50; 
				break;
			default: 
				return VIDEO_NULL; 
				break;
		}
	}
	return VIDEO_NULL;
}

bool PatchCountryVideo(F_COUNTRY Country, F_VIDEO Video)
{
	BYTE	*Kernel = (BYTE *)0x80010000;
	DWORD	i, j;
	DWORD	*CountryPtr;
	BYTE	CountryValues[4] = {0, 1, 2, 4};
	BYTE	VideoTyValues[5] = {0, 1, 2, 3, 3};
	BYTE	VideoFrValues[5] = {0x00, 0x40, 0x40, 0x80, 0x40};

	if( (Country == 0) && (Video == 0) )
	{
		return false;
	}

	if( Country == 0 )
	{
		Country = COUNTRY_EUR;

		if( Video == VIDEO_NTSCM )
		{
			Country = COUNTRY_USA;
		}

		if( Video == VIDEO_NTSCJ )
		{
			Country = COUNTRY_JAP;
		}
	}

	if( Video == 0 )
	{
		Video = VIDEO_PAL50;

		if( Country == COUNTRY_USA )
		{
			Video = VIDEO_NTSCM;
		}

		if( Country == COUNTRY_JAP )
		{
			Video = VIDEO_NTSCJ;
		}
	}

	for( i = 0x1000; i < 0x14000; i++ )
	{
		if( Kernel[i] != OriginalData[0] )
		{
			continue;
		}

		for( j = 0; j < 57; j++ )
		{
			if( Kernel[i+j] != OriginalData[j] )
			{
				break;
			}
		}

		if( j == 57 )
		{
			break;
		}
	}

	if( j == 57 )
	{
		j = (Kernel[i+57]) + (Kernel[i+58] << 8) + (Kernel[i+59] << 16) + (Kernel[i+60] << 24);
		CountryPtr = (DWORD *)j;
	}

	else
	{
		for( i = 0x1000; i < 0x14000; i++ )
		{
			if( Kernel[i] != PatchData[0] )
			{
				continue;
			}

			for( j =0 ; j < 25; j++ )
			{
				if( Kernel[i+j] != PatchData[j] )
				{
					break;
				}
			}

			if( j == 25 )
			{
				break;
			}
		}

		if( j == 25 )
		{
			j = (Kernel[i+66]) + (Kernel[i+67] << 8) + (Kernel[i+68] << 16) + (Kernel[i+69] << 24);
			CountryPtr = (DWORD *)j;
		}

		else
		{
			return false;
		}
	}

	j = MmQueryAddressProtect(&Kernel[i]);
	MmSetAddressProtect(&Kernel[i], 70, PAGE_READWRITE);
	memcpy(&Kernel[i], &PatchData[0], 70);

	*CountryPtr = (DWORD)CountryValues[Country];
	Kernel[i+0x1f] = CountryValues[Country];
	Kernel[i+0x19] = VideoTyValues[Video];
	Kernel[i+0x1a] = VideoFrValues[Video];

	j = (DWORD)CountryPtr;
	Kernel[i+66] = (BYTE)(j & 0xff);
	Kernel[i+67] = (BYTE)((j >> 8) & 0xff);
	Kernel[i+68] = (BYTE)((j >> 16) & 0xff);
	Kernel[i+69] = (BYTE)((j >> 24) & 0xff);
	MmSetAddressProtect(&Kernel[i], 70, j);

	return true;
}