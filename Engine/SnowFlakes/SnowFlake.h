/*

	XBOX SnowFlake Class

*/

#ifndef __SNOWFLAKE_H__
#define __SNOWFLAKE_H__


#include <xtl.h>
#include "..\\XbMesh.h"
#include "..\\XbUtil.h"

#define MAX_SF 1000

class CXBSnowFlake
{
	protected:

	CXBMesh			m_meshFlake[MAX_SF];			// Snowflakes Models
	D3DXVECTOR3		m_vecPosition[MAX_SF];	// Snowflakes Position
	D3DXVECTOR3		m_vecRotation;			// Snowflakes Rotation

	D3DXVECTOR3		m_vecRotation1;			// Snowflakes Rotation
	D3DXVECTOR3		m_vecRotation2;			// Snowflakes Rotation
	D3DXVECTOR3		m_vecRotation3;			// Snowflakes Rotation

	float			m_fSpeed[MAX_SF];		// Snowflakes Speed
	int				m_iMeshNumber[MAX_SF];	// Which Snowflake Model
	float			m_fRotateSpeed;			// Speed To Rotate Flake

	float			m_fRotateSpeed1;			// Speed To Rotate Flake
	float			m_fRotateSpeed2;			// Speed To Rotate Flake
	float			m_fRotateSpeed3;			// Speed To Rotate Flake

	int				m_iFlakes;				// Number of Snowflakes To Render
	float			m_fStartZ;				// Z Value of Start Position
	float			m_fEndZ;				// Z Value of End Position
	float			m_fStartX;				// X Value of Start Position
	float			m_fEndX;				// X Value of End Position
	float			m_fStartY;				// Y Value of Start Position
	float			m_fEndY;				// Y Value of End Position
	float			m_fMaxSpeed;			// Fastest A Flake Will Fall
	float			m_fMinSpeed;			// Slowest A Flake Will Fall
	bool			m_bUsed[MAX_SF];		// Flake Being Used?


	public:

	CXBSnowFlake();		// Constructor
	~CXBSnowFlake();	// Destructor

	// Load The 3 Snowflakes
	void LoadModels( char* szSnowflake1, char* szSnowflake2, char* szSnowflake3 );
	//void LoadModels( char* szFileName, char* szTexture = "" );

	// Set The Snowflake Boundaries
	void SetBoundaries( float fXStart, float fXEnd, float fYStart, float fYEnd, float fZStart, float fZEnd );

	// Set Snowflake Speed
	void SetSpeed( float fMinSpeed, float fMaxSpeed );

	// Set Snowflake Scale
	void SetScale(float fX, float fY, float fZ );

	// Set Snowflake Rotation Speed
	//void SetRotationSpeed( float fSpeed );
	void SetRotationSpeed( float fSpeed1, float fSpeed2, float fSpeed3 );

	// Set Number of Flakes To Render at One Time
	void SetFlakes( int iFlakeCount );

	// Update Frame Then Render The Snow Flakes
	void Render();
};


#endif