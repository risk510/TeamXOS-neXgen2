#include "Menu.h"
#include "Utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

void MenuList::AddNode (PMenuNode newitem) {
	PMenuNode MenuCurr = MenuHead;
	// If the menu is NULL set it to newitem
	if (MenuCurr==NULL) {
		MenuHead = newitem;
		return;
	}
	while(MenuCurr->Next!=NULL) {
		if(strcmp(MenuCurr->Title, newitem->Title) > 0) {
			if(MenuCurr->Prev!=NULL) {
				newitem->Prev = MenuCurr->Prev;
				newitem->Prev->Next = newitem;
			}
			if(MenuCurr->Next!=NULL) {
				newitem->Next = MenuCurr;
				newitem->Next->Prev = newitem;
			}
			if(MenuCurr==MenuHead) MenuHead = newitem;
			return;
		}
		MenuCurr=MenuCurr->Next;
	}
	// Reached the end and didnt find anywhere to add the newitem
	// so add it to the end
	newitem->Prev = MenuCurr;
	newitem->Prev->Next = newitem;
	return;
}

PMenuNode MenuList::FindNode (const char *name) {
	for(PMenuNode node=MenuHead; node!=NULL; node=node->Next) {
		if(!strcmp(name,node->Title))
			return node;
	}
	return NULL;
}

void MenuList::Clear () {
	MenuHead = NULL;
}

void MenuList::Rename (char* str) {
	strncpy(MenuName, str, 255);
}

char* MenuList::GetName () {
	return (char*)MenuName;
}

// Check TitleImage.xbx Exsists
BOOL  FileFound( LPCTSTR szFilename )
{
	BOOL bReturn = FALSE;
	DWORD dwAttributes;

		if ( !bReturn )
		{
			dwAttributes = GetFileAttributes( szFilename );
			if ( dwAttributes != (DWORD)-1 )
			{
				bReturn = TRUE;
			}
		}

	return bReturn;
}

//End FileFound

PMenuNode MenuList::AddXBE (char *filename) {
	DWORD baseaddr, certaddr, cert_id, region,rating;
	WCHAR cert_string[41];
	WCHAR alt_cert_string[41];

	char *path = new char[MAX_PATH];
	char *xbepath = new char[MAX_PATH];
	char *xbxpath = new char[MAX_PATH];
	char *xbxdir = new char[MAX_PATH];
	char *metaxbxdir = new char[MAX_PATH];
	char *metaxbxname = new char[MAX_PATH];
	char *realdir = new char[MAX_PATH];

	HANDLE xbefd=CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if(xbefd!=INVALID_HANDLE_VALUE) {
		DWORD readin;

		SetFilePointer(xbefd, 0x104, NULL, FILE_BEGIN);
		ReadFile(xbefd, &baseaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, 0x118, NULL, FILE_BEGIN);
		ReadFile(xbefd, &certaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0x8, NULL, FILE_BEGIN);
		ReadFile(xbefd, &cert_id, 0x4, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0xc, NULL, FILE_BEGIN);
		ReadFile(xbefd, &cert_string, 0x50, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0x5c, NULL, FILE_BEGIN);
		ReadFile(xbefd, &alt_cert_string, 0x50, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0xa0, NULL, FILE_BEGIN);
		ReadFile(xbefd, &region, 0x4, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0xa4, NULL, FILE_BEGIN);
		ReadFile(xbefd, &rating, 0x4, &readin, NULL);


		cert_string[0x50]=0x0000;

		CloseHandle(xbefd);

		PMenuNode newitem = new MenuNode;

		if(newitem!=NULL) {
			int added=0;
			newitem->TitleID=cert_id;
			sprintf(newitem->File, "%s", filename);
			if(!added) {
				int x=lstrlenW(cert_string);
				if(x==0) {
					//strcpy(newitem->Title, "Unknown Title");
					ZeroMemory(newitem->Title,256);
					strcpy(newitem->Title, FolderNameFromFilePathA(filename));
				} else if(x<44) {
					ZeroMemory(newitem->Title,256);
					sprintf(newitem->Title, "%S", cert_string);
				} else {
					WCHAR tstr[33];
					ZeroMemory(tstr,33);
					lstrcpynW(tstr, cert_string, 32);
					sprintf(newitem->Title, "%S...", tstr);
				}
			}
			
			{
				char * idtmp = strdup(filename);
				char * imtmp = strrchr(idtmp, '\\');
				imtmp[0]=0;
				newitem->DBlocks = XGetDisplayBlocks(idtmp);
				free(idtmp);
			}

			//Search XBE Path 1st.
			strcpy(path, PathFromFilePathA(filename));
			sprintf(newitem->Icon, "%s\\TitleImage.xbx", path);
			if ( !FileFound( newitem->Icon ) )
			{
				
				//No TitleImage.xbx found in app path look in app's UDATA folder.
				//sprintf(newitem->Icon, "E:\\UDATA\\%08x\\TitleImage.xbx", cert_id);
				ZeroMemory(newitem->Icon,sizeof(newitem->Icon)+1);
				sprintf(newitem->Icon, "STORAGE:\\UDATA\\%08x\\TitleImage.xbx", cert_id);

				//Extract XBX from XBE File....
				if ( !FileFound( newitem->Icon ) )
				{
					sprintf(xbepath, "%s\\default.xbe", path);
					sprintf(xbxpath, "STORAGE:\\UDATA\\%08x\\TitleImage.xbx", cert_id);
					sprintf(realdir, "STORAGE:\\UDATA\\%08x", cert_id);
					if (CreateDirectory(realdir,NULL))
					{
					}
					xbeReader.ExtractIcon(xbepath, xbxpath);
					sprintf(newitem->Icon, "STORAGE:\\UDATA\\%08x\\TitleImage.xbx", cert_id);

					//Write TitleMeta.XBX
					sprintf(metaxbxdir, "STORAGE:\\UDATA\\%08x\\TitleMeta.xbx", cert_id);
					if (FileExists(metaxbxdir))
					{
						//File Exists do nothing
					}
					else
					{
						//Create new TitleMeta.xbx for this Title

						HANDLE hTitleMeta ( CreateFile( metaxbxdir, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL ) );
        
						if (hTitleMeta == INVALID_HANDLE_VALUE) {
							return false;
						}
        
						sprintf(metaxbxname, "TitleName=%s", newitem->Title);
						OutputDebugString(metaxbxname);
						DWORD dwWrote;
						WriteFile( (HANDLE)hTitleMeta,metaxbxname,(DWORD)strlen(metaxbxname),&dwWrote,NULL);
						CloseHandle(hTitleMeta);
					}

				}


				if ( !FileFound( newitem->Icon ) )
				{
					ZeroMemory(newitem->Icon,sizeof(newitem->Icon)+1);
					//No TitleImage.xbx found load neXgen's TitleImage.xbx
					sprintf(newitem->Icon, "U:\\TitleImage.xbx");
				}

			}

			if (region) {
				newitem->Region=region;
			} else {
				newitem->Region = 0;
			}
			
			if (rating) {
				newitem->Rating = rating;
			} else {
				newitem->Rating = 0;
			}

		}
		// Add the newitem to the MenuHead node
		AddNode(newitem);
		// not sure if return is needed will need to test
		return newitem;
	} else {
		OutputDebugString("Error adding ");
		OutputDebugString(filename);
		OutputDebugString("to a list.\n");
		return NULL;
	}
}

PMenuNode MenuList::AddSave(char *filename) {
	
	char *path = new char[MAX_PATH];
	OutputDebugString ("Processing file ");
	OutputDebugString (filename);
	OutputDebugString ("\n");
	PMenuNode newitem = new MenuNode;
	HANDLE xbefd=CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if(xbefd!=INVALID_HANDLE_VALUE) {
		DWORD readin;
		strcpy(path, PathFromFilePathA(filename));
		sprintf(newitem->Icon, "%s\\TitleImage.xbx", path);
		if ( !FileFound( newitem->Icon ) )
		{
			OutputDebugString ("Did not find custom xbx file\n");
			//No TitleImage.xbx found load neXgen's TitleImage.xbx
			sprintf(newitem->Icon, "U:\\TitleImage.xbx");
		}

		// Add the DBlocks
		char * idtmp = strdup(filename);
		char * imtmp = strrchr(idtmp, '\\');
		imtmp[0]=0;
		newitem->DBlocks = XGetDisplayBlocks(idtmp);
		free(idtmp);
		
		// Try to work out the TitleMeta name. 
		char tmpa;
		bool foundend = FALSE;
		bool foundstart = FALSE;
		char name[256] = "";
		int strpos = 0;
		DWORD pos=0;
		OutputDebugString ("Going to look in titlemeta.xbx\n");
		while (!(foundend)) {
			SetFilePointer(xbefd, pos, NULL, FILE_BEGIN);
			ReadFile(xbefd, &tmpa, 0x2, &readin, NULL);
			OutputDebugString ("Read char\n");
			if (tmpa && !foundstart) {
				OutputDebugString ("tmpa && !foundstart\n");
				if (tmpa == 0x3d) {
					OutputDebugString ("found 0x3d\n");
					foundstart=true;
				}
			} else if (tmpa && !foundend) {
				OutputDebugString ("tmpa && !foundend\n");
				if (tmpa == 0x0d) {
					OutputDebugString ("found 0x0d\n"); 
					foundend=true;
				} else {
					name[strpos] = tmpa;
					strpos++;
					OutputDebugString ("done printf\n");
				}
			} else {
					OutputDebugString ("found null value\n");
			}
			pos++;
		}
		CloseHandle(xbefd);
		sprintf(newitem->Title,"%S", name);
		OutputDebugString ("Going to add node\n");
		AddNode(newitem);
		// not sure if return is needed will need to test
		OutputDebugString ("Going to return\n");
		return newitem;
	} else {
		OutputDebugString("Error adding ");
		OutputDebugString(filename);
		OutputDebugString("to a list.\n");
		return NULL;
	}
}
		
/*		if(newitem!=NULL) {
			int added=0;
			newitem->TitleID=cert_id;
			sprintf(newitem->File, "%s", filename);
			if(!added) {
				int x=lstrlenW(cert_string);
				if(x==0) {
					//strcpy(newitem->Title, "Unknown Title");
					strcpy(newitem->Title, FolderNameFromFilePathA(filename));
				} else if(x<44) {
					sprintf(newitem->Title, "%S", cert_string);
				} else {
					WCHAR tstr[33];
					lstrcpynW(tstr, cert_string, 32);
					sprintf(newitem->Title, "%S...", tstr);
				}
			}
			
			{
				char * idtmp = strdup(filename);
				char * imtmp = strrchr(idtmp, '\\');
				imtmp[0]=0;
				newitem->DBlocks = XGetDisplayBlocks(idtmp);
				free(idtmp);
			}


		// Add the newitem to the MenuHead node
		*/

PMenuNode MenuList::AddFromCache (char *Title, char *File, char *Icon, char *Cmdline, DWORD Action, DWORD TitleID, DWORD Region, DWORD Rating, DWORD DBlocks) 
{
	PMenuNode newitem = new MenuNode;
	sprintf (newitem->Title,"%s", Title);
	sprintf (newitem->File, "%s", File);
	sprintf (newitem->Icon,"%s",Icon);
	sprintf (newitem->Cmdline, "%s",Cmdline);
	newitem->Action = Action;
	newitem->TitleID = TitleID;
	newitem->Rating = Rating;
	newitem->Region = Region;
	OutputDebugString ("Setting DBlocks\n");
	newitem->DBlocks = DBlocks;
   	AddNode(newitem);
	return newitem;
}

PMenuNode MenuList::AddFile (char *filename) {
	PMenuNode newitem = new MenuNode;
	if(newitem!=NULL) {
		strcpy(newitem->File, filename);
		if(char *c=strrchr(filename,'\\')) {
			c++;
			strcpy(newitem->Title, c);
		} else
			strcpy(newitem->Title, filename);

		HANDLE fd=CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
		if(fd!=INVALID_HANDLE_VALUE) {
			newitem->DBlocks=GetFileSize(fd, NULL);
			CloseHandle(fd);
		}

		AddNode(newitem);

		return newitem;
	}
	return NULL;
}


PMenuNode MenuList::AddURL (char *url, DWORD size, int type) {
	PMenuNode newitem = new MenuNode;
	if(newitem!=NULL) {
		if(char *c=strrchr(url,'/')) {
			c++;
			if(type)
				sprintf(newitem->Title, "[%s]", c);
			else
				strcpy(newitem->Title, c);
		} else {
			if(type)
				sprintf(newitem->Title, "[%s]", url);
			else
				strcpy(newitem->Title, url);
		}
		strcpy(newitem->File, url);
		newitem->DBlocks=size;
		newitem->Action=type;

		AddNode(newitem);

		return newitem;
	}
	return NULL;
}

void MenuList::AddFiles(const char *where, const char *filter, int norecursion,int type) {
	int u,w,z=0;
	WIN32_FIND_DATA fd;
	HANDLE hFD;

	char *tmpc = new char[MAX_PATH];
	char *tmpb = new char[MAX_PATH];
	char **paths = new char*[128];
	for(z=0; z<128; z++) paths[z] = new char[MAX_PATH];

	if(where==NULL) {
		return;
	}

	if(!norecursion) {
		strcpy(tmpb, where);
		sprintf(tmpc, "%s\\*", where);
		if((hFD=FindFirstFile(tmpc, &fd))!=INVALID_HANDLE_VALUE) {
			for(z=0;z<128;) {
				if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					sprintf(paths[z], "%s\\%s", tmpb, fd.cFileName);
					z++;
				}
	
				if(!FindNextFile(hFD, &fd)) {
					FindClose(hFD);
					break;
				}
			}
		} else {
			DWORD Err= GetLastError();
			return;
		}
	} else {
		strcpy(paths[0], where);
		z=1;
	}

	w=0;
	if(z>0) for(u=0; u<z; u++) {
		if(w==0) {
			if(filter!=NULL)
				sprintf(tmpc, "%s\\%s", paths[u], filter);
			else
				sprintf(tmpc, "%s\\default.xbe", paths[u]);
			
			if((hFD=FindFirstFile(tmpc, &fd))!=INVALID_HANDLE_VALUE)
				w=1;
		}
		while(w==1) {
			sprintf(tmpb, "%s\\%s", paths[u], fd.cFileName);
			
			if (!(type)) {
				if(!filter || !strcmp(filter, "*.xbe")) {
					AddXBE(tmpb);
				} else {
					AddFile(tmpb);
				}
			} 	else  {
				OutputDebugString ("AddSave\n");
				AddSave(tmpb);
				OutputDebugString ("Returned\n");
			} 
			if(!FindNextFile(hFD, &fd))
				w=2;
		}

		if(w==2) {
			FindClose(hFD);
			w=0;
		}
	}
	OutputDebugString ("Finished AddFiles\n");
}

int MenuList::NodeCount() {
	int i=0;
	PMenuNode MenuCurr = MenuHead;
	while(MenuCurr!=NULL) {
		MenuCurr=MenuCurr->Next;
		i++;
	}
	return i;
}

int MenuList::IndexList() {
	int i = NodeCount();
	if (IndexedMenu!=NULL) delete IndexedMenu;
	IndexedMenu = new PMenuNode[i];
	PMenuNode MenuCurr = MenuHead;
	for(i=0, MenuCurr=MenuHead; MenuCurr!=NULL; i++, MenuCurr=MenuCurr->Next)
		IndexedMenu[i]=MenuCurr;
	return i;
}

MenuList::MenuList() {
	strcpy(MenuName, "Untitled Menu");
	MenuHead=NULL;
	MenuTail=NULL;
	IndexedMenu=NULL;
}

MenuList::~MenuList() {
	for(int i=NodeCount(); i>0; i--)
		delete IndexedMenu[i];
	delete IndexedMenu;
}
