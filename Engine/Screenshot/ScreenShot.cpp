#include <xtl.h>
#include "..\utils.h"
#include "..\main.h"
#include "screenshot.h"


void CXBScreenShot::NextFreeFile()
{
	while (FileExists(imagefilename) && imagenumber < 9999) {
		_itoa(imagenumber,textnumber,10);
		imagenumber++;
		strncpy(imagefilename+(17-strlen(textnumber)),textnumber,strlen(textnumber));
	}
	return;
}

void CXBScreenShot::Take()
{
	IDirect3DSurface8 *pFrontBuffer;
	my3D.Device()->GetBackBuffer(-1,D3DBACKBUFFER_TYPE_MONO,&pFrontBuffer);
	NextFreeFile();
	XGWriteSurfaceToFile(pFrontBuffer, imagefilename);
	pFrontBuffer->Release();
	return;
}

CXBScreenShot::CXBScreenShot()
{
	strcpy(imagefilename,"e:\\screenshot0000.bmp");
	while (FileExists(imagefilename) && imagenumber < 9999) {
		_itoa(imagenumber,textnumber,10);
		imagenumber++;
		strncpy(imagefilename+(17-strlen(textnumber)),textnumber,strlen(textnumber));
	}
}

CXBScreenShot::~CXBScreenShot()
{
}