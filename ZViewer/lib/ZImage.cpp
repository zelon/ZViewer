
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

	if ( strExt == "bmp" ||
		strExt == "jpg" ||
		strExt == "jpeg" ||
		strExt == "gif" ||
		strExt == "ico" ||
		strExt == "pcx" ||
		strExt == "psd" ||
		strExt == "tif" ||
		strExt == "tga" ||
		strExt == "dds" ||
		strExt == "png" 

		)
	{
		return true;
	}


	/*
	if ( PathMatchSpec ( szFilename, ("*.bmp") ) ||
		PathMatchSpec ( szFilename, ("*.jpg") ) ||
		PathMatchSpec ( szFilename, ("*.jpeg") ) ||
		PathMatchSpec ( szFilename, ("*.gif") ) ||
		PathMatchSpec ( szFilename, ("*.ico") ) ||
		PathMatchSpec ( szFilename, ("*.pcx") ) ||
		PathMatchSpec ( szFilename, ("*.psd") ) ||
		PathMatchSpec ( szFilename, ("*.tif") ) ||
		PathMatchSpec ( szFilename, ("*.tga") ) ||
		PathMatchSpec ( szFilename, ("*.png") ) ||
		PathMatchSpec ( szFilename, ("*.jpg") )
		)
	{
		return true;
	}
	*/
	return false;
}
