/*-----------------------------------------------------------------------------

	File: WMVPlayer.h

	Desc: WMV playback class

-----------------------------------------------------------------------------*/


#ifndef __WMVPLAYER_H__
#define __WMVPLAYER_H__

#include <xtl.h>
#include <assert.h>
#include <stdio.h>
#include <tchar.h>
#include "WmvXmo.h"


#ifdef _DEBUG
	#pragma comment(lib, "WmvPlayer\\wmvdecd.lib")
#else
	#pragma comment(lib, "WmvPlayer\\wmvdec.lib")
#endif


// This structure is used to hold data about a video frame
typedef struct {
    LPDIRECT3DTEXTURE8  pTexture;
    REFERENCE_TIME      rtTimestamp;
    BYTE*               pBits;
} VIDEOFRAME;

// You'll need to adjust NUM_PACKETS based off your video content
// to ensure that you never run out during the course of playback
// There is an assert that will fire if the class runs out of 
// packets
// Packet Size needs to be large enough to hold at least 2048
// decoded samples
#define NUM_PACKETS 256
#define PACKET_SIZE 2048 * 2 * 2


class CWMVPlayer
{
public:
    CWMVPlayer();
    ~CWMVPlayer();

    HRESULT Initialize( LPDIRECT3DDEVICE8 pd3dDevice, 
                        LPDIRECTSOUND8 pDSound );       // Initialize the player
    HRESULT OpenFile( CHAR* strFilename );              // Open a file for playback
    HRESULT CloseFile();                                // Close file
    HRESULT GetVideoInfo( WMVVIDEOINFO* pVideoInfo );   // Get video info

    BOOL    IsReady();                                  // Ready to switch?
    HRESULT DecodeNext();                               // Decode next frame

	LPDIRECT3DTEXTURE8	GetTexture( void );				// Return texture to be displayed

	void	SetVideoSound( bool bSound = true ) { m_bVideoSound = bSound; };
	bool	GetVideoSound( void ) { return m_bHasAudio; };

	void	SetVideoLoop( bool bLoop = true ) { m_iLoopControl = bLoop?-1:m_iLoopControl+1; };
	bool	GetVideoLoop( void ) { return (m_iLoopControl==(-1)); };
	bool	IsDoneLooping( void );

	void	ReSync(void);
	void	SetLoopControl( int iLoopControl ) { m_iLoopControl = iLoopControl; };
	int		GetLoopControl( void ) { return m_iLoopControl; };
	int		GetLoopCount( void ) { return m_iLoopCount; };
	int		GetHeight(void);
	int		GetWidth(void);

private:
	int		m_iLoopCount;
	int		m_iLoopControl;

    BOOL FindFreePacket( DWORD * pdwIndex );            // Find a free audio packet


	bool				m_bVideoSound;
	bool				m_bHasAudio;

    LPDIRECT3DDEVICE8   m_pd3dDevice;                   // Direct3D device
    LPWMVDECODER        m_pWMVDecoder;                  // WMV Decoder XMO
    WMVVIDEOINFO        m_wmvVideoInfo;                 // Video format info
    WAVEFORMATEX        m_wfx;                          // Audio format info
    REFERENCE_TIME      m_rtDuration;                   // Video duration

    REFERENCE_TIME      m_rtStartTime;                  // Start time
    BOOL                m_bFirstFrame;                  // BOOL to track first frame

    VIDEOFRAME          m_VideoFrame;                   // Video frame

    LPDIRECTSOUND8      m_pDSound;                      // DirectSound object
    LPDIRECTSOUNDSTREAM m_pStream;                      // Stream for playback
    DWORD               m_adwStatus[NUM_PACKETS];       // Packet status
    BYTE*               m_pbSampleData;                 // Audio buffer
};


#endif
