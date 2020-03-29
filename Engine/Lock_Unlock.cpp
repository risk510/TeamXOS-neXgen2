// Lock / Unlock XBOX HDD
#include "Lock_Unlock.h"

BOOL	m_EnryptedRegionValid;
BOOL	m_XBOX_EEPROM_Current;
LPEEPROMDATA EEPROMData;
BOOL EEPROMData_Enc;

XBOX_VERSION xver;

void GetEEpromData()
{
	// Read EEprom from XBOX
	m_xkeeprom.ReadFromXBOX();

	m_XBOX_EEPROM_Current = TRUE;

}


void LockHDD()
{

	    GetEEpromData();
		//lua_pushstring(LUA_STATE, "Locking XBOX HDD");
		//lua_setglobal( LUA_STATE, "XBOX_LOCK_HDD_CHECK1" );

		//lua_pushstring(LUA_STATE, "Creating EEPROM Backup..");
		//lua_setglobal( LUA_STATE, "XBOX_LOCK_HDD_CHECK2" );

		//ATA Command Structure..
		XKHDD::ATA_COMMAND_OBJ hddcommand;
		UCHAR HddPass[32];
		UCHAR MasterPassword[13] = "TEAMASSEMBLY";
		
		//Dont Lock if the EEPROM data was NOT read from XBOX
		if(m_XBOX_EEPROM_Current)
		{
			//Decrypting EEPROM, if it fails.. Display fail message!
			if (m_xkeeprom.Decrypt())
			{
				// Get XBOX Version
				xver = m_xkeeprom.GetXBOXVersion();

				//lua_pushstring(LUA_STATE, "Querying HDD..");
				OutputDebugString("Querying HDD..\n");
				//Get IDE_ATA_IDENTIFY Data for HDD ..
				ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
				hddcommand.DATA_BUFFSIZE = 0;
				hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
				hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
				XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

				XKEEPROM::EEPROMDATA tmpData;
				m_xkeeprom.GetEEPROMData(&tmpData);
				XKHDD::GenerateHDDPwd(tmpData.HDDKkey, hddcommand.DATA_BUFFER, HddPass);

				//Get ATA Locked State
				DWORD SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
				//Check if Disk is already locked..
				if ((SecStatus & IDE_SECURITY_SUPPORTED) && !(SecStatus & IDE_SECURITY_ENABLED))
				{

					OutputDebugString("Setting Master Pasword: TEAMASSEMBLY\n");
					//Execute HDD Unlock..
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_SETPASSWORD;
					LPBYTE HDDP = (LPBYTE)&hddcommand.DATA_BUFFER;
					LPDWORD pMastr = (LPDWORD) HDDP;
					*pMastr = 0x0001; //Set Master Pwd..
					memcpy(HDDP+2, MasterPassword, 13);
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);


					OutputDebugString("Querying HDD..\n");
					//Query HDD To see if we succeeded..
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
					SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
					
					OutputDebugString("Locking HDD..\n");
					//Execute HDD Unlock..
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_SETPASSWORD;
					memcpy(HDDP+2, HddPass, 20);
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);


					//We Found That sometimes that by the first query it hasnt locked yet.. so we query twice if needed..
					int retrycnt = 0;
					do
					{
					
						OutputDebugString("Querying HDD..\n");
						//Query HDD To see if we succeeded..
						ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
						hddcommand.DATA_BUFFSIZE = 512;
						hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
						hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
						XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
						SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);

						retrycnt++;

					} while (!((SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED)) && (retrycnt < 3));

									

					if ((SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED))
					{
						OutputDebugString("Remember Master PASS : TEAMASSEMBLY \n LOCKED HDD!\n");
						//lua_pushstring(LUA_STATE, "XBOX HDD Now Locked..");
					}
					else
					{
						OutputDebugString("FAILED TO LOCK HDD!\n");
						//lua_pushstring(LUA_STATE, "Failed to Lock HDD..");
					}
					lua_setglobal(LUA_STATE, "XBOX_HDD_LOCK_STATUS");

					
				}
				else
				{
					if (!(SecStatus & IDE_SECURITY_SUPPORTED))
					{
						OutputDebugString("HDD DOES NOT SUPPORT LOCKING !\n");
						//lua_pushstring(LUA_STATE, "HDD DOES NOT SUPPORT LOCKING !");
					}
					else
					{
						OutputDebugString("XBOX HDD IS ALREADY LOCKED ???\n");
						//lua_pushstring(LUA_STATE, "XBOX HDD IS ALREADY LOCKED ???");
					}
					//lua_setglobal(LUA_STATE, "XBOX_HDD_LOCK_STATUS");
				}

				// Re-Encrypt EEPROM with current XBOX Version..
				m_xkeeprom.EncryptAndCalculateCRC(xver);

			}
			else
			{
				//Encrypted Region is Invalid.. DONT Allow Locking/Unlocking..
				m_EnryptedRegionValid = FALSE;

				//lua_pushstring(LUA_STATE, "EEPROM ENCRYPTED REGION INVALID");
			}
		}
		else
		{
			//lua_pushstring(LUA_STATE, "LOAD XBOX EEPROM TO LOCK !!");
		}
}

void UnlockHDD()
{
	    //GetEEpromData();
		OutputDebugString("Unlocking HDD \n Please Wait...\n");
		//lua_pushstring(LUA_STATE, "Unlocking HDD Please Wait...");
		//lua_setglobal( LUA_STATE, "XBOX_UNLOCK_HDD_CHECK1" );

		//ATA Command Structure..
		XKHDD::ATA_COMMAND_OBJ hddcommand;
		UCHAR HddPass[32];

		//Dont Lock if the EEPROM data was NOT read from XBOX
		if(m_XBOX_EEPROM_Current)
		{
			//Decrypting EEPROM, if it fails.. Display fail message!
			if (m_xkeeprom.Decrypt())
			{
				OutputDebugString("Querying HDD..\n");

				//Get IDE_ATA_IDENTIFY Data for HDD ..
				ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
				hddcommand.DATA_BUFFSIZE = 0;
				hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
				hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
				XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

				XKEEPROM::EEPROMDATA tmpData;
				m_xkeeprom.GetEEPROMData(&tmpData);
				XKHDD::GenerateHDDPwd(tmpData.HDDKkey, hddcommand.DATA_BUFFER, HddPass);
				
				//Get ATA Locked State
				DWORD SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
				//Check if Disk is Reall locked..
				if ((SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED))
				{
					OutputDebugString("Unlocking HDD..\n");

					//Execute HDD Unlock..
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_UNLOCK;
					LPBYTE HDDP = (LPBYTE)&hddcommand.DATA_BUFFER;
					memcpy(HDDP+2, HddPass, 20);
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

					OutputDebugString("Querying HDD..\n");

					//Query HDD To see if we succeeded..
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

					OutputDebugString("Disable HDD Password..\n");

					//Execute HDD Lock Disable
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_DISABLE;
					memcpy(HDDP+2, HddPass, 20);
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

					OutputDebugString("Querying HDD..\n");

					//We Found That sometimes that by the first query it hasnt unlocked yet.. so we query three times if needed..
					int retrycnt = 0;
					do
					{
					
						//Query HDD To see if we succeeded..
						ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
						hddcommand.DATA_BUFFSIZE = 512;
						hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
						hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
						XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
						SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);

						retrycnt++;

					} while (!((SecStatus & IDE_SECURITY_SUPPORTED) && !(SecStatus & IDE_SECURITY_ENABLED)) && (retrycnt < 3));

									

					if ((SecStatus & IDE_SECURITY_SUPPORTED) && !(SecStatus & IDE_SECURITY_ENABLED))
					{
						OutputDebugString("Remember MASTER PASS: TEAMASSEMBLY \n HDD NOW UNLOCKED !\n");
					}
					else
					{
						OutputDebugString("FAILED TO UNLOCK HDD !\n");
					}

					
				}
				else
				{
					OutputDebugString("XBOX HDD NOT LOCKED ??\n");
				}

				//Re Encrypt EEPROM with Current XBOX Version
				m_xkeeprom.EncryptAndCalculateCRC(xver);

			}
			else
			{
				//Encrypted Region is Invalid.. DONT Allow Locking/Unlocking..
				m_EnryptedRegionValid = FALSE;

				OutputDebugString("EEPROM ENCRYPTED REGION INVALID !\n");
			}
		}
		else
		{
			OutputDebugString("LOAD XBOX EEPROM TO UNLOCK !\n");
		}
}