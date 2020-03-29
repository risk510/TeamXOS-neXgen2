#include "SaveConfig.h"
//struct stDashConfig	myDash;
//DASHBOARD_CONFIG

bool SaveConfig()
{
	// Save XML Configuration file...
	TiXmlDocument xmlDoc;
	TiXmlElement xmlRootElement("XOS");
	TiXmlNode *pRoot = xmlDoc.InsertEndChild(xmlRootElement);
	if (!pRoot) return false;

	// General Settings
	TiXmlElement generalNode("General");
	TiXmlNode *pNode = pRoot->InsertEndChild(generalNode);
	if (!pNode) return false;
		XmlSetString(pNode, "XboxName", myDash.sXboxName);
		XmlSetString(pNode, "Skin", myDash.sSkin);

		XmlSetBoolean(pNode, "AutoLaunchDVD", myDash.bAutoLaunchDVD);
		XmlSetBoolean(pNode, "AutoLaunchGame", myDash.bAutoLaunchGame);
		XmlSetBoolean(pNode, "AutoLaunchUnknown", myDash.bAutoLaunchUnknown);

		XmlSetString(pNode, "MSDashboard", myDash.sDashboard);
		XmlSetString(pNode, "DVDPlayer", myDash.sDVDPlayer);
		XmlSetString(pNode, "UnknownPlayer", myDash.sUnknownPlayer);

		XmlSetInteger(pNode, "ScreenSaverTime", myDash.iScreenSaverTime);
		XmlSetInteger(pNode, "ScreenSaverMode", myDash.iScreenSaverMode);
		XmlSetInteger(pNode, "ScreenSaverFade", myDash.iScreenSaverFadeLevel);

	// Network settings
	TiXmlElement networkNode("Network");
	pNode = pRoot->InsertEndChild(networkNode);
	if (!pNode) return false;
		XmlSetBoolean(pNode, "DHCP", myDash.bDHCP);
		XmlSetString(pNode, "IP", myDash.sIP);
		XmlSetString(pNode, "SubnetMask", myDash.sSubnetMask);
		XmlSetString(pNode, "Gateway", myDash.sGateway);
		XmlSetString(pNode, "DNS1", myDash.sDNS1);
		XmlSetString(pNode, "DNS2", myDash.sDNS2);

		XmlSetString(pNode, "FTPUser", myDash.sFTPUser);
		XmlSetString(pNode, "FTPPass", myDash.sFTPPass);

		XmlSetString(pNode, "HTTPUser", myDash.sHTTPUser);
		XmlSetString(pNode, "HTTPPass", myDash.sHTTPPass);

		XmlSetString(pNode, "RemoteFTPHost", myDash.sRemoteFTPHost);
		XmlSetString(pNode, "RemoteFTPPort", myDash.sRemoteFTPPort);
		XmlSetString(pNode, "RemoteFTPUser", myDash.sRemoteFTPUser);
		XmlSetString(pNode, "RemoteFTPPass", myDash.sRemoteFTPPass);

	// Search Paths
	TiXmlElement searchNode("SearchPaths");
	pNode = pRoot->InsertEndChild(searchNode);
	if (!pNode) return false;
		XmlSetString(pNode, "Games", myDash.sGamesPath);
		XmlSetString(pNode, "Applications", myDash.sAppsPath);
		XmlSetString(pNode, "Emulators", myDash.sEmusPath);
		XmlSetString(pNode, "Sounds", myDash.sSoundsPath);

	// Music Options
	TiXmlElement musicNode("Music");
	pNode = pRoot->InsertEndChild(musicNode);
	if (!pNode) return false;
		XmlSetBoolean(pNode, "AutoStart", myDash.bAutoStartMusic);
		XmlSetBoolean(pNode, "RandomPlay", myDash.bRandomMusicPlay);

	// Advanced Options
	TiXmlElement advancedNode("Advanced");
	pNode = pRoot->InsertEndChild(advancedNode);
	if (!pNode) return false;
		XmlSetBoolean(pNode, "UseCache", myDash.bUseCache);
		XmlSetInteger(pNode, "FanSpeed", myDash.iFanSpeed);
		XmlSetBoolean(pNode, "RegionPatch", myDash.bRegionPatch);
		XmlSetInteger(pNode, "AutoPowerOffTime", myDash.iAutoPowerOff);

	// LED Colour
	TiXmlElement ledNode("Ledcolour");
	pNode = pRoot->InsertEndChild(ledNode);
	if (!pNode) return false;
		XmlSetInteger(pNode, "Complete", myDash.iLEDComplete);
		XmlSetInteger(pNode, "Transfer", myDash.iLEDTransfer);
		XmlSetInteger(pNode, "FTPconnect", myDash.iLEDFTPConnect);
		XmlSetInteger(pNode, "Error", myDash.iLEDError);
		XmlSetInteger(pNode, "Startup", myDash.iLEDStartup);

	// Get Xenium Settings...	
	TiXmlElement xeniumNode("Xenium");
	pNode = pRoot->InsertEndChild(xeniumNode);
	if (!pNode) return false;
		XmlSetBoolean(pNode, "Enabled", myDash.bXeniumEnabled);
		XmlSetInteger(pNode, "XeniumMode", myDash.iXeniumMode);

	//Write XML File
	xmlDoc.SaveFile(DASHBOARD_CONFIG);

	return true;
}