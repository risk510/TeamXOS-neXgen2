/*============================================================================
 * dyncntnt.c
 * Dynamic content utilities
 * M. Tim Jones <mtj@mtjones.com>
 *==========================================================================*/

#include <stdio.h>
#include <string.h>
#include "httpd.h"
#include "httpd-filehdlr.h"

static int init = 0;

#define MAX_DYNAMIC_CONTENT	10

struct dynamicContentStructure {
  char variableName[80];
  void (*pfunc)(httpstate_t *hs);
} dynamicContent[MAX_DYNAMIC_CONTENT];


/*----------------------------------------------------------------------------
 * defaultFunction() - Default to fill unused dynamic content rows
 *--------------------------------------------------------------------------*/
void defaultFunction( httpstate_t *hs )
{
  char buf[] = " SSI Include Error ";
  send(hs->socket, buf, strlen(buf), 0);
}


/*----------------------------------------------------------------------------
 * initContent() - Initialize the dynamic content array       
 *--------------------------------------------------------------------------*/
void initContent()
{
  int i;

  for (i = 0 ; i < MAX_DYNAMIC_CONTENT ; i++) {
    dynamicContent[i].variableName[0] = 0;
    dynamicContent[i].pfunc = &defaultFunction;
  }
}


/*----------------------------------------------------------------------------
 * addDynamicContent() - Add a new element to the dynamic content array
 *--------------------------------------------------------------------------*/
int addDynamicContent(char *name, void (*function)( httpstate_t *hs )) 
{
  int i;

  if (!init) {
    initContent();
    init=1;
  }

  /* First, ensure that the 'name' does not exist in the current list */
  for (i = 0 ; i < MAX_DYNAMIC_CONTENT ; i++) {
    if (dynamicContent[i].variableName[0] != 0) {
      if (!strcmp(name, dynamicContent[i].variableName)) {
        return(-1);
      }
    }
  }

  /* Next, look for an empty slot */
  for (i = 0 ; i < MAX_DYNAMIC_CONTENT ; i++) {
    if (dynamicContent[i].variableName[0] == 0) {
      strncpy(dynamicContent[i].variableName, name, 80);
      dynamicContent[i].pfunc = function;
	  return(0); 
    }
  }

  return(0); 
}


/*----------------------------------------------------------------------------
 * getDynamicContent()
 *--------------------------------------------------------------------------*/
void getDynamicContent(char *name, httpstate_t *hs)
{
  int j, i;

  if (!init) {
    initContent();
    init=1;
  }

  for (j = 0 ; j < MAX_DYNAMIC_CONTENT ; j++) {

    /* Search for the name in the list, avoid '>' trailing name... */
    for (i = 0 ; name[i] != '>' ; i++) {
      if (dynamicContent[j].variableName[i] != name[i]) break;
    }

    if (name[i] == '>') {
      /* We've reached the end, so it was a good match */
      dynamicContent[j].pfunc(hs);
      return;
    }

  }

  return;
}

