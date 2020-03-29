/*

XBOX Screen Shot Class

*/

#ifndef __XBOX_SSHOT_H

#define __XBOX_SSHOT_H

extern CXBDirect3D	my3D;	// Global DirectX Device

class CXBScreenShot
{
private:
		char imagefilename[22];
		int imagenumber;
		char textnumber[5];
		void NextFreeFile();
public:
		void Take();
		CXBScreenShot();
		~CXBScreenShot();
};

#endif
