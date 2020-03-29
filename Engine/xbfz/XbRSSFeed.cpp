
#include <vector>
#include <string>
#include "..\tinyxml\tinystr.h"
#include "..\main.h"
using namespace std;

#include "xbhttpdownloader.h"
#include "xbrssfeed.h"

struct RSS_Item_Breakdown
{
	char title[1024];
};

struct RSS_Breakdown
{
	char channel_title[1024];
	vector<RSS_Item_Breakdown> Items;
};

RSS_Breakdown myRss;

static float extent_x=0;
static float extent_y=0;
static string rsstext;
static WCHAR *wcRss;

void get_rss()
{
	char* HOSTURL= "www.xbox-scene.com";
	char*  URLPATH= "/xbox1data/xbox-scene.xml";
	char*  SAVEFILE= "z:\\news.xml";

	char strValue[1024]="";


	bool download_rssfile=HTTPDownloadToFile(HOSTURL,URLPATH,SAVEFILE);
	if (download_rssfile)
	{
		TiXmlDocument document(SAVEFILE);
		bool success = document.LoadFile();
		if(!success)
			return;

		TiXmlElement *root = document.RootElement();
		TiXmlElement *element = root->FirstChildElement() ;
		while(element)
		{
			std::string element_name(element->Value());
			if(element_name == "channel")
			{
				TiXmlElement *rss_element= element->FirstChildElement();
				while (rss_element)
				{
					std::string rss_element_name(rss_element->Value());

					if(rss_element_name == "title")
					{
						TiXmlNode* text = rss_element->FirstChild();
						if (text)
						{
							sprintf(strValue ,"%s",text->Value());
							if(strValue)
							{
								strcpy(myRss.channel_title,strValue);
							}
						}
					}
					if (rss_element_name == "item")
					{
						TiXmlElement *item_element= rss_element->FirstChildElement();
						while(item_element)
						{
							std::string item_element_name(item_element->Value());
							if (item_element_name == "title")
							{
								TiXmlNode* text = item_element->FirstChild();
								if (text)
								{
									sprintf(strValue ,"%s",text->Value());
									if(strValue)
									{
										RSS_Item_Breakdown new_item;	
										strcpy(new_item.title,strValue);
										myRss.Items.push_back(new_item);
									}
								}
							}
							item_element= item_element->NextSiblingElement();
						}
					}
					rss_element=rss_element->NextSiblingElement();
				}
			}
			element = element->NextSiblingElement();
		}
	}
	else
	{
		strcpy(myRss.channel_title,"No RSS connection was made");
	}

	rsstext.assign(myRss.channel_title);
	rsstext.append(" ");
	for (unsigned int i =0 ; i<myRss.Items.size();i++)
	{
		rsstext.append(":: ");
		rsstext.append(myRss.Items[i].title);
		rsstext.append(" ::");
	}
	wcRss=(WCHAR *)malloc(rsstext.size()*sizeof(WCHAR));
	wsprintfW(wcRss, L"%S", rsstext.c_str());
}

void render_rss()
{
	static float cursorx=640;
	static float cursory=448;
	extent_x=myFonts[0].pFont->GetTextWidth(wcRss);
	if (cursorx >= -extent_x)
	{
		cursorx--;
	}
	else
	{
		cursorx=640;
	}

	if (IsEthernetConnected())
	{
		myFonts[0].pFont->Begin();
		myFonts[0].pFont->SetCursorPosition(cursorx, cursory);
		myFonts[0].pFont->DrawText(rsstext,0XFFFFFFFF);
		myFonts[0].pFont->End();
	}
	else
	{
		myFonts[0].pFont->Begin();
		myFonts[0].pFont->SetCursorPosition(cursorx, cursory);
		myFonts[0].pFont->DrawText("RSS Error: No Network Connection.",0XFFFFFFFF);
		myFonts[0].pFont->End();
	}
}


