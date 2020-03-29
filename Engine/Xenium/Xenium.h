/*
	Xenium Lib v0.2 - SpOoK & Swolsten

	XeniumOS v1.0, v2.x+ Compliant.
*/

class CXBXeniumOS
{
public:
  CXBXeniumOS();
	virtual ~CXBXeniumOS();

	bool Reboot();
	bool Recovery();
	bool OriginalBios();

	/* BIOS Bank Switches */

	/* 1x1MB */
	bool Bank1_1MB();

	/* 2x512k */
	bool Bank1_512k();
	bool Bank2_512k();

	/* 2x256k + 1x512k */
	bool Bank1_256_512();
	bool Bank2_256_512();
	bool Bank3_256_512();

	/* 4x256k */
	bool Bank1_256();
	bool Bank2_256();
	bool Bank3_256();
	bool Bank4_256();

	/* Xenium LED Colours */

	bool LEDOff();
	bool LEDRed();
	bool LEDGreen();
	bool LEDBlue();
	bool LEDAmber();
	bool LEDTeal();
	bool LEDPurple();
	bool LEDWhite();
	bool LEDCycle();

};
