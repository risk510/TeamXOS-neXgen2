/*

XBOX iR Remote Control Class

*/

#include "InputIR.h"

//Constructor
CXBRemoteCtrl::CXBRemoteCtrl()
{
}

// Destructure
CXBRemoteCtrl::~CXBRemoteCtrl()
{
}

void CXBRemoteCtrl::Initialize()
{
	XBInput_CreateIR_Remotes( );
}

void CXBRemoteCtrl::UpdateInput()
{
	XBInput_GetInput( m_IR_Remote );
	ZeroMemory( &m_DefaultIR_Remote, sizeof(m_DefaultIR_Remote) );

	for( int i=0; i<4; i++ )
	{
		if( m_IR_Remote[i].hDevice)
		{
						m_DefaultIR_Remote.wButtons        = m_IR_Remote[i].wButtons;
		}
	}
}
// iR Button Delay ToDo