// neXgen AutoUpdate
#include "AutoUpdate.h"


bool bAutoUpdateComplete = false;

bool get_updates()
{
	string sUpdated_File_MD5;
	string sLocal_File_MD5;

	sUpdated_File_MD5 = "";
	sLocal_File_MD5 = "";

	DWORD dwRead=0;

	char szMD5_UpdateBuffer[256]="";
	char szDashName[255];
	char szOriginalDash[255];
	
	char szOutputFile[255];

	char*  HOSTURL= "www.nexgen.no-ip.com";
	char*  MD5_URL_PATH= "/updates/nexgen_md5.dat";
	char*  UPDATE_URL_PATH= "/updates/nexgen.zip";
	char*  MD5_SAVEFILE= "z:\\nexgen_md5.dat";
	char*  UPDATE_SAVEFILE= "z:\\nexgen.zip";

	/* ToDo: 
		1) Rar support
		2) Get final update server url for AutoUpdates.
		3) Fix any nasty bugs in this clunkster code :P
		4) Get Skin extraction support. Sub folders are shitty atm needs fixing.
	*/
	lua_pushstring( LUA_STATE, "Checking for Updates..." );
	lua_setglobal( LUA_STATE, "AUTOUPDATE_CHECK1" );

	bool bDownload_AutoUpdateCheck = HTTPDownloadToFile(HOSTURL,MD5_URL_PATH,MD5_SAVEFILE);
	if (bDownload_AutoUpdateCheck)
	{
		// Get Dashboards Current Name
		XI_GetProgramXBE(szDashName);
		sprintf(szOriginalDash, "SYSTEM:\\%s", szDashName);

		// Calculate MD5 of existing local file(s)
		if ( myMD5.CalcFile( szOriginalDash ) )
		{
			sLocal_File_MD5 = myMD5.GetMD5String();
			OutputDebugString(sLocal_File_MD5.c_str());
			OutputDebugString("\n");
		}

		// Read MD5 from our downloaded file. Quicker than downloading then checking files.
		HANDLE hMD5File = CreateFile(MD5_SAVEFILE, GENERIC_READ, FILE_SHARE_READ,
                   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		ZeroMemory(szMD5_UpdateBuffer,256);
		BOOL bReadMD5file = ReadFile( hMD5File, &szMD5_UpdateBuffer, 0x100, &dwRead, NULL);

		OutputDebugString(szMD5_UpdateBuffer);

		// Set Updated MD5 String to value we just downloaded
		sUpdated_File_MD5 = szMD5_UpdateBuffer;

		// Compare MD5 Checksums - See if new update(s) are available
		if (strcmp(sLocal_File_MD5.c_str(), sUpdated_File_MD5.c_str()))
		{
			// MD5 MissMatch - Lets Start some updating
			OutputDebugString("MD5 MissMatch - Going to Update File\n");
			lua_pushstring( LUA_STATE, "Downloading Update Please Wait..." );
			lua_setglobal( LUA_STATE, "AUTOUPDATE_CHECK2" );

			bool bDownload_Update = HTTPDownloadToFile(HOSTURL,UPDATE_URL_PATH,UPDATE_SAVEFILE);
			if (bDownload_Update)
			{
				myUnZip.unZipTo(UPDATE_SAVEFILE, ".xbe", "z:");
				
				sprintf(szOutputFile, "SYSTEM:\\%s", szDashName);

				// Delete Existing XBE
				DeleteFile(szOutputFile);

				// Got the updated file(s) now lets do something with them
				MoveFileEx(myUnZip.m_szExtractedFilePath.c_str(), szOutputFile, MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING);

				lua_pushstring( LUA_STATE, "Update Complete." );
				lua_setglobal( LUA_STATE, "AUTOUPDATE_CHECK3" );

				lua_pushstring( LUA_STATE, "Restart to Complete Update." );
				lua_setglobal( LUA_STATE, "AUTOUPDATE_CHECK4" );

				// Delete downloaded file(s), just to save a little space :P
				DeleteFile(UPDATE_SAVEFILE);

			} else {
				lua_pushstring( LUA_STATE, "AutoUpdate: Failed!" );
				lua_setglobal( LUA_STATE, "AUTOUPDATE_CHECK3" );
			}
		}
		else {
			// Match
			OutputDebugString("MD5 Match - Deleting Downloaded File\n");
			lua_pushstring( LUA_STATE, "No New Updates Available." );
			lua_setglobal( LUA_STATE, "AUTOUPDATE_CHECK2" );
		}

		// Close any File Handle's we have used
		CloseHandle(hMD5File);
		DeleteFile(MD5_SAVEFILE);
	}
	else {
		lua_pushstring( LUA_STATE, "AutoUpdate: Failed!" );
		lua_setglobal( LUA_STATE, "AUTOUPDATE_CHECK1" );
	}
	return true;
}