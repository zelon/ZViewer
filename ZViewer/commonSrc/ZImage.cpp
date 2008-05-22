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

bool ZImage::IsValidImageFileExt(const TCHAR * szFilename)
{
	TCHAR szExt[MAX_PATH];

	SplitPath(szFilename, NULL,0, NULL,0 , NULL,0 , szExt,MAX_PATH);

	if ( _tcslen(szExt) < 2 ) return false;

	TCHAR szLowerExt[MAX_PATH];

	_tcsncpy_s(szLowerExt, szExt, min(_tcslen(szExt)+1, MAX_PATH));

	_tcslwr_s(szLowerExt);

	const TCHAR * szExtOnly = szLowerExt + 1;

	if ( _tcscmp(szExtOnly, TEXT("bmp")) == 0 ||
		0 == _tcscmp(szExtOnly, TEXT("wbmp")) ||
		0 == _tcscmp(szExtOnly, TEXT("jpg")) ||
		0 == _tcscmp(szExtOnly, TEXT("jpeg")) ||
		0 == _tcscmp(szExtOnly, TEXT("jp2")) ||
		0 == _tcscmp(szExtOnly, TEXT("j2k")) ||
		0 == _tcscmp(szExtOnly, TEXT("gif")) ||
		0 == _tcscmp(szExtOnly, TEXT("ico")) ||
		0 == _tcscmp(szExtOnly, TEXT("pcx")) ||
		0 == _tcscmp(szExtOnly, TEXT("psd")) ||
		0 == _tcscmp(szExtOnly, TEXT("tif")) ||
		0 == _tcscmp(szExtOnly, TEXT("tiff")) ||
		0 == _tcscmp(szExtOnly, TEXT("tga")) ||
		0 == _tcscmp(szExtOnly, TEXT("dds")) ||
		0 == _tcscmp(szExtOnly, TEXT("xbm")) ||
		0 == _tcscmp(szExtOnly, TEXT("xpm")) ||
		0 == _tcscmp(szExtOnly, TEXT("cut")) ||
		0 == _tcscmp(szExtOnly, TEXT("hdr")) ||
		0 == _tcscmp(szExtOnly, TEXT("jng")) ||
		0 == _tcscmp(szExtOnly, TEXT("koa")) ||
		0 == _tcscmp(szExtOnly, TEXT("mng")) ||
		0 == _tcscmp(szExtOnly, TEXT("pcd")) ||
		0 == _tcscmp(szExtOnly, TEXT("ras")) ||
		0 == _tcscmp(szExtOnly, TEXT("png")) 
		)
	{
		return true;
	}
	return false;
}
