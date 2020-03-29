#pragma once

#include <xtl.h>
#include "xbeheader.h"

typedef struct MenuNode {
	char Title[256];
	char File[256];
	char Icon[256];
	char Cmdline[256];
	DWORD Action;
	DWORD TitleID;
	DWORD DBlocks;
	DWORD Region;
	DWORD Rating;

	MenuNode *Prev;
	MenuNode *Next;

	MenuNode() {
		ZeroMemory(Title,256);
		ZeroMemory(File,256);
		ZeroMemory(Icon,256);
		ZeroMemory(Cmdline,256);
		Next=NULL;
		Prev=NULL;
		//strcpy(Title,"Unknown Item");
		File[0]=0;
		strcpy(Icon,"default");
		Cmdline[0]=0;
		Action=0;
		TitleID=0;
		DBlocks=0;
		Region=0;
		Rating=0;
	}
	~MenuNode() {
		if(Next!=NULL) Next->Prev = Prev;
		if(Prev!=NULL) Prev->Next = Next;
	}
} MenuNode;
typedef MenuNode* PMenuNode;

class MenuList {
	char MenuName[256];
public:
	PMenuNode MenuHead;
	PMenuNode MenuTail; 
	PMenuNode *IndexedMenu;

	MenuList();
	~MenuList();

	void		AddNode(PMenuNode newitem);
	PMenuNode	FindNode(const char *name);
	PMenuNode	AddFile(char *filename);
	PMenuNode	AddURL(char *url, DWORD size, int type);
	PMenuNode	AddXBE(char *filename);
	PMenuNode	AddSave(char *filename);
	PMenuNode	AddFromCache(char *Title, char *File, char *Icon, char *Cmdline, DWORD Action, DWORD TitleID, DWORD Region, DWORD Rating,DWORD DBlocks);
	void		Clear();
	char*		GetName();
	void		Rename(char* str);
	void		AddFiles(const char *where, const char *filter, int norecursion, int type);
	int			NodeCount();
	int			IndexList();
	
};

int FindMenu(const char *name);
