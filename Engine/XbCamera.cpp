#include "XbCamera.h"

CXBCamera::CXBCamera( LPDIRECT3DDEVICE8 pDevice )
{
	m_pDevice = pDevice;

	m_bCamYAxis = false;

	m_vecUp.y  = 1.0f;
	m_vecEye.z = -7.0f;
	m_vecEye.y = DEFAULT_EYE_HEIGHT;

	D3DXMatrixTranslation(&m_matTranslation, -m_vecEye.x, -m_vecEye.y, -m_vecEye.z);

	D3DXMATRIX mRX, mRY, mRZ;
	D3DXMatrixRotationX(&mRX, -m_vecOrient.x);
	D3DXMatrixRotationY(&mRY, -m_vecOrient.y);
	D3DXMatrixRotationZ(&mRZ, -m_vecOrient.z);
	m_matRotation = mRZ;
	D3DXMatrixMultiply(&m_matRotation, &m_matRotation, &mRY);
	D3DXMatrixMultiply(&m_matRotation, &m_matRotation, &mRX);

	m_fFOV = D3DX_PI/4;
	m_fAspect = 640.0f / 480.0f;

	UpdateCamera();
}

CXBCamera::~CXBCamera()
{
}

void CXBCamera::MoveForward(float Speed)
{
	D3DXVec3Normalize(&m_vecDirection, &(m_vecLookAt - m_vecEye));
	m_vecEye += m_vecDirection * Speed;
	m_vecLookAt += m_vecDirection * Speed;

	if (!m_bCamYAxis)
	{
		m_vecEye.y = DEFAULT_EYE_HEIGHT;
	}
}

void CXBCamera::MoveBackward(float Speed)
{
	D3DXVec3Normalize(&m_vecDirection, &(m_vecLookAt - m_vecEye));
	m_vecEye -= m_vecDirection * Speed;
	m_vecLookAt -= m_vecDirection * Speed;

	if (!m_bCamYAxis)
	{
		m_vecEye.y = DEFAULT_EYE_HEIGHT;
	}
}

void CXBCamera::MoveLeft(float Speed)
{
	D3DXVec3Cross(&m_vecDirection, &(m_vecLookAt - m_vecEye), &m_vecUp);
	D3DXVec3Normalize(&m_vecDirection, &m_vecDirection);
	m_vecEye += m_vecDirection * Speed;
	m_vecLookAt += m_vecDirection * Speed;

	if (!m_bCamYAxis)
	{
		m_vecEye.y = DEFAULT_EYE_HEIGHT;
	}
}

void CXBCamera::MoveRight(float Speed)
{
	D3DXVec3Cross(&m_vecDirection, &(m_vecLookAt - m_vecEye), &m_vecUp);
	D3DXVec3Normalize(&m_vecDirection, &m_vecDirection);
	m_vecEye -= m_vecDirection * Speed;
	m_vecLookAt -= m_vecDirection * Speed;

	if (!m_bCamYAxis)
	{
		m_vecEye.y = DEFAULT_EYE_HEIGHT;
	}
}

void CXBCamera::SetCamYAxis(bool Value)
{
	m_bCamYAxis = Value;
}

D3DXVECTOR3 CXBCamera::GetLookAt()
{
	return m_vecOrient;
}

void CXBCamera::SetLookAt(D3DXVECTOR3 Orient)
{
	m_vecOrient = Orient;

	D3DXMATRIX mRX, mRY, mRZ;
	D3DXMatrixRotationX(&mRX, -m_vecOrient.x);
	D3DXMatrixRotationY(&mRY, -m_vecOrient.y);
	D3DXMatrixRotationZ(&mRZ, -m_vecOrient.z);

	m_matRotation = mRZ;
	D3DXMatrixMultiply(&m_matRotation, &m_matRotation, &mRY);
	D3DXMatrixMultiply(&m_matRotation, &m_matRotation, &mRX);
}

D3DXVECTOR3 CXBCamera::GetPosition()
{
	return m_vecEye;
}

void CXBCamera::SetPosition(D3DXVECTOR3 Position)
{
	m_vecEye = Position;
}

void CXBCamera::SetFieldOfView( float fFOV )
{
	m_fFOV = fFOV;
}

void CXBCamera::SetAspectRatio( float fAspect )
{
	m_fAspect = fAspect;
}

void CXBCamera::UpdateCamera()
{
	D3DXMatrixTranslation(&m_matTranslation, -m_vecEye.x, -m_vecEye.y, -m_vecEye.z);

	m_vecLookAt.x = m_vecEye.x + sinf(m_vecOrient.y) * cosf(m_vecOrient.x);
	m_vecLookAt.y = m_vecEye.y + cosf(m_vecOrient.y) * sinf(m_vecOrient.x);
	m_vecLookAt.z = m_vecEye.z + cosf(m_vecOrient.y);

	D3DXMatrixLookAtLH(&m_matView, &m_vecEye, &m_vecLookAt, &m_vecUp);
	D3DXMatrixMultiply(&m_matWorld, &m_matTranslation, &m_matRotation);
	m_pDevice->SetTransform(D3DTS_VIEW, &m_matWorld); 

	D3DXMATRIX m_matProj;
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFOV, m_fAspect, 1.0f, 1000.0f );
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
}