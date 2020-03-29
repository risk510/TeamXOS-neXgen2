#include "XbSndTrk.h"

// Init Sound & Tracks
void CXBSndTrk::Initialize()
{
	// Reset Sound Device
	DirectSoundCreate( NULL, &m_pDSound, NULL );

	// Get Sound Tracks
	Refresh();
}

// Get Current SoundTrack
char* CXBSndTrk::GetCurrentSoundTrackName()
{
	return &m_szCurrentSoundTrack[0];
}

// Get Current Song
char* CXBSndTrk::GetCurrentSongName()
{
	return &m_szCurrentSong[0];
}

// Get Time Played
char* CXBSndTrk::GetCurrentSongTimePlayedString()
{
	// Convert to seconds
    DWORD dwSeconds = m_dwCurrentTimePlayed / 1000;

    // Determine minutes
    DWORD dwMinutes = dwSeconds / 60;

    // Remaining seconds
    dwSeconds -= ( dwMinutes * 60 );

    // Format
    sprintf( m_szTimeBuffer, "%i:%02i", dwMinutes, dwSeconds );
	return &m_szTimeBuffer[0];
}

// Get Time Total
char* CXBSndTrk::GetCurrentSongTimeTotalString()
{
	// Convert to seconds
    DWORD dwSeconds = m_dwCurrentTimeTotal / 1000;

    // Determine minutes
    DWORD dwMinutes = dwSeconds / 60;

    // Remaining seconds
    dwSeconds -= ( dwMinutes * 60 );

    // Format
    sprintf( m_szTimeBuffer, "%i:%02i", dwMinutes, dwSeconds );
	return &m_szTimeBuffer[0];
}

// Get Time Remains
char* CXBSndTrk::GetCurrentSongTimeRemainsString()
{
	// Convert to seconds
    DWORD dwSeconds = m_dwCurrentTimeRemains / 1000;

    // Determine minutes
    DWORD dwMinutes = dwSeconds / 60;

    // Remaining seconds
    dwSeconds -= ( dwMinutes * 60 );

    // Format
    sprintf( m_szTimeBuffer, "%i:%02i", dwMinutes, dwSeconds );
	return &m_szTimeBuffer[0];
}

// Random PLay
void CXBSndTrk::SetRandomPlay( bool bRandom )
{
	m_bRandomPlay = bRandom;
}

// Update Sound Per Render
void CXBSndTrk::Update()
{
	// Only If Playing
	if( m_bIsPlaying )
	{
		m_dwCurrentTimePlayed = m_pStream->GetPlayed();
		m_dwCurrentTimeRemains = m_dwCurrentTimeTotal - m_dwCurrentTimePlayed;

		// Check Song Status
		if( m_dwCurrentTimeRemains < 50 )
		{
			m_dwCurrentPercent = 0;
			m_bIsPlaying = false;

			// Continous Play?
			if( m_bContinuous )
			{
				// Play Next Tune
				Stop();
				Play();
			}

			// Stop
			else
			{
				Stop();
			}
		}

		m_pStream->Process( &m_dwCurrentPercent );
		DirectSoundDoWork();
	}
}

// Has SoundTracks?
bool CXBSndTrk::HasSoundTracks()
{
	return m_bFoundSoundTracks;
}

// Play Song
void CXBSndTrk::Play()
{
	// Make Sure There Are Tracks
	if( !m_bFoundSoundTracks ) { return; }
	if( m_vSongList.size() == 0 ) { return; }

	// Random?
	if( m_bRandomPlay ) { m_iCurrentSong = rand() % (m_vSongList.size() - 1); }
	// Next Track
	else { m_iCurrentSong += 1; }

	// Make Sure Track No Isnt Greater Then Amount Of Tracks!
	if( m_iCurrentSong > (int)(m_vSongList.size() - 1) ) { m_iCurrentSong = 0; }

	// Get Track Info
	DWORD dwSongID			= m_vSongList[m_iCurrentSong].dwID;
	m_dwCurrentTimeTotal	= m_vSongList[m_iCurrentSong].dwLength;
	sprintf( m_szCurrentSong, m_vSongList[m_iCurrentSong].szTrack );
	sprintf( m_szCurrentSoundTrack, m_vSongList[m_iCurrentSong].szAlbum);

	// Get File Handle
	m_hSongFile = XOpenSoundtrackSong( dwSongID, true );

	// Play Song
	SAFE_DELETE( m_pStream );
	m_pStream = new CWMAFileStream;
	m_pStream->Initialize( m_hSongFile );
	m_bIsPlaying = true;
}

// Stop Song
void CXBSndTrk::Stop()
{
	// Reset Info
	m_dwCurrentTimeTotal = 0;
	m_dwCurrentTimeRemains = 0;
	m_dwCurrentTimePlayed = 0;
	sprintf( m_szCurrentSong, "None" );
	sprintf( m_szCurrentSoundTrack, "None" );

	// Close WMASteam
	SAFE_DELETE( m_pStream );

	// Close File
	if( m_hSongFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hSongFile );
        m_hSongFile = INVALID_HANDLE_VALUE;
    }
}

// Refresh/Reget SoundTracks Info
void CXBSndTrk::Refresh()
{
	// Song & SoundTracks
	sprintf( m_szCurrentSong, "None" );
	sprintf( m_szCurrentSoundTrack, "None" );

	// Turn Everything Off
	m_bFoundSoundTracks	= false;
	m_bIsPlaying		= false;
	m_bIsPaused			= false;
	m_bRandomPlay		= false;
	m_bContinuous		= true;
	m_iCurrentSong		= 0;

	// Empty SongList
	m_vSongList.clear();

	// Reget All Songs
	HANDLE hFind;
	XSOUNDTRACK_DATA pSoundData;

	// Find First SoundTrack
	hFind = XFindFirstSoundtrack( &pSoundData );

	// Did Any SoundTracks Get Found?
	if( hFind != INVALID_HANDLE_VALUE )
	{
		// Found First! - Add To SongList
		m_bFoundSoundTracks = true;
		AddSoundTrack( pSoundData );

		// Keep Searching
		while( XFindNextSoundtrack( hFind, &pSoundData ) )
		{
			// Add Next SoundTrack
			AddSoundTrack( pSoundData );

			// Cleanup
			XFindClose( hFind );
		}
	}
}

// Add Current SoundTrack Songs To SongList
void CXBSndTrk::AddSoundTrack( XSOUNDTRACK_DATA &pSound )
{
	// For Every Song In SoundTrack
	for( UINT i = 0; i < pSound.uSongCount; i++ )
	{
		// Vars
		DWORD dwSongID;
		DWORD dwSongLength;
		WCHAR wszSongName[MAX_SONG_NAME];

		// Get Song Info
		XGetSoundtrackSongInfo( pSound.uSoundtrackId, i, &dwSongID, &dwSongLength,
								wszSongName, MAX_SONG_NAME );

		// Put It Into Song Structure
		Song pSong;
		pSong.dwID		= dwSongID;
		pSong.dwLength	= dwSongLength;
		sprintf( pSong.szAlbum, "%ls", pSound.szName );
		sprintf( pSong.szTrack, "%ls", wszSongName );

		// Put Song Into SongList
		m_vSongList.push_back( pSong );
	}
}

// Constructor
CXBSndTrk::CXBSndTrk()
{
}

// Destructor
CXBSndTrk::~CXBSndTrk()
{
}