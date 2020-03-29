#include <xtl.h>
#include <tchar.h>
#include "..\\XKUtils\\XKUtils.h"
#include "..\\XKUtils\\XKExports.h"
#include "XbHTTPDownloader.h"
#include "string"

#define HTTP_BUFFERSIZE 4096 // size of buffer received data is stored in

bool HTTPDownloadToFile(char * host, char* path, char* save_file)
{
	if (!host)
	{
		return false;
	}
	if (!path)
	{
		return false;
	}
	if (!save_file)
	{
		return false;
	}

	// Startup Winsock
	WSADATA wsaData;
	int iWinsockData = WSAStartup( MAKEWORD(2,2), &wsaData );
	// Check Winsock Started OK
	if( iWinsockData != 0 )
	{
		return false;
	}

	char http_buffer[HTTP_BUFFERSIZE];
	SOCKET m_socket;
	m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	if (m_socket == INVALID_SOCKET) {
		shutdown( m_socket, SD_SEND );
		closesocket( m_socket );
		WSACleanup();
		return false;
	}

	WSAEVENT hEvent = WSACreateEvent();
	XNDNS * pxndns = NULL;
	INT err = XNetDnsLookup(host, hEvent, &pxndns);
	WaitForSingleObject(hEvent, INFINITE);
	
	if (pxndns->iStatus == 0)
	{
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = pxndns->aina->S_un.S_addr;
		service.sin_port = htons(80);
		int results=connect(m_socket,(sockaddr*) &service,sizeof(struct sockaddr));
		if( results == SOCKET_ERROR)
		{
			XNetDnsRelease(pxndns);
			shutdown( m_socket, SD_SEND );
			closesocket( m_socket );
			WSACleanup();
			return false;
		}
		char getpath[1024];
		sprintf(getpath,"GET %s \r\n",path);
		send(m_socket, getpath, strlen(getpath), 0);
		HANDLE xmlfile=CreateFile( save_file, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL );
		int read_bytes = 1;
		while (read_bytes)
		{
			read_bytes = recv(m_socket, http_buffer, HTTP_BUFFERSIZE, 0);
			if (read_bytes)
			{
				DWORD dwWritten=0;
				WriteFile( xmlfile, http_buffer, read_bytes, &dwWritten, NULL );
			}
		}
		CloseHandle(xmlfile);
		//shutdown( m_socket, SD_SEND );
		//closesocket( m_socket );
		//WSACleanup();
	}
	else
	{
		XNetDnsRelease(pxndns);
		shutdown( m_socket, SD_SEND );
		closesocket( m_socket );
		WSACleanup();
		return false;
	}

	XNetDnsRelease(pxndns);
	shutdown( m_socket, SD_SEND );
	closesocket( m_socket );
	WSACleanup();
	return true;
}


