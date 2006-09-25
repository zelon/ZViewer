/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	DesktopWallPaper.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "DesktopWallPaper.h"
#include "CommonFunc.h"

void CDesktopWallPaper::SetDesktopWallPaper(const tstring & strBitmapFileName, eDesktopWallPaperStyle style)
{
	tstring strStyle = TEXT("0");

	switch ( style )
	{
	case eDesktopWallPaperStyle_CENTER:
		strStyle = TEXT("0");
		break;

	case eDesktopWallPaperStyle_TILE:
		strStyle = TEXT("1");
		break;

	case eDesktopWallPaperStyle_STRETCH:
		strStyle = TEXT("2");
		break;

	default:
		_ASSERTE(false);
	}
	SetRegistryValue(HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop"), TEXT("WallpaperStyle"), strStyle.c_str());
	SetRegistryValue(HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop"), TEXT("Wallpaper"), strBitmapFileName.c_str());

	if ( strStyle == TEXT("1") )
	{
		SetRegistryValue(HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop"), TEXT("TileWallpaper"), TEXT("1"));
	}
	else
	{
		SetRegistryValue(HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop"), TEXT("TileWallpaper"), TEXT("0"));
	}

	// 실제로 적용시킨다.
	if ( 0 == SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)strBitmapFileName.c_str(), SPIF_SENDCHANGE) )
	{
		_ASSERTE(false);
	}

}

/// 바탕화면 이미지를 없앤다.
void CDesktopWallPaper::ClearDesktopWallPaper()
{
	CDesktopWallPaper::SetDesktopWallPaper(TEXT(""), eDesktopWallPaperStyle_CENTER);
}