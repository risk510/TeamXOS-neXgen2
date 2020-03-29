/*---------------------------------------------------------------------------------------------------------------------

 File: XbCamera.h
 Name: 3D Camera Class
 Desc: Xbox Camera Object
 Code: By g0at3r (c) 2003



 Info:

 CXBCamera myCam( pDevice )		-=-     Creates Camera Object

 myCam.MoveForward( 0.1f )		-=-     Move Forwards At Speed 0.1f
 myCam.MoveBackward( 0.1f )		-=-     Move Backwards At Speed 0.1f
 myCam.MoveLeft( 0.1f )			-=-		Move Left (Strafe / Side Step) At Speed 0.1f
 myCam.MoveRight( 0.1f )		-=-		Move Right (Strafe / Side Step) At Speed 0.1f

 myCam.SetYAxis(true)			-=-		This Will Allow Camera To Move Up And Down Y-Axis To Where Your Looking At.
										This Is False As Default For A First-Person Shooter Persepective, As If
										Feet Are Always On The Ground, But You Can Still Look Around

 myCam.GetLookAt()				-=-     (D3DXVECTOR3) - Retrieves A D3DXVECTOR3 Of Where The Camera Is Looking
 myCam.SetLookAt(vecLookAt)		-=-     Set A New LookAt With A D3DXVECTOR3

 myCam.GetPosition()			-=-		(D3DXVECTOR3) - Retreives The Position Of Camera Into A D3DXVECTOR3
 myCam.SetPosition(Position)	-=-     Set A New Position With A D3DXVECTOR3

---------------------------------------------------------------------------------------------------------------------*/

#pragma once
#include <xtl.h>


#define DEFAULT_EYE_HEIGHT 2.0f


class CXBCamera
{
	private:

	LPDIRECT3DDEVICE8 m_pDevice;

	D3DXMATRIX m_matView;
	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matRotation;
	D3DXMATRIX m_matTranslation;

	D3DXVECTOR3 m_vecUp;
	D3DXVECTOR3 m_vecEye;
	D3DXVECTOR3 m_vecOrient;
	D3DXVECTOR3 m_vecLookAt;
	D3DXVECTOR3 m_vecDirection;

	float m_fFOV;
	float m_fAspect;

	bool m_bCamYAxis;

	
	public:

	CXBCamera::CXBCamera( LPDIRECT3DDEVICE8 pDevice );
	CXBCamera::~CXBCamera();

	void MoveForward(float Speed);
	void MoveBackward(float Speed);
	void MoveLeft(float Speed);
	void MoveRight(float Speed);

	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetPosition();
	
	void SetLookAt(D3DXVECTOR3 LookAt);
	void SetPosition(D3DXVECTOR3 Position);

	void SetFieldOfView( float fFOV );
	void SetAspectRatio( float fAspect );

	void SetCamYAxis(bool Value);

	void UpdateCamera();
};