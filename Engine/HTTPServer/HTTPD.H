#ifndef _HTTPD_H
#define _HTTPD_H
#include <xtl.h>
#include "httpd-filetype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct httpstate_s {
	char * data;
	unsigned long left;
	SOCKET socket;
	struct sockaddr * name;
	int namelen;
	char *url;
} httpstate_t;

extern char notimplemented[];
extern char notfound[];
extern char success[];
extern char needauth[];

extern char *content_types[];

void getFilename(char *inbuf, char *out, int start);
int determineContentType(int fileOffset);
void returnFileHeader(httpstate_t *hs, int ct);
void parseAndEmitFile(httpstate_t *hs, struct fileHdrStruct *filehdr);
void returnFile(httpstate_t *hs, struct fileHdrStruct *filehdr, int HeaderOnly);

#ifdef __cplusplus
}
#endif

HRESULT httpd_init();
DWORD __stdcall HTTPDThread( void* pData );

#endif