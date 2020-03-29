#include <xtl.h>

#include "xLCD.h"
#include "..\\main.h"


char X3LcdAnimIndex=0;

//*************************************************************************************************************
static void outb(unsigned short port, unsigned char data)
{
	__asm
	{
		nop
			mov dx, port
			nop
			mov al, data
			nop
			out dx,al
			nop
			nop
	}
}



//*************************************************************************************************************
static unsigned char inb(unsigned short port)
{
	unsigned char data;
	__asm
	{
		mov dx, port
			in al, dx
			mov data,al
	}
	return data;
}

void xLCDSysInfo::SetBackLight( int iBLight )
{
	m_iBackLight = iBLight;
}

//************************************************************************************************************************
//Set brightness level 
//************************************************************************************************************************
void xLCDSysInfo::DisplaySetBacklight(unsigned char level) 
{
	outb(DISP_O_LIGHT, level);
}

bool xLCDSysInfo::Init()
{
	int iOriginalLight=-1;

	if (myDash.x3Settings.enabled && myDash.x3Settings.xLCD.enabled) {
		myxLCD.SetRenderTextStatus(false);
		bRunning = true;
		while (bRunning) {
			X3LcdAnimIndex=7;

			// Backlight idea from XBMC
			if (m_iBackLight != iOriginalLight)
			{
				// backlight setting changed
				iOriginalLight=m_iBackLight;
				DisplaySetBacklight(m_iBackLight);
			}

			myxLCD.DisplayProgressBarChars();
			switch (pos) {
				case 1:
					myxLCD.RenderDriveStatus(2);
					break;
				case 2:
					myxLCD.RenderDVDDriveStatus();
					break;
				case 3:
					myxLCD.RenderDriveStatus(1);
					break;
				case 4:
					myxLCD.RenderDriveStatus(6);
					break;
				case 5:
					myxLCD.RenderDriveStatus(7);
					break;
				case 6:
					myxLCD.RenderDriveStatus(3);
					break;
				case 7:
					myxLCD.RenderDriveStatus(4);
					break;
				case 8:
					myxLCD.RenderDriveStatus(5);
					break;
			}
			if (pos == 8 ) 
				pos =1;
			else
				pos++;
			Sleep(1000);
		}
		myxLCD.SetRenderTextStatus(true);
		return true;
	}
	return false;
}

void xLCD::SetBackLight( int iBLight )
{
	m_iBackLight = iBLight;
}


//************************************************************************************************************************
// Init: setup the dispay
// Input: ()
//************************************************************************************************************************
bool xLCD::Init()
{
	int iOriginalLight=-1;

	if (myDash.x3Settings.enabled && myDash.x3Settings.xLCD.enabled) {
		DisplayInit();
		bDeviceRunning = true;
		char tmpLine[21];
		for (int i=0; i < 4 ; i++)
			DisplayClearChars(0,i,20);

		while ( bDeviceRunning )
		{
			if (bRenderText) {
				// Lets slow things down a bit to stop it running away
				Sleep(SCROLL_SPEED_IN_MSEC);

				// Backlight idea from XBMC
				if (m_iBackLight != iOriginalLight)
				{
					// backlight setting changed
					iOriginalLight=m_iBackLight;
					DisplaySetBacklight(m_iBackLight);
				}

				while(UpdateInProgress || UpdateLock)
					Sleep(10);		// Dont do anything while were updating whats on the LCD
				// Stop any further updates from happening while were displaying the current LCD
				UpdateLock = true;
				DisplayBuildCustomChars();
				for (int iLine = 0; iLine < (int)m_iRows; ++iLine)
				{
					if (RowData[iLine].Updated) {
						DisplaySetPos(0,iLine);
						if (strlen(RowData[iLine].text) > 20) {
							strncpy(tmpLine,RowData[iLine].text,20);
							tmpLine[20]='\0';
							DisplayClearChars(0,iLine,20);
							DisplayWriteString(tmpLine);
							RowData[iLine].ScrollForwards = true;
							RowData[iLine].StartPos = 0;
						} else {
							DisplayClearChars(0,iLine,20);
							DisplayWriteString(RowData[iLine].text);
						}
							
						RowData[iLine].Updated = false;
					}
					if (strlen(RowData[iLine].text) > (unsigned int) m_iCols && !RowData[iLine].Updated ) {
						DisplaySetPos(0,iLine);
						if (strlen(RowData[iLine].text) > 20) {
							// text is longer than the display
							DisplaySetPos(0,iLine);
							if (RowData[iLine].ScrollForwards && RowData[iLine].StartPos + 21 > (int) strlen(RowData[iLine].text) ) {
								RowData[iLine].ScrollForwards = false;
							} else if (!RowData[iLine].ScrollForwards && RowData[iLine].StartPos == 0 ) {
								RowData[iLine].ScrollForwards = true;
							}
							DisplayClearChars(0,iLine,20);
							if (RowData[iLine].ScrollForwards) {
								strncpy(tmpLine,RowData[iLine].text + RowData[iLine].StartPos,20);
								RowData[iLine].StartPos++;
							} else {
								strncpy(tmpLine,RowData[iLine].text + RowData[iLine].StartPos,20);
								RowData[iLine].StartPos--;
							}
							tmpLine[20]='\0';
							DisplayWriteString(tmpLine);
						} 
					}
				}
				UpdateLock = false;
			} else
				Sleep(SCROLL_SPEED_IN_MSEC);
		}

		return true;
	} else {
		return false;
	}
}

void xLCD::UpdateLine(int line, char *pointer)
{
	// Does the line match what is there at the moment?

	if (strcmp(pointer,myxLCD.RowData[line - 1].text) != 0) {
		// Only update if something has changed :-)
		while (UpdateLock || UpdateInProgress)
			Sleep(10);
		// Stop the screen
		UpdateInProgress = true;
		myxLCD.RowData[line -1].Updated = true;
		strcpy(myxLCD.RowData[line -1].text,pointer);
		UpdateInProgress = false;
	}
}
void xLCD::UpdateDisplayText(char *pointer)
{
	char szLCDText[255];
	strcpy(szLCDText,pointer);
	// Only update if something has changed :-)
	while (UpdateLock || UpdateInProgress)
		Sleep(10);

	UpdateInProgress = true;
	if (strcmp(szDisplayText,szLCDText) == 0)
		return;
	OutputDebugString ("Yup, clearing display\n");

    strcpy(szDisplayText,szLCDText);
	int RowId = 0;
	char *token;
	token = strtok( szLCDText, "\n");
	while( token != NULL && RowId < 4)
	{
		// Erase everything and start again
		strcpy(RowData[RowId].text,token);
		RowData[RowId].ScrollForwards = true;
		RowData[RowId].StartPos = 0;
		RowData[RowId].Updated = true;
		RowId++;
		token = strtok( NULL, "\n");
	}
	while (RowId < 4 ) {
		strcpy(RowData[RowId].text,"");
		RowData[RowId].Updated = true;
		RowData[RowId].StartPos = 0;
		RowData[RowId].ScrollForwards = true;
		RowId++;
	}
	UpdateInProgress = false;
}
//************************************************************************************************************************
// wait_us: delay routine
// Input: (wait in ~us)
//************************************************************************************************************************
void xLCD::wait_us(unsigned int value) 
{
	// 1 us = 1000msec
	int iValue = value / 30;
	if (iValue>10) 
		iValue = 10;
	if (iValue) 
		Sleep(iValue);
}

void xLCD::DisplayInit()
{
	//initialize GP/IO
	outb(DISP_O_DAT, 0);
	outb(DISP_O_CMD, 0);
	outb(DISP_O_DIR_DAT, 0xFF);
	outb(DISP_O_DIR_CMD, 0x07);

	DisplayOut(DISP_FUNCTION_SET | DISP_DL_FLAG, INI);	// 8-Bit Datalength if display is already initialized to 4 bit
	Sleep(5);
	DisplayOut(DISP_FUNCTION_SET | DISP_DL_FLAG, INI);	// 8-Bit Datalength if display is already initialized to 4 bit
	Sleep(5);
	DisplayOut(DISP_FUNCTION_SET | DISP_DL_FLAG, INI);	// 8-Bit Datalength if display is already initialized to 4 bit
	Sleep(5);
	DisplayOut(DISP_FUNCTION_SET, INI);													// 4-Bit Datalength
	Sleep(5);
	DisplayOut(DISP_FUNCTION_SET | DISP_N_FLAG ,CMD);	// 4-Bit Datalength, 2 Lines, Font 5x7, clear RE Flag
	Sleep(5);
	DisplayOut(DISP_CONTROL | DISP_D_FLAG ,CMD);		// Display on
	Sleep(5);
	DisplayOut(DISP_CLEAR, CMD);				// Display clear
	Sleep(5);
	DisplayOut(DISP_ENTRY_MODE_SET | DISP_ID_FLAG,CMD);	// Cursor autoincrement
	Sleep(5);
	
	DisplayBuildCustomChars();				
	DisplaySetPos(0,0);
}

//************************************************************************************************************************
//Set brightness level 
//************************************************************************************************************************
void xLCD::DisplaySetBacklight(unsigned char level) 
{
	outb(DISP_O_LIGHT, level);
}

void xLCD::RenderDVDDriveStatus()
{
	LPCSTR szDirectory;
	ULARGE_INTEGER u64Free, u64FreeTotal, u64Total;

	// Make sure nothing else is writing to the screen
	while (UpdateLock || UpdateInProgress) 
		Sleep(10);
	
	UpdateLock = true;

	if (bRenderText)
		SetRenderTextStatus(false);
	char displaytxt[21]="";
	char blank[21]="                    ";
	DisplaySetPos(0,0);
	XBOXDriveStatus.GetDriveState();
	if ( XBOXDriveStatus.m_bTrayOpen ) {
		strcat(displaytxt,"DVD: OPEN");
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		for (int i=1; i < 4 ; i++) {
			DisplaySetPos(0,i);
			DisplayWriteString(blank);
		}
	} else if( XBOXDriveStatus.m_iDVDType == XBI_DVD_INIT ) {
		strcat(displaytxt,"DVD: INIT");
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		for (int i=1; i < 4 ; i++) {
			DisplaySetPos(0,i);
			DisplayWriteString(blank);
		}
	} else if( XBOXDriveStatus.m_iDVDType == XBI_DVD_EMPTY ) {
		strcat(displaytxt,"DVD: EMPTY");
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," "); 
		DisplayWriteString(displaytxt);
		for (int i=1; i < 4 ; i++) {
			DisplaySetPos(0,i);
			DisplayWriteString(blank);
		}
	} else if( XBOXDriveStatus.m_iDVDType == XBI_DVD_XBOX ) { 
		strcat(displaytxt,"DVD: GAME");
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		DisplaySetPos(0,1);
		strcpy(displaytxt,"Name: ");

		strcat(displaytxt,xbeReader.GetXboxTitleName("D:\\Default.xbe"));

		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);

		DisplaySetPos(0,2);

		strcpy(displaytxt,"Size: ");
		// Display game size
		szDirectory = _T("D:\\");
		if ( GetDiskFreeSpaceEx(szDirectory, &u64Free, &u64Total, &u64FreeTotal) )
		{
			string sSpace = XMakeNiceNumber(u64Total.QuadPart,_T('.'));
			strcat(displaytxt,sSpace.c_str());
		} else {
			strcat(displaytxt,"Unknown");
		}
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		for (int i=3; i < 4 ; i++) {
			DisplaySetPos(0,i);
			DisplayWriteString(blank);
		}
	} else if( XBOXDriveStatus.m_iDVDType == XBI_DVD_MOVIE ) {
		strcat(displaytxt,"DVD: MOVIE");
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		for (int i=1; i < 4 ; i++) {
			DisplaySetPos(0,i);
			DisplayWriteString(blank);
		}
	} else {
		strcat(displaytxt,"DVD: UNKNOWN");
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		// NEED TO CLEAR THE OTHER THREE LINES BUW THAT CAN WAIT
	}
	UpdateLock = false;

}

void xLCD::RenderDriveStatus(int iPartition)
{
	LPCSTR szDirectory;
	ULARGE_INTEGER u64Free, u64FreeTotal, u64Total;
	
	// Make sure nothing else is writing to the screen
	while (UpdateLock || UpdateInProgress) 
		Sleep(10);
	
	UpdateLock = true;

	if (bRenderText)
		SetRenderTextStatus(false);

	switch (iPartition) {
		case 1:
			szDirectory = _T("E:\\");
			break;
		case 2:
			szDirectory = _T("C:\\");
			break;
		case 3:
			szDirectory = _T("X:\\");
			break;
		case 4:
			szDirectory = _T("Y:\\");
			break;
		case 5:
			szDirectory = _T("Z:\\");
			break;
		case 6:
			szDirectory = _T("F:\\");
			break;
		case 7:
			szDirectory = _T("G:\\");
			break;
		default:
			UpdateLock = false;
			SetRenderTextStatus(true);
			break;
	}

	char displaytxt[21]="";
	char blank[21]="                    ";
	if ( GetDiskFreeSpaceEx(szDirectory, &u64Free, &u64Total, &u64FreeTotal) )
	{
		unsigned __int64 free	= u64FreeTotal.QuadPart;
		unsigned __int64 total	= u64Total.QuadPart;
		double percentage = (double)free/(double)total;
		percentage *= 100.0;
		if (percentage > 100.0f)
			percentage = 100.0f;
		DisplaySetPos(0,0);
		
		char szpcnt[5] = "";
		
		strcpy(displaytxt,szDirectory);
		strcat(displaytxt," ");
		itoa(100 - (int) percentage,szpcnt,10);
		strcat(displaytxt,szpcnt);
		strcat(displaytxt,"%");
		while (strlen(displaytxt) < 10)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		DisplayProgressBar(100 - (int) percentage ,10);
		DisplaySetPos(0,1);
		strcpy(displaytxt,"Total : ");
		string sSpace = XMakeNiceNumber(u64Total.QuadPart,_T('.'));
		strcat(displaytxt,sSpace.c_str());
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		DisplaySetPos(0,2);
		strcpy(displaytxt,"Used  : ");
		ULONGLONG used = u64Total.QuadPart - u64FreeTotal.QuadPart;
		sSpace = XMakeNiceNumber(used, _T('.'));
		strcat(displaytxt,sSpace.c_str());
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		DisplaySetPos(0,3);
		strcpy(displaytxt,"Free  : ");
		sSpace = XMakeNiceNumber(u64FreeTotal.QuadPart,_T('.'));
		strcat(displaytxt,sSpace.c_str());
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
	} else {
		DisplaySetPos(0,0);
		strcpy(displaytxt,szDirectory);
		strcat(displaytxt," ");
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		DisplaySetPos(0,1);
		strcpy(displaytxt,"Not Used");
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		DisplayWriteString(displaytxt);
		while (strlen(displaytxt) < 20)
			strcat(displaytxt," ");
		for (int i  = 2; i < 4 ; i++) {
			DisplaySetPos(0,i);
			DisplayWriteString(blank);
		}
	}
	UpdateLock = false;
	return;
}

void xLCD::SetRenderTextStatus(bool bnewStatus)
{
	// Lets clear the screen now matter what
	for (int i = 0 ; i < 4 ; i++) {
        DisplayClearChars(0,i,20);
		if (bnewStatus == true )
			RowData[i].Updated = true;
	}

	bRenderText = bnewStatus;
	return;
}
//************************************************************************************************************************
// DisplaySetPos: sets cursor position
// Input: (row position, line number from 0 to 3)
//************************************************************************************************************************
void xLCD::DisplaySetPos(unsigned char pos, unsigned char line) 
{

	unsigned int cursorpointer;

	cursorpointer = pos % m_iCols;

	if (line == 0) {
		cursorpointer += m_iRow1Addr;
	}
	if (line == 1) {
		cursorpointer += m_iRow2Addr;
	}
	if (line == 2) {
		cursorpointer += m_iRow3Addr;
	}
	if (line == 3) {
		cursorpointer += m_iRow4Addr;
	}
	DisplayOut(DISP_DDRAM_SET | cursorpointer, CMD);
	m_iActualpos = cursorpointer;
}


xLCD::xLCD() {
	m_iRow1Addr = 0x00;
	m_iRow2Addr = 0x40;
	m_iRow3Addr = 0x14;
	m_iRow4Addr = 0x54;
	m_iRows = 4;
	m_iCols = 20;
	m_iActualpos = 0;
	m_iBackLight = 32;
	szDisplayText = (char *) malloc (255); // Max LCD buffer siez of 255
	ZeroMemory(szDisplayText,255);
	DisplayInit();
	bneXgenSplash = true;
	bRenderText = true;
}


//************************************************************************************************************************
// DisplayWriteFixText: write a fixed text to actual cursor position
// Input: ("fixed text like")
//************************************************************************************************************************
void xLCD::DisplayWriteFixtext(const char *textstring)
{
	unsigned char  c;
	while (c = *textstring++) {
		DisplayOut(c,DAT);
	}
}


//************************************************************************************************************************
// DisplayWriteString: write a string to acutal cursor position 
// Input: (pointer to a 0x00 terminated string)
//************************************************************************************************************************
void xLCD::DisplayWriteString(char *pointer) 
{
	/* display a normal 0x00 terminated string on the LCD display */
	unsigned char c;
	do {
		c = *pointer;
		if (c == 0x00) 
			break;

		DisplayOut(c, DAT);
		*pointer++;
	} while(1);

}		



//************************************************************************************************************************
// DisplayClearChars:  clears a number of chars in a line and resets cursor position to it's startposition
// Input: (Startposition of clear in row, row number, number of chars to clear)
//************************************************************************************************************************
void xLCD::DisplayClearChars(unsigned char startpos , unsigned char line, unsigned char lenght) 
{
	int i;

	DisplaySetPos(startpos,line);
	for (i=0;i<lenght; i++){
		DisplayOut(0x20,DAT);
	}
	DisplaySetPos(startpos,line);

}



//************************************************************************************************************************
// DisplayProgressBar: shows a grafic bar staring at actual cursor position
// Input: (percent of bar to display, lenght of whole bar in chars when 100 %)
//************************************************************************************************************************
void xLCD::DisplayProgressBar(unsigned char percent, unsigned char charcnt) 
{

	unsigned int barcnt100;

	barcnt100 = charcnt * 5 * percent / 100;

	int i;	
	int a;
	for (i=1; i<=charcnt; i++) {
		if (i<=(int)(barcnt100 / 5)){
			DisplayOut(4,DAT);
		}
		else 
		{
			if ( (i == (barcnt100 /5)+1) && (barcnt100 % 5 != 0) ){
				a =  (barcnt100 % 5)-1;
				DisplayOut(a,DAT);
			}
			else{
				DisplayOut(0x20,DAT);
			}
		}
	}
}



//************************************************************************************************************************
//DisplayBuildCustomChars: load customized characters to character ram of display, resets cursor to pos 0
//************************************************************************************************************************
void xLCD::DisplayBuildCustomChars() 
{
	int I;

	static char Bar0[] ={0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
	static char Bar1[] ={0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
	static char Bar2[] ={0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c};
	static char REW[8][8]=
	{
		{0x00, 0x05, 0x0a, 0x14, 0x0a, 0x05, 0x00, 0x00},
		{0x00, 0x0a, 0x14, 0x08, 0x14, 0x0a, 0x00, 0x00},
		{0x00, 0x14, 0x08, 0x10, 0x08, 0x14, 0x00, 0x00},
		{0x00, 0x08, 0x10, 0x00, 0x10, 0x08, 0x00, 0x00},
		{0x00, 0x10, 0x00, 0x01, 0x00, 0x10, 0x00, 0x00},
		{0x00, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00},
		{0x00, 0x01, 0x02, 0x05, 0x02, 0x01, 0x00, 0x00},
		{0x00, 0x02, 0x05, 0x0a, 0x05, 0x02, 0x00, 0x00}

	};
	static char FF[8][8]=
	{
		{0x00, 0x14, 0x0a, 0x05, 0x0a, 0x14, 0x00, 0x00},
		{0x00, 0x0a, 0x05, 0x02, 0x05, 0x0a, 0x00, 0x00},
		{0x00, 0x05, 0x02, 0x01, 0x02, 0x05, 0x00, 0x00},
		{0x00, 0x02, 0x01, 0x00, 0x01, 0x02, 0x00, 0x00},
		{0x00, 0x01, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00},
		{0x00, 0x00, 0x10, 0x08, 0x10, 0x00, 0x00, 0x00},
		{0x00, 0x10, 0x08, 0x14, 0x08, 0x10, 0x00, 0x00},
		{0x00, 0x08, 0x14, 0x0a, 0x14, 0x08, 0x00, 0x00}
	};
	static char Play[] ={0x00, 0x04, 0x06, 0x07, 0x07, 0x06, 0x04, 0x00};
	static char Stop[] ={0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x00};
	static char Pause[]={0x00, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x00};
	//static char Backslash[]={0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00};
	//static char Underscore[]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F};

	DisplayOut(DISP_CGRAM_SET, CMD);
	for(I=0;I<8;I++) DisplayOut(Bar0[I], DAT);  			// Bar0
	for(I=0;I<8;I++) DisplayOut(Bar1[I], DAT);  			// Bar1
	for(I=0;I<8;I++) DisplayOut(Bar2[I], DAT);  			// Bar2
	for(I=0;I<8;I++) DisplayOut(REW[X3LcdAnimIndex][I], DAT);   	// REW
	for(I=0;I<8;I++) DisplayOut(FF[X3LcdAnimIndex][I], DAT);    	// FF
	for(I=0;I<8;I++) DisplayOut(Play[I], DAT);  			// Play
	for(I=0;I<8;I++) DisplayOut(Stop[I], DAT);  			// Stop
	for(I=0;I<8;I++) DisplayOut(Pause[I], DAT); 			// Pause

	//for(I=0;I<8;I++) DisplayOut(Underscore[I], DAT);		// Underscore
	//for(I=0;I<8;I++) DisplayOut(Backslash[I], DAT);			// Backslash

	DisplayOut(DISP_DDRAM_SET, CMD);
	X3LcdAnimIndex=(X3LcdAnimIndex+1) & 0x7;
}

void xLCD::DisplayProgressBarChars() 
{
	int I;
	static char Bar0[] ={0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
	static char Bar1[] ={0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
	static char Bar2[] ={0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c};
	static char Bar3[] ={0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e};
	static char Bar4[] ={0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
	DisplayOut(DISP_CGRAM_SET, CMD);
	for(I=0;I<8;I++) DisplayOut(Bar0[I], DAT);  			// Bar0
	for(I=0;I<8;I++) DisplayOut(Bar1[I], DAT);  			// Bar0
	for(I=0;I<8;I++) DisplayOut(Bar2[I], DAT);  			// Bar0
	for(I=0;I<8;I++) DisplayOut(Bar3[I], DAT);  			// Bar0
	for(I=0;I<8;I++) DisplayOut(Bar4[I], DAT);  			// Bar0
	DisplayOut(DISP_DDRAM_SET, CMD);
}




//************************************************************************************************************************
// DisplayOut: writes command or datas to display
// Input: (Value to write, token as CMD = Command / DAT = DATAs / INI for switching to 4 bit mode)
//************************************************************************************************************************
void xLCD::DisplayOut(unsigned char data, unsigned char command) 
{
	unsigned char cmd = 0;
	static DWORD	dwTick=0;

	if ((GetTickCount()-dwTick) < 3)
	{
		Sleep(1);
	}
	dwTick=GetTickCount();

	if (command & DAT)
	{
		cmd |= DISPCON_RS;
	}

	//outbut higher nibble
	
	outb(DISP_O_DAT, data & 0xF0);			// set Data high nibble
	outb(DISP_O_CMD, cmd);					// set RS if needed
	outb(DISP_O_CMD, DISPCON_E | cmd);		// set E
	outb(DISP_O_CMD, cmd);					// reset E

	if ((command & INI) == 0) 
	{							
		// if it's not the init command, do second nibble
		//outbut lower nibble
		outb(DISP_O_DAT, (data << 4) & 0xF0);		// set Data low nibble
		outb(DISP_O_CMD, cmd);				// set RS if needed
		outb(DISP_O_CMD, DISPCON_E | cmd);	// set E
		outb(DISP_O_CMD, cmd);				// reset E
		if ((data & 0xFC) == 0) 
		{						                          
			// if command was a Clear Display
			// or Cursor Home, wait at least 1.5 ms more
			m_iActualpos = 0;
			Sleep(2);							                                   
		}
		if ((command & DAT) == 0x02)
			m_iActualpos++;
	}
	else 
	{										
		m_iActualpos = 0;
		//wait_us(2500);		// wait 2.5 msec	
	}

	wait_us(30);
}

