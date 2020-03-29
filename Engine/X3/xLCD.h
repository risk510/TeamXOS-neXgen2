
#ifndef _xLCD_H

#define _xLCD_H

#define MAX_ROWS 20

/*

Following code taked direct from XMBC
	dont you just love open souce

*/

#define SCROLL_SPEED_IN_MSEC 	250
#define DISP_O_DAT				0xF504
#define DISP_O_CMD				0xF505
#define DISP_O_DIR_DAT			0xF506
#define DISP_O_DIR_CMD			0xF507
#define DISP_O_LIGHT			0xF503
#define DISP_CTR_TIME		    2		    // Controll Timing for Display routine
#define DISPCON_RS				0x01
#define DISPCON_RW				0x02
#define DISPCON_E				0x04

#define INI						0x01
#define CMD						0x00
#define DAT						0x02

#define DISP_CLEAR				0x01			// cmd: clear display command
#define DISP_HOME				0x02			// cmd: return cursor to home
#define DISP_ENTRY_MODE_SET		0x04			// cmd: enable cursor moving direction and enable the shift of entire display
#define DISP_S_FLAG				0x01
#define DISP_ID_FLAG			0x02

#define DISP_CONTROL			0x08			// cmd: display ON/OFF
#define DISP_D_FLAG				0x04			// display on
#define DISP_C_FLAG				0x02			// cursor on
#define DISP_B_FLAG				0x01			// blinking on

#define DISP_SHIFT				0x10			// cmd: set cursor moving and display shift control bit, and the direction without changing ddram data
#define DISP_SC_FLAG			0x08
#define DISP_RL_FLAG			0x04

#define DISP_FUNCTION_SET		0x20			// cmd: set interface data length
#define DISP_DL_FLAG			0x10			// set interface data length: 8bit/4bit
#define DISP_N_FLAG				0x08			// number of display lines:2-line / 1-line
#define DISP_F_FLAG				0x04			// display font type 5x11 dots or 5x8 dots

#define DISP_RE_FLAG			0x04

#define DISP_CGRAM_SET			0x40			// cmd: set CGRAM address in address counter
#define DISP_SEGRAM_SET			0x40			// RE_FLAG = 1

#define DISP_DDRAM_SET			0x80			// cmd: set DDRAM address in address counter

/* END of XBMC Code */

typedef struct xLCDRow {
	char text[255];
	int StartPos;
	bool ScrollForwards;
	bool Updated;
	bool Scrolling;
} xLCDRow;

typedef xLCDRow *PxLCDRow;

class xLCDSysInfo
{
public:
	xLCDSysInfo()
	{
		bRunning = false;
		DoDrive[0] = true;
		DoDrive[1] = true;
		pos = 1;
		m_iBackLight = 32;
	};
	bool Init();
//	bool Start();
//	bool End();
//	bool Pause();
	void Kill() { bRunning = false;};
	bool GetStatus() {return bRunning;};
	void SetBackLight(int iBLight);
	
private:

	void DisplaySetBacklight(unsigned char level) ;
	bool bRunning;
	bool DoDrive[7];
	bool DoCPU;
	bool DoGPU;
	bool DoLAN;
	int pos;
	int m_iBackLight;
};


class xLCD
{
public:
	xLCD();
	bool Init();
	bool bDeviceRunning;
	void UpdateDisplayText(char *pointer);
	void UpdateLine(int line, char *pointer);
	bool GetRenderTextStatus() { return bRenderText; };
	void SetRenderTextStatus (bool NewStatus);
	void RenderDriveStatus(int iPartition);
	void RenderDVDDriveStatus();
	void SetBackLight(int iBLight);
	 // moved to allow other classes to write custom chars to LCD
	void DisplayProgressBarChars();

private:

	
	void DisplayOut(unsigned char data, unsigned char command);
	void wait_us(unsigned int value);
	void DisplaySetPos(unsigned char pos, unsigned char line);
	void DisplaySetBacklight(unsigned char level) ;
	void DisplayInit();
	
	void DisplayWriteFixtext(const char *textstring);
	void DisplayProgressBar(unsigned char percent, unsigned char charcnt);
	void DisplayClearChars(unsigned char startpos , unsigned char line, unsigned char lenght);
	void DisplayWriteString(char *pointer);
	void DisplayBuildCustomChars();

	char* szDisplayText;
	bool bDisplayUpdateRequired;
	bool bneXgenSplash;
	int m_iBackLight;
	int m_iRows;
	int m_iCols;
	unsigned int m_iRow1Addr;
	unsigned int m_iRow2Addr;
	unsigned int m_iRow3Addr;
	unsigned int m_iRow4Addr;
	unsigned int m_iActualpos;			// actual cursor possition
	bool         m_bUpdate[MAX_ROWS];
	char		DisplayText[255];		// Max amount of text we can display 
	xLCDRow		RowData[4];
	bool		UpdateLock;				// Were outputting something to the LCD
	bool		UpdateInProgress;		// Were changing whats on the LCD
	bool		bRenderText;
};




#endif