/*

	XmlLoader.h

*/


#ifndef __XMLLOADER_H__
#define __XMLLOADER_H__



#include <xtl.h>
#include <string>
using namespace std;
#include "TinyXml.h"


void XmlGetString(const TiXmlElement* pRootElement, string sTagName, string &sBuffer);
void XmlGetDword(const TiXmlElement* pRootElement, string sTagName, DWORD &dwBuffer);
void XmlGetBoolean(const TiXmlElement* pRootElement, string sTagName, bool &bBuffer);
void XmlGetInteger(const TiXmlElement* pRootElement, string sTagName, int &iBuffer);
void XmlGetFloat(const TiXmlElement* pRootElement, string sTagName, float &fBuffer);

bool  XmlCmpNoCase(const char* str1, const char* str2);
DWORD XmlStringToDWORD(string sValue);

void XmlGetAttribString(const TiXmlElement* pRootElement, string sTagName, string sChild, string &sBuffer);
void XmlGetAttribDword(const TiXmlElement* pRootElement, string sTagName, string sChild, DWORD &dwBuffer);
void XmlGetAttribBoolean(const TiXmlElement* pRootElement, string sTagName, string sChild, bool& bValue);
void XmlGetAttribInteger(const TiXmlElement* pRootElement, string sTagName, string sChild, int& iValue);
void XmlGetAttribFloat(const TiXmlElement* pRootElement, string sTagName, string sChild, float &fValue);

void XmlWrite(const TiXmlElement* pRootElement, string sTagName, string sValue);

// Write XML
void XmlSetString(TiXmlNode* pRootNode, string sTagName, string &sBuffer);
//void XmlSetDword(TiXmlNode* pRootNode, string sTagName, DWORD &dwBuffer); // ToDO
void XmlSetBoolean(TiXmlNode* pRootNode, string sTagName, bool& bBuffer);
void XmlSetInteger(TiXmlNode* pRootNode, string sTagName, int& iBuffer);
void XmlSetFloat(TiXmlNode* pRootNode, string sTagName, float &fBuffer);

#endif