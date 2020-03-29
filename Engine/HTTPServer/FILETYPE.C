#include <xtl.h>
#include <stdio.h>
#include "httpd.h"
#include "httpd-filetype.h"
#include "httpd-filehdlr.h"
#include "httpd-dyncntnt.h"
#include "httpd-log.h"


extern unsigned char *filedata;

char notimplemented[]="HTTP/1.1 501 Not Implemented\nContent-type: text/plain\n\nERROR 501: Not Implemented.\n";
char notfound[]="HTTP/1.1 404 File not found\nContent-type: text/plain\n\nERROR 404: File not found.\n";
char success[]="HTTP/1.1 200 OK\n";
char needauth[]="HTTP/1.1 401 Unauthorized -- authentication failed\n"
				"WWW-Authenticate: Basic realm=\"neXgen Dashboard\"\nContent-type: text/plain\n\nERROR 401: Unauthorized -- authentication failed.\n";

char *content_types[]={
	"text/html",
	"application/octet-stream",
	"image/jpeg",
	"image/gif",
	"text/plain"
};


/*---------------------------------------------------------------------------
 * getFilename() - Parse a filename out of the URL
 *-------------------------------------------------------------------------*/
void getFilename(char *inbuf, char *out, int start)
{
  unsigned int i=start, j=0;

  /* 
   * Skip any initial spaces 
   */
  while (inbuf[i] == ' ') i++;

  for ( ; i < strlen(inbuf) ; i++) {
    if (inbuf[i] == ' ') {
      out[j] = 0;
      break;
    }
    out[j++] = inbuf[i];
  }

  //if (!strcmp(out, "/")) strcpy(out, "/index.cgi");
  if (!strcmp(out, "/")) strcpy(out, "/main.html");
}


/*---------------------------------------------------------------------------
 * determineContentType() - self explanatory
 *-------------------------------------------------------------------------*/
int determineContentType(int fileOffset)
{
  char suffix[20];
  int  i;

  fileOffset+=2;

  for ( ; filedata[fileOffset] != 0 ; fileOffset++) {
    if (filedata[fileOffset] == '.') break;
  }

  if (filedata[fileOffset] == 0) return(TEXT_PLAIN);
  else {

    fileOffset++;
    for (i = 0 ; filedata[fileOffset+i] != 0 ; i++) {
      suffix[i] = filedata[fileOffset+i];
    }
    suffix[i] = 0;

    /* 
     * Now that we've go the suffix, determine the content type 
     */
    if (!strnicmp(suffix, "cgi", 3) ||
		!strnicmp(suffix, "lua", 3)) {
      return(CGISCRIPT);
    } else if (!strnicmp(suffix, "html", 4) ||
			   !strnicmp(suffix, "htm", 3)) {
      return(TEXT_HTML);
    } else if (!strnicmp(suffix, "class", 5) ||
               !strnicmp(suffix, "jar", 3)   ||
               !strnicmp(suffix, "gz", 2)   ||
               !strnicmp(suffix, "zip", 3))   {
      return(OCTET_STREAM);
    } else if (!strnicmp(suffix, "jpeg", 4) ||
               !strnicmp(suffix, "jpg", 4))  {
      return(JPEG_IMAGE);
    } else if (!strnicmp(suffix, "gif", 3)) {
      return(GIF_IMAGE);
    } else {
      return(TEXT_PLAIN);
    }
  }
}


/*---------------------------------------------------------------------------
 * returnFileHeader() - Emit the HTTP response message through the socket
 *-------------------------------------------------------------------------*/
void returnFileHeader(httpstate_t *hs, int ct)
{
  char line[80];

  send(hs->socket, success, strlen(success), 0);
  sprintf(line, "Server: neXgen Dashboard \n"); 
  send(hs->socket, line, strlen(line), 0);

  sprintf(line, "Connection: close\n");
  send(hs->socket, line, strlen(line), 0);

  if(ct < CGISCRIPT) {
	sprintf(line, "Content-Type: %s\n\n", content_types[ct]);
	send(hs->socket, line, strlen(line), 0);
  }
}


/*---------------------------------------------------------------------------
 * parseAndEmitFile() - Parse the HTML and replace dynamic content tags
 *-------------------------------------------------------------------------*/
void parseAndEmitFile(httpstate_t *hs, struct fileHdrStruct *filehdr)
{
  unsigned long i;

  /* Emit the dynamic HTML file replacing the <DATA #> with the appropriate
   * content.
   */

  for (i = 0 ; i < filehdr->size ; i++) {
    if (filedata[filehdr->fileStart+i] == '<') {
      if (!strncmp(&filedata[filehdr->fileStart+i+1], "DATA", 4)) {
        i+= 6;
        getDynamicContent(&filedata[filehdr->fileStart+i], hs);

        for ( ; filedata[filehdr->fileStart+i] != '>' ; i++);
        i++;

      } else if (!strncmp(&filedata[filehdr->fileStart+i+1], "FILE", 4)) {
		int fi=1;
		char filename[MAX_PATH];
		i+= 6;

		strcpy(filename, "/");
        for ( ; filedata[filehdr->fileStart+i] != '>' ; i++) {
			if(fi<MAX_PATH-1) filename[fi++] = filedata[filehdr->fileStart+i];
		}
        i++;
		filename[fi]=0;

		if(strlen(filename)) {
			struct fileHdrStruct filehdr2;
			int ret = lookupFilename(filename, &filehdr2);
		    if (ret > 0) {
				send(hs->socket, &filedata[filehdr2.fileStart], filehdr2.size, 0);
			} else {
				char buf[] = " SSI Include Error ";
				send(hs->socket, buf, strlen(buf), 0);
		        LogString(LOG_EFS, "Unknown file ", buf);
			}
		}
      } else {
        send(hs->socket, &filedata[filehdr->fileStart+i], 1, 0);
      }
    } else {
      send(hs->socket, &filedata[filehdr->fileStart+i], 1, 0);
    }
  }
}

#include "..\\LUA\\liblua\\lua.h"
#include "..\\LUA\\liblua\\lualib.h"
#include "..\\LUA\\liblua\\lobject.h"
#include "..\\LUA\\liblua\\lapi.h"

char g_httppass[32] = "xbox";

int lua_httpprint(lua_State *L)
{
	int n=lua_gettop(L), i;
	char *string=NULL;
	httpstate_t *hs=NULL;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1:	string=(char *)lua_tostring(L, i); break;
		}
	}

	lua_getglobal(L, "httpstate");
	hs = (httpstate_t *)(lua_touserdata(L, -1));

	send(hs->socket, string, strlen(string), 0);

	return 0;
}

int lua_weblaunchxbe(lua_State *L)
{
	int n=lua_gettop(L), i;
	char *string=NULL;
	httpstate_t *hs=NULL;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1:	string=(char *)lua_tostring(L, i); break;
		}
	}

	OutputDebugString(string);
	//LaunchMyXBE(string, NULL);
	//nexgenDoAction(Launch, string);
	
	return 0;
}

int lua_customprint(lua_State *L)
{
	int n=lua_gettop(L);
    int i;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1:	OutputDebugStringA(lua_tostring(L, i)); break;
		}
	}
	OutputDebugStringA("\n");
	return 0;
}

int lua_showlog(lua_State *L)
{
	httpstate_t *hs=NULL;

	lua_getglobal(L, "httpstate");
	hs = (httpstate_t *)(lua_touserdata(L, -1));

	sendLog(hs);
	return 0;
}

int lua_showstatus(lua_State *L)
{
	httpstate_t *hs=NULL;

	lua_getglobal(L, "httpstate");
	hs = (httpstate_t *)(lua_touserdata(L, -1));

	//TEMP

	//sendLog(hs);
	return 0;
}

int lua_getvarfromurl(lua_State *L)
{
	int n=lua_gettop(L);
    int i,b;
	char *varname=NULL, *cvar=NULL, *varstart=NULL, *tofind=NULL, *ovar=NULL;
	httpstate_t *hs=NULL;

	for (i=1; i <= n; i++) {
		switch(i) {
			case 1:	varname=(char *)lua_tostring(L, i); break;
		}
	}

	lua_getglobal(L, "httpstate");
	hs = (httpstate_t *)(lua_touserdata(L, -1));

	if(varname!=NULL && hs!=NULL && hs->url!=NULL) {
		tofind=malloc(strlen(varname)+2);
		sprintf(tofind, "%s=", varname);
		varstart=strchr(hs->url, '?');
		if(varstart!=NULL) {
			varstart++;
			cvar=strstr(varstart, tofind);
			if(cvar!=NULL) {
				cvar+=strlen(tofind);
				for(b=0; cvar[b]!='&' && cvar[b]!=' ' && cvar[b]!='\n' && cvar[b]!='\r' && cvar[b]!=0; b++);
				ovar=malloc(b);
				strncpy(ovar, cvar, b);
				ovar[b]=0;
			}
		}
		free(tofind);
	}

	if(ovar!=NULL) {
		lua_pushstring(L, ovar);
//		free(ovar);
	} else {
		lua_pushstring(L, "");
	}
	return 1;
}

void doCgiScript( httpstate_t *hs, struct fileHdrStruct *filehdr ) {
	char *cgilua;

	lua_State *LUA_STATE=lua_open();
	lua_mathlibopen(LUA_STATE);
	lua_register(LUA_STATE, "_ERRORMESSAGE", lua_customprint);
	lua_register(LUA_STATE, "httpprint", lua_httpprint);
	lua_register(LUA_STATE, "getvarfromurl", lua_getvarfromurl);
	lua_register(LUA_STATE, "weblaunchxbe", lua_weblaunchxbe);
	lua_register(LUA_STATE, "showlog", lua_showlog);

	{
		//BUGFIX
		//int nTag = lua_newtag(LUA_STATE);
		//lua_pushusertag(LUA_STATE, hs, nTag);
		lua_setglobal(LUA_STATE, "httpstate");
	}

	lua_pushstring(LUA_STATE, g_httppass);
	lua_setglobal(LUA_STATE, "httppass");

	cgilua=(char*)malloc(filehdr->size);
	memcpy(cgilua, &filedata[filehdr->fileStart], filehdr->size);
	// BUGFIX
	lua_dobuffer(LUA_STATE, cgilua, filehdr->size, "CGI");
	lua_close(LUA_STATE);
	free(cgilua);
}


/*---------------------------------------------------------------------------
 * returnFile() - High level return message function
 *-------------------------------------------------------------------------*/
void returnFile(httpstate_t *hs, struct fileHdrStruct *filehdr, int HeaderOnly)
{
  int ct;

  ct = determineContentType(filehdr->hdrStart);

  returnFileHeader(hs, ct);

  if(HeaderOnly) return;

  if (ct == CGISCRIPT) {
    doCgiScript(hs, filehdr);
  } else if (ct == TEXT_HTML) {
    parseAndEmitFile(hs, filehdr);
  } else {
	unsigned long moredata=filehdr->size;
	unsigned long index=0;
	while(moredata) {
		unsigned long towrite=min(moredata, 8192);
	    send(hs->socket, &filedata[filehdr->fileStart+index],towrite, 0);
		index+=towrite;
		moredata-=towrite;
	}
  }
}

