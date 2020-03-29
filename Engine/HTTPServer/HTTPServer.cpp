// HTTP C
#include "HTTPServer.h"

// Constructor
CXBHTTPServer::CXBHTTPServer()
{
}

// Destructor
CXBHTTPServer::~CXBHTTPServer()
{
}


extern "C" unsigned char *filedata;
extern "C" DWORD filedata_size;

DWORD CXBHTTPServer::ReadAFile(char *fn, unsigned char **out) {
	DWORD filesize;
	HANDLE infile = CreateFileA( fn, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if(infile == INVALID_HANDLE_VALUE) {
		OutputDebugStringA("Could not load ");
		OutputDebugStringA(fn);
		OutputDebugStringA("\n");
		DWORD eno=GetLastError();
		*out=NULL;
		return 0;
	}

	filesize=GetFileSize(infile, NULL);
	*out=(unsigned char*)malloc(filesize+1);
	memset(*out, 0, filesize+1);
	DWORD readin;
	ReadFile( infile, *out, filesize, &readin, NULL );
	CloseHandle( infile );
	return readin;
}

HRESULT CXBHTTPServer::LoadHTTPData()
{
	XSetFileCacheSize(2048*1024);

	HANDLE fd_handle = XGetSectionHandle("EFSA");
	if(fd_handle==INVALID_HANDLE_VALUE) {
		return GetLastError();
	} else {
	    filedata=(unsigned char *) XLoadSectionByHandle(fd_handle);
		filedata_size=XGetSectionSize(fd_handle);
	}

	XFreeSection("EFSA");
	filedata_size=ReadAFile("SYSTEM:\\nexgenhttp.dat", &filedata);
	CloseHandle(fd_handle);
	return S_OK;
}

void CXBHTTPServer::Initialize()
{
	LoadHTTPData();
	httpd_init();

}