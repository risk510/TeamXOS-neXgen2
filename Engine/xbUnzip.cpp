// XBOX Unzip CLass
#include "xbUnzip.h"

CXBUnzip myUnZip;

// Constructor
CXBUnzip::CXBUnzip()
{
}

// Destructor
CXBUnzip::~CXBUnzip()
{
}

bool CXBUnzip::unZipTo(char *FilePath, char *Filter, char *Destination){
	const string m_szFilename = FilePath;

	// load the file if it's a zip
	if (m_szFilename.substr(m_szFilename.length() - 4) == ".zip")
	{
		int retVal;
		unzFile	fp;
		fp = unzOpen(m_szFilename.c_str());

		retVal = unzGoToFirstFile(fp);

		if (retVal != UNZ_OK)
		{
			unzClose(fp);
			return false;
		}

		do
		{
			unz_file_info fileInfo;
			char szUnzFileName[_MAX_FNAME];
			
			retVal = unzGetCurrentFileInfo(fp, &fileInfo, szUnzFileName, _MAX_FNAME, NULL, 0, NULL, 0);

			if (retVal != UNZ_OK)
			{
				unzClose(fp);
				return false;
			}

			string szFn(szUnzFileName);

			// find a valid file in the zip file
			if (szFn.substr(szFn.length() - 4) == Filter)
			/*||	szFn.substr(szFn.length() - 4) == ".xml"
			||	szFn.substr(szFn.length() - 4) == ".lua"
			||	szFn.substr(szFn.length() - 4) == ".skin")*/
			{
				retVal = unzOpenCurrentFile(fp);
				//unzOpenCurrentFilePassword(zip, password);

				if (retVal != UNZ_OK)
				{
					unzClose(fp);
					return false;
				}
				
				// create a buffer big enough to hold uncompressed file in memory
				void *buffer = malloc( fileInfo.uncompressed_size );

				if( !buffer )
				{
					// Out of Memory
					unzCloseCurrentFile( fp );
					unzClose( fp );

					return false;
				}

				// load into memory
				unzReadCurrentFile( fp, buffer, fileInfo.uncompressed_size );

				sprintf(szExtractedFile, "%s\\%s", Destination, szUnzFileName);

				// Write the file file.
				HANDLE unzipedfile=CreateFile( szExtractedFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL );
				DWORD dwWritten=0;
				WriteFile( unzipedfile, buffer, fileInfo.uncompressed_size, &dwWritten, NULL );
				m_szExtractedFilePath = szExtractedFile;
				CloseHandle(unzipedfile);
  
				unzCloseCurrentFile( fp );

				free(buffer);
				break;
			}
		} while (unzGoToNextFile(fp) == UNZ_OK);

		unzClose(fp);
		return true;
	}
	else
	{
		return false;
	}
}
