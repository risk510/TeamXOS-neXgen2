#include "Network.h"

volatile bool bNetworkingUp = FALSE;

//extern HRESULT httpd_init();
extern void bringup_xbfz();

IN_ADDR MYIP;
SOCKADDR_IN saSendTo;

DWORD netstate;

//NEW Network Code
//(W-I-P)
BOOL InitializeNetwork()
{
	if (!IsEthernetConnected())
		return FALSE;

	// Thanks and credits to Team Evox for the description of the 
	// XNetConfigParams structure.

	//Enable / Disable DHCP
	bool dhcpenabled=myDash.bDHCP;
	//bool dhcpenabled=false;

	TXNetConfigParams configParams;   

	OutputDebugString( "Loading Network Configuration...\r\n" );
	XNetLoadConfigParams( (LPBYTE) &configParams );
	OutputDebugString( "Ready.\r\n" );
	BOOL bXboxVersion2 = (configParams.V2_Tag == 0x58425632 );	// "XBV2"
	BOOL bDirty = FALSE;
	if (bXboxVersion2)
		OutputDebugString( "IbXboxVersion2.\r\n" );
	else
		OutputDebugString( "no bXboxVersion2.\r\n" );

	/*OutputDebugString( "IP Address: " );
	OutputDebugString( myDash.sIP.c_str() );
	OutputDebugString( "\n" );*/
	if (bXboxVersion2)
	{
		if (configParams.V2_IP != inet_addr(myDash.sIP.c_str()))
		{
			configParams.V2_IP = inet_addr(myDash.sIP.c_str());
			bDirty = TRUE;
		}
	}
	else
	{
		if (configParams.V1_IP != inet_addr(myDash.sIP.c_str()))
		{
			configParams.V1_IP = inet_addr(myDash.sIP.c_str());
			bDirty = TRUE;
		}
	}

	/*OutputDebugString( "Subnet: " );
	OutputDebugString( myDash.sSubnetMask.c_str() );
	OutputDebugString( "\n" );*/

	if (bXboxVersion2)
	{
		if (configParams.V2_Subnetmask != inet_addr(myDash.sSubnetMask.c_str()))
		{
			configParams.V2_Subnetmask = inet_addr(myDash.sSubnetMask.c_str());
			bDirty = TRUE;
		}
	}
	else
	{
		if (configParams.V1_Subnetmask != inet_addr(myDash.sSubnetMask.c_str()))
		{
			configParams.V1_Subnetmask = inet_addr(myDash.sSubnetMask.c_str());
			bDirty = TRUE;
		}
	}

	/*OutputDebugString( "Gateway: " );
	OutputDebugString( myDash.sGateway.c_str() );
	OutputDebugString( "\n" );*/

	if (bXboxVersion2)
	{
		if (configParams.V2_Defaultgateway != inet_addr(myDash.sGateway.c_str()))
		{
			configParams.V2_Defaultgateway = inet_addr(myDash.sGateway.c_str());
			bDirty = TRUE;
		}
	}
	else
	{
		if (configParams.V1_Defaultgateway != inet_addr(myDash.sGateway.c_str()))
		{
			configParams.V1_Defaultgateway = inet_addr(myDash.sGateway.c_str());
			bDirty = TRUE;
		}
	}

	//DNS Servers
	/*OutputDebugString( "DNS Servers: " );
	OutputDebugString( "\n" );
	OutputDebugString( "DNS1: " );
	OutputDebugString( myDash.sDNS1.c_str() );
	OutputDebugString( "\n" );
	OutputDebugString( "DNS2: " );
	OutputDebugString( myDash.sDNS2.c_str() );
	OutputDebugString( "\n" );*/

	if (bXboxVersion2)
	{
		if (configParams.V2_DNS1 != inet_addr(myDash.sDNS1.c_str()))
		{
			configParams.V2_DNS1 = inet_addr(myDash.sDNS1.c_str());
			bDirty = true;
		}
	}
	else
	{
		if (configParams.V1_DNS1 != inet_addr(myDash.sDNS1.c_str()))
		{
			configParams.V1_DNS1 = inet_addr(myDash.sDNS1.c_str());
			bDirty = true;
		}
	}

	if (bXboxVersion2)
		{
			if (configParams.V2_DNS2 != inet_addr(myDash.sDNS2.c_str()))
			{
				configParams.V2_DNS2 = inet_addr(myDash.sDNS2.c_str());
				bDirty = true;
			}
		}
		else
		{
			if (configParams.V1_DNS2 != inet_addr(myDash.sDNS2.c_str()))
			{
				configParams.V1_DNS2 = inet_addr(myDash.sDNS2.c_str());
				bDirty = true;
			}
		}
	//-----------

	//DHCP: Static / Auto Assign IP Address.
	//Auto IP
	if ( dhcpenabled == true)
	{
		if (configParams.Flag != (0) )
		{
			configParams.Flag = 0;
			bDirty = TRUE;
		}
	}

	//Fixed IP
	else if ( dhcpenabled == false )
	{
		if (configParams.Flag != (0x04|0x08) )
		{
			configParams.Flag = 0x04 | 0x08;
			bDirty = TRUE;
		}
	}
	

	if (bDirty) XNetSaveConfigParams((LPBYTE)&configParams);

	// Initialize Xbox Networking
	XNetStartupParams xnsp;
	memset(&xnsp,0,sizeof(xnsp));

	xnsp.cfgSizeOfStruct=sizeof(XNetStartupParams);
	xnsp.cfgFlags=XNET_STARTUP_BYPASS_SECURITY;
	
	// create more memory for networking
	xnsp.cfgPrivatePoolSizeInPages = 64; // == 256kb, default = 12 (48kb)
	xnsp.cfgEnetReceiveQueueLength = 16; // == 32kb, default = 8 (16kb)
	xnsp.cfgIpFragMaxSimultaneous = 16; // default = 4
	xnsp.cfgIpFragMaxPacketDiv256 = 32; // == 8kb, default = 8 (2kb)
	xnsp.cfgSockMaxSockets=64; // default = 64
	xnsp.cfgSockDefaultRecvBufsizeInK=56;//128; // default = 16
	xnsp.cfgSockDefaultSendBufsizeInK=56;//128; // default = 16
	INT err = XNetStartup(&xnsp);

	XNADDR xna;
	DWORD dwState;
	do
	{
		dwState = XNetGetTitleXnAddr(&xna);
		Sleep(500);
	} while (dwState==XNET_GET_XNADDR_PENDING);
	
	WSADATA WsaData;
    if (XNetStartup(&xnsp)) return false;
	 
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2,2),&WsaData)) return false;

	//Initialize HTTP and FTP Servers
	bNetworkingUp=TRUE;

	//httpd_init(); // Initialise HTTP Server.
	bringup_xbfz(); // Initialise FTP Server.

	return true;
}

BOOL IsEthernetConnected()
{
	if (!(XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE))
		return FALSE;

	return TRUE;
}