/*============================================================================
 * dyncntnt.h
 * Dynamic content funciton prototypes.
 * M. Tim Jones <mtj@mtjones.com>
 *==========================================================================*/

#ifndef _DYNCNTNT_H
#define _DYNCNTNT_H
#include "httpd.h"

#ifdef __cplusplus
extern "C"{
#endif

int addDynamicContent(char *name, void(*function)(httpstate_t *hs));
void getDynamicContent(char *name, httpstate_t *hs);

#ifdef __cplusplus
}
#endif

#endif /* _DYNCNTNT_H */
