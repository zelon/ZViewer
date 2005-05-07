/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZImage.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "ZImage.h"

bool ZImage::IsValidImageFileExt(const char * szFilename)
{
	char szExt[MAX_PATH];

	_splitpath(szFilename, NULL, NULL, NULL, szExt);

	if ( strlen(szExt) < 2 ) return false;

	char szLowerExt[MAX_PATH];

	strncpy(szLowerExt, szExt, min(strlen(szExt)+1, MAX_PATH));

	strlwr(szLowerExt);

	std::string strExt = szLowerExt + 1;

	if ( strcmp(strExt.c_str(), "bmp") == 0 ||
		0 == strcmp(strExt.c_str(), "jpg") ||
		0 == strcmp(strExt.c_str(), "jpeg") ||
		0 == strcmp(strExt.c_str(), "gif") ||
		0 == strcmp(strExt.c_str(), "ico") ||
		0 == strcmp(strExt.c_str(), "pcx") ||
		0 == strcmp(strExt.c_str(), "psd") ||
		0 == strcmp(strExt.c_str(), "tif") ||
		0 == strcmp(strExt.c_str(), "tga") ||
		0 == strcmp(strExt.c_str(), "dds") ||
		0 == strcmp(strExt.c_str(), "png") 
		)
	{
		return true;
	}
	return false;
}
