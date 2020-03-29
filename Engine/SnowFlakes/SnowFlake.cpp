#include "SnowFlake.h"

// Constructor
CXBSnowFlake::CXBSnowFlake()
{
}

// Destructor
CXBSnowFlake::~CXBSnowFlake()
{
}

// Load The 3 Snowflakes
void CXBSnowFlake::LoadModels( char* szSnowflake1, char* szSnowflake2, char* szSnowflake3 )
{
	m_meshFlake[0].Create( szSnowflake1, "SKINS:\\neXgen 3D\\snow.bmp" );
	m_meshFlake[1].Create( szSnowflake2, "SKINS:\\neXgen 3D\\snow.bmp" );
	m_meshFlake[2].Create( szSnowflake3, "SKINS:\\neXgen 3D\\snow.bmp" );
}
/*void CXBSnowFlake::LoadModels( char* szFileName, char* szTexture )
{
	m_meshFlake[0].Create( szFileName, szTexture );
	m_meshFlake[1].Create( szFileName );
	m_meshFlake[2].Create( szFileName );
}*/

// Set The Snowflake Boundaries
void CXBSnowFlake::SetBoundaries( float fXStart, float fXEnd, float fYStart, float fYEnd, float fZStart, float fZEnd )
{
	m_fStartX = fXStart;
	m_fEndX	  = fXEnd;
	m_fStartY = fYStart;
	m_fEndY	  = fYEnd;
	m_fStartZ = fZStart;
	m_fEndZ	  = fZEnd;
}

// Set Snowflake Speed
void CXBSnowFlake::SetSpeed( float fMinSpeed, float fMaxSpeed )
{
	m_fMinSpeed = fMinSpeed;
	m_fMaxSpeed = fMaxSpeed;
}

// Set Snowflake Scale
void CXBSnowFlake::SetScale( float fX, float fY, float fZ )
{
	m_meshFlake[0].Scale( fX, fY, fZ );
	m_meshFlake[1].Scale( fX, fY, fZ );
	m_meshFlake[2].Scale( fX, fY, fZ );
}

// Set Snowflake Rotation Speed
void CXBSnowFlake::SetRotationSpeed( float fSpeed1, float fSpeed2, float fSpeed3 )
{
	//m_fRotateSpeed = fSpeed;
	m_fRotateSpeed1 = fSpeed1;
	m_fRotateSpeed2 = fSpeed2;
	m_fRotateSpeed3 = fSpeed3;
}

// Set Number of Flakes To Render at One Time
void CXBSnowFlake::SetFlakes( int iFlakeCount )
{
	m_iFlakes = iFlakeCount;
}

// Update Frame Then Render
void CXBSnowFlake::Render()
{
	// Update Rotate Flake
	//m_vecRotation.x += m_fRotateSpeed;
	//m_vecRotation.y += m_fRotateSpeed;
	//m_vecRotation.z += m_fRotateSpeed;

	// NEW
	m_vecRotation1.x += m_fRotateSpeed1;
	m_vecRotation1.y += m_fRotateSpeed1;

	m_vecRotation2.x += m_fRotateSpeed2;
	m_vecRotation2.y += m_fRotateSpeed2;

	m_vecRotation2.x += m_fRotateSpeed2;
	m_vecRotation2.y += m_fRotateSpeed2;
	// NEW

	// Rotate Flakes
	// Might Change This later For Individual Rotation to Provide More Realism
	m_meshFlake[0].Rotate( m_vecRotation1.x, m_vecRotation1.y, m_vecRotation.z );
	m_meshFlake[1].Rotate( m_vecRotation2.x, m_vecRotation2.y, m_vecRotation.z );
	m_meshFlake[2].Rotate( m_vecRotation3.x, m_vecRotation3.y, m_vecRotation.z );
	/*m_meshFlake[0].Rotate( m_vecRotation.x, m_vecRotation.y, m_vecRotation.z );
	m_meshFlake[1].Rotate( m_vecRotation.x, m_vecRotation.y, m_vecRotation.z );
	m_meshFlake[2].Rotate( m_vecRotation.x, m_vecRotation.y, m_vecRotation.z );*/
	
	// Check Rendering Correct Amount
	for( int i = 0; i < m_iFlakes; i++ )
	{
		// Is Flake Being Used?
		if( !m_bUsed[i] )
		{
			m_bUsed[i] = true;												// Now Used
			m_fSpeed[i]		   = RandomNumber( m_fMinSpeed, m_fMaxSpeed );	// Speed
			m_vecPosition[i].x = RandomNumber( m_fStartX, m_fEndX );		// X Position
			m_vecPosition[i].y = m_fStartY;									// Y Position
			m_vecPosition[i].z = RandomNumber( m_fStartZ, m_fEndZ );		// Z Position
			m_iMeshNumber[i]   = RandomNumber( 0, 2 );						// Which Model?
		}

		else
		{
			// Update Flakes Position
			m_vecPosition[i].y -= m_fSpeed[i];

			// Flake Out Of Bounds?
			if( m_vecPosition[i].y < m_fEndY )
			{
				m_bUsed[i] = false; // Reset Flake!
			}

			// Render Flake!
			else
			{
				//XBMESH_ALPHAON XBMESH_XRAY
				m_meshFlake[ m_iMeshNumber[i] ].Move( m_vecPosition[i].x, m_vecPosition[i].y, m_vecPosition[i].z );
				m_meshFlake[ m_iMeshNumber[i] ].Render(XBMESH_XRAY);
			}
		}
	}
}