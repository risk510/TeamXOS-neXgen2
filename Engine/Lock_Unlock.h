// Lock / Unlock XBOX HDD
#include "LUA\\LUACalls.h"
#include "XKUtils\\XKUtils.h"
#include "XKUtils\\XKHDD.h"
#include "XKUtils\\XKEEPROM.h"
#include "XKUtils\\XKGeneral.h"

extern XKEEPROM m_xkeeprom;
extern XBOX_VERSION xver;

extern void GetEEpromData();
extern void LockHDD();
extern void UnlockHDD();
extern void CreateEEPROMBackup(LPCSTR BackupFilePrefix);

extern LPEEPROMDATA EEPROMData;
extern BOOL EEPROMData_Enc;


/*class CXBHDDTools
{
	private:

	BOOL				m_EnryptedRegionValid;
	BOOL				m_XBOX_EEPROM_Current;
	XKEEPROM*			m_pXKEEPROM;
	XBOX_VERSION		m_XBOX_Version;
	LPEEPROMDATA EEPROMData;
	BOOL EEPROMData_Enc;

	public:
	
	// Const & Dest
	CXBHDDTools();
	~CXBHDDTools();

	void LockHDD();
	void UnlockHDD();
};*/