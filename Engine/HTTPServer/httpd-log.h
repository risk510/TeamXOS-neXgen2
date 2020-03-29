/*===========================================================================
 * log.h
 * Log symbolics and function prototypes.
 * M. Tim Jones <mtj@mtjones.com>
 *=========================================================================*/

#ifndef _LOG_H
#define _LOG_H
#include "httpd.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	LOG_EFS=0x01,
	LOG_NET,
	LOG_HTTP,
	LOG_FTP,
	LOG_CTP,
	LOG_Unknown
};

void LogString(unsigned char, char *, char *);
void sendLog(httpstate_t *);

#ifdef __cplusplus
}
#endif

#endif /* _LOG_H */
