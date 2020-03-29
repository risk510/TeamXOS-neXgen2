// Screensaver .H
#pragma once

#include <xtl.h>
#include "Main.h"
#include "StdString.h"

#define FADE_SCREENSAVER_MODE   1
#define BLACK_SCREENSAVER_MODE  2
#define MATRIX_SCREENSAVER_MODE 3

class CXBScreenSaver
{
	private:

	// Screensaver
	bool	bInactive;
	bool	bScreenSave;
	DWORD	dwSaverTick;
	D3DGAMMARAMP OldRamp;

	public:
	
	// Const & Dest
	CXBScreenSaver();
	~CXBScreenSaver();

	void CheckScreenSaver();
	void ResetScreensaver();
};