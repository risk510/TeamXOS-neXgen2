#include "LoadConfig.h"

struct stDashConfig	myDash;
//DASHBOARD_CONFIG

bool LoadConfig()
{

 	myDash.sXboxName="myxbox";
 	myDash.sSkin="Default.skin";
 	myDash.bAutoLaunchDVD=false;
 	myDash.bAutoLaunchGame=false;
 	myDash.bAutoLaunchUnknown=false;
 	myDash.sDashboard="C:\\xboxdash.xbe";
 
 	myDash.sDVDPlayer="E:\\APPS\\DVDX\\default.xbe";
 	myDash.sUnknownPlayer="E:\\APPS\\XBMC\\default.xbe";
 
 	myDash.iScreenSaverTime=1;
 	myDash.iScreenSaverMode=3;
 	myDash.iScreenSaverFadeLevel=20;
 
 	//myDash.bDHCP=true;
	myDash.bDHCP=true;
 	myDash.sIP="192.168.0.48";
 	myDash.sSubnetMask="255.255.255.0";
 	myDash.sGateway="192.168.0.1";
 	myDash.sDNS1="192.168.0.1";
 	myDash.sDNS2="0.0.0.0";

	myDash.sRemoteFTPHost="192.168.0.30";
	myDash.sRemoteFTPPort="21";
	myDash.sRemoteFTPUser="xbox";
	myDash.sRemoteFTPPass="xbox";
 
 	myDash.sFTPUser="xbox";
 	myDash.sFTPPass="xbox";

	myDash.sHTTPUser="xbox";
 	myDash.sHTTPPass="xbox";
 
 	myDash.sGamesPath="e:\\games;f:\\games";
 	myDash.sEmusPath="e:\\emus;f:\\emus";
 	myDash.sAppsPath="e:\\apps;f:\\apps";
 	myDash.sSoundsPath="c:\\sounds\\waves";
 
 	myDash.bAutoStartMusic=true;
 	myDash.bRandomMusicPlay=true;
 
 	myDash.bUseCache=true;
 	myDash.iFanSpeed=10;
 	myDash.bRegionPatch=true;
 	myDash.iAutoPowerOff=0;
 
 	myDash.bXeniumEnabled=false;
 	myDash.iXeniumMode=1;

	// Load XML Configuration file...
	TiXmlDocument xmlDoc;

	//Path Check
	if ( !xmlDoc.LoadFile( DASHBOARD_CONFIG ) ) 
	{
		OutputDebugString("unable to load:");
		OutputDebugString(DASHBOARD_CONFIG);
		OutputDebugString("\n");
		return false;
	}

	TiXmlElement* pRootElement =xmlDoc.RootElement();
	string strValue=pRootElement->Value();
	if ( strValue != "XOS") return false;

	// Sub Section Config

	// Get General Settings...		
	TiXmlElement *pGeneral = pRootElement->FirstChildElement("General");
	if (pGeneral)
	{
		XmlGetString(pGeneral, "XboxName", myDash.sXboxName);
		XmlGetString(pGeneral, "Skin", myDash.sSkin);
		XmlGetBoolean(pGeneral, "AutoLaunchDVD", myDash.bAutoLaunchDVD);
		XmlGetBoolean(pGeneral, "AutoLaunchGame", myDash.bAutoLaunchGame);
		XmlGetBoolean(pGeneral, "AutoLaunchUnknown", myDash.bAutoLaunchUnknown);
		XmlGetString(pGeneral, "MSDashboard", myDash.sDashboard);

		XmlGetString(pGeneral, "DVDPlayer", myDash.sDVDPlayer);
		XmlGetString(pGeneral, "UnknownPlayer", myDash.sUnknownPlayer);

		XmlGetInteger(pGeneral, "ScreenSaverTime", myDash.iScreenSaverTime);
		XmlGetInteger(pGeneral, "ScreenSaverMode", myDash.iScreenSaverMode);
		XmlGetInteger(pGeneral, "ScreenSaverFade", myDash.iScreenSaverFadeLevel);
	
	}

	// Get Network Settings...		
	TiXmlElement *pNetwork = pRootElement->FirstChildElement("Network");
	if (pNetwork)
	{
		XmlGetBoolean(pNetwork, "DHCP", myDash.bDHCP);
		XmlGetString(pNetwork, "IP", myDash.sIP);
		XmlGetString(pNetwork, "SubnetMask", myDash.sSubnetMask);
		XmlGetString(pNetwork, "Gateway", myDash.sGateway);
		XmlGetString(pNetwork, "DNS1", myDash.sDNS1);
		XmlGetString(pNetwork, "DNS2", myDash.sDNS2);

		XmlGetString(pNetwork, "FTPUser", myDash.sFTPUser);
		XmlGetString(pNetwork, "FTPPass", myDash.sFTPPass);

		XmlGetString(pNetwork, "HTTPUser", myDash.sHTTPUser);
		XmlGetString(pNetwork, "HTTPPass", myDash.sHTTPPass);

		XmlGetString(pNetwork, "RemoteFTPHost", myDash.sRemoteFTPHost);
		XmlGetString(pNetwork, "RemoteFTPPort", myDash.sRemoteFTPPort);
		XmlGetString(pNetwork, "RemoteFTPUser", myDash.sRemoteFTPUser);
		XmlGetString(pNetwork, "RemoteFTPPass", myDash.sRemoteFTPPass);
	
	}

	// Get Search Paths...		
	TiXmlElement *pFileLocation = pRootElement->FirstChildElement("SearchPaths");
	if (pFileLocation)
	{
		XmlGetString(pFileLocation, "Games", myDash.sGamesPath);
		XmlGetString(pFileLocation, "Emulators", myDash.sEmusPath);
		XmlGetString(pFileLocation, "Applications", myDash.sAppsPath);
		XmlGetString(pFileLocation, "Sounds", myDash.sSoundsPath);
	}

	// Get Music Settings...		
	TiXmlElement *pMusicManager = pRootElement->FirstChildElement("Music");
	if (pMusicManager)
	{
		XmlGetBoolean(pMusicManager, "AutoStart", myDash.bAutoStartMusic);
		XmlGetBoolean(pMusicManager, "RandomPlay", myDash.bRandomMusicPlay);
	}

	// Get Advanced Settings...	
	TiXmlElement *pAdvancedSettings = pRootElement->FirstChildElement("Advanced");
	if (pAdvancedSettings)
	{
		XmlGetBoolean(pAdvancedSettings, "UseCache", myDash.bUseCache);
		XmlGetInteger(pAdvancedSettings, "FanSpeed", myDash.iFanSpeed);
		XmlGetBoolean(pAdvancedSettings, "RegionPatch", myDash.bRegionPatch);
		XmlGetInteger(pAdvancedSettings, "AutoPowerOffTime", myDash.iAutoPowerOff);
	}

	// Get XBOX Front LED Settings...	
	TiXmlElement *pLEDConfig = pRootElement->FirstChildElement("Ledcolour");
	if (pLEDConfig)
	{
		XmlGetInteger(pLEDConfig, "Complete", myDash.iLEDComplete);
		XmlGetInteger(pLEDConfig, "Transfer", myDash.iLEDTransfer);
		XmlGetInteger(pLEDConfig, "FTPconnect", myDash.iLEDFTPConnect);
		XmlGetInteger(pLEDConfig, "Error", myDash.iLEDError);
		XmlGetInteger(pLEDConfig, "Startup", myDash.iLEDStartup);
	}


	// Get Xenium Settings...	
	TiXmlElement *pXeniumConfig = pRootElement->FirstChildElement("Xenium");
	if (pXeniumConfig)
	{
		XmlGetBoolean(pXeniumConfig, "Enabled", myDash.bXeniumEnabled);
		XmlGetInteger(pXeniumConfig, "XeniumMode", myDash.iXeniumMode);
	}

	return true;
}