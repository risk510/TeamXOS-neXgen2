#include "MapPath.h"

// Paths
pathconv_s pathconv_table[] = {
//	{ "DVD:", "\\Device\\Cdrom0" },
	{ "D:", "\\Device\\Cdrom0" },

	{ "C:", "\\Device\\Harddisk0\\Partition2" },
	{ "E:", "\\Device\\Harddisk0\\Partition1" },
	{ "F:", "\\Device\\Harddisk0\\Partition6" },
	{ "G:", "\\Device\\Harddisk0\\Partition7" },
	{ "X:", "\\Device\\Harddisk0\\Partition3" },
	{ "Y:", "\\Device\\Harddisk0\\Partition4" },
	{ "Z:", "\\Device\\Harddisk0\\Partition5" },

	{ "SYSTEM:", "\\Device\\Harddisk0\\Partition2" },
	{ "STORAGE:", "\\Device\\Harddisk0\\Partition1" },
	{ "EXTENDED:", "\\Device\\Harddisk0\\Partition6" },
	{ "EXTENDED G:", "\\Device\\Harddisk0\\Partition7" },
	{ "TEMPX:", "\\Device\\Harddisk0\\Partition3" },
	{ "TEMPY:", "\\Device\\Harddisk0\\Partition4" },
	{ "TEMPZ:", "\\Device\\Harddisk0\\Partition5" },

	{ "BSYSTEM:", "\\Device\\Harddisk1\\Partition2" },
	{ "BSTORAGE:", "\\Device\\Harddisk1\\Partition1" },
	{ "BEXTENDED:", "\\Device\\Harddisk1\\Partition6" },
	{ "BEXTENDED G:", "\\Device\\Harddisk1\\Partition7" },
	{ "BTEMPX:", "\\Device\\Harddisk1\\Partition3" },
	{ "BTEMPY:", "\\Device\\Harddisk1\\Partition4" },
	{ "BTEMPZ:", "\\Device\\Harddisk1\\Partition5" },
	{ NULL, NULL }
};


char * getpath(char * DriveLetter) {
	int i;

	for(i=0; pathconv_table[i].DriveLetter!=NULL; i++) {
		if(!lstrcmpi(pathconv_table[i].DriveLetter,DriveLetter))
			return pathconv_table[i].FullPath;
	}
	return NULL;
}

HRESULT mappath(char * Device, char * DriveLetter) {
	char *tmp=NULL;

	if(DriveLetter[0]!='\\') {
		tmp=(char *)malloc(5+strlen(DriveLetter));
		sprintf(tmp, "\\??\\%s", DriveLetter);
		DriveLetter=tmp;
	}

	ANSI_STRING DeviceName = {
		strlen(Device),
		strlen(Device) + 1,
		Device
	};

	ANSI_STRING LinkName = {
		strlen(DriveLetter),
		strlen(DriveLetter) + 1,
		DriveLetter
	};

#if 0
	IoDeleteSymbolicLink(&LinkName);
	IoCreateSymbolicLink(&LinkName, &DeviceName);
#else
	char U, M;

	if(IoDeleteSymbolicLink(&LinkName))
		U='N';
	else
		U='Y';
	if(IoCreateSymbolicLink(&LinkName, &DeviceName))
		M='N';
	else
		M='Y';

	//DbgPrintf("Mounting %s on %s U:%c M:%c\n", Device, DriveLetter, U, M);
#endif

	if(tmp)
		free(tmp);

	return S_OK;
}

void resetpaths() {
	int i;

	for(i=0; pathconv_table[i].DriveLetter!=NULL; i++) {
		mappath(pathconv_table[i].FullPath , pathconv_table[i].DriveLetter);
	}
}

HRESULT formatdriveletter(WCHAR *DriveLetter) {
	int i;
	char *fp=NULL, *DriveLetterA=NULL;

	if(DriveLetter==NULL) return ERROR_INVALID_PARAMETER;
	DriveLetterA=(char *)malloc(lstrlenW(DriveLetter)+1);
	if(DriveLetterA==NULL) return ERROR_NOT_ENOUGH_MEMORY;
	sprintf(DriveLetterA, "%S", DriveLetter);

	for(i=0; pathconv_table[i].DriveLetter!=NULL; i++) {
		if(!lstrcmpi(pathconv_table[i].DriveLetter, DriveLetterA)) 
			fp=pathconv_table[i].FullPath;
	}

	if(fp!=NULL) {
		OutputDebugString("Formatting ");
		OutputDebugString(DriveLetterA);
		OutputDebugString(" (");
		OutputDebugString(fp);
		OutputDebugString(") ");
		
		ANSI_STRING DevicePath = {
			strlen(fp),
			strlen(fp) + 1,
			fp
		};

		XapiFormatFATVolume(&DevicePath);
	} else {
		OutputDebugString("Format: Unable to map ");
		OutputDebugString(DriveLetterA);
		OutputDebugString(" to path\n");
	}

	free(DriveLetterA);

	return S_OK;
}
