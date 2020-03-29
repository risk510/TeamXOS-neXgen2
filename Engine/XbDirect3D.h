/*-------------------------------------------------------------------------------------------

 File: XbDirect3D.h
 Desc: Xbox Direct3D Initialisation
 Code: By g0at3r (c) 2003



 Use:

 CXBDirect3D my3D					-=-		Direct3D Object

 my3D.Create()						-=-     Creates & Initialises The Xbox Direct3D
 my3D.Device()						-=-		Returns The Device For GFX/DirectX Usage
											Example:	my3D.Device()->Present()

-------------------------------------------------------------------------------------------*/

#ifndef XBDIRECT3D_H
#define XBDIRECT3D_H

#include <xtl.h>

class CXBDirect3D
{
	private:

	IDirect3D8*				m_pDirect3D;
	IDirect3DDevice8*		m_pDevice;
	D3DPRESENT_PARAMETERS	m_D3DParams;


	public:

	CXBDirect3D();
	~CXBDirect3D();

	void Create();
	IDirect3DDevice8* Device();
};

#endif