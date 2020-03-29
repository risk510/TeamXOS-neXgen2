//HTTP SERVER HEADER.

#ifndef __XBOX_HTTPSERVER_H

#define __XBOX_HTTPSERVER_H

#include <xtl.h>
#include "HTTPD.h"

class CXBHTTPServer
{
private:
	HRESULT LoadHTTPData();
	DWORD ReadAFile(char *fn, unsigned char **out);
public:

	void	Initialize();
	void	UpdateInput();

	CXBHTTPServer();
	~CXBHTTPServer();
};

#endif