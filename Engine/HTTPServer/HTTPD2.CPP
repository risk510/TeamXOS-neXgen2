#include <xtl.h>
#include <stdio.h>
#include "nexgendash.h"
#include "httpd.h"
#include "httpd-dyncntnt.h"
#include "httpd-filetype.h"
#include "httpd-log.h"
#include "httpd-filehdlr.h"

volatile extern bool bNetworkingUp;

char g_httpuser[32] = "xbox";
char g_httppass[32] = "xbox";
short g_httpport = 80;
bool g_httpnoauth = FALSE;
char g_httpencup[320];
unsigned int g_httpencupl = 0;
unsigned long g_http_connections = 0;

void HTMLOutput_MainStatus( httpstate_t *hs ) {
	register char *buffer=(char *)malloc(1024);
	/*char *tempa=(char *)malloc(4);
	char *tempb=(char *)malloc(4);*/

//	strcpy(buffer,"neXgen Dashboard Version: <B>0.95</B><BR>");
	strcpy(buffer,"neXgen Dashboard Version: <B>v1.0</B><BR>");
	send(hs->socket, buffer, strlen(buffer), 0);


/*#define CtoF(a) (((9.0f/5.0f)*(a))+32.0f)

	gettemp(tempa,tempb);
	sprintf(buffer, "<TT>Temperature 1: <I>%.2f</I> 'F (<I>%i</I> 'C)<BR>", CtoF(*tempa), *tempa);
	send(hs->socket, buffer, strlen(buffer), 0);
	sprintf(buffer, "Temperature 2: <I>%.2f</I> 'F (<I>%i</I> 'C)</TT><BR><BR>", CtoF(*tempb), *tempb);
	send(hs->socket, buffer, strlen(buffer), 0);

#undef CtoF*/

	free(buffer);
	/*free(tempa);
	free(tempb);*/

}

typedef struct ULI_S {
        DWORD LowPart;
        DWORD HighPart;
} ULI_T;

void HTMLOutput_DriveInfo( httpstate_t *hs ) {
	register char *buffer;
	register ULARGE_INTEGER *freespace, *totalspace;
	register ULI_T *ufreespace, *utotalspace;
	register unsigned long fspace, tspace;

	buffer=(char *)malloc(4096);
	freespace=(ULARGE_INTEGER *)malloc(sizeof(ULARGE_INTEGER));
	ufreespace=(ULI_T *)freespace;
	totalspace=(ULARGE_INTEGER *)malloc(sizeof(ULARGE_INTEGER));
	utotalspace=(ULI_T *)totalspace;

	strcpy(buffer, "<table>"
		"<tr><td>Device</td><td>MountPoint</td><td align=\"right\">Free /</TD><TD>Total Space</td></tr>");

	GetDiskFreeSpaceEx("C:\\", NULL, totalspace, freespace);
	fspace=((ufreespace->HighPart&0x3ff) << 22) | ((ufreespace->LowPart)>>10);
	tspace=((utotalspace->HighPart&0x3ff) << 22) | ((utotalspace->LowPart)>>10);
	sprintf(buffer+strlen(buffer),
		"<tr><td>\\Devices\\Harddisk0\\Partition2</td><td align=\"center\">C:</td><td align=\"right\">%u /</td><td>%u</td><td> KB</td></tr>",
		fspace, tspace
	);

	GetDiskFreeSpaceEx("E:\\", NULL, totalspace, freespace);
	fspace=((ufreespace->HighPart&0x3ff) << 22) | ((ufreespace->LowPart)>>10);
	tspace=((utotalspace->HighPart&0x3ff) << 22) | ((utotalspace->LowPart)>>10);
	sprintf(buffer+strlen(buffer),
		"<tr><td>\\Devices\\Harddisk0\\Partition1</td><td align=\"center\">E:</td><td align=\"right\">%u /</td><td>%u</td><td> KB</td></tr>",
		fspace, tspace
	);

	GetDiskFreeSpaceEx("F:\\", NULL, totalspace, freespace);
	fspace=((ufreespace->HighPart&0x3ff) << 22) | ((ufreespace->LowPart)>>10);
	tspace=((utotalspace->HighPart&0x3ff) << 22) | ((utotalspace->LowPart)>>10);
	sprintf(buffer+strlen(buffer),
		"<tr><td>\\Devices\\Harddisk0\\Partition6</td><td align=\"center\">F:</td><td align=\"right\">%u /</td><td>%u</td><td> KB</td></tr>",
		fspace, tspace
	);

	GetDiskFreeSpaceEx("X:\\", NULL, totalspace, freespace);
	fspace=((ufreespace->HighPart&0x3ff) << 22) | ((ufreespace->LowPart)>>10);
	tspace=((utotalspace->HighPart&0x3ff) << 22) | ((utotalspace->LowPart)>>10);
	sprintf(buffer+strlen(buffer),
		"<tr><td>\\Devices\\Harddisk0\\Partition3</td><td align=\"center\">X:</td><td align=\"right\">%u /</td><td>%u</td><td> KB</td></tr>",
		fspace, tspace
	);

	GetDiskFreeSpaceEx("Y:\\", NULL, totalspace, freespace);
	fspace=((ufreespace->HighPart&0x3ff) << 22) | ((ufreespace->LowPart)>>10);
	tspace=((utotalspace->HighPart&0x3ff) << 22) | ((utotalspace->LowPart)>>10);
	sprintf(buffer+strlen(buffer),
		"<tr><td>\\Devices\\Harddisk0\\Partition4</td><td align=\"center\">Y:</td><td align=\"right\">%u /</td><td>%u</td><td> KB</td></tr>",
		fspace, tspace
	);

	GetDiskFreeSpaceEx("Z:\\", NULL, totalspace, freespace);
	fspace=((ufreespace->HighPart&0x3ff) << 22) | ((ufreespace->LowPart)>>10);
	tspace=((utotalspace->HighPart&0x3ff) << 22) | ((utotalspace->LowPart)>>10);
	sprintf(buffer+strlen(buffer),
		"<tr><td>\\Devices\\Harddisk0\\Partition5</td><td align=\"center\">Z:</td><td align=\"right\">%u /</td><td>%u</td><td> KB</td></tr>",
		fspace, tspace
	);
	strcat(buffer, "</table>");

	send(hs->socket, buffer, strlen(buffer), 0);
	free(buffer);
	free(freespace);
	free(totalspace);
}

void HTMLOutput_Version( httpstate_t *hs ) {
	char buf[] = "1.0.932";
	send(hs->socket, buf, strlen(buf), 0);
}

void HTMLOutput_Log( httpstate_t *hs ) {
	sendLog(hs);
}

void HTMLCGI_PowerOff( httpstate_t *hs ) {
//	pic_PowerOff();
}
void HTMLCGI_Reboot( httpstate_t *hs ) {
	//pic_DoReset(0);
}
void HTMLCGI_TrayEject( httpstate_t *hs ) {
	//pic_Eject(1);
}
void HTMLCGI_TrayClose( httpstate_t *hs ) {
	//pic_Eject(0);
}


int httpd_checkauth( char *data ) {
	register int i=0;
	char *authstr;
	if(g_httpnoauth==TRUE) return 1;

	while(data[i]!=0) {
		if(!strnicmp(data+i, "Authorization: Basic", 18)) {
			int z=0, x=i+21;
			i=x;
			while((data[i]!='\r') && (data[i]!='\n') && (data[i]!=0)) {
				z++;
				i++;
			}

			authstr=(char *)malloc(z+1);
			strncpy(authstr, data+x, z);
			authstr[z]=0;

			if(strlen(authstr) != strlen(g_httpencup)) {
				free(authstr);
				return 0;
			}
				
			if(!strcmp(authstr, g_httpencup)) {
				free(authstr);
				return 1;
			} else {
				free(authstr);
				return 0;
			}
		}
		while((data[i]!='\n') && (data[i]!=0)) i++;
		if(data[i]=='\n') i++;
	}
	return 0;
}

#define HTTP_MAXRECV 4096
static HRESULT httpd_recv( httpstate_t *hs ) {
	register int len, max, loop, ret;
	register char *data;
	register char *filename;
	register struct fileHdrStruct *filehdr;

	filename=(char *) malloc(256);
	filehdr=(struct fileHdrStruct *) malloc(sizeof(struct fileHdrStruct));
	data=(char *)malloc(HTTP_MAXRECV);

	if(hs->socket != INVALID_SOCKET) {

		//recvlen=recv(hs->socket, data, HTTP_MAXRECV, 0);
		max = 0; 
		loop = 1;
		while (loop) {
			len = recv(hs->socket, &data[max], 256, 0);
			if(len==SOCKET_ERROR) { free(data); return S_OK; }

			data[max+len] = 0;
			max += len;
			if((len==0) || (max+256 > HTTP_MAXRECV)) loop = 0;
			if ((data[max-4] == 0x0d) && (data[max-3] == 0x0a) &&
				(data[max-2] == 0x0d) && (data[max-1] == 0x0a)) loop = 0;
		}

		if(max==0) { free(data); free(filename); free(filehdr); return S_OK; }

		if(!strncmp(data, "GET", 3)) {
			getFilename(data, filename, 4);
	        LogString(LOG_HTTP, "GET ", filename);
			if(httpd_checkauth(data)) {
				char *fullname = (char *)malloc(strlen(filename)+6);
				sprintf(fullname, "%s", filename);
				ret = lookupFilename(fullname, filehdr);
				free(fullname);
				if (ret > 0) {
					returnFile(hs, filehdr, 0);
				} else {
			        LogString(LOG_HTTP, "404 ", filename);
					send(hs->socket, notfound, strlen(notfound), 0);
				}
			} else {
				send(hs->socket, needauth, strlen(needauth), 0);
			}
		} else if (!strncmp(data, "HEAD", 4)) {
			getFilename(data, filename, 5);
	        LogString(LOG_HTTP, "HEAD ", filename);
			char *fullname = (char *)malloc(strlen(filename)+6);
			sprintf(fullname, "%s", filename);
			ret = lookupFilename(fullname, filehdr);
			free(fullname);
			if (ret > 0) {
				returnFile(hs, filehdr, 1);
			} else {
		        LogString(LOG_HTTP, "404 ", filename);
				send(hs->socket, notfound, strlen(notfound), 0);
			}
		} else {
	        LogString(LOG_HTTP, "Unknown request ", data);
			send(hs->socket, notimplemented, strlen(notimplemented), 0);
		}
	}

	free(data);
	free(filename);
	free(filehdr);

	return S_OK;
}


static HRESULT httpd_serve( SOCKET s, const struct sockaddr FAR *name, int namelen ) {
	httpstate_t *hs;

	g_http_connections++;

	// Allocate memory for the structure that holds the state of the connection.
	hs = (httpstate_t *)malloc(sizeof(httpstate_t));

	if(hs == NULL) {
        LogString(LOG_HTTP, "Out of memory.", "");
		return ERROR_NOT_ENOUGH_MEMORY;
	}
  
	// Initialize the structure.
	hs->data = NULL;
	hs->left = 0;
	hs->socket = s;
	
	httpd_recv( hs );

	closesocket(s);

	g_http_connections--;

	return S_OK;
}

DWORD WINAPI httpd_thread( LPVOID lpParam ) {
	int err;

	if(bNetworkingUp!=TRUE) return 0;

	// Create TCP socket
	SOCKET sLBrd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if(sLBrd == INVALID_SOCKET) {
		return 0;
	}

	// Bind to INADDR_ANY
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons( g_httpport );
	err = bind( sLBrd, (const sockaddr*)( &sa ), sizeof( SOCKADDR_IN ) );
	if(err != 0) {
        LogString(LOG_HTTP, "Unable to bind to port.", "");
		goto httpd_exit;
	}

	err = listen( sLBrd, 5 );
	if(err != 0) {
        LogString(LOG_HTTP, "Unable to listen to socket.", "");
		goto httpd_exit;
	}

	// Register our CGI functions
	addDynamicContent("STATUS", HTMLOutput_MainStatus);
	addDynamicContent("DRIVE", HTMLOutput_DriveInfo);
	addDynamicContent("VERSION", HTMLOutput_Version);
	addDynamicContent("LOG", HTMLOutput_Log);

	addDynamicContent("POWEROFF", HTMLCGI_PowerOff);
	addDynamicContent("REBOOT", HTMLCGI_Reboot);
	addDynamicContent("TRAYEJECT", HTMLCGI_TrayEject);
	addDynamicContent("TRAYCLOSE", HTMLCGI_TrayClose);

	while(bNetworkingUp == TRUE) {
		SOCKET newsock;
		SOCKADDR_IN newsa;
		int sasize = sizeof(newsa);
		newsock = accept( sLBrd, (sockaddr*)( &newsa ), &sasize );
		if(newsock != INVALID_SOCKET) {
	        LogString(LOG_HTTP, "Recieved connection.", "");
			httpd_serve(newsock, (const sockaddr*)( &newsa ), sasize );
		}
	}

httpd_exit:

	shutdown(sLBrd, SD_SEND);
	closesocket(sLBrd);

	return 0;
}

extern "C" {
	int encode64(const char *_in, unsigned int inlen, char *_out, unsigned int outmax, unsigned int *outlen);
}

HRESULT httpd_init() {
	DWORD	dwThreadId;
	HANDLE	hThread; 

	{
		char *tstr = (char *)malloc(strlen(g_httpuser) + strlen(g_httppass) + 2);
		sprintf(tstr, "%s:%s", g_httpuser, g_httppass);
		encode64(tstr, strlen(tstr), g_httpencup, 320, &g_httpencupl);
	}

	hThread = CreateThread( 
		NULL,						// (this parameter is ignored)
		0,							// use default stack size  
		httpd_thread,				// thread function 
		NULL,						// argument to thread function 
		0,							// use default creation flags 
		&dwThreadId);				// returns the thread identifier 
 
	// Check the return value for success. 
	if (hThread == NULL) 
	{
        LogString(LOG_HTTP, "Failed to create thread.", "");
	}

//	LogString(LOG_HTTP, "Thread created.", "");
	CloseHandle( hThread );
	return S_OK;
}

