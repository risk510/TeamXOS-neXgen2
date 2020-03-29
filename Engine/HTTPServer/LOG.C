/*============================================================================
 * log.c
 * Compressed Log utilities.
 * M. Tim Jones <mtj@mtjones.com>
 *==========================================================================*/

#include <xtl.h>
#include "httpd-log.h"

#define HI_PRIORITY	0x80
#define MED_PRIORITY	0x40
#define LOW_PRIORITY	0x00

#define LOG_TYPE_MASK	0x3f

struct logConstStruct {
  unsigned char id;
  char          *string;
} logStrings[] = {
  { (LOG_EFS | LOW_PRIORITY), "Filesystem: " },
  { (LOG_NET | LOW_PRIORITY), "Network: " },
  { (LOG_HTTP | LOW_PRIORITY), "HTTPD: " },
  { (LOG_FTP | LOW_PRIORITY), "FTPD: " },
  { (LOG_CTP | LOW_PRIORITY), "CTPD: " },
  { (LOG_Unknown | LOW_PRIORITY), "Unknown: " },
  { 0, NULL }
};

#define MAX_LOG		100

static unsigned char logdata[MAX_LOG][512];
static int curWrite = 0;


/*----------------------------------------------------------------------------
 * emitString() - Emit a string (with null char) to the log
 *--------------------------------------------------------------------------*/
volatile int loglock = 0;
void LogString(unsigned char byte, char *string1, char *string2)
{
	int i,x, len = strlen(string1);

	i=curWrite; curWrite++;

	logdata[i][0] = byte;
	strncpy(logdata[i]+1, string1, 500);
	strncpy(logdata[i]+1+len, string2, 500-len);

	while(loglock==1) {
		Sleep(5);
	}
	loglock=1;
	if (curWrite == MAX_LOG) {
		for(i=0; i<MAX_LOG-1; i++)
			memcpy(logdata[0], logdata[1], 512);
		curWrite--;
	} else if(curWrite > MAX_LOG) {
		curWrite=0;
	}
	loglock=0;

	for( x=0; logStrings[x].string != NULL; x++ )
		if(logStrings[x].id==byte) break;
	if(logStrings[x].id==byte) {
		OutputDebugString(logStrings[x].string);
	} else {
		OutputDebugString("Unknown: ");
	}
	OutputDebugString(string1);
	OutputDebugString(string2);
	OutputDebugString("\n");
}


/*----------------------------------------------------------------------------
 * sendLog() - Emit the current log through the connected HTTP client socket.
 *--------------------------------------------------------------------------*/
void sendLog(httpstate_t *hs)
{
	int x, i = curWrite-1;

	/* First, make sure we're on a valid header. */
	i = curWrite;
	while (i >= 0) {
		char *text = logdata[i]+1;
		if((logdata[i][0]&LOG_TYPE_MASK)!=0) {
			for( x=0; logStrings[x].string != NULL; x++ )
				if(logStrings[x].id==logdata[i][0]) break;
			if(logStrings[x].id==logdata[i][0]) {
				send(hs->socket, logStrings[x].string, strlen(logStrings[x].string), 0);
				send(hs->socket, text, strlen(text), 0);
				send(hs->socket, "<BR>\n", 5, 0);
			} else {
				send(hs->socket, "Unknown: ", 9, 0);
				send(hs->socket, text, strlen(text), 0);
				send(hs->socket, "<BR>\n", 5, 0);
			}
		}
		i--;
	}
}
