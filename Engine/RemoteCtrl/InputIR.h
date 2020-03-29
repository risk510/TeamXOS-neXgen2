/*

XBOX iR Remote Control Class

*/

#ifndef __XBOX_IRREMOTE_H

#define __XBOX_IRREMOTE_H

#include <xtl.h>
#include "XBInputEx.h"

class CXBRemoteCtrl
{
public:
	XBIR_REMOTE				m_IR_Remote[4];
    XBIR_REMOTE				m_DefaultIR_Remote;

	void	Initialize();
	void	UpdateInput();

	CXBRemoteCtrl();
	~CXBRemoteCtrl();
};

#endif