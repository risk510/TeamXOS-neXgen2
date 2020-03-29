#include "ftpclient.h"
#include "ftpparse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>
//#include "nexgendash.h"
#include <conio.h> 
#include "..\Menu.h"
//#include "settings.h"
#include "..\Main.h"

//extern MenuList Menu[8];

extern int ftpparse(struct ftpparse *fp,char *buf,int len);
extern int progress;
extern DWORD progressbar;
extern DWORD progressmax;

ftp::ftp() {
	Connected = 0;
	hListenSocket = INVALID_SOCKET;
	hControlSocket = INVALID_SOCKET;
	hDataSocket = INVALID_SOCKET;
	bSendPort   = 1;
	ReadCommand = 0;
	bMode=ASCII;

	InitWinsock();
	strcpy(szLocalFtpRoot, "/");
};

ftp::~ftp() {
	
};

void ftp::getrealpath(char *out, char *in) {
	register DWORD o=0, i=0;
	register char * ptr;

	while(in[i]=='/') i++;

	ptr=strchr(in+i, '/');
	if(ptr!=NULL) {
		while((in+i)<ptr) {
			out[o]=toupper(in[i]);
			o++; i++;
		}
		strcpy(out+o, ":\\");
		o++;

		while(in[i]!=0) {
			if(in[i]=='/') out[o]='\\';
			else out[o]=in[i];
			o++; i++;
		}
	} else { /* no slashes in path */
		while(in[i]!=0) {
			out[o]=in[i];
			o++; i++;
		}
		strcpy(out+o, ":\\");
		o++;
	}

	out[o]=0;

#if 1
	OutputDebugString("Unix path: '");
	OutputDebugString(in);
	OutputDebugString("'  ");
	OutputDebugString("Real path: ");
	OutputDebugString(out);
	OutputDebugString("\n");
#endif
}

void ftp::transformpath(char *out, char* s1, char* s2) {
	if((out == NULL) || (s1 == NULL) || (s2 == NULL)) return;

	if((s2[0]!='/') && (strlen(s1) > 1)) {
		sprintf(out, "%s/%s",  s1, s2);
	} else {
		strcpy(out, s2);
	}
}

int ftp::filevalid(char *param, char *rpout) {
	register char *filepath = (char *)malloc(strlen(szLocalFtpRoot) + strlen(param) + 16);
	if(filepath == NULL) {
		return 0;
	}
	register char *realpath = (char *)malloc(strlen(szLocalFtpRoot) + strlen(param) + 16);
	if(realpath == NULL) {
		free(filepath);
		return 0;
	}

	transformpath(filepath, szLocalFtpRoot, param);
	getrealpath(realpath, filepath);
	free(filepath);
	if(rpout!=NULL) strcpy(rpout, realpath);

	HANDLE tfile = CreateFile(realpath, 0, 0, NULL, OPEN_EXISTING, 0, NULL);

	free(realpath);

	if(tfile != INVALID_HANDLE_VALUE) {
		CloseHandle(tfile);
		return 1;
	}

	return 0;
}

int ftp::pathvalid(char *param, char *rpout) {
	register char *filepath = (char *)malloc(strlen(szLocalFtpRoot) + strlen(param) + 16);
	if(filepath == NULL) {
		return 0;
	}
	register char *realpath = (char *)malloc(strlen(szLocalFtpRoot) + strlen(param) + 16);
	if(realpath == NULL) {
		free(filepath);
		return 0;
	}

	transformpath(filepath, szLocalFtpRoot, param);
	getrealpath(realpath, filepath);
	free(filepath);
	if(rpout!=NULL) strcpy(rpout, realpath);

	HANDLE tfile = CreateFile(realpath, 0, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	free(realpath);
	if(tfile != INVALID_HANDLE_VALUE) {
		CloseHandle(tfile);
		return 1;
	}
	return 0;
}

/*
 * DoOpen
 * this function is called when the o,open command is issued.
 * it connects to the requested host, and logs the user in
 *
 */
HRESULT ftp::DoOpen( char *command)
{
  char *szHost=NULL;  /* remote host */
  char *port=NULL;
  char tmp[100] = {0};
  char *portAddr;

  /* 
    * do not do anything if we are already connected.
    */
   if( Connected ) {
	   OutputDebugStringA("ftpclient: Already connected.  Close connection first.\n");
       return S_FALSE;
   }
   
   szHost = strdup(command);

// check for port number
   if ( portAddr = strrchr(szHost, ':') )
   {
		port = strdup(&portAddr[1]);
		*portAddr = '\0';
   }

   sprintf(tmp, "ftpclient: Connecting to %s\r\n",szHost);
   OutputDebugStringA(tmp);
   if (port)
		hControlSocket = ConnectToServer(szHost,port);
   else
		hControlSocket = ConnectToServer(szHost,"21");


   Sleep(1);

   if( hControlSocket != INVALID_SOCKET)  {
	 sprintf(tmp,"ftpclient: Connected to %s\r\n", szHost);
	 OutputDebugStringA(tmp);  // set console window title 

     Connected = 1;         /* we ar now connected */
     GetReply();            /* get reply (welcome message) from server */
     //DoLogin((char *)NULL); /* prompt for username and password */
     //DoBinary();            /* default binary mode */
   }
   free(szHost); /* free the strdupped string */
   free(port);

   if (hControlSocket == INVALID_SOCKET)
	   return S_FALSE;
   else
	   return S_OK;
}



/*
 * DoLogin
 * this function logs the user into the remote host.
 * prompts for username and password
 * or parses username and password from command (user <username>:<password>)
 */
HRESULT ftp::DoLogin( char *logincommand)
{
	char *User=NULL;
	char *Password=NULL;
	int iReply;

	if( logincommand && *logincommand)
		User=strdup(logincommand);

	if( Connected )
	{
		/* 
		 * ignore leading whitespace
		 */
		while(User && (*User == ' ' || *User == '\t') && *User)
			User++;
		Password = strchr(User, ':');
		if (Password)
		{
			OutputDebugString("ftpclient: password given\n");
			*Password = '\0';
			Password++;
		};

		/*
		 * send user name & password to server  & get reply message
		 */
		sprintf(szBuffer,"USER %s\r\n",User);
		SendControlMsg(szBuffer,strlen(szBuffer));
		iReply = GetReply();
		if ( iReply != 331 )
		{
			OutputDebugString("ftpclient: Invalid username\n");
			return S_FALSE;
		}
		sprintf(szBuffer,"PASS %s\r\n",Password);
		SendControlMsg(szBuffer,strlen(szBuffer));
		int iReply = GetReply();
		if ( iReply == 530 )
		{
			OutputDebugString("ftpclient: Wrong password\n");
			return S_FALSE;
		}
	}
	else
		OutputDebugString("ftpclient: Not Connected.\n");

	free(User); /* free the strdupped string */
	return S_OK;
}

/*
 * DoClose
 * closes connection to the ftp server
 */
void ftp::DoClose( void )
{
   if( !Connected  ) {
     printf("Not Connected.\n");
   } else {
       Menu[6].Clear();
	   SendControlMsg("QUIT\r\n",6);
	   GetReply();
	   CloseControlConnection();
	   hControlSocket = INVALID_SOCKET;
	   OutputDebugStringA("ftpclient: Connection closed\r\n");
	   Connected = 0;
	   //WSACleanup();
   }

}

/*
 * DoList
 * perform directory listing i.e: ls
 */
void ftp::DoList( char *command)
{
	char* szDirBuffer, *cutPos;
	struct ftpparse fp;
	char tmp[100] = {0};

	if( !Connected )
	{
		printf("Not Connected.\n");
		return;
   }

   /*
    * obtain a listening socket
    */
	if( GetListenSocket() == INVALID_SOCKET)
	{
		printf("Cannot obtain a listen socket.\n");
		return;
	}
   
   /*
    * parse command
    */
	if( !strcmp(command,"ls") )
	{
		sprintf(szBuffer,"NLST\r\n");
	}
	else if( !strcmp(command,"dir") ) 
		sprintf(szBuffer,"LIST\r\n");

	else if( !strcmp(command,"dirFiltered") ) 
		sprintf(szBuffer,"LIST\r\n");

	else if( !strncmp(command, "ls ",3))
	{
		while( *command == ' ') command++;
		sprintf(szBuffer,"LIST %s\r\n",&command[3]);
	}

	/*
	 * send command to server and get response
	 */
	SendControlMsg(szBuffer,strlen(szBuffer));
	GetReply();
	memset(szBuffer,0,1024);

	/*
	 * accept connection and open data socket
     */
	if(AcceptConnection() == INVALID_SOCKET)
	{
		printf("Cannot accept connection.\n");
		return;
	}

	CloseListenSocket();       /* close listening socket */

	/*
     * display directory listing.
     */
	Menu[6].Clear();
	Menu[6].AddURL("..", 0, 1);
	while (1)
	{
		szDirBuffer = GetDataLine();
		if (szDirBuffer == NULL)
			break;


		// cut off end-of-line
		cutPos = strchr(szDirBuffer, '\r');
		*cutPos = 0;
		if( !strcmp(command,"dirFiltered") )
		{
			if (ftpparse(&fp, szDirBuffer, strlen(szDirBuffer)))
			{
				if(strcmp(fp.name, ".")) {
					Menu[6].AddURL(fp.name, fp.size, fp.flagtrycwd);
					Menu[6].IndexList();
					OutputDebugStringA(fp.name);
					OutputDebugStringA(", fileSize: ");
					sprintf(tmp, "%d", fp.size);
					OutputDebugString(tmp);
					
				}
			}
		}
		else
			OutputDebugStringA(szDirBuffer);
		
		OutputDebugStringA("\n");
   }

/*   while( ReadDataMsg(szBuffer,1024) > 0)
   {
       OutputDebugStringA(szBuffer);
       memset(szBuffer,0,1024);
   }
*/

   CloseDataConnection(hDataSocket);

   /*
    * read response
    */
   (void)GetReply();

}

/*
 * DoCD
 * chang to another directory on the remote system
 */
HRESULT ftp::DoCD( char *command)
{
	char *dir=command;

	if( !Connected )
	{
		OutputDebugString("ftpclient: Not Connected.\n");
		return S_FALSE;
	}

	/*
     * ignore leading whitespace
     */
	while( *dir && (*dir == ' ' || *dir == '\t') ) 
		dir++;

	/*
     * if dir is not specified, read it in
     */
	if( !(*dir) )
	{
	OutputDebugString("ftpclient wrong usage, use: cd remote-directory\n");
	return S_FALSE;
	}
   
	/*
     * send command to server and read response
     */
	sprintf(szBuffer, "CWD %s\r\n",dir);
	SendControlMsg(szBuffer,strlen(szBuffer));
	int iReturn = GetReply();
	if (iReturn == 550)
		return S_FALSE;				// 550 = Directory Not Found
	else
		Menu[6].Clear();
		return S_OK;
}

/*
 * DoLCD
 * change directory on the local system
 */
void ftp::DoLCD( char *command)
{

   char *dir = command;

   while(*dir && (*dir == ' ' || *dir == '\t') ) dir++;

   /*
    * if dir is not specified, then print the current dir
    */
   if( ! *dir ) {
      dir = szLocalFtpRoot;
      if(!dir)
		perror("getcwd");
      else
		printf("Current directory is: %s\n",dir);
   }
   else {
		if(!strcmp(dir,"..")) {
			char *t=strrchr(szLocalFtpRoot, '/');
			if(t!=NULL) {
				if(t!=szLocalFtpRoot)
					t[0]=0;
				else
					t[1]=0;
			}
		} else if(dir[0]=='/') {
			strcpy(szLocalFtpRoot, dir);
		} else {
			if(szLocalFtpRoot[strlen(szLocalFtpRoot)-1]!='/')
				strcat(szLocalFtpRoot, "/");
			strcat(szLocalFtpRoot, dir);
		}
		dir = szLocalFtpRoot;
		if(!dir)
			perror("getcwd");
		else
		printf("Current directory is: %s\n",dir);
   }
}

/*
 * DoLLS
 * perform local directory listing.  winqvt implements this, but
 * this is not supported by many systems.  it's not really needed
 * since we already have '!' command.  you can just do !ls or !dir.
 */
void ftp::DoLLS( char *command )
{
	char tmpb[MAX_PATH];
	char tmpc[MAX_PATH];
	HANDLE hFD;
	WIN32_FIND_DATA fd;

	// Clear Local File List
	Menu[5].Clear();

	if(!strcmp(szLocalFtpRoot, "/")) {
		//if (g_stSettings.m_bClassicDrives){
		Menu[5].AddURL("C", 0, 1);
		Menu[5].AddURL("E", 0, 1);
		Menu[5].AddURL("F", 0, 1);
		Menu[5].AddURL("G", 0, 1);
		Menu[5].AddURL("X", 0, 1);
		Menu[5].AddURL("Y", 0, 1);
		Menu[5].AddURL("Z", 0, 1);
		Menu[5].AddURL("DVD", 0, 1);
		Menu[5].IndexList();
		//}else{
		/*Menu[5].AddURL("SYSTEM", 0, 1);
		Menu[5].AddURL("STORAGE", 0, 1);
		Menu[5].AddURL("EXTENDED", 0, 1);
		Menu[5].AddURL("EXTENDED G", 0, 1);
		Menu[5].AddURL("TEMPX", 0, 1);
		Menu[5].AddURL("TEMPY", 0, 1);
		Menu[5].AddURL("TEMPZ", 0, 1);
		Menu[5].AddURL("DVD", 0, 1);
		Menu[5].IndexList();
		}*/
	} else {

		Menu[5].AddURL("..", 0, 1);

		transformpath(tmpb, szLocalFtpRoot, command);
		getrealpath(tmpc, tmpb);

		if((hFD=FindFirstFile(tmpc, &fd))!=INVALID_HANDLE_VALUE) {
			for(int z=0;z<128;) {
				Menu[5].AddURL(fd.cFileName, fd.nFileSizeLow, (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0);
				if(!FindNextFile(hFD, &fd)) {
					FindClose(hFD);
					break;
				}
			}
		}
		Menu[5].IndexList();
	}
}

/*
 * DoBinary
 * set file transfer mode to binary
 */
void ftp::DoBinary()
{
  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
   sprintf(szBuffer, "TYPE I\r\n");
   SendControlMsg(szBuffer,strlen(szBuffer));
   GetReply();
   printf("File transfer modes set to binary.\n");
   bMode = BINARY;
}

/*
 * DoAscii
 * set file transfer mode to ascii text
 */
void ftp::DoAscii()
{
  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
   sprintf(szBuffer, "TYPE A\r\n");
   SendControlMsg(szBuffer,strlen(szBuffer));
   GetReply();
   printf("File transfer modes set to ascii.\n");
   bMode = ASCII;

}

/*
 * DoGet
 * retrieve a file from the remote host.  calls GetFile(..)
 */
void ftp::DoGet( char *command)
{

  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
  //(void)strtok(command," ");
  //GetFile(strtok((char *)NULL, " "));
  for(int i=0; i<Menu[6].NodeCount(); i++) {
	  if(!strcmp(Menu[6].IndexedMenu[i]->File, command))
	  {
		  /* neXgen 2 Fixup Later*/
		  //progressmax=Menu[6].IndexedMenu[i]->DBlocks;
	  }
  }
  GetFile(command);
}

/*
 * DoPut
 * send a file to the remote host.  calls PutFile(..)
 */
void ftp::DoPut( char *command )
{
  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
  //(void)strtok(command," ");
  //PutFile(strtok((char *)NULL, " "));
  PutFile(command);
}

/*
 * DoRhelp
 * sends a help command to the server.
 */
void ftp::DoRhelp( char *command )
{
  char *szCommand;

  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
  (void)strtok(command," ");
  szCommand=strtok((char *)NULL, " ");

  if( szCommand && *szCommand )
     sprintf(szBuffer,"HELP %s\r\n",szCommand);
  else 
     sprintf(szBuffer, "HELP\r\n");
  
  SendControlMsg(szBuffer, strlen(szBuffer));
  GetReply();
}

/*
 * retrieves the current directory on the remote host
 */
void ftp::DoPWD()
{

  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
  sprintf(szBuffer, "PWD\r\n");
  SendControlMsg(szBuffer,strlen(szBuffer));
  GetReply();
}



int ftp::GetListenSocket()
{
    int sockfd, flag=1,len;
    struct sockaddr_in  serv_addr, TempAddr;
    char *port;
    char szBuffer[64]={0};
	XNADDR xnaddr;
	IN_ADDR ipaddr;
	char szip[16];
	char tmp[100] = {0};
	XNetGetTitleXnAddr( &xnaddr );
	ipaddr=xnaddr.ina;
	XNetInAddrToString( ipaddr, szip, 16);


    /*
     * Open a TCP socket (an Internet stream socket).
     */

	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		perror("socket");
		return INVALID_SOCKET;
	}



    /*
     * Fill in structure fields for binding
     */

   if( bSendPort )
   {
       bzero((char *) &serv_addr, sizeof(serv_addr));
       serv_addr.sin_family      = AF_INET;
       serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
       serv_addr.sin_port        = htons(0); /* let system choose */
   }

   else
   {
     /* reuse the control socket then */
      if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, (char *)&flag,sizeof(flag)) < 0)
	  {
		perror("setsockopt");
		shutdown(sockfd, SD_SEND);
		closesocket(sockfd);
		return INVALID_SOCKET;
      }

   }

    /*
     * bind the address to the socket
     */

    if (bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
	{
		perror("bind");
		shutdown(sockfd, SD_SEND);
        closesocket(sockfd);
		return INVALID_SOCKET;
    }

    len = sizeof(serv_addr);

    if(getsockname(sockfd, (struct sockaddr *)&serv_addr, &len)<0)
	{
	   perror("getsockname");
	   shutdown(sockfd, SD_SEND);
       closesocket(sockfd);
       return INVALID_SOCKET;
    }

    len = sizeof(TempAddr);

    if(getsockname(hControlSocket, (struct sockaddr *)&TempAddr, &len)<0)
	{
       perror("getsockname");
	   shutdown(sockfd, SD_SEND);
       closesocket(sockfd);
       return INVALID_SOCKET;
    }

//    ipaddr = (char *)&TempAddr.sin_addr;
    port  = (char *)&serv_addr.sin_port;

#define  UC(b)  (((int)b)&0xff)

	char* pos = strchr(szip, '.');
	*(pos++) = ',';
	pos = strchr(szip, '.');
	*(pos++) = ',';
	pos = strchr(szip, '.');
	*(pos++) = ',';

	sprintf(szBuffer,"PORT %s,%d,%d\r\n", szip, UC(port[0]), UC(port[1]));

	/*
     * allow ftp server to connect
     * allow only one server
     */

    if( listen(sockfd, SOMAXCONN) < 0)
	{
		perror("listen");
		shutdown(sockfd, SD_SEND);
        closesocket(sockfd);
		return INVALID_SOCKET;
    }

    SendControlMsg(szBuffer,strlen(szBuffer));
    GetReply();
    hListenSocket = sockfd;
    return sockfd;
}



int ftp::AcceptConnection()
{

	struct sockaddr_in cli_addr;
    int clilen = sizeof(cli_addr);
    int sockfd;
    sockfd = accept(hListenSocket, (struct sockaddr *) &cli_addr, &clilen);

    if (sockfd == INVALID_SOCKET)
	{
        perror("accept");
		return INVALID_SOCKET;
    }

    hDataSocket = sockfd;
    return sockfd;
}





int ftp::ConnectToServer(char *name, char *port)
{
  int s;
  unsigned int portnum;
  struct sockaddr_in server;
  char tmp[100] = {0};
//  struct hostent *hp;
//  struct timeval tv;
  bool optint = TRUE;
  struct linger {
	  u_short    l_onoff;
	  u_short    l_linger;
  };

  while( name && *name == ' ') name++;

  if( !name || ! (*name) )
      return INVALID_SOCKET;

  portnum = atoi(port);
  bzero((char *) &server, sizeof(server));

  if( isdigit(name[0])) {
   server.sin_family      = AF_INET;
   server.sin_addr.s_addr = inet_addr(name);
   server.sin_port        = htons(portnum);
  }
/*
  else{ 
   if ( (hp = gethostbyname(name)) == NULL)
	 if (1)
    {
#if (!defined( _WIN32 ) || !defined( WIN32 ))
      perror("gethostbyname");
#else 
	  printf("gethostbyname: error code: %d\r\n", WSAGetLastError());
#endif
	  return INVALID_SOCKET;
    }

    bcopy(hp->h_addr,(char *) &server.sin_addr,hp->h_length);
    server.sin_family = hp->h_addrtype;
    server.sin_port = htons(portnum);  
   }/* else */
  /* create socket */

  if( (s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
  {
	sprintf(tmp, "socket: error code: %d\r\n", WSAGetLastError());
	OutputDebugStringA(tmp);
	return INVALID_SOCKET;
  }

/*
  memset(&tv, 0, sizeof(tv));
  tv.tv_sec = 30 ; tv.tv_usec = 0;

  setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char *) &tv, sizeof(tv));
  setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv));
  */

  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, "1",1);

  if (connect(s,(struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
  {
//	  if (connect(s,(struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
//	  {
		  sprintf(tmp, "connect: error code: %d\r\n", WSAGetLastError());
		OutputDebugStringA(tmp);
		return INVALID_SOCKET;
//	  }
  }

  linger l;
  l.l_onoff = 1;
  l.l_linger = 0;

  setsockopt(s,SOL_SOCKET,SO_LINGER,(char*) &l,sizeof(l));
//  setsockopt(s,SOL_SOCKET,SO_REUSEADDR,0,0);
//  setsockopt(s,SOL_SOCKET,SO_KEEPALIVE,0,0);

  hDataSocket = s;
  return s;

}



int ftp::ReadControlMsg( char *szBuffer, int len)

{
  int ret;
  if( (ret=recv(hControlSocket,szBuffer,len,0)) <= 0)
       return 0;

  return ret;
}



int ftp::SendControlMsg( char *szBuffer, int len)
{
   if( send(hControlSocket,szBuffer,len,0) <= 0)
	   return 0;
   return 1;
}



int ftp::ReadDataMsg( char *szBuffer, int len)

{
   int ret;
   if( (ret=recv(hDataSocket,szBuffer,len,0)) <= 0)
       return 0;
   return ret;
}



int ftp::SendDataMsg( char *szBuffer, int len)
{
   if( send(hDataSocket,szBuffer,len,0) <= 0)
       return 0;
   return 1;
}



void ftp::CloseDataConnection( int hDataSocket )
{
	  shutdown(hDataSocket, SD_SEND);
      closesocket(hDataSocket);
      hDataSocket = INVALID_SOCKET;
}



void ftp::CloseControlConnection( void )
{
	  shutdown(hControlSocket, SD_SEND);
      closesocket(hControlSocket);
      hControlSocket = INVALID_SOCKET;
}


void ftp::CloseListenSocket( void )
{
	  shutdown(hListenSocket, SD_SEND);
      closesocket(hListenSocket);
      hListenSocket = INVALID_SOCKET;
}


int ftp::CheckControlMsg( char *szPtr, int len)
{
//    return recv(hControlSocket,szPtr,len,MSG_PEEK);
	    return recv(hControlSocket,szPtr,len,0);
}

char* ftp::GetDataLine()
{
	int done=0, iRetCode =0, iLen, iBuffLen=0;
	char *szPtr = szBuffer, nCode[3]={0},ch=0;
	bool aborted = false;
	while( (iBuffLen < 1024) )
	{
		iLen = ReadDataMsg(&ch,1);
		if (iLen <= 0)
		{
			aborted = true;
			break;
		}
		iBuffLen += iLen;
		*szPtr = ch;
		szPtr += iLen;

		if( ch == '\n' )
			break;    // we have a line: return
	}

	if (aborted)
		return NULL;

	*(szPtr+1) = (char)0;
	return szBuffer;
}

int ftp::GetLine()
{
   int done=0, iRetCode =0, iLen, iBuffLen=0;
   char *szPtr = szBuffer, nCode[3]={0},ch=0;

   while( (iBuffLen < 1024) )
   {
        iLen = ReadControlMsg(&ch,1);
		if (iLen <= 0)
			break;
		iBuffLen += iLen;
		*szPtr = ch;
		szPtr += iLen;

		if( ch == '\n' )
		    break;    // we have a line: return
   }

   *(szPtr+1) = (char)0;

   strncpy(nCode, szBuffer, 3);
   return (atoi(nCode));
}



int ftp::GetReply()
{
   int done = 0, iRetCode = 0;

   memset(szBuffer,0,1024);

   while(!done )
   {
        iRetCode = GetLine();
		(void)strtok(szBuffer,"\r\n");

		OutputDebugStringA(szBuffer);
		OutputDebugStringA("\n");

		if( szBuffer[3] != '-' && iRetCode > 0 )
		    done = 1;

		memset(szBuffer,0,1024);
   }

   return iRetCode;

}



int ftp::CheckInput()
{
//  int rval; //, i;
 //fd_set readfds, writefds; //, exceptfds;
// struct timeval timeout;



#if (0)
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
  FD_CLR (fileno(stdin),&readfds);

  timeout.tv_sec = 0 ;                /* 0-second timeout. */
  timeout.tv_usec = 0 ;               /* 0 microseconds.  */
  FD_SET(fileno(stdin),&readfds);

  i=select ( fileno(stdin)+1,
	         &readfds,
			 &writefds,
			 &exceptfds,
			 &timeout);
 /* SELECT interrupted by signal - try again.  */

  if (errno == EINTR && i ==-1)  {
 	  return 0;

  }
  return ( FD_ISSET(fileno(stdin),&readfds) );
#else
	   return ( 0 );
#endif

}

int  ftp::CheckFds( char *command)
{
 int rval, i;
 fd_set readfds, writefds, exceptfds;
 struct timeval timeout;
 char *szInput=command;


/*  memset(command,0,1024); */
  memset(szBuffer,0,1024);
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
#if (!defined(WIN32) || !defined(_WIN32) )
  FD_CLR (fileno(stdin),&readfds);
#endif
  if( hControlSocket > 0) 
      FD_CLR (hControlSocket,&readfds);
  timeout.tv_sec = 0 ;                /* 1-second timeout. */
  timeout.tv_usec = 0 ;               /* 0 microseconds.  */

#if (!defined( _WIN32 ) || !defined( WIN32 ) )
  FD_SET(fileno(stdin),&readfds);
#endif

  if( hControlSocket > 0) 
      FD_SET(hControlSocket,&readfds);

  i=select ((hControlSocket > 0) ? (hControlSocket+1) : 1,
	         &readfds,
			 &writefds,
			 &exceptfds,
			 &timeout);
 /* SELECT interrupted by signal - try again.  */
if (errno == EINTR && i ==-1)  {
   /*memset(command,0,1024);*/
   return 0;
}


if( (hControlSocket > 0) && FD_ISSET(hControlSocket, &readfds) ) 
{
  if ( ( rval = ReadControlMsg(szBuffer,1024))  > 0)
	  {
	  printf(szBuffer);
	  printf("ftp>");
	  fflush(stdout);
	  return 0;
	  }
  else {
	 OutputDebugStringA("ftpclient: Connection closed by server\n");
	 CloseControlConnection();
	 hControlSocket = -1;
	 return 0;
  }

 }

#if (!defined( _WIN32 ) || !defined( WIN32 ) )

   if( FD_ISSET(fileno(stdin),&readfds) )  
       return (ReadCommand = GetUnixInput(command));
   return (ReadCommand = 0);
#else
    
   return (ReadCommand = GetWin32Input(command));

#endif

}


int ftp::InitWinsock()
{

	WSADATA WsaData;

	if( !WSAStartup(0x0101,&WsaData) ) {
		OutputDebugStringA("ftpclient: Connection closed\n");
		return 1;
	}
	else {
	 printf("Winsock cannot be started\r\n");
	 return 0;
	}
	
}

void ftp::CleanUp()
{

	WSACleanup();
}



/*
 * GetPassword
 * read in the user's password.  turn off echoing before reading.
 * then turn echoing back on.
 */
void ftp::GetPassword( char *szPass )
{
     
#if (0) 
     EchoOff();
     (void)printf("Password:");
     (void)fgets(szPass,PASSWORD_LENGTH,stdin);
     (void)strtok(szPass,"\n");
     EchoOn(stdin,1);
//#else
     /*
      * for WIN32 we just use getch() instead of getche()
      */
     int i=0;
     char ch=0;
	 
	 (void)printf("Password:");

     while( (i < PASSWORD_LENGTH) && ( ch != '\r') ) {
	  
	  ch = getch();
	  if( ch != '\r' && ch != '\n')
	     szPass[i++] = ch;
     }
#endif
//     (void)printf("\r\n");
}

/*
 * GetUnixInput
 * function called to get user input on the UNIX side.  
 */

int ftp::GetUnixInput(char *command)
{
#if (!defined( _WIN32 ) || !defined( WIN32 )) 
	char ch;
	int i;
	
	/*
	 * ignore leading whitespace
	 */
	while( (ch=getchar()) == ' ' || ch == '\t') ;
	
	if( ch != '\n') {
	    command[0] = ch;
	    fgets(&command[1],1024,stdin);
	    strtok(command,"\n");
	    i = strlen(command) - 1;

	    while( i>0 && isspace(command[i]))
	       i--;
	    if( i>= 0)
	       command[i+1] = 0;

	}
	return 1;
#else
       return 0;
#endif
}

/*
 * GetWin32Input
 * called to get input on the WIN32 side.
 */
int ftp::GetWin32Input( char *command)
{
#if (0) 
   char ch;
   static int i=0;   
   
   /*
    * i, above, is static because this function can be called
    * many times before '\r' is pressed.  we set i=0 when
    * '\r' is encountered.
    */


   while( kbhit() ) {
      if( (ch=getch()) == 0)
	  getch();          /* ignore */
      else {
	 if( ch == '\r') {
	     command[i] = 0;
	     i = 0;
	     printf("\r\n");
	     i = strlen(command) - 1;

	     /*
	      * ignore trailing whitespace
	      */
	     while( i>0 && isspace(command[i]))
		i--;
		 if( i>= 0)
			 command[i+1] = 0;
		 i = 0;

	     return 1;
	 }
	 else if( ch == (char)8 && i > 0) {   /* process backspace */
	     printf("%c %c",8,8);
	     command[i] = 0;
	     i--;
	 }
	 else if( ( ch >= 32) && ( ch <= 126) ) {
	     printf("%c", ch);
	     command[i++] = ch;
	 }
      }
   }
   return 0;
#else
   return 0; 
#endif
}


/*
 * GetFile
 * called to retrive a file from remote host
 */
void ftp::GetFile( char *fname)
{
   DWORD NumberOfBytesWritten;
   int nTotal=0, nBytesRead=0, /*retval,*/ aborted=0;
   char *abortstr = "ABOR\r\n";
   char tmp[100] = {0};
   char fullfname[256];

   /*/void (*OldHandler)(int); */

   /*
    * did we get a filename?
    */
   if( !fname || ! (*fname)) {
      printf("No file specified.\n");
      return;
   }

   transformpath(tmp, szLocalFtpRoot, fname);
   getrealpath(fullfname, tmp);
   HANDLE fp = CreateFile( fullfname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );

   if ( fp == INVALID_HANDLE_VALUE )
   {
	   sprintf(tmp, "ftpclient: fopen-Error: %d\n",GetLastError());
	   OutputDebugStringA(tmp);
	   return;
   }

   /*
    * obtain a listen socket
    */
   if( GetListenSocket() == INVALID_SOCKET)
   {
       CloseHandle(fp);
       return;
   }
   
   /*
    * send command to server and read response
    */
   sprintf(szBuffer,"RETR %s\r\n",fname);
   if(!SendControlMsg(szBuffer,strlen(szBuffer)))
   {
      CloseHandle(fp);
      return;
   }
   GetReply();
   
   /*
    * accept server connection
    */
   if( AcceptConnection() == INVALID_SOCKET)
   {
       CloseHandle(fp);
       return;
   }
   
   while( (nBytesRead=ReadDataMsg(szBuffer,1024)) > 0)
   {
	   //Progress
       //progressbar+=nBytesRead;
       WriteFile(fp, szBuffer, nBytesRead, &NumberOfBytesWritten, NULL);
	   nTotal+=nBytesRead;
	}
	   
   CloseHandle(fp);
   CloseDataConnection(hDataSocket);
   GetReply();
}

/*
 * PutFile
 * called to transfer a file to the remote host using the current
 * file transfer mode.  it's just like GetFile.
 *
 * i have commented out lines that would have helped with trapping
 * ctrl-c because longjmp did not work :((
 * if you figure it out, then uncomment them
 */
 void ftp::PutFile( char *fname)
{
   DWORD NumberOfBytesRead;
   int nTotal=0, nBytesRead=0, /* retval, */ aborted=0;
   char *abortstr = "ABOR\r\n";
   char tmp[100] = {0};
   char fullfname[256];

   if( !fname || ! (*fname)) {
      printf("No file specified.\n");
      return;
   }

   transformpath(tmp, szLocalFtpRoot, fname);
   getrealpath(fullfname, tmp);

   HANDLE fp = CreateFile( fullfname, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );

   if ( fp == INVALID_HANDLE_VALUE )
   {
	   sprintf(tmp, "ftpclient: fopen-Error: %d\n",GetLastError());
	   OutputDebugStringA(tmp);
	   return;
   }

   if( GetListenSocket() == INVALID_SOCKET)
   {
       CloseHandle(fp);
       return;
   }
   
   /*
    * send command to server & read reply
    */
   sprintf(szBuffer,"STOR %s\r\n",fname);
   if(!SendControlMsg(szBuffer,strlen(szBuffer)))
   {
      CloseHandle(fp);
      return;
   }
   GetReply();
   
   /*
    * accept server connection
    */
   if( AcceptConnection() == INVALID_SOCKET)
   {
       CloseHandle(fp);
       return;
   }
   /* 
    * now send file
    */
   
   while (1)
   {
		ReadFile(fp,szBuffer,1024,&NumberOfBytesRead,NULL);
		SendDataMsg(szBuffer,NumberOfBytesRead);
		nTotal+=NumberOfBytesRead;
		if (NumberOfBytesRead == 0)
			break;
		//      printf("%s : %d sent\r",fname,nTotal);
   }

   /*
    * close data connection
    */
   CloseDataConnection(hDataSocket);
   CloseHandle(fp);
   GetReply();
}

