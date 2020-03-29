// Xbox Unzip Class
/*

	W-I-P Very early dev stage. needs lots of work doing.
	TODO:
		(1) Folder Extraction
		(2) Complete Filter support for extracting user specific file types
		(3) Anything else thats thought of that needs adding

*/

#pragma once
#include <xtl.h>
#include "Main.h"
#include "StdString.h"

class CXBUnzip
{
	private:

		char szExtractedFile[255];

	public:
	
		// Const & Dest
		CXBUnzip();
		~CXBUnzip();

		// Extracted File Name
		string m_szExtractedFilePath;

		// Filter will be in the form of ".xbe"
		bool unZipTo(char *FilePath, char *Filter, char *Destination);
};

extern CXBUnzip myUnZip;