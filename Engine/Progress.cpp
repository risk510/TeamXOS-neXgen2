//-----------------------------------------------------------------------------
// File: Progress.cpp
//
// Desc: Support class for rendering an image.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>
#include <stdio.h>
#include "Progress.h"


//-----------------------------------------------------------------------------
// Name: CProgress()
// Desc: Help class constructor
//-----------------------------------------------------------------------------
CProgress::CProgress()
{
	m_nCurrent	= 0;
	m_nMaximum	= 0;
	m_nPosX		= 0;
	m_nPosY		= 0;
	m_nBarWidth = 0;
}




//-----------------------------------------------------------------------------
// Name: ~CProgress()
// Desc: Progress class destructor
//-----------------------------------------------------------------------------
CProgress::~CProgress()
{
    Destroy();
}


HRESULT CProgress::Create(	//CUserInterface* pUserInterface,
							LPDIRECT3DTEXTURE8 pBar,
							LPDIRECT3DTEXTURE8 pNib,
							FLOAT nPosX, FLOAT nPosY )
{
//	CControl::Create(pUserInterface);

//	if (FAILED(m_Bar.Create(GetDevice(),pBar)))
//		return E_FAIL;

//	if (FAILED(m_Nib.Create(GetDevice(),pNib)))
//		return E_FAIL;

	m_nPosX		= nPosX;
	m_nPosY		= nPosY;
	m_nBarWidth = m_Bar.GetWidth();
	m_nRealNibWidth = m_Nib.GetWidth() / 2;

    return S_OK;
}

HRESULT CProgress::Create(	//CUserInterface* pUserInterface,
							LPDIRECT3DTEXTURE8 pNib, FLOAT nBarWidth,
							FLOAT nPosX, FLOAT nPosY)
{
//	CControl::Create(pUserInterface);

	

//	if (FAILED(m_Nib.Create(g_pd3dDevice,pNib, true)))
	//if (FAILED(m_Nib.Create(my3D.m_pDevice,pNib, true)))
	//	return E_FAIL;

	m_nPosX		= nPosX;
	m_nPosY		= nPosY;
	m_nBarWidth = nBarWidth;
	m_nRealNibWidth = m_Nib.GetWidth() / 2;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroys the Progress class' internal objects/
//-----------------------------------------------------------------------------
HRESULT CProgress::Destroy()
{
	if (m_Bar.GetWidth()>0)
		m_Bar.Destroy();

	m_Nib.Destroy();

    return S_OK;
}






//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the control.
//-----------------------------------------------------------------------------
HRESULT CProgress::Render()
{
    return Render(m_nPosX,m_nPosY);
}


HRESULT CProgress::Render(FLOAT nPosX, FLOAT nPosY)
{
	if (m_Bar.GetWidth()>0)
		m_Bar.Render(nPosX,nPosY);

	FLOAT offsetX = (FLOAT) ((m_nCurrent* (m_nBarWidth-m_nRealNibWidth) )/m_nMaximum);

//	if (HasFocus())
//	{
//		m_Nib.Render(m_nRealNibWidth,0,m_nRealNibWidth,m_Nib.GetHeight(),nPosX+offsetX,nPosY);
//	}
//	else
//	{
		m_Nib.Render(0,0,offsetX,m_Nib.GetHeight(),nPosX,nPosY);
//	}

    return S_OK;
}



HRESULT CProgress::SetRange(UINT64 absoluteMinimum, UINT64 absoluteMaximum)
{
	m_nMinimum = absoluteMinimum;
    m_nMaximum = absoluteMaximum;
	return S_OK;
}

HRESULT CProgress::Increment(UINT64 relativeAmount)
{
	m_nCurrent+=relativeAmount;

	if (m_nCurrent>m_nMaximum)
	{
		m_nCurrent=m_nMaximum;
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CProgress::Decrement(UINT64 relativeAmount)
{
	if (m_nCurrent>relativeAmount)
	{
		m_nCurrent-=relativeAmount;
		return S_OK;
	}

	m_nCurrent = m_nMinimum;

	return E_FAIL;
}

HRESULT CProgress::SetValue(UINT64 absoluteAmount)
{
	if (absoluteAmount>m_nMaximum)
	{
		m_nCurrent=m_nMaximum;
		return E_FAIL;
	}

	m_nCurrent=absoluteAmount;
	return S_OK;
}

UINT64 CProgress::GetValue()
{
	return m_nCurrent;
}

