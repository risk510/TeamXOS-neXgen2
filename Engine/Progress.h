//-----------------------------------------------------------------------------
// File: Progress.h
//
// Desc: Support class for rendering a Progress image.
//-----------------------------------------------------------------------------
#ifndef Progress_H
#define Progress_H
#include <XBResource.h>
#include <XBUtil.h>
#include <XBFont.h>
#include <xbapp.h>

#include "Panel.h"
#include "Main.h"

//-----------------------------------------------------------------------------
// Name: class CProgress
// Desc: Class for rendering an Progress image.
//-----------------------------------------------------------------------------


class CProgress  
{
public:
    // Constructor/destructor
    CProgress();
    ~CProgress();

	HRESULT Create(	//CUserInterface* pUserInterface,
					LPDIRECT3DTEXTURE8 pBar,
					LPDIRECT3DTEXTURE8 pNib,
					FLOAT nPosX, FLOAT nPosY );

	HRESULT Create(	//CUserInterface* pUserInterface,
					LPDIRECT3DTEXTURE8 pNib, FLOAT nBarWidth,
					FLOAT nPosX=0, FLOAT nPosY=0);

    HRESULT Destroy();

	HRESULT SetRange( UINT64 absoluteMinimum, UINT64 absoluteMaximum);
	HRESULT SetValue( UINT64 absoluteAmount );
	UINT64	GetValue();

	HRESULT Increment( UINT64 relativeAmount );
	HRESULT Decrement( UINT64 relativeAmount );

    HRESULT Render();
	HRESULT Render(FLOAT nPosX, FLOAT nPosY);

protected:

	FLOAT m_nPosX;
	FLOAT m_nPosY;
	FLOAT m_nBarWidth;
	FLOAT m_nRealNibWidth;

	UINT64 m_nMinimum;
	UINT64 m_nMaximum;
	UINT64 m_nCurrent;

	CPanel m_Bar;
	CPanel m_Nib;
};

#endif

