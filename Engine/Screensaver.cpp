// Screensaver.cpp
#include "Screensaver.h"

// Constructor
CXBScreenSaver::CXBScreenSaver()
{
	bInactive = false;
	bScreenSave = false;
	dwSaverTick = 0;
}

// Destructor
CXBScreenSaver::~CXBScreenSaver()
{
}


void CXBScreenSaver::CheckScreenSaver()
{
	D3DGAMMARAMP Ramp;
	FLOAT FadeLevel;

	if (!bInactive)
	{
		bInactive=true;

		if (bInactive) 
		{
			dwSaverTick=timeGetTime();
		}
	}
	else
	{
		if (!bScreenSave)	// Check we're not already in screensaver mode
		{
			if ( (long)(timeGetTime() - dwSaverTick) >= (long)(myDash.iScreenSaverTime*60L*1000L) )
			{
				bScreenSave = true;

				// ScreenSaver Mode
				switch ( myDash.iScreenSaverMode )
				{
					case FADE_SCREENSAVER_MODE:
					{
						FadeLevel = (FLOAT) myDash.iScreenSaverFadeLevel / 100;
					}
					break;

					case BLACK_SCREENSAVER_MODE:
					{
						FadeLevel = 0;
					}
					break;

					case MATRIX_SCREENSAVER_MODE:
					{
						bUseMatrixScreensaver = true;
						return;
					}
					break;

				}
				// End Mode

				my3D.Device()->GetGammaRamp(&OldRamp); // Store our old gamma ramp
				// Fade to our set FadeLevel ...
				for (float fade=1.f; fade>=FadeLevel; fade-=0.01f)
				{
					for(int i=0;i<256;i++)
					{
						Ramp.red[i]=(int)((float)OldRamp.red[i]*fade);
						Ramp.green[i]=(int)((float)OldRamp.green[i]*fade);
						Ramp.blue[i]=(int)((float)OldRamp.blue[i]*fade);
					}
					Sleep(5);
					my3D.Device()->SetGammaRamp(D3DSGR_IMMEDIATE, &Ramp);
				}
			}
		}
	}

	return;
}

void CXBScreenSaver::ResetScreensaver()
{
	bInactive=false;
	if (bScreenSave)		// Screen saver is active
	{
		if (bUseMatrixScreensaver)
		{
			bUseMatrixScreensaver = false; // Stop Matrix Screensaver Render
			bScreenSave = false;	// Reset the screensaver active flag
		}
		else 
		{
			my3D.Device()->SetGammaRamp(0, &OldRamp);	// put the old gamma ramp back in place
			bScreenSave = false;	// Reset the screensaver active flag
		}
	}
}