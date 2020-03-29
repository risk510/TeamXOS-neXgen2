/*
XBOX Header Information
*/
#include "xbeheader.h"

CXBE xbeReader;

CXBE::CXBE()
{
  // Assume 256K is enough for header information
  m_iHeaderSize = ( 256 * 1024 );
  m_pHeader = new char[ m_iHeaderSize ] ; 

  m_iImageSize = ( 256 * 1024 );
  m_pImage = new char[m_iImageSize];

}
CXBE::~CXBE()
{
  if (m_pHeader) delete [] m_pHeader;

  m_pHeader=NULL;
  if (m_pImage) delete [] m_pImage;
  m_pImage=NULL;
}

bool CXBE::ExtractIcon(char *szFilename, char *szIconPath)
{

	// Open the local file
	HANDLE hFile( CreateFile( szFilename,
                      GENERIC_READ,
                      0,
                      NULL,
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL ) );

	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

  // Read the header
  DWORD dwRead;
  if ( !::ReadFile( (HANDLE)hFile,
                        m_pHeader,
                        min( m_iHeaderSize,
                        ( int )GetFileSize( (HANDLE)hFile, ( LPDWORD )NULL ) ),
                        &dwRead,
                        NULL ) )
  {
    return false;
  }

  // Header read. Copy information about header
  memcpy( &m_XBEInfo.Header, m_pHeader, sizeof( m_XBEInfo.Header ) );
  
  // Header read. Copy information about certificate
  int    iNumSections;
  char * pszSectionName;

  // Assume 256K is enough for image information

  // Initialize
  iNumSections = m_XBEInfo.Header.sections;
 // Position at the first section
  m_XBEInfo.pSection_Header = ( _xbe_info_::section_header * )( m_pHeader + ( m_XBEInfo.Header.section_headers_addr -
                                                                              m_XBEInfo.Header.base ) );

  // Sections read. Parse all the section headers
  while ( iNumSections-- > ( int )0 )
  {
    // Is this section an inserted file?
    if ( ( int )m_XBEInfo.pSection_Header->Flags.inserted_file == ( int )1 )
    {
      // Inserted file. Position at the name of the section
      pszSectionName = ( m_pHeader + ( m_XBEInfo.pSection_Header->section_name_addr -
                                        m_XBEInfo.Header.base ) );

      // Title image?
      if ( strcmp( pszSectionName, "$$XTIMAGE" ) == ( int )0 )
      {
        // Position at 'title image' in the XBE itself
        if ( SetFilePointer( (HANDLE)hFile,
                              m_XBEInfo.pSection_Header->raw_addr,
                              NULL,
                              FILE_BEGIN ) == ( DWORD )0xFFFFFFFF )
        {
            // Unable to set file position. Break out of loop
            return false;
        }

        // Read the image
		if ( ::ReadFile( (HANDLE)hFile,
                          m_pImage,
                          min( m_iImageSize,
                          ( int )m_XBEInfo.pSection_Header->sizeof_raw ),
                          &dwRead,
                          NULL ) == ( BOOL )FALSE )
        {
          // Image not read. Break out of loop
          return false;
        }

        // The m_pImage variable now contains the image data itself
		HANDLE hIcon ( CreateFile( szIconPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL ) );
        
		if (hIcon == INVALID_HANDLE_VALUE) {
			return false;
		}
        
				DWORD dwWrote;
				WriteFile( (HANDLE)hIcon,m_pImage,dwRead,&dwWrote,NULL);
				CloseHandle(hIcon);
        return true;
      }
    }

	
    // Position at the next section
    m_XBEInfo.pSection_Header++;
  }
  return false;
}

char* CXBE::GetXboxTitleName(char *filename)
{
	DWORD baseaddr, certaddr;
	WCHAR cert_string[41];

	HANDLE xbefd=CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
	OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if(xbefd!=INVALID_HANDLE_VALUE) {
		DWORD readin;

		SetFilePointer(xbefd, 0x104, NULL, FILE_BEGIN);
		ReadFile(xbefd, &baseaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, 0x118, NULL, FILE_BEGIN);
		ReadFile(xbefd, &certaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0xc, NULL, FILE_BEGIN);
		ReadFile(xbefd, &cert_string, 0x50, &readin, NULL);

		cert_string[0x50]=0x0000;

		CloseHandle(xbefd);

		int x=lstrlenW(cert_string);

		if(x<44 && x > 0) {
			sprintf(szTitleImageName, "%ls", cert_string);
			return szTitleImageName;
		} else {
			return "UNKNOWN";
		}

		
	}
	return "";
}


char* CXBE::GetXboxTitleID(char *filename)
{
	DWORD baseaddr, certaddr, cert_id;

	HANDLE xbefd=CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
	OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if(xbefd!=INVALID_HANDLE_VALUE) {
		DWORD readin;

		SetFilePointer(xbefd, 0x104, NULL, FILE_BEGIN);
		ReadFile(xbefd, &baseaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, 0x118, NULL, FILE_BEGIN);
		ReadFile(xbefd, &certaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0x8, NULL, FILE_BEGIN);
		ReadFile(xbefd, &cert_id, 0x4, &readin, NULL);

		CloseHandle(xbefd);

 		sprintf(szTitleID, "%08x", cert_id);		
	}
	return szTitleID;
}
