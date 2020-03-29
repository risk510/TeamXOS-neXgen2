#include "XmlLoader.h"

void XmlGetString(const TiXmlElement* pRootElement, string sTagName, string &sBuffer)
{
	sBuffer = "";
	const TiXmlNode *pChild = pRootElement->FirstChild(sTagName.c_str());

	if (pChild)
	{
		string strValue = pChild->FirstChild()->Value();

		if (strValue.size() )
		{
			if (strValue != "-" )
			{
				sBuffer = strValue;
			}
		}
	}

	if (strlen(sBuffer.c_str()) == 0 )
	{
		sBuffer = "";
	}
}

void XmlGetDword(const TiXmlElement* pRootElement, string sTagName, DWORD &dwBuffer)
{
	const TiXmlNode *pChild = pRootElement->FirstChild(sTagName.c_str());

	if (pChild)
	{
		string strValue = pChild->FirstChild()->Value();

		if ( strValue.size() )
		{
			if (strValue != "-" )
			{
				dwBuffer = XmlStringToDWORD(strValue);
				return;
			}
		}
	}

	dwBuffer = 0;
}

void XmlGetBoolean(const TiXmlElement* pRootElement, string sTagName, bool& bValue)
{
	string szString;

	XmlGetString(pRootElement, sTagName, szString);

	if ( XmlCmpNoCase(szString.c_str(), "enabled" ) || XmlCmpNoCase(szString.c_str(), "yes" ) ||
			 XmlCmpNoCase(szString.c_str(), "on" ) || XmlCmpNoCase(szString.c_str(), "true" ) )
	{
		bValue = true;
		return;
	}

	bValue = false;
}

void XmlGetInteger(const TiXmlElement* pRootElement, string sTagName, int& iValue)
{
	const TiXmlNode *pChild = pRootElement->FirstChild(sTagName.c_str());

	if (pChild)
	{
		iValue = atoi( pChild->FirstChild()->Value() );
	}
}

void XmlGetFloat(const TiXmlElement* pRootElement, string sTagName, float& fValue)
{
	const TiXmlNode *pChild = pRootElement->FirstChild(sTagName.c_str());

	if (pChild)
	{
		fValue = (float)atof( pChild->FirstChild()->Value() );
	}
}

bool XmlCmpNoCase(const char* str1,const char* str2)
{
	int iLen;
	if ( strlen(str1) != strlen(str2) ) return false;
	
	iLen=strlen(str1);
	for (int i=0; i < iLen;i++ )
	{
		if (tolower((unsigned char)str1[i]) != tolower((unsigned char)str2[i]) ) return false;
	}
	return true;
}

DWORD XmlStringToDWORD( string sValue )
{
	DWORD dwReturn = 0;

	if( sValue.length() == 10 )
	{
		if ( ( sValue[0] == '0' ) && ((sValue[1] == 'x') || (sValue[1] == 'X')) )
		{
				int iBase = 16;

				dwReturn = strtoul( sValue.c_str(), NULL, iBase );
		}
	}

	return dwReturn;
}


void XmlGetAttribString(const TiXmlElement* pRootElement, string sTagName, string sChild, string &sBuffer)
{
	TiXmlElement* pTag = pRootElement->FirstChildElement(sTagName.c_str());

	if ( pTag )
	{
		string sValue = pTag->Attribute(sChild.c_str());
		
		if( strlen(sValue.c_str()) > 0 )
		{
			sBuffer = sValue;
			return;
		}
	}

	sBuffer = "";
}

void XmlGetAttribDword(const TiXmlElement* pRootElement, string sTagName, string sChild, DWORD &dwBuffer)
{
	TiXmlElement* pTag = pRootElement->FirstChildElement(sTagName.c_str());

	if ( pTag )
	{
		string sValue = pTag->Attribute(sChild.c_str());
		
		if( strlen(sValue.c_str()) > 0 )
		{
			dwBuffer = XmlStringToDWORD(sValue);
			return;
		}
	}

	dwBuffer = 0;
}

void XmlGetAttribBoolean(const TiXmlElement* pRootElement, string sTagName, string sChild, bool& bValue)
{
	TiXmlElement* pTag = pRootElement->FirstChildElement(sTagName.c_str());

	if( pTag )
	{
		string sValue = pTag->Attribute(sChild.c_str());

		if ( XmlCmpNoCase(sValue.c_str(), "enabled" ) || XmlCmpNoCase(sValue.c_str(), "yes" ) ||
			 XmlCmpNoCase(sValue.c_str(), "on" ) || XmlCmpNoCase(sValue.c_str(), "true" ) )
		{
			bValue = true;
			return;
		}
	}

	bValue = false;
}

void XmlGetAttribInteger(const TiXmlElement* pRootElement, string sTagName, string sChild, int& iValue)
{
	TiXmlElement* pTag = pRootElement->FirstChildElement(sTagName.c_str());

	if( pTag )
	{
		string sValue = pTag->Attribute(sChild.c_str());

		if( strlen(sValue.c_str()) > 0 )
		{
			iValue = atoi(sValue.c_str());
			return;
		}
	}

	iValue = 0;
}

void XmlGetAttribFloat(const TiXmlElement* pRootElement, string sTagName, string sChild, float &fValue)
{
	TiXmlElement* pTag = pRootElement->FirstChildElement(sTagName.c_str());

	if( pTag )
	{
		string sValue = pTag->Attribute(sChild.c_str());

		if( strlen(sValue.c_str()) > 0 )
		{
			fValue = (float)atof(sValue.c_str());
			return;
		}
	}

	fValue = 0.0f;
}

// Write to our XML File
/*void XmlWrite(const TiXmlElement* pRootElement, string sTagName, string sValue)
{
}*/

void XmlSetString(TiXmlNode* pRootNode, string sTagName, string &sBuffer)
{
	TiXmlElement newElement(sTagName.c_str());
	TiXmlNode *pNewNode = pRootNode->InsertEndChild(newElement);
	if (pNewNode)
	{
		TiXmlText value(sBuffer.c_str());
		pNewNode->InsertEndChild(value);
	}
}

void XmlSetBoolean(TiXmlNode* pRootNode, string sTagName, bool& bBuffer)
{
	string sValue;
	if (bBuffer)
	{
		sValue = "true";
		XmlSetString(pRootNode, sTagName.c_str(), sValue);
	}
	else
	{
		sValue = "false";
		XmlSetString(pRootNode, sTagName.c_str(), sValue);
	}
}

void XmlSetInteger(TiXmlNode* pRootNode, string sTagName, int& iBuffer)
{
	string sValue;
	char temp[255];
	sprintf(temp, "%i", iBuffer);
	sValue = temp;
	XmlSetString(pRootNode, sTagName.c_str(), sValue);
}

void XmlSetFloat(TiXmlNode* pRootNode, string sTagName, float &fBuffer)
{
	string sValue;
	char temp[255];
	sprintf(temp, "%f", fBuffer);
	sValue = temp;
	XmlSetString(pRootNode, sTagName.c_str(), sValue);
}