#include "XbDirect3D.h"
#include "splash.h"

CXBDirect3D::CXBDirect3D()
{
	Create();
	
}

CXBDirect3D::~CXBDirect3D()
{
	m_pDirect3D->Release();
}

void CXBDirect3D::Create()
{
	DWORD dwPack = XGetAVPack();
    DWORD dwVideoFlags = XGetVideoFlags();

	m_pDirect3D = Direct3DCreate8(D3D_SDK_VERSION);

	ZeroMemory(&m_pDirect3D, sizeof(D3DPRESENT_PARAMETERS));

	m_D3DParams.BackBufferWidth                 = 640;
	m_D3DParams.BackBufferHeight                = 480;
	m_D3DParams.Flags = (dwVideoFlags & XC_VIDEO_FLAGS_HDTV_480p) ? D3DPRESENTFLAG_PROGRESSIVE : D3DPRESENTFLAG_INTERLACED;
	m_D3DParams.BackBufferFormat                = D3DFMT_LIN_A8R8G8B8;
	m_D3DParams.BackBufferCount                 = 1;
	m_D3DParams.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
	m_D3DParams.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	m_D3DParams.MultiSampleType=D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR;
	m_D3DParams.EnableAutoDepthStencil          = TRUE;
	m_D3DParams.AutoDepthStencilFormat          = D3DFMT_D24S8;
	m_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 0, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_D3DParams, &m_pDevice);
	splash_init();
	draw_splash(1.0f);

}

IDirect3DDevice8* CXBDirect3D::Device()
{
	return m_pDevice;
}