/*

	XBFOG.H

	FOG CLASS (c) g0at3r



	FOG MODES:..
				D3DFOG_LINEAR
				D3DFOG_EXP (USED AS DEFAULT - BEST IMO)
				D3DFOG_EXP2

*/


#ifndef __FOGHELPER_H__
#define __FOGHELPER_H__

#include <xtl.h>
#include "XbUtil.h"



class CXBFog
{
	private:

	D3DXMATRIX			m_matWorld, m_matView, m_matProj;
	FLOAT				m_fFogStartValue, m_fFogEndValue, m_fFogDensity;
	DWORD				m_dwFogColor;
	DWORD				m_dwVertexShader;
	DWORD				m_dwFogMode;
	DWORD				m_dwVertexDecl[5];
	bool				m_bUseFog;

	void SetupVertexShader();


	public:

	CXBFog();
	~CXBFog();

	void SetVertexShader(char* szVertexShader);
	void Render();
	void ChangeFog(DWORD dwFogMode, FLOAT fFogStart, FLOAT fFogEnd, FLOAT fFogDensity, DWORD dwFogColor);
	void Enable();
	void Disable();
};



#endif