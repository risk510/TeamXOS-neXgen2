#include <xtl.h>
#include "drivestatus.h"
#include "IoSupport.h"
#include "..\\XKUtils\\XKUtils.h"
#include "..\\mappath.h"

CIoSupport IOSupport;

#define CHECKTRAY_INTERVAL	100
#define REMOUNT_INTERVAL	200
#define CHECKDISK_INTERVAL	400

WORD wTrayState;
char tmpDriveMount[255];

CXboxInfo::CXboxInfo()
{
	m_bTrayOpen = false;
	m_bTrayClosed = false;
	m_bTrayEmpty = true;
	m_bTrayWasClosed = false;
	m_iDVDType = -1;
	m_iPrevDVDType = -2;
	m_dwTimeStamp = GetTickCount()+CHECKTRAY_INTERVAL;
	m_dwRemountTimeStamp = 0;
	m_dwCheckDVDTimeStamp = GetTickCount()+CHECKTRAY_INTERVAL+CHECKDISK_INTERVAL+CHECKDISK_INTERVAL;
}

CXboxInfo::~CXboxInfo()
{
}

void CXboxInfo::GetDriveState()
{
	bool bChangedState = false;
	bool bRemountDVD = false;

	if ( m_dwTimeStamp < GetTickCount() )
	{

		if ( SUCCEEDED( HalReadSMBusValue(0x20, 0x03, 0, (BYTE*)&wTrayState) ) )
		{
			if ( wTrayState&16 ) // Tray is open
			{
				m_iDVDType = XBI_DVD_EMPTY;
				if ( ( m_bTrayOpen == false ) || (m_bTrayClosed == true ) )
				{
					bChangedState = true;
				}
				m_bTrayWasClosed = m_bTrayClosed;
				m_bTrayOpen = true;
				m_bTrayClosed = false;
				m_bTrayEmpty = true;
			}
			else
			{
				if ( (wTrayState & 0x40) == 0x40 ) // Tray is closed
				{
					if ( !m_bTrayClosed )
					{
						if ( (wTrayState & 0x20) == 0x20 )
						{
							m_iDVDType = XBI_DVD_UNKNOWN;
							m_bTrayEmpty = false;
							bRemountDVD = true;
							bChangedState = true;
						}
						else
						{
							m_iDVDType = XBI_DVD_EMPTY;
							m_bTrayEmpty = true;
						}
						m_bTrayClosed = true;
					}
					else
					{
						if ( (wTrayState & 0x20) == 0x20 )
						{
							if ( m_bTrayEmpty )
							{
								m_iDVDType = XBI_DVD_UNKNOWN;
								m_bTrayEmpty = false;
								bRemountDVD = true;
								bChangedState = true;
							}
						}
						else
						{
							m_iDVDType = XBI_DVD_EMPTY;
							m_bTrayEmpty = true;
						}
					}
					m_bTrayWasClosed = m_bTrayClosed;
					if ( ( m_bTrayOpen == true ) || (m_bTrayClosed == false ) )
					{
						bChangedState = true;
					}
				}
				else // Tray is in init state
				{
					if ( ( m_bTrayOpen == true ) || (m_bTrayClosed == true ) )
					{
						bChangedState = true;
					}
					m_iDVDType = XBI_DVD_INIT;
					//m_iDVDType = XBI_DVD_EMPTY;
					m_bTrayWasClosed = m_bTrayClosed;
					m_bTrayEmpty = true;
					m_bTrayClosed = false;
				}
				m_bTrayOpen = false;
			}
		}
		m_dwTimeStamp = GetTickCount()+CHECKTRAY_INTERVAL;
	}
	if ( m_bTrayClosed )
	{
		if ( bRemountDVD )
		{
			m_dwRemountTimeStamp = 0;
			IOSupport.Remount("D:", "cdrom0");
			m_dwCheckDVDTimeStamp = GetTickCount()+CHECKDISK_INTERVAL;
		}

		if ( m_dwCheckDVDTimeStamp && (m_dwCheckDVDTimeStamp < GetTickCount()) )
		{
			m_iDVDType = XBI_DVD_UNKNOWN;
			bChangedState = true;
			if ( FileExists("D:\\default.xbe"))
			{
				m_iDVDType = XBI_DVD_XBOX;
			}
			else if ( FileExists("D:\\VIDEO_TS\\VIDEO_TS.IFO"))
			{
				m_iDVDType = XBI_DVD_MOVIE;
			}
			m_dwCheckDVDTimeStamp = 0;
		}
	}
	else
	{
		m_dwCheckDVDTimeStamp = 0;
	}
}