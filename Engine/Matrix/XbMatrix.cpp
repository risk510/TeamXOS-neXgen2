#include "XbMatrix.h"

void CXBMatrix::Create( char* szFont )
{
	srand( time(NULL) );

	pFont.Create( szFont );
	char szAlphabetArray[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for( int i = 0; i < 40; i++ )
	{
		dwTime[i] = GetTickCount();
		dwWait[i] = (DWORD) RandomNumber( 30, 200 );

		for( int j = 0; j < 30; j++ )
		{
			szMatrix[i][j] = szAlphabetArray[ rand() % 25 ];
		}
	}
}

void CXBMatrix::Render( void )
{
	// Clear Scene to Black
	my3D.Device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0 );

	// Update Chars
	for( int i = 0; i < 40; i++ )
	{
		// Get Time Passed
		DWORD dwTimePassed = GetTickCount() - dwTime[i];

		// Update
		for( int j = 0; j < 30; j++ )
		{
			// Time To Update?
			if( dwWait[i] > dwTimePassed ) { break; }

			// Reset Time
			dwTime[i] = GetTickCount();

			// Leading Char?
			if( dwColor[i][j] == D3DCOLOR_ARGB( 255, 255, 255, 255 ) )
			{
				// Reset All
				for( int r = 0; r < 30; r++ ) { dwColor[i][r] = D3DCOLOR_ARGB(0,0,0,0); }

				// Update
				dwColor[i][j+1] = D3DCOLOR_ARGB(255,255,255,255);
				if( j >= 0 ) { dwColor[i][j] = D3DCOLOR_ARGB(255,0,255,0); }
				if( j-1 >= 0 ) { dwColor[i][j-1] = D3DCOLOR_ARGB(200,0,255,0); }
				if( j-2 >= 0 ) { dwColor[i][j-2] = D3DCOLOR_ARGB(150,0,255,0); }
				if( j-3 >= 0 ) { dwColor[i][j-3] = D3DCOLOR_ARGB(100,0,255,0); }
				if( j-4 >= 0 ) { dwColor[i][j-4] = D3DCOLOR_ARGB(50,0,255,0); }

				// Done With This Trail
				break;
			}

			// Theres No Leading Char
			else if( j == 29 )
			{
				// All Black
				for( int r = 0; r < 30; r++ ) { dwColor[i][r] = D3DCOLOR_ARGB(0,0,0,0); }

				// First White
				dwColor[i][0] = D3DCOLOR_ARGB(255,255,255,255);

				// Random Speed Again
				dwWait[i] = (DWORD) RandomNumber( 30, 200 );
			}
		}
	}


	// Render Chars
	char szBuffer[2];
	pFont.Begin();
	for( int i = 0; i < 40; i++ )
	{
		for( int j = 0; j < 30; j++ )
		{
			// Out of Bounds?
			if( i*16 > 640 ) { break; }
			if( j*16 > 480 ) { break; }

			// If It Cant Be Seen Skip It
			if( dwColor[i][j] != D3DCOLOR_ARGB(0,0,0,0) )
			{
				sprintf( szBuffer, "%c", szMatrix[i][j] );

				pFont.SetCursorPosition( (float)i*16, (float)j*16 );
				pFont.DrawText( szBuffer, dwColor[i][j] );
			}
		}
	}
	pFont.End();
}