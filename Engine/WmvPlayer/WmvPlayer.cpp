/*-----------------------------------------------------------------------------

	File: WMVPlayer.cpp

	Desc: WMV decoding and playback class

-----------------------------------------------------------------------------*/


#include "wmvplayer.h"


//-----------------------------------------------------------------------------
// Name: CWMVPlayer (ctor)
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CWMVPlayer::CWMVPlayer()
{
	m_iLoopControl = -1;
	m_iLoopCount	= 0;
    m_pWMVDecoder   = NULL;
    m_pDSound       = NULL;
    m_pStream       = NULL;
    m_pbSampleData  = NULL;
	m_bHasAudio		= false;
	m_bVideoSound   = false;
}

bool CWMVPlayer::IsDoneLooping( void )
{
	bool bReturn = false;
	if ( ( m_iLoopControl != -1 ) &&
		 ( m_iLoopControl <= m_iLoopCount ) )
	{
		bReturn = true;
	}
	return bReturn;
}



//-----------------------------------------------------------------------------
// Name: ~CWMVPlayer (dtor)
// Desc: Performs whatever cleanup is necessary
//-----------------------------------------------------------------------------
CWMVPlayer::~CWMVPlayer()
{
    // Calling code should have already called CloseFile, which would 
    // release and NULL out these pointers
    assert( !m_pWMVDecoder );
    assert( !m_pStream );

    if( m_pDSound )
        m_pDSound->Release();
    if( m_pbSampleData )
        delete[] m_pbSampleData;
}




//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Initializes the player, using the given d3d device and directsound
//       object
//-----------------------------------------------------------------------------
HRESULT CWMVPlayer::Initialize( LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECTSOUND8 pDSound )
{
    assert( pd3dDevice );
    m_pd3dDevice = pd3dDevice;
    m_pd3dDevice->AddRef();

    assert( pDSound );
    m_pDSound = pDSound;
    m_pDSound->AddRef();

    m_pbSampleData = new BYTE[ NUM_PACKETS * PACKET_SIZE ];
    assert( m_pbSampleData );

    for( int j = 0; j < NUM_PACKETS; j++ )
        m_adwStatus[ j ] = XMEDIAPACKET_STATUS_SUCCESS;
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: OpenFile
// Desc: Opens the specified WMV file.  This involves:
//       1) Creating the decoder XMO
//       2) Creating a texture to decode to based off the video size
//       3) Creating a direct sound stream for playback
//       4) Decoding the first chunk of video
//-----------------------------------------------------------------------------
HRESULT CWMVPlayer::OpenFile( CHAR* strFilename )
{
    HRESULT hr;
    
    hr = WmvCreateDecoder( strFilename,
                           NULL,
                           WMVVIDEOFORMAT_YUY2,
                           NULL,
                           &m_pWMVDecoder );
    if( FAILED( hr ) )
        return hr;

	m_bHasAudio		= true;


    // Get information about the video
    REFERENCE_TIME rtPreroll;
    m_pWMVDecoder->GetVideoInfo( &m_wmvVideoInfo );
    if ( m_pWMVDecoder->GetAudioInfo( &m_wfx ) != DS_OK )
	{
		m_bHasAudio = false;
	}
	else if ( m_wfx.nChannels == 0 )
	{
		m_bHasAudio = false;
	}

	{
		m_pWMVDecoder->GetPlayDuration( &m_rtDuration, &rtPreroll );
		m_rtDuration -= rtPreroll;

		// Create our video frame
		m_pd3dDevice->CreateTexture( m_wmvVideoInfo.dwWidth,
									m_wmvVideoInfo.dwHeight,
									0,
									0,
									D3DFMT_YUY2,
									NULL,
									&m_VideoFrame.pTexture );

		D3DLOCKED_RECT lr;
		m_VideoFrame.pTexture->LockRect( 0, &lr, NULL, 0 );
		m_VideoFrame.pBits = (BYTE *)lr.pBits;
		m_VideoFrame.pTexture->UnlockRect( 0 );
		m_VideoFrame.rtTimestamp = 0;
	    
		if ( m_bHasAudio )
		{
			// Create a stream for audio playback
			DSSTREAMDESC dssd = {0};
			dssd.dwFlags = 0;
			dssd.dwMaxAttachedPackets = NUM_PACKETS;
			dssd.lpwfxFormat = &m_wfx;

			if ( DirectSoundCreateStream( &dssd, &m_pStream ) == DS_OK )
			{
				if ( m_pStream )
				{
					m_pStream->Pause( DSSTREAMPAUSE_PAUSE );
				}
				else
				{
					m_bVideoSound = false;
				}
			}
			else
			{
				m_bVideoSound = false;
			}
		}
		else
		{
			m_bVideoSound = false;
			m_pStream = NULL;
		}

		m_bFirstFrame = TRUE;
		m_rtStartTime = 0;

		DecodeNext();
		hr = S_OK;
	}

    return hr;
}




//-----------------------------------------------------------------------------
// Name: CloseFile
// Desc: Closes the currently opened WMV file.  This releases the decoder, the
//       video texture, and the directsound stream
//-----------------------------------------------------------------------------
HRESULT CWMVPlayer::CloseFile()
{
    if( m_pWMVDecoder )
    {
        m_pWMVDecoder->Release();
        m_pWMVDecoder = NULL;
    }

    if( m_VideoFrame.pTexture )
    {
        m_VideoFrame.pTexture->Release();
        m_VideoFrame.pTexture = NULL;
    }

    if( m_pStream )
    {
        m_pStream->Pause( DSSTREAMPAUSE_RESUME );
        m_pStream->Release();
        m_pStream = NULL;
    }

	if( m_pbSampleData )
	{
        delete[] m_pbSampleData;
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetVideoInfo
// Desc: Returns information on the currently opened video file
//-----------------------------------------------------------------------------
HRESULT CWMVPlayer::GetVideoInfo( WMVVIDEOINFO* pVideoInfo )
{
    assert( m_pWMVDecoder );

    *pVideoInfo = m_wmvVideoInfo;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetWidth
// Desc: Returns Width of the currently opened video file
//-----------------------------------------------------------------------------
int CWMVPlayer::GetWidth(void)
{
	return (int)m_wmvVideoInfo.dwWidth;
}

//-----------------------------------------------------------------------------
// Name: GetHeight
// Desc: Returns Height of the currently opened video file
//-----------------------------------------------------------------------------
int CWMVPlayer::GetHeight(void)
{
	return (int)m_wmvVideoInfo.dwHeight;
}


void CWMVPlayer::ReSync(void)
{

	if ( IsDoneLooping() )
	{
		// Special case, reset the logic under this case.
		m_iLoopCount = -1;
		m_bFirstFrame = TRUE;
		m_rtStartTime = 0;

		DecodeNext();
	}

	// m_iLoopControl
    // Calculate current time
    REFERENCE_TIME rtCurrent;
    m_pDSound->GetTime( &rtCurrent );
	rtCurrent -= m_VideoFrame.rtTimestamp;

    // Reset "StartTime" to relative to syncing with frame NOW
    m_rtStartTime = rtCurrent;
}

//-----------------------------------------------------------------------------
// Name: IsReady
// Desc: Returns TRUE when the next video frame is ready to be displayed.
//-----------------------------------------------------------------------------
BOOL CWMVPlayer::IsReady()
{
    assert( m_pWMVDecoder );

    DirectSoundDoWork();

    // Calculate current time
    REFERENCE_TIME rtCurrent;
    m_pDSound->GetTime( &rtCurrent );
    rtCurrent -= m_rtStartTime;

    // If we're ready to display next frame, then let the caller know
    if( rtCurrent >= m_VideoFrame.rtTimestamp )
    {
        return TRUE;
    }
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: GetTexture
// Desc: Returns texture to be displayed
//-----------------------------------------------------------------------------
LPDIRECT3DTEXTURE8 CWMVPlayer::GetTexture()
{
	/*
    assert( m_pWMVDecoder );

	LPDIRECT3DTEXTURE8 pTexture = NULL;
    D3DLOCKED_RECT lr;

    pTexture->LockRect( 0, &lr, NULL, 0 );
    memcpy( lr.pBits, m_VideoFrame.pBits, m_wmvVideoInfo.dwHeight * lr.Pitch );
    pTexture->UnlockRect( 0 );

    if( m_bFirstFrame )
    {
        m_bFirstFrame = FALSE;
		if ( m_pStream )
		{
			m_pStream->Pause( DSSTREAMPAUSE_RESUME );
		}
        m_pDSound->GetTime( &m_rtStartTime );
    }

    return pTexture;
	*/

	assert( m_pWMVDecoder );

	if( m_bFirstFrame )
    {
        m_bFirstFrame = FALSE;
		if ( m_pStream )
		{
			m_pStream->Pause( DSSTREAMPAUSE_RESUME );
		}
        m_pDSound->GetTime( &m_rtStartTime );
    }

	return m_VideoFrame.pTexture;
}



//-----------------------------------------------------------------------------
// Name: DecodeNext
// Desc: Decodes the next video frame, and all audio up to the following video
//       frame.  Except for the first time this is called (when opening the 
//       video), the first thing to be decoded should always be a video frame
//-----------------------------------------------------------------------------
HRESULT CWMVPlayer::DecodeNext()
{
    assert( m_pWMVDecoder );

    HRESULT hr;
    BOOL    bGotVideo = FALSE;

    do
    {
        DWORD dwIndex;

		XMEDIAPACKET *pAudioXMP = NULL;

        // Video packet
        DWORD vidStat, vidSize = 0;
        XMEDIAPACKET xmpVideo = {0};
        REFERENCE_TIME rtVideo;

		// Verify that we can decode audio.  If this assert fires, then you
		// need to increase the amount of audio buffer for the stream by
		// increasing the #define NUM_PACKETS in wmvplayer.h
		BOOL bCanDecodeAudio;

		// Audio packet
		DWORD audStat, audSize = 0;

		XMEDIAPACKET xmpAudio = {0};
		REFERENCE_TIME rtAudio;

		if ( m_bHasAudio )
		{
			// Verify that we can decode audio.  If this assert fires, then you
			// need to increase the amount of audio buffer for the stream by
			// increasing the #define NUM_PACKETS in wmvplayer.h
			bCanDecodeAudio = FindFreePacket( &dwIndex );
			
			//assert( bCanDecodeAudio );

			if( bCanDecodeAudio )
			{
				// Audio packet
				audSize = 0;

				// Set up audio packet
				xmpAudio.dwMaxSize        = PACKET_SIZE;
				xmpAudio.pvBuffer         = m_pbSampleData + dwIndex * PACKET_SIZE;
				xmpAudio.pdwStatus        = &audStat;
				xmpAudio.pdwCompletedSize = &audSize;
				xmpAudio.prtTimestamp     = &rtAudio;

				pAudioXMP = &xmpAudio;
			}
		}

        // Set up video packet
        xmpVideo.dwMaxSize        = m_wmvVideoInfo.dwWidth *
                                    m_wmvVideoInfo.dwHeight *
                                    m_wmvVideoInfo.dwOutputBitsPerPixel / 8;
        xmpVideo.pdwStatus        = &vidStat;
        xmpVideo.pdwCompletedSize = &vidSize;
        xmpVideo.prtTimestamp     = &rtVideo;
        xmpVideo.pvBuffer         = m_VideoFrame.pBits;

		hr = m_pWMVDecoder->ProcessMultiple( bGotVideo ? NULL : &xmpVideo, m_bHasAudio?pAudioXMP:NULL );
        if( hr == S_FALSE )
        {
			m_iLoopCount++;

			// If looping, go back to start!
			if ( ( m_iLoopControl==-1) || (m_iLoopCount<m_iLoopControl) )
			{
				REFERENCE_TIME rTime;

				rTime = 0;
				m_pWMVDecoder->Flush();
				if ( m_pStream )
				{
					m_pStream->Pause( DSSTREAMPAUSE_PAUSE );
				}

				m_bFirstFrame = TRUE;
				m_rtStartTime = 0;
				hr = m_pWMVDecoder->ProcessMultiple( bGotVideo ? NULL : &xmpVideo, m_bHasAudio?pAudioXMP:NULL );
			}
			else
			{
	            // Tell the stream we're done
				if ( m_pStream )
				{
					m_pStream->Discontinuity();
				}
			}
        }

		if ( m_bVideoSound )
		{
			if( audSize > 0 )
			{
				xmpAudio.dwMaxSize        = audSize;
				xmpAudio.pdwCompletedSize = NULL;
				xmpAudio.pdwStatus        = &m_adwStatus[ dwIndex ];
				xmpAudio.prtTimestamp     = NULL;
				if ( m_pStream )
				{
					m_pStream->Process( &xmpAudio, NULL );
				}
			}
        }

        if( vidSize > 0 )
        {
            m_VideoFrame.rtTimestamp = rtVideo;
            bGotVideo = TRUE;
        }
    } while( (DS_OK == hr) && (m_bHasAudio));

    if( DS_OK == hr || E_PENDING == hr )
        return S_OK;
    else
        return S_FALSE;
}




//-----------------------------------------------------------------------------
// Name: FindFreePacket
// Desc: Attempts to find a free audio packet (one that isn't currently 
//       submitted to the stream).  Returns TRUE if it found one
//-----------------------------------------------------------------------------
BOOL CWMVPlayer::FindFreePacket( DWORD * pdwIndex )
{
    // Check the status of each packet
    for( int i = 0; i < NUM_PACKETS; i++ )
    {
        // If we find a non-pending packet, return it
        if( m_adwStatus[ i ] != XMEDIAPACKET_STATUS_PENDING )
        {
            *pdwIndex = i;
            return TRUE;
        }
    }

    return FALSE;
}


