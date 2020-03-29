#include <stdio.h>
#include "..\\Main.h"
#include "..\\Utils.h"
#include "..\\XKUtils\\XKUtils.h"

#define LOG_FTP 0


inline int mstrlen( char* in ) {
	int i = 0;
	if(in == NULL) DebugBreak();
	while((in[i]!=0) && (i < 32000)) i++;
	if(i>=32000) DebugBreak();
	return i;
}
#define strlen mstrlen

/*
inline void LogString(byte a, char* b, char* c) {
	OutputDebugString(b);
	OutputDebugString(c);
	OutputDebugString("\n");
}
*/

volatile extern bool bNetworkingUp;
extern IN_ADDR MYIP;

typedef struct ftpd_state_s {
	SOCKET ctrlsock;
	SOCKET datasock;
	int userok;
	int authenticated;
	int intransfer;
	char type;
	char path[MAX_PATH];
	unsigned char remoteaddr[6];
	unsigned char localaddr[6];
	char renamefrom[MAX_PATH];
	HANDLE file;
	long seekto;
} FTPDSTATE, *LPFTPDSTATE;

const char *FTPUsername;
const char *FTPPassword;

char *ftpuser;
char *ftppass;
char g_ftpuser[32] = "xbox";
char g_ftppass[32] = "xbox";
short g_ftpport = 21;
bool g_ftpnoauth = FALSE;
unsigned long g_ftp_connections=0;

inline void ftpd_close_data(LPFTPDSTATE fs) {
	if(fs->datasock == INVALID_SOCKET) return;
	shutdown(fs->datasock, SD_BOTH);
	closesocket(fs->datasock);
	fs->datasock = INVALID_SOCKET;
}

inline void ftpd_crapout(LPFTPDSTATE fs) {
	if(fs->datasock!=INVALID_SOCKET)
		closesocket(fs->datasock);
	if(fs->ctrlsock!=INVALID_SOCKET)
		closesocket(fs->ctrlsock);

	free(fs);

	g_ftp_connections--;
	
	ExitThread(0);

	for(;;);
}

inline void ftpd_send_reply(LPFTPDSTATE fs, int number, char *msg) {
	register char *buf=(char *)malloc(16 + strlen(msg));
	if(buf == NULL) ftpd_crapout(fs);

	sprintf(buf, "%3d %s\r\n", number, msg);
	send(fs->ctrlsock, buf, strlen(buf), 0);
	
	free(buf);
}

void ftpd_quit_421(LPFTPDSTATE fs, char *msg) {
	ftpd_send_reply(fs, 421, msg);

	ftpd_crapout(fs);
}

void getrealpath(char *out, char *in) {
	register DWORD o=0, i=0;
	register char * ptr;

	while(in[i]=='/') i++;

	ptr=strchr(in+i, '/');
	if(ptr!=NULL) {
		while((in+i)<ptr) {
			out[o]=toupper(in[i]);
			o++; i++;
		}
		strcpy(out+o, ":\\");
		o++;

		while(in[i]!=0) {
			if(in[i]=='/') out[o]='\\';
			else out[o]=in[i];
			o++; i++;
		}
	} else { /* no slashes in path */
		while(in[i]!=0) {
			out[o]=in[i];
			o++; i++;
		}
		strcpy(out+o, ":\\");
		o++;
	}

	out[o]=0;

#if 1
	OutputDebugString("Unix path: '");
	OutputDebugString(in);
	OutputDebugString("'  ");
	OutputDebugString("Real path: ");
	OutputDebugString(out);
	OutputDebugString("\n");
#endif
}

void transformpath(char *out, char* s1, char* s2) {
	if((out == NULL) || (s1 == NULL) || (s2 == NULL)) return;

	if((s2[0]!='/') && (strlen(s1) > 1)) {
		sprintf(out, "%s/%s",  s1, s2);
	} else {
		strcpy(out, s2);
	}
}

inline int filevalid(LPFTPDSTATE fs, char *param, char *rpout) {
	register char *filepath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(filepath == NULL) {
		ftpd_quit_421(fs, "Out of memory.");
		return 0;
	}
	register char *realpath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(realpath == NULL) {
		free(filepath);
		ftpd_quit_421(fs, "Out of memory.");
		return 0;
	}

	transformpath(filepath, fs->path, param);
	getrealpath(realpath, filepath);
	free(filepath);
	if(rpout!=NULL) strcpy(rpout, realpath);

	HANDLE tfile = CreateFile(realpath, 0, 0, NULL, OPEN_EXISTING, 0, NULL);

	free(realpath);

	if(tfile != INVALID_HANDLE_VALUE) {
		CloseHandle(tfile);
		return 1;
	}

	return 0;
}

inline int pathvalid(LPFTPDSTATE fs, char *param, char *rpout) {
	register char *filepath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(filepath == NULL) {
		ftpd_quit_421(fs, "Out of memory.");
		return 0;
	}
	register char *realpath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(realpath == NULL) {
		free(filepath);
		ftpd_quit_421(fs, "Out of memory.");
		return 0;
	}

	transformpath(filepath, fs->path, param);
	getrealpath(realpath, filepath);
	free(filepath);
	if(rpout!=NULL) strcpy(rpout, realpath);

	HANDLE tfile = CreateFile(realpath, 0, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	free(realpath);
	if(tfile != INVALID_HANDLE_VALUE) {
		CloseHandle(tfile);
		return 1;
	}
	return 0;
}

/***************************************************************************/
/* Access Control Commands                                                 */
/***************************************************************************/

void ftpd_cmd_user(LPFTPDSTATE fs, char *param) {
	if(fs->authenticated) {
		ftpd_send_reply( fs, 530, "You're already logged in." );
		return;
	}
	
	if(g_ftpnoauth==TRUE) {
		fs->userok=1;
		fs->authenticated=0;
		ftpd_send_reply( fs, 331, "User name okay, need password." );
		return;
	}
	
	//if(strlen(param)!=strlen(g_ftpuser)) goto user_eout;
	if(strlen(param)!=strlen((char *)myDash.sFTPUser.c_str())) goto user_eout;

	//if(!strcmp(param, g_ftpuser)) {
	if(!strcmp(param, (char *)myDash.sFTPUser.c_str())) {
		fs->userok=1;
		fs->authenticated=0;
		ftpd_send_reply( fs, 331, "User name okay, need password." );
		return;
	}

user_eout:
	fs->userok=0;
	fs->authenticated=0;
	ftpd_send_reply( fs, 530, "User not accepted." );
}


void ftpd_cmd_pass(LPFTPDSTATE fs, char *param) {
	if(fs->authenticated) {
		ftpd_send_reply( fs, 530, "You're already logged in." );
		return;
	}
	
	if(g_ftpnoauth==TRUE) {
		fs->authenticated=1;
		strcpy(fs->path, "/");
		ftpd_send_reply( fs, 230, "OK.  Current directory is /" );
		SetXBOXLEDColour(myDash.iLEDFTPConnect);
		return;
	}

	if(!fs->userok) {
		ftpd_send_reply( fs, 530, "Must send user first." );
		return;
	}

	//if(strlen(param)!=strlen(g_ftppass)) goto pass_eout;
	if(strlen(param)!=strlen((char *)myDash.sFTPPass.c_str())) goto pass_eout;

	//if(!strcmp(param, g_ftppass)) {
	if(!strcmp(param, (char *)myDash.sFTPPass.c_str())) {
		fs->authenticated=1;
		strcpy(fs->path, "/");
		ftpd_send_reply( fs, 230, "OK.  Current directory is /" );
		SetXBOXLEDColour(myDash.iLEDFTPConnect);
		return;
	}

pass_eout:
	fs->authenticated=0;
	ftpd_send_reply( fs, 530, "Password not accepted." );
}

void ftpd_cmd_acct(LPFTPDSTATE fs, char *param) {
	ftpd_send_reply( fs, 202, "Command not implemented, superfluous at this site." );
}

void ftpd_cmd_syst(LPFTPDSTATE fs, char *param) {
	ftpd_send_reply( fs, 215, "UNIX nXgDFTP." );
}

void ftpd_cmd_rein(LPFTPDSTATE fs, char *param) {
	fs->userok = 0;
	fs->authenticated = 0;
	ftpd_send_reply( fs, 220, "Service ready for new user." );
}

void ftpd_cmd_quit(LPFTPDSTATE fs, char *param) {
	SetXBOXLEDColour(myDash.iLEDStartup);
	ftpd_send_reply( fs, 221, "Closing control connection." );
	ftpd_crapout(fs);
}

void ftpd_cmd_cwd(LPFTPDSTATE fs, char *param) {
	if(!strcmp(param, "/")) {
		strcpy(fs->path, "/");
		ftpd_send_reply( fs, 200, "Requested file action okay, completed." );
	} else if(pathvalid(fs, param, NULL)) {
		if(param[0] == '/') {
			strncpy(fs->path, param, MAX_PATH);
			if((strlen(fs->path) > 1) && (fs->path[strlen(fs->path)-1]=='/')) fs->path[strlen(fs->path)-1]=0;
			ftpd_send_reply( fs, 200, "Requested file action okay, completed." );
		} else {
			if(strlen(fs->path) + strlen(param) + 1 < MAX_PATH) {
				if(fs->path[strlen(fs->path)-1]!='/') strcat(fs->path, "/");
				strcat(fs->path, param);
				if((strlen(fs->path) > 1) && (fs->path[strlen(fs->path)-1]=='/')) fs->path[strlen(fs->path)-1]=0;
				ftpd_send_reply( fs, 200, "Requested file action okay, completed." );
			} else {
				ftpd_send_reply( fs, 501, "Path too long." );
			}
		}
	} else {
		ftpd_send_reply( fs, 550, "Path not found." );
	}
}

void ftpd_cmd_cdup(LPFTPDSTATE fs, char *param) {
	char *lastslash = strrchr(fs->path, '/');
	if(lastslash == fs->path) {
		if(!strcmp(fs->path, "/")) {
			ftpd_send_reply( fs, 501, "Already at top level." );
		} else {
			strcpy(fs->path, "/");
			ftpd_send_reply( fs, 200, "Command okay." );
		}
	} else {
		lastslash[0]=0;
		ftpd_send_reply( fs, 200, "Command okay." );
	}
}

void ftpd_cmd_smnt(LPFTPDSTATE fs, char *param) {
	ftpd_send_reply( fs, 202, "Command not implemented, superfluous at this site." );
}



/***************************************************************************/
/* Transfer Paramater Commands                                             */
/***************************************************************************/

void ftpd_cmd_port(LPFTPDSTATE fs, char *param) {
	ftpd_close_data(fs);
	sscanf(param, "%d,%d,%d,%d,%d,%d",
		&fs->remoteaddr[0], &fs->remoteaddr[1], &fs->remoteaddr[2],
		&fs->remoteaddr[3], &fs->remoteaddr[4], &fs->remoteaddr[5] );
	ftpd_send_reply( fs, 200, "Command okay." );
}

void ftpd_cmd_pasv(LPFTPDSTATE fs, char *param) {
	register char *buf = (char *)malloc(512);
	if(buf != NULL) {
		sprintf(buf, "Entering Passive Mode (%i,%i,%i,%i,%i,%i).",
			fs->localaddr[0], fs->localaddr[1], fs->localaddr[2], 
			fs->localaddr[3], fs->localaddr[4], fs->localaddr[5]);
		ftpd_send_reply( fs, 227, buf );

		free(buf);
	} else {
		ftpd_quit_421(fs, "Out of memory.");
	}
}

void ftpd_cmd_type(LPFTPDSTATE fs, char *param) {
	if((param[0]=='A') || (param[0]=='I')) {
		fs->type = param[0];
		ftpd_send_reply(fs, 200, "Command okay.");
	} else {
		ftpd_send_reply(fs, 504, "Unknown representation type.");
	}
}

void ftpd_cmd_stru(LPFTPDSTATE fs, char *param) {
	if(param[0] == 'F')
		ftpd_send_reply(fs, 200, "Command okay.");
	else
		ftpd_send_reply(fs, 504, "Unknown file structure.");
}

void ftpd_cmd_mode(LPFTPDSTATE fs, char *param) {
	if(param[0] == 'S')
		ftpd_send_reply(fs, 200, "Command okay.");
	else
		ftpd_send_reply(fs, 504, "Unknown transfer mode.");
}

void ftpd_cmd_rest(LPFTPDSTATE fs, char *param) {
	sscanf(param, "%d", &fs->seekto );
	ftpd_send_reply( fs, 350, "Requested file action pending futher information." );
}



/***************************************************************************/
/* FTP Data Functions                                                      */
/***************************************************************************/

int lport=0;
int ftpd_open_data(LPFTPDSTATE fs) {
	int err;
	
	if((fs->datasock != NULL) && (fs->datasock != INVALID_SOCKET))
		ftpd_close_data(fs);

	fs->datasock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	SOCKADDR_IN remotesa;
	memset(&remotesa,0,sizeof(remotesa));
	remotesa.sin_family = AF_INET;
	remotesa.sin_addr.s_addr= (fs->remoteaddr[3]<<24) | (fs->remoteaddr[2]<<16) | (fs->remoteaddr[1]<<8) | (fs->remoteaddr[0]);
	remotesa.sin_port = htons( (fs->remoteaddr[4]<<8) | (fs->remoteaddr[5]) );

	err = connect( fs->datasock, (const sockaddr*)( &remotesa ), sizeof( SOCKADDR_IN ) );
	if(err) {
		ftpd_send_reply(fs, 425, "Can't open data connection.");
		return 0;
	}

	return 1;
}

#define BUFSIZE (4096*1024)
#define MAXSEND (8192)
inline void ftpd_send_file(LPFTPDSTATE fs, char *realpath) {
	register char *buf = (char *)malloc(BUFSIZE);
	if(buf==NULL) {
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}

	HANDLE file = CreateFile(realpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(file != INVALID_HANDLE_VALUE) {
		register LPDWORD readin = (LPDWORD)malloc(sizeof(DWORD));

		//Set LED Colour...
		SetXBOXLEDColour(myDash.iLEDTransfer);

		SetFilePointer(file, fs->seekto, NULL, FILE_BEGIN);
		if(readin != NULL) {
			for(;;) {
				register DWORD written;
				if(!ReadFile(file, buf, BUFSIZE, readin, NULL)) {
					char tmpbuf[512];
					sprintf(tmpbuf, "%i", GetLastError());
//					LogString(LOG_FTP, "Error reading: ", tmpbuf);
					break;
				}
				if(*readin == 0) 
					break;
				written=send(fs->datasock, buf, min(*readin,MAXSEND), 0);
				char *pbuf=buf;
				while((written>=0) && (written<*readin)) {
					pbuf+=written;
					*readin-=written;
					written=send(fs->datasock, pbuf, min(*readin,MAXSEND), 0);
				}
				if(written==SOCKET_ERROR) {
					ftpd_send_reply(fs, 451, "Socket error during transfer.");
				}
			}
			CloseHandle(file);
			free(readin);
			ftpd_send_reply(fs, 226, "Closing data connection.");
			ftpd_close_data(fs);
		} else {
			CloseHandle(file);
			ftpd_quit_421(fs, "Out of memory.");
			SetXBOXLEDColour(myDash.iLEDError);
		}
	} else {
		ftpd_send_reply(fs, 550, "Reqest failed; File not found.");
		SetXBOXLEDColour(myDash.iLEDError);
	}
	free(buf);
	//Set LED Colour...
	SetXBOXLEDColour(myDash.iLEDComplete);
}
#undef BUFSIZE

#define BUFSIZE (4160*1024)
#define BUFBLOCK (4096*1024)

inline void ftpd_recv_file(LPFTPDSTATE fs, char *param) {
	register char *buf = (char *)malloc(BUFSIZE);
	if(buf==NULL) {
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}

	register char *filepath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(filepath==NULL) {
		free(buf);
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}
	register char *realpath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(realpath==NULL) {
		free(filepath);
		free(buf);
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}

	transformpath(filepath, fs->path, param);
	getrealpath(realpath, filepath);
	HANDLE file;
	if(fs->seekto==0) {
		file = CreateFile(realpath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	} else {
		file = CreateFile(realpath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	}
	free(filepath);
	free(realpath);
	if(file != INVALID_HANDLE_VALUE) {

	//Set LED Colour...
	SetXBOXLEDColour(myDash.iLEDTransfer);

		register int readin, treadin=0;
		register LPDWORD written = (LPDWORD)malloc(sizeof(DWORD));
		SetFilePointer(file, fs->seekto, NULL, FILE_BEGIN);
		if(written != NULL) {
			for(;;) {
				readin = recv(fs->datasock, buf+treadin, 8192, 0);
				if(readin == SOCKET_ERROR) break;
				if(readin == 0) break;
				treadin+=readin;
				if(treadin > BUFBLOCK) {
					WriteFile(file, buf, BUFBLOCK, written, NULL);
					memmove(buf, buf+(*written), BUFSIZE-(*written));
					treadin-=(*written);
				}
			}
			if(treadin>0) WriteFile(file, buf, treadin, written, NULL);
			SetEndOfFile(file);
			CloseHandle(file);
			free(written);
			ftpd_send_reply(fs, 226, "Closing data connection.");
			ftpd_close_data(fs);
		} else {
			CloseHandle(file);
			ftpd_quit_421(fs, "Out of memory.");
			SetXBOXLEDColour(myDash.iLEDError);
		}
	} else {
		ftpd_send_reply(fs, 550, "Reqest failed; File not found.");
		SetXBOXLEDColour(myDash.iLEDError);
	}
	free(buf);
	//Set LED Colour...
	SetXBOXLEDColour(myDash.iLEDComplete);
}




/***************************************************************************/
/* FTP Service Commands                                                    */
/***************************************************************************/

void ftpd_cmd_retr(LPFTPDSTATE fs, char *param) {
	register char *realpath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(realpath==NULL) {
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}
	
	if(filevalid(fs, param, realpath)) {
		if(fs->datasock != INVALID_SOCKET) {
			ftpd_send_reply(fs, 125, "Data connection already open; transfer starting.");
			ftpd_send_file(fs, realpath);
		} else {	
			ftpd_send_reply(fs, 150, "File status okay; about to open data connection.");
			if(fs->remoteaddr[0]!=0) {
				if(ftpd_open_data(fs)) {
					ftpd_send_file(fs, realpath);
				}
			} else {
				ftpd_send_reply(fs, 425, "Unable to open data connection.");
			}
		}
	} else {
		ftpd_send_reply(fs, 550, "Requested file action not taken; File not found.");
	}
	fs->seekto =0;
	free(realpath);
}

void ftpd_cmd_stor(LPFTPDSTATE fs, char *param) {
//	if(filevalid(fs, param, NULL)) {
		if(fs->datasock != INVALID_SOCKET) {
			ftpd_send_reply(fs, 125, "Data connection already open; transfer starting.");
			ftpd_recv_file(fs, param);
		} else {	
			ftpd_send_reply(fs, 150, "File status okay; about to open data connection.");
			if(fs->remoteaddr[0]!=0) {
				if(ftpd_open_data(fs)) {
					ftpd_recv_file(fs, param);
				}
			} else {
				ftpd_send_reply(fs, 425, "Unable to open data connection.");
			}
		}
//	} else {
//		ftpd_send_reply(fs, 550, "Requested file action not taken; File not found.");
//	}
	fs->seekto =0;
}

void ftpd_cmd_size(LPFTPDSTATE fs, char *param) {
	register char *texttemp = (char *)malloc(128);
	if(texttemp==NULL) {
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}
	register char *realpath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(realpath==NULL) {
		free(texttemp);
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}
	if(filevalid(fs, param, realpath)) {
		LPWIN32_FILE_ATTRIBUTE_DATA attr = (LPWIN32_FILE_ATTRIBUTE_DATA)malloc(sizeof(WIN32_FILE_ATTRIBUTE_DATA));
		GetFileAttributesEx(realpath, GetFileExInfoStandard, attr);
		sprintf(texttemp, "%i", attr->nFileSizeLow);
		ftpd_send_reply(fs, 200, texttemp);
		free(attr);
	} else {
		ftpd_send_reply(fs, 550, "Requested file action not taken; File not found.");
	}
	free(texttemp);
	free(realpath);
}

void ftpd_cmd_allo(LPFTPDSTATE fs, char *param) {
	ftpd_send_reply(fs, 202, "Command not implemented, superfluous at this site.");
}

void ftpd_cmd_rnfr(LPFTPDSTATE fs, char *param) {
	if(strlen(param) > MAX_PATH) {
		ftpd_send_reply(fs, 501, "Parameter exceeds maximum path length");
	} else {
		strncpy(fs->renamefrom, param, MAX_PATH);
		ftpd_send_reply(fs, 350, "Requested file action pending further information.");
	}
}

void ftpd_cmd_rnto(LPFTPDSTATE fs, char *param) {
	if(fs->renamefrom[0] != 0) {
		register char *fromname=(char *)malloc(strlen(fs->path) + strlen(fs->renamefrom) + 16);
		if(fromname==NULL) {
			ftpd_quit_421(fs, "Out of memory.");
			return;
		}
		register char *toname=(char *)malloc(strlen(fs->path) + strlen(param) + 16);
		if(toname==NULL) {
			free(fromname);
			ftpd_quit_421(fs, "Out of memory.");
			return;
		}
		register char *temp = (char *)malloc(strlen(fs->path) + max(strlen(param),strlen(fs->renamefrom)) + 16);
		if(temp==NULL) {
			free(fromname);
			free(toname);
			ftpd_quit_421(fs, "Out of memory.");
			return;
		}

		transformpath(temp, fs->path, fs->renamefrom);
		getrealpath(fromname, temp);
		transformpath(temp, fs->path, param);
		getrealpath(toname, temp);

		if(MoveFile(fromname, toname)==TRUE) {
			ftpd_send_reply(fs, 250, "Requested file action okay, completed.");
		} else {
			ftpd_send_reply(fs, 550, "Requested action not taken; File unavailable.");
		}

		free(fromname);
		free(toname);
		free(temp);
	} else {
		ftpd_send_reply(fs, 503, "Bad sequence of commands.");
	}

	fs->renamefrom[0]=0;
}

void ftpd_cmd_abor(LPFTPDSTATE fs, char *param) {
	if(fs->intransfer) {
		fs->intransfer=0;
		ftpd_send_reply(fs, 426, "Connection closed; transfer aborted.");
	}
	ftpd_send_reply(fs, 226, "Closing data connection.");
	ftpd_close_data(fs);
}

void ftpd_cmd_dele(LPFTPDSTATE fs, char *param) {
	register char *filepath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(filepath == NULL) {
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}
	register char *realpath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(realpath == NULL) {
		free(filepath);
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}

	transformpath(filepath, fs->path, param);
	getrealpath(realpath, filepath);
	if(DeleteFile(realpath)==TRUE) {
		ftpd_send_reply(fs, 250, "Requested file action okay, completed.");
	} else {
		ftpd_send_reply(fs, 550, "Requested action not taken; File unavailable.");
	}

	free(filepath);
	free(realpath);
}

void ftpd_cmd_rmd(LPFTPDSTATE fs, char *param) {
	register char *filepath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(filepath == NULL) {
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}
	register char *realpath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(realpath == NULL) {
		free(filepath);
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}

	transformpath(filepath, fs->path, param);
	getrealpath(realpath, filepath);
	free(filepath);

	if(RemoveDirectory(realpath)==TRUE) {
		ftpd_send_reply(fs, 250, "Requested file action okay, completed.");
	} else {
		ftpd_send_reply(fs, 550, "Requested action not taken; File unavailable.");
		char tmpbuf[512];
		sprintf(tmpbuf, "%i", GetLastError());
//		LogString(LOG_FTP, "Error reading: ", tmpbuf);
	}

	free(realpath);	
}

void ftpd_cmd_mkd(LPFTPDSTATE fs, char *param) {
	register char *filepath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(filepath == NULL) {
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}
	register char *realpath = (char *)malloc(strlen(fs->path) + strlen(param) + 16);
	if(realpath == NULL) {
		free(filepath);
		ftpd_quit_421(fs, "Out of memory.");
		return;
	}

	transformpath(filepath, fs->path, param);
	getrealpath(realpath, filepath);
	free(filepath);

	if(CreateDirectory(realpath, NULL)==TRUE) {
		register char *buf = (char *)malloc(512);
		if(buf != NULL) {
			sprintf(buf, "\"%s\" created.", param);
			ftpd_send_reply(fs, 257, buf);
			free(buf);
		} else {
			ftpd_quit_421(fs, "Out of memory.");
		}
	} else {
		ftpd_send_reply(fs, 550, "Requested action not taken; File unavailable.");
	}

	free(realpath);
}

void ftpd_cmd_pwd(LPFTPDSTATE fs, char *param) {
	register char *buf = (char *)malloc(512);
	if(buf != NULL) {
		sprintf(buf, "Current directory: \"%s\"", fs->path);
		ftpd_send_reply(fs, 257, buf);
		free(buf);
	} else {
		ftpd_quit_421(fs, "Out of memory.");
	}
}

char defaultentries[] = "drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 .\r\ndrwxrwxrwx  xbox  xbox                 0 Jan  1  1980 ..\r\n";

char rootpaths[][80] = {
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 DVD\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 SYSTEM\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 STORAGE\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 EXTENDED\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 EXTENDED G\r\n",

/*	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 H\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 I\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 J\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 K\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 L\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 M\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 N\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 O\r\n",
*/	
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 TEMPX\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 TEMPY\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 TEMPZ\r\n",
	""
};

char classicrootpaths[][80] = {
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 C\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 D\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 E\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 F\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 G\r\n",

/*	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 H\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 I\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 J\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 K\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 L\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 M\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 N\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 O\r\n",
*/	
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 X\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 Y\r\n",
	"drwxrwxrwx  xbox  xbox                 0 Jan  1  1980 Z\r\n",
	""
};

char rootnames[][16] = {
	"DVD\r\n",
	"SYSTEM\r\n",
	"STORAGE\r\n",
	"EXTENDED\r\n",
	"EXTENDED G\r\n",
/*
	"H\r\n",
	"I\r\n",
	"J\r\n",
	"K\r\n",
	"L\r\n",
	"M\r\n",
	"N\r\n",
	"O\r\n",
*/	
	"TEMPX\r\n",
	"TEMPY\r\n",
	"TEMPZ\r\n",
	""
};

char classicrootnames[][16] = {
	"C\r\n",
	"D\r\n",
	"E\r\n",
	"F\r\n",
	"G\r\n",
/*
	"H\r\n",
	"I\r\n",
	"J\r\n",
	"K\r\n",
	"L\r\n",
	"M\r\n",
	"N\r\n",
	"O\r\n",
*/	
	"X\r\n",
	"Y\r\n",
	"Z\r\n",
	""
};


void ftpd_cmd_list(LPFTPDSTATE fs, char *param) {
	register char *filepath;
	register char *realpath;
	register char *buf;

	param = ""; // Temp Fix for -al Commands. (Fixes FlashFXP v3, CuteFTP v6, SmartFTP needs work)
	
	if(strlen(fs->path) == 1) {
		if((fs->datasock != INVALID_SOCKET) && (fs->datasock != NULL)) {
			ftpd_send_reply(fs, 125, "Data connection already open; transfer starting.");
		} else {	
			ftpd_send_reply(fs, 150, "File status okay; about to open data connection.");
			if(!ftpd_open_data(fs))
				return;
		}

		//if(g_stSettings.m_bClassicDrives){
			for(int i = 0; strlen(classicrootpaths[i]); i++)
			send(fs->datasock, classicrootpaths[i], strlen(classicrootpaths[i]), 0);
		/*}else{
		for(int i = 0; strlen(rootpaths[i]); i++)
			send(fs->datasock, rootpaths[i], strlen(rootpaths[i]), 0);
		}*/
		ftpd_send_reply(fs, 226, "Closing data connection; Request sucessful.");
		ftpd_close_data(fs);
		return;
	}

	buf = (char *)malloc(8192);
	if(buf == NULL)
		ftpd_quit_421(fs, "Out of memory.");
	
	register LPWIN32_FIND_DATA finddata = (LPWIN32_FIND_DATA)malloc(sizeof(WIN32_FIND_DATA));
	if(finddata == NULL) {
		free(buf);
		ftpd_quit_421(fs, "Out of memory.");
	}

	register int tm=0;
	if(param!=NULL) tm = strlen(param);
	filepath = (char *)malloc(strlen(fs->path) + tm + 16);
	if(filepath == NULL) {
		free(buf);
		free(finddata);
		ftpd_quit_421(fs, "Out of memory.");
	}

	realpath = (char *)malloc(strlen(fs->path) + tm + 16);
	if(realpath == NULL) {
		free(filepath);
		free(buf);
		free(finddata);
		ftpd_quit_421(fs, "Out of memory.");
	}

	if(param != NULL && strlen(param)) {
		transformpath(filepath, fs->path, param);
		getrealpath(realpath,filepath);
	} else {
		transformpath(filepath, fs->path, "*"); // "*"
		getrealpath(realpath,filepath);
	}

	if(fs->datasock != INVALID_SOCKET) {
		ftpd_send_reply(fs, 125, "Data connection already open; transfer starting.");
	} else {	
		ftpd_send_reply(fs, 150, "File status okay; about to open data connection.");
		if(!ftpd_open_data(fs)) {
			free(buf);
			free(finddata);
			free(filepath);
			free(realpath);
			return;
		}
	}

	//send(fs->datasock, defaultentries, strlen(defaultentries), 0);

	register HANDLE hFind=FindFirstFile(realpath, finddata);
	if(hFind != INVALID_HANDLE_VALUE) {
		char bufa[5];
		do {
			if(finddata->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				strcpy(bufa, "d");
			else 
				strcpy(bufa, "-");

			if(finddata->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				strcpy(bufa+1, "r-x");
			else 
				strcpy(bufa+1, "rwx");

			//Get Time and Date from Files
			char *thisfile = (char *)HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, 1024);
			*thisfile = 0;  // start with a empty string.
			// size
            char bbb[512];
            wsprintf (bbb, "%14d ", finddata->nFileSizeLow);
            strcat (thisfile, bbb);

            SYSTEMTIME st;
            FileTimeToSystemTime (&finddata->ftCreationTime, &st);
            switch (st.wMonth)
            {
                case 1:
                    strcat(thisfile, "Jan ");
                    break;
                case 2:
                    strcat(thisfile, "Feb ");
                    break;
                case 3:
                    strcat(thisfile, "Mar ");
                    break;
                case 4:
                    strcat(thisfile, "Apr ");
                    break;
                case 5:
                    strcat(thisfile, "May ");
                    break;
                case 6:
                    strcat(thisfile, "Jun ");
                    break;
                case 7:
                    strcat(thisfile, "Jul ");
                    break;
                case 8:
                    strcat(thisfile, "Aug ");
                    break;
                case 9:
                    strcat(thisfile, "Sep ");
                    break;
                case 10:
                    strcat(thisfile, "Oct ");
                    break;
                case 11:
                    strcat(thisfile, "Nov ");
                    break;
                case 12:
                    strcat(thisfile, "Dec ");
                    break;
            }
            wsprintf (bbb, "%2d %02d:%02d ", st.wDay, st.wHour, st.wMinute);
            strcat (thisfile, bbb);

            strcat (thisfile, finddata->cFileName);
            strcat (thisfile, "\r\n");
			//End Time and Date
			sprintf(buf, "%s%s%s  xbox  xbox  %s\r\n",
					bufa, bufa+1, bufa+1, thisfile);
			
			/*sprintf(buf, "%s%s%s  xbox  xbox  %16d Jan  1  1980 %s\r\n",
					bufa, bufa+1, bufa+1, finddata->nFileSizeLow, finddata->cFileName );*/
			send(fs->datasock, buf, strlen(buf), 0);
			HeapFree(GetProcessHeap (), HEAP_ZERO_MEMORY, thisfile);
		} while(FindNextFile(hFind, finddata));
		ftpd_send_reply(fs, 226, "Closing data connection; Request sucessful.");
		ftpd_close_data(fs);
	} else {
		switch(GetLastError()) {
		case ERROR_FILE_NOT_FOUND:
			ftpd_send_reply(fs, 226, "Closing data connection; Request sucessful.");
			ftpd_close_data(fs);
			break;
		default:
			ftpd_send_reply(fs, 451, "Request aborted; local error in processing.");
		}
	}

	
	free(buf);
	free(finddata);
	FindClose(hFind);
	free(filepath);
	free(realpath);
}


void ftpd_cmd_nlst(LPFTPDSTATE fs, char *param) {
	register char *filepath;
	register char *realpath;
	register WIN32_FIND_DATA *finddata;
	register HANDLE hFind;
	
	if(strlen(fs->path) == 1) {
		if((fs->datasock != INVALID_SOCKET) && (fs->datasock != NULL)) {
			ftpd_send_reply(fs, 125, "Data connection already open; transfer starting.");
		} else {	
			ftpd_send_reply(fs, 150, "File status okay; about to open data connection.");
			if(!ftpd_open_data(fs))
				return;
		}

		//if (g_stSettings.m_bClassicDrives){
		for(int i = 0; strlen(classicrootnames[i]); i++)
			send(fs->datasock, classicrootnames[i], strlen(classicrootnames[i]), 0);
		/*}else{
		for(int i = 0; strlen(rootnames[i]); i++)
			send(fs->datasock, rootnames[i], strlen(rootnames[i]), 0);
		}*/
		ftpd_send_reply(fs, 226, "Closing data connection; Request sucessful.");
		ftpd_close_data(fs);
		return;
	}

	finddata = (LPWIN32_FIND_DATA)malloc(sizeof(WIN32_FIND_DATA));
	if(finddata == NULL)
		ftpd_quit_421(fs, "Out of memory.");

	register int tm=0;
	if(param!=NULL) tm = strlen(param);
	filepath = (char *)malloc(strlen(fs->path) + tm + 16);
	if(filepath == NULL) {
		free(finddata);
		ftpd_quit_421(fs, "Out of memory.");
	}

	realpath = (char *)malloc(strlen(fs->path) + tm + 16);
	if(realpath == NULL) {
		free(filepath);
		free(finddata);
		ftpd_quit_421(fs, "Out of memory.");
	}

	if(param != NULL && strlen(param)) {
		transformpath(filepath, fs->path, param);
		getrealpath(realpath,filepath);
	} else {
		transformpath(filepath, fs->path, "*");
		getrealpath(realpath,filepath);
	}

	if(fs->datasock != INVALID_SOCKET) {
		ftpd_send_reply(fs, 125, "Data connection already open; transfer starting.");
	} else {	
		ftpd_send_reply(fs, 150, "File status okay; about to open data connection.");
		if(!ftpd_open_data(fs)) {
			free(finddata);
			free(filepath);
			free(realpath);
			return;
		}
	}

	send(fs->datasock, ".\r\n..\r\n", 7, 0);
	hFind=FindFirstFile(realpath, finddata);
	if(hFind != INVALID_HANDLE_VALUE) {
		do {
			send(fs->datasock, finddata->cFileName, strlen(finddata->cFileName), 0);
			send(fs->datasock, "\r\n", 2, 0);
		} while(FindNextFile(hFind, finddata));
		ftpd_send_reply(fs, 226, "Closing data connection; Request sucessful.");
		ftpd_close_data(fs);
	} else {
		switch(GetLastError()) {
		case ERROR_FILE_NOT_FOUND:
			ftpd_send_reply(fs, 226, "Closing data connection; Request sucessful.");
			ftpd_close_data(fs);
			break;
		default:
			ftpd_send_reply(fs, 451, "Request aborted; local error in processing.");
		}
	}

	free(finddata);
	free(filepath);
	free(realpath);
}

void ftpd_cmd_noop(LPFTPDSTATE fs, char *param) {
	ftpd_send_reply(fs, 200, "Command okay.");
}

void ftpd_cmd_poweroff(LPFTPDSTATE fs, char *param) {
	ftpd_send_reply(fs, 200, "Command okay. Session Ends Now.");
	//ftpd_crapout(fs);
	//Sleep(5);
	XKUtils::XBOXPowerOff();
}

void ftpd_cmd_reboot(LPFTPDSTATE fs, char *param) {
	ftpd_send_reply(fs, 200, "Command okay. Session Ends Now.");
	//ftpd_crapout(fs);
	//Sleep(5);
	XKUtils::XBOXReset();
}

void ftpd_cmd_trayeject(LPFTPDSTATE fs, char *param) {
	ftpd_send_reply(fs, 200, "Command okay.");
	XKUtils::DVDEjectTray();
}

void ftpd_cmd_trayclose(LPFTPDSTATE fs, char *param) {
	ftpd_send_reply(fs, 200, "Command okay.");
	XKUtils::DVDLoadTray();
}

void ftpd_cmd_execute(LPFTPDSTATE fs, char *param) {
	register int tm=0;
	if(param!=NULL) tm = strlen(param);
	char *filepath = (char *)malloc(strlen(fs->path) + tm + 16);
	if(filepath == NULL) {
		ftpd_quit_421(fs, "Out of memory.");
	}

	char *realpath = (char *)malloc(strlen(fs->path) + tm + 16);
	if(realpath == NULL) {
		free(filepath);
		ftpd_quit_421(fs, "Out of memory.");
	}

	transformpath(filepath, fs->path, param);
	getrealpath(realpath, filepath);
	myXBELauncher.LaunchMyXBE(realpath, NULL);
}

void ftpd_cmd_version(LPFTPDSTATE fs, char *param) {
	ftpd_send_reply(fs, 200, "neXgen v2.0 Build 1451");
}

/***************************************************************************/
/* FTP Protocol Interpreter                                                */
/***************************************************************************/

struct {
	char *text;
	void (*cmd)(LPFTPDSTATE fs, char *param);
} cmdlist[] = {
	{ "USER", ftpd_cmd_user },
	{ "PASS", ftpd_cmd_pass },
	{ "ACCT", ftpd_cmd_acct },
	{ "CWD", ftpd_cmd_cwd },
	{ "CDUP", ftpd_cmd_cdup },
	{ "SMNT", ftpd_cmd_smnt },
	{ "REIN", ftpd_cmd_rein },
	{ "QUIT", ftpd_cmd_quit },
	{ "PORT", ftpd_cmd_port },
	//{ "PASV", ftpd_cmd_pasv },
	{ "TYPE", ftpd_cmd_type },
	{ "STRU", ftpd_cmd_stru },
	{ "MODE", ftpd_cmd_mode },
	{ "REST", ftpd_cmd_rest },
	{ "RETR", ftpd_cmd_retr },
	{ "STOR", ftpd_cmd_stor },
	{ "RNFR", ftpd_cmd_rnfr },
	{ "RNTO", ftpd_cmd_rnto },
	{ "ABOR", ftpd_cmd_abor },
	{ "DELE", ftpd_cmd_dele },
	{ "RMD", ftpd_cmd_rmd },
	{ "XRMD", ftpd_cmd_rmd },
	{ "MKD", ftpd_cmd_mkd },
	{ "PWD", ftpd_cmd_pwd },
	{ "XPWD", ftpd_cmd_pwd },
	{ "SYST", ftpd_cmd_syst },
	{ "SIZE", ftpd_cmd_size },
	{ "LIST", ftpd_cmd_list },
	{ "NLST", ftpd_cmd_nlst },
	{ "NOOP", ftpd_cmd_noop },
	{ "POWEROFF", ftpd_cmd_poweroff },
	{ "REBOOT", ftpd_cmd_reboot },
	{ "TRAYEJECT", ftpd_cmd_trayeject },
	{ "TRAYCLOSE", ftpd_cmd_trayclose },
	{ "EXECUTE", ftpd_cmd_execute },
	{ "VERSION", ftpd_cmd_version },
	{ NULL, NULL }
};


DWORD WINAPI ftpd_pi( LPVOID lpParam ) {
	LPFTPDSTATE fs = (LPFTPDSTATE)lpParam;

	g_ftp_connections++;

	ftpd_send_reply(fs, 220, "Service ready for new user.");
	int i;

	char *buf = (char *) malloc(8192);
	if(buf == NULL) ftpd_quit_421(fs, "Out of memory.");

	for(;;) {
	nextcmd:
		char *param=NULL;
		int readin = recv(fs->ctrlsock, buf, 8192, 0);
		if(readin == SOCKET_ERROR) return 0;

		for(i=0; i<readin; i++) {
			if((buf[i]==' ') && (param==NULL))
				param=buf+i+1;
			if((buf[i]=='\r') || (buf[i]=='\n'))
				buf[i]=0;
		}
		for(i=0; cmdlist[i].text!=NULL; i++) {
			if(!strnicmp(buf, cmdlist[i].text, strlen(cmdlist[i].text))) {
				cmdlist[i].cmd(fs, param);
				goto nextcmd;
			}
		}
//		LogString(LOG_FTP, "Command not implemented: ", buf);
		ftpd_send_reply(fs, 502, "Command not implemented.");
	}

	ftpd_crapout(fs);
	free(buf); //Delete Fix

	return 0;
}

DWORD WINAPI ftpd_thread( LPVOID lpParam ) {
	int err;

	if(bNetworkingUp!=TRUE) return 0;

	// Create TCP socket
	SOCKET sLBrd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if(sLBrd == INVALID_SOCKET) {
		return 0;
	}

	// Create UDP socket
	SOCKET sBrd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if(sBrd == INVALID_SOCKET) {
		return 0;
	}

	// Bind to INADDR_ANY
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons( g_ftpport );
	err = bind( sLBrd, (const sockaddr*)( &sa ), sizeof( SOCKADDR_IN ) );
	if(err != 0) {
//		LogString(LOG_FTP, "Unable to bind to port 21.", "");
		goto ftpd_exit;
	}

	err = listen( sLBrd, 5 );
	if(err != 0) {
//		LogString(LOG_FTP, "Unable to listen to socket.", "");
		goto ftpd_exit;
	}

	// Ava / Qwix Detection....
	// Create broadcast address
	sa.sin_addr.s_addr = INADDR_BROADCAST;
	sa.sin_port = htons( 4905 );
	sendto(sBrd,"ping",5,0,(struct sockaddr *)(&sa),sizeof(sa));

	while(bNetworkingUp == TRUE) {
		SOCKET newsock;
		SOCKADDR_IN newsa;
		int sasize = sizeof(newsa);
		newsock = accept( sLBrd, (sockaddr*)( &newsa ), &sasize );
		if(newsock != INVALID_SOCKET) {
			char *tbuf=(char*)malloc(32);
			if(tbuf!=NULL) {
				sprintf(tbuf, "%i.%i.%i.%i", (newsa.sin_addr.S_un.S_addr)&0xff, (newsa.sin_addr.S_un.S_addr>>8)&0xff, (newsa.sin_addr.S_un.S_addr>>16)&0xff, (newsa.sin_addr.S_un.S_addr<<24)&0xff);
//				LogString(LOG_FTP, "Recieved connection from ", tbuf);
				free(tbuf);
			}
			LPFTPDSTATE fs = (LPFTPDSTATE) malloc(sizeof(FTPDSTATE));
			memset(fs, 0, sizeof(FTPDSTATE));
			fs->ctrlsock = newsock;
			fs->localaddr[0] = (unsigned char)((MYIP.S_un.S_addr>>24)&0xff);
			fs->localaddr[1] = (unsigned char)((MYIP.S_un.S_addr>>16)&0xff);
			fs->localaddr[2] = (unsigned char)((MYIP.S_un.S_addr>>8)&0xff);
			fs->localaddr[3] = (unsigned char)(MYIP.S_un.S_addr&0xff);
			fs->localaddr[4] = (g_ftpport-1)>>8;
			fs->localaddr[5] = (g_ftpport-1)&0xff;
			//Small Changes: localaddr --> remoteaddr.
			fs->remoteaddr[0] = (unsigned char)((newsa.sin_addr.S_un.S_addr>>24)&0xff);
			fs->remoteaddr[1] = (unsigned char)((newsa.sin_addr.S_un.S_addr>>16)&0xff);
			fs->remoteaddr[2] = (unsigned char)((newsa.sin_addr.S_un.S_addr>>8)&0xff);
			fs->remoteaddr[3] = (unsigned char)(newsa.sin_addr.S_un.S_addr&0xff);
			fs->remoteaddr[4] = (unsigned char)((newsa.sin_port>>8)&0xff);
			fs->remoteaddr[5] = (unsigned char)(newsa.sin_port&0xff);
			fs->type='A';
			strcpy(fs->path, "/");
			{
				DWORD	dwThreadId;
				HANDLE	hThread; 

				hThread = CreateThread( 
					NULL,						// (this parameter is ignored)
					0,							// use default stack size  
					ftpd_pi,						// thread function 
					fs,							// argument to thread function 
					0,							// use default creation flags 
					&dwThreadId);				// returns the thread identifier 
 
				// Check the return value for success. 
				if (hThread == NULL) {
//					LogString(LOG_FTP, "Worker thread creation failed.", "");
				}

				//LogString(LOG_FTP, "Worker thread created.", "");
				CloseHandle( hThread );
			}
		}
	}

ftpd_exit:

	shutdown(sLBrd, SD_SEND);
	closesocket(sLBrd);

	return 0;
}


HRESULT ftpd_init() {
	DWORD	dwThreadId;
	HANDLE	hThread; 

	hThread = CreateThread( 
		NULL,						// (this parameter is ignored)
		0,							// use default stack size  
		ftpd_thread,				// thread function 
		NULL,						// argument to thread function 
		0,							// use default creation flags 
		&dwThreadId);				// returns the thread identifier 
 
	// Check the return value for success. 
	if (hThread == NULL) 
	{
//			LogString(LOG_FTP, "Thread creation failed.", "");
	}

//	LogString(LOG_FTP, "Thread created.", "");
	CloseHandle( hThread );
	return S_OK;
}

