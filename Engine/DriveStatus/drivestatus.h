#pragma once 

extern BOOL  FileExists( LPCTSTR szFilename );
extern DWORD FileLength( LPCTSTR szFilename );

#define XBI_DVD_UNKNOWN		0
#define XBI_DVD_XBOX		1
#define XBI_DVD_MOVIE		2
#define XBI_DVD_DATA		3
#define XBI_DVD_MUSIC		4
#define XBI_DVD_EMPTY		5
#define XBI_DVD_INIT		6

class CXboxInfo
{
	DWORD	m_dwTimeStamp;
	DWORD	m_dwRemountTimeStamp;
	DWORD	m_dwCheckDVDTimeStamp;
public:
	CXboxInfo();
	~CXboxInfo();
	bool		m_bTrayOpen;
	bool		m_bTrayClosed;
	bool		m_bTrayEmpty;
	int			m_iDVDType;
	int			m_iPrevDVDType;
	bool		m_bTrayWasClosed;
	void GetDriveState();
};