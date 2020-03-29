#ifndef __XBSNDTRK_H__
#define __XBSNDTRK_H__


#include <xtl.h>
#include <vector>
#include <dsound.h>
#include "WmaStream.h"
#include "..\\XbUtil.h"


// Song Structure
struct Song {
	DWORD dwID;
	CHAR  szAlbum[MAX_SOUNDTRACK_NAME+1];
    CHAR  szTrack[MAX_SONG_NAME+1];
	DWORD dwLength;
	DWORD dwCurrent;
};

// SongList is Really A Vector of Songs
typedef std::vector< Song > SongList;


// SoundTrack Class
class CXBSndTrk
{
	protected:

	bool	 m_bFoundSoundTracks;	// SoundTracks Found?
	bool	 m_bIsPlaying;			// SoundTracks Playing?
	bool	 m_bIsPaused;			// SoundTracks Paused?
	bool	 m_bRandomPlay;			// Play SoundTracks In Random Order?
	bool	 m_bContinuous;			// Auto Play Next SoundTrack?

	int		 m_iCurrentSong;								// Current Song
	char	 m_szCurrentSong[MAX_SONG_NAME+1];				// Current Song
	char	 m_szCurrentSoundTrack[MAX_SOUNDTRACK_NAME+1];	// Current SoundTrack
	char	 m_szTimeBuffer[50];							// Time Buffer
	DWORD	 m_dwCurrentTimeTotal;							// Song Length
	DWORD	 m_dwCurrentTimeRemains;						// Song Remains
	DWORD	 m_dwCurrentTimePlayed;							// Song Played
	DWORD	 m_dwCurrentPercent;							// Percent Played

	SongList m_vSongList;			// Vector Containing All Songs

	CWMAFileStream* m_pStream;		// WMA playback object
	LPDIRECTSOUND8  m_pDSound;		// DirectSound object

	HANDLE	m_hSongFile;	// Current Song File Handle
	

	// Add Current SoundTrack Songs To SongList
	void	AddSoundTrack( XSOUNDTRACK_DATA &pSound );


	public:

	// Const & Dest
	CXBSndTrk();
	~CXBSndTrk();

	// Init Sound & Tracks
	void	Initialize();

	// Update Sound Per Render
	void	Update();

	// Refresh/Reget SoundTracks Info
	void	Refresh();

	// Play Song
	void	Play();
	void	Stop();

	// Random
	void	SetRandomPlay( bool bRandom );

	// Do We Have Any SoundTracks
	bool	HasSoundTracks();

	// Get Current SoundTrack & Song
	char*	GetCurrentSoundTrackName();
	char*	GetCurrentSongName();

	// Get Current Song Total, Remains, & Played Time
	DWORD	GetCurrentSongTimeTotal();
	char*	GetCurrentSongTimeTotalString();
	DWORD	GetCurrentSongTimeRemains();
	char*	GetCurrentSongTimeRemainsString();
	DWORD	GetCurrentSongTimePlayed();
	char*	GetCurrentSongTimePlayedString();
};

#endif