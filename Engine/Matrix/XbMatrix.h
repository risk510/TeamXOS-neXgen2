#ifndef __XBMATRIX_H__
#define __XBMATRIX_H__


#include <xtl.h>
#include <time.h>
#include "..\\XbFont.h"
#include "..\\XbUtil.h"
#include "..\\XbDirect3D.h"

extern CXBDirect3D	my3D;	// Global DirectX Device


class CXBMatrix
{
	protected:

	CXBFont		pFont;
	char		szMatrix[40][30];
	D3DCOLOR	dwColor[40][30];
	DWORD		dwTime[40];
	DWORD		dwWait[40];
	

	public:

	void Create( char* szFont );
	void Render( void );
};


#endif