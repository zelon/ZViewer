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

	const char * szExtOnly = szLowerExt + 1;

	if ( strcmp(szExtOnly, "bmp") == 0 ||
		0 == strcmp(szExtOnly, "jpg") ||
		0 == strcmp(szExtOnly, "jpeg") ||
		0 == strcmp(szExt, "gif") ||
		0 == strcmp(szExtOnly, "ico") ||
		0 == strcmp(szExtOnly, "pcx") ||
		0 == strcmp(szExtOnly, "psd") ||
		0 == strcmp(szExtOnly, "tif") ||
		0 == strcmp(szExtOnly, "tiff") ||
		0 == strcmp(szExtOnly, "tga") ||
		0 == strcmp(szExtOnly, "dds") ||
		0 == strcmp(szExtOnly, "xbm") ||
		0 == strcmp(szExtOnly, "xpm") ||
		0 == strcmp(szExtOnly, "cut") ||
		0 == strcmp(szExtOnly, "hdr") ||
		0 == strcmp(szExtOnly, "jng") ||
		0 == strcmp(szExtOnly, "koa") ||
		0 == strcmp(szExtOnly, "mng") ||
		0 == strcmp(szExtOnly, "pcd") ||
		0 == strcmp(szExtOnly, "ras") ||
		0 == strcmp(szExtOnly, "png") 
		)
	{
		return true;
	}
	return false;
}
