#include "XbFTP.h"

// Globals
CXBFileZilla g_XbFileZilla;
sFTPServer				 g_ServerSettings;
//CNotificationDebugOutput gNotificationExample;

bool Network_Shutdown()
{
	// TODO: Propper Network Shutdown code.
	return true;
}

// Config FTP
void Network_FTP_Config( sFTPServer ServerSettings )
{
	g_ServerSettings = ServerSettings;

	g_XbFileZilla.mSettings.SetServerPort( g_ServerSettings.Port );
	g_XbFileZilla.mSettings.SetWelcomeMessage( g_ServerSettings.WelcomeMessage );
	g_XbFileZilla.mSettings.SetFreeSpace( "C:\\", true );
	g_XbFileZilla.mSettings.SetFreeSpace( "E:\\", true );
	g_XbFileZilla.mSettings.SetFreeSpace( "F:\\", true );
	g_XbFileZilla.mSettings.SetFreeSpace( "G:\\", true );
	g_XbFileZilla.mSettings.SetFreeSpace( "X:\\", true );
	g_XbFileZilla.mSettings.SetFreeSpace( "Y:\\", true );
	g_XbFileZilla.mSettings.SetFreeSpace( "Z:\\", true );
	g_XbFileZilla.mSettings.SetNoTransferTimeout(9999999);
	g_XbFileZilla.mSettings.SetTimeout(9999999);
	g_XbFileZilla.mSettings.SetSfvEnabled(FALSE);
	g_XbFileZilla.mSettings.SetCrcEnabled(FALSE);

}

// Add User
bool Network_FTP_AddUser( char* Name, char* Password )
{
	CXFUser* ftpUser = NULL;

	XFSTATUS status = g_XbFileZilla.AddUser( Name, ftpUser );

	if( status != XFS_OK )
	{
		delete [] ftpUser;
		ftpUser = NULL;
		return false;
	}

	ftpUser->SetUserLimit( g_ServerSettings.MaxLogins );
    ftpUser->SetPassword( Password );
	ftpUser->SetShortcutsEnabled( true );
    ftpUser->SetUseRelativePaths( false );
	ftpUser->AddDirectory( "C:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS );
	ftpUser->AddDirectory( "D:\\", XBFILE_READ|XBDIR_LIST|XBDIR_SUBDIRS );
	ftpUser->AddDirectory( "E:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS );
	ftpUser->AddDirectory( "F:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS );
	ftpUser->AddDirectory( "G:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS );
	ftpUser->AddDirectory( "X:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS );
	ftpUser->AddDirectory( "Y:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS );
	ftpUser->AddDirectory( "Z:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS );
	ftpUser->AddDirectory( "\\", XBFILE_READ|XBDIR_LIST|XBDIR_SUBDIRS|XBDIR_HOME );
	ftpUser->CommitChanges();

	delete [] ftpUser;
	ftpUser = NULL;
	return true;
}


// Start FTP Server
bool Network_FTP_StartServer( void )
{
	return g_XbFileZilla.Start();
}

// Stop FTP Server
bool Network_FTP_StopServer( void )
{
	return g_XbFileZilla.Stop();
}

void bringup_xbfz()
{
	Network_FTP_StartServer();
	sFTPServer nexgenServerConfig;
	nexgenServerConfig.MaxLogins=2;
	nexgenServerConfig.Port=21;
	strcpy(nexgenServerConfig.WelcomeMessage,"Welcome to Nexgen");
	Network_FTP_Config(nexgenServerConfig);
	Network_FTP_AddUser("xbox","xbox" );
}