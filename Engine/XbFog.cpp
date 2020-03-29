/*
	XBFOG.CPP

	FOG CLASS (c) g0at3r
*/

#include "XbFog.h"


CXBFog::CXBFog()
{
	m_dwVertexDecl[0] = D3DVSD_STREAM( 0 );
	m_dwVertexDecl[1] = D3DVSD_REG( 0, D3DVSDT_FLOAT3 );
	m_dwVertexDecl[2] = D3DVSD_REG( 2, D3DVSDT_FLOAT3 );
	m_dwVertexDecl[3] = D3DVSD_REG( 5, D3DVSDT_FLOAT2 );
	m_dwVertexDecl[4] = D3DVSD_END();

	m_dwVertexShader  = NULL;
	m_dwFogMode       = D3DFOG_EXP;
	m_dwFogColor	  = D3DCOLOR_XRGB(0,0,130);
	m_fFogStartValue  = 50.0f;
	m_fFogEndValue    = 70.0f;
	m_fFogDensity     = 0.03f;
	m_bUseFog		  = true;
}

CXBFog::~CXBFog()
{
}

void CXBFog::Enable()
{
	m_bUseFog = true;
}

void CXBFog::Disable()
{
	m_bUseFog = false;
}

void CXBFog::ChangeFog(DWORD dwFogMode, FLOAT fFogStart, FLOAT fFogEnd, FLOAT fFogDensity, DWORD dwFogColor)
{
	m_dwFogMode       = dwFogMode;
	m_dwFogColor	  = dwFogColor;
	m_fFogStartValue  = fFogStart;
	m_fFogEndValue    = fFogEnd;
	m_fFogDensity     = fFogDensity;
}

void CXBFog::SetVertexShader(char* szVertexShader)
{
	XBUtil_CreateVertexShader(szVertexShader, m_dwVertexDecl, &m_dwVertexShader); // VXU
}

void CXBFog::Render()
{
	if( m_bUseFog && (m_dwFogMode != D3DFOG_NONE) )
	{
		SetupVertexShader();
		D3DDevice::SetTexture( 0, NULL );
		D3DDevice::SetRenderState( D3DRS_LIGHTING,         TRUE );
		D3DDevice::SetRenderState( D3DRS_ZENABLE,          TRUE );
		D3DDevice::SetRenderState( D3DRS_DITHERENABLE,     TRUE );
		D3DDevice::SetRenderState( D3DRS_SPECULARENABLE,   FALSE );
		D3DDevice::SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		D3DDevice::SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
		D3DDevice::SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		D3DDevice::SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		D3DDevice::SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		D3DDevice::SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		D3DDevice::SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		D3DDevice::SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		D3DDevice::SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		D3DDevice::SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		D3DDevice::SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
		D3DDevice::SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
		D3DDevice::SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
	}

	else
	{
		D3DDevice::SetRenderState( D3DRS_FOGENABLE,    FALSE );
	}
}

void CXBFog::SetupVertexShader()
{
	D3DDevice::SetVertexShader( m_dwVertexShader );

	D3DXMATRIX matW, matWV, matWVP;
	D3DXMatrixMultiply( &matWV,  &m_matWorld, &m_matView );
	D3DXMatrixMultiply( &matWVP, &matWV, &m_matProj );
	D3DXMatrixTranspose( &matW,   &m_matWorld ); 
	D3DXMatrixTranspose( &matWV,  &matWV ); 
	D3DXMatrixTranspose( &matWVP, &matWVP );
	D3DDevice::SetVertexShaderConstant( 10, &matW,   4 );
	D3DDevice::SetVertexShaderConstant( 20, &matWV,  4 );
	D3DDevice::SetVertexShaderConstant( 30, &matWVP, 4 );

	D3DXVECTOR4 vZeroes( 0.0f, 0.0f, 0.0f, 0.0f );
	D3DXVECTOR4 vLight( 0.0f, 50.0f, 0.0f, 0.0f );
	D3DXVECTOR4 vDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
	D3DXVECTOR4 vAmbient( 0x44/255.0f, 0x44/255.0f, 0x44/255.0f, 1.0f );
 
	D3DXMATRIX matInvWorld;
	D3DXMatrixInverse( &matInvWorld, NULL, &m_matWorld );
	D3DXVec3TransformCoord( (D3DXVECTOR3*)&vLight, (D3DXVECTOR3*)&vLight, &matInvWorld );
    
	D3DDevice::SetVertexShaderConstant(  0, &vZeroes,   1 );
	D3DDevice::SetVertexShaderConstant( 40, &vLight,    1 );
	D3DDevice::SetVertexShaderConstant( 41, &vDiffuse,  1 );
	D3DDevice::SetVertexShaderConstant( 42, &vAmbient,  1 );

	D3DXVECTOR4 vFog( 1.0f, 0.0f, 0.0f, 0.0f );
	D3DDevice::SetVertexShaderConstant( 44, &vFog,  1 );

	D3DDevice::SetRenderState( D3DRS_FOGENABLE,    TRUE );
	D3DDevice::SetRenderState( D3DRS_FOGCOLOR,     m_dwFogColor );
	D3DDevice::SetRenderState( D3DRS_FOGTABLEMODE, m_dwFogMode );
	D3DDevice::SetRenderState( D3DRS_FOGSTART,     FtoDW(m_fFogStartValue) );
	D3DDevice::SetRenderState( D3DRS_FOGEND,       FtoDW(m_fFogEndValue) );
	D3DDevice::SetRenderState( D3DRS_FOGDENSITY,   FtoDW(m_fFogDensity) );
}