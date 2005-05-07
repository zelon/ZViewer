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

void CDesktopWallPaper::SetDesktopWallPaper()
{
	std::string strStyle = "0";

	switch ( m_eStyle )
	{
	case eDesktopWallPaperStyle_CENTER:
		strStyle = "0";
		break;

	case eDesktopWallPaperStyle_TILE:
		strStyle = "1";
		break;

	case eDesktopWallPaperStyle_STRETCH:
		strStyle = "2";
		break;

	default:
		_ASSERTE(false);
	}
	SetRegistryValue(HKEY_CURRENT_USER, "Control Panel\\Desktop", "WallpaperStyle", strStyle.c_str());
	SetRegistryValue(HKEY_CURRENT_USER, "Control Panel\\Desktop", "Wallpaper", m_strFileName.c_str());

	if ( strStyle == "1" )
	{
		SetRegistryValue(HKEY_CURRENT_USER, "Control Panel\\Desktop", "TileWallpaper", "1");
	}
	else
	{
		SetRegistryValue(HKEY_CURRENT_USER, "Control Panel\\Desktop", "TileWallpaper", "0");
	}

	// 실제로 적용시킨다.
	if ( 0 == SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)m_strFileName.c_str(), SPIF_SENDCHANGE) )
	{
		_ASSERTE(false);
	}

}

void CDesktopWallPaper::ClearDesktopWallPaper()
{
	CDesktopWallPaper wallPaper("");
	wallPaper.SetDesktopWallPaper();
}