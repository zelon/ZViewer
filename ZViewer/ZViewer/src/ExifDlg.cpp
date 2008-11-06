/* ------------------------------------------------------------------------
 *
 * ExifDlh.cpp
 *
 * 2008.10.27 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"

#include "ExifDlg.h"
#include "ZResourceManager.h"

#include "resource.h"

int CALLBACK ExifDlgWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

void ExifDlg::SetWndProc()
{
	m_wndProc = (WNDPROC)ExifDlgWndProc;
}

int CALLBACK ExifDlgWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			/*
			SetDlgItemText(hWnd, IDC_STATIC_VERSION, g_strVersion.c_str());

			TCHAR szTemp[MAX_PATH];
			StringCchPrintf(szTemp, MAX_PATH, TEXT("CacheHitRate : %d%%"), ZMain::GetInstance().GetLogCacheHitRate());
			SetDlgItemText(hWnd, IDC_STATIC_HITRATE, szTemp);

			NUMBERFMT nFmt = { 0, 0, 3, TEXT("."), TEXT(","), 1 };

			TCHAR szOUT[20];
			StringCchPrintf(szTemp, MAX_PATH, TEXT("%d"),ZMain::GetInstance().GetCachedKByte());
			::GetNumberFormat(NULL, NULL, szTemp, &nFmt, szOUT, 20);

			StringCchPrintf(szTemp, MAX_PATH, TEXT("CachedMemory : %sKB, Num of Cached Image : %d"), szOUT, ZCacheImage::GetInstance().GetNumOfCacheImage());
			SetDlgItemText(hWnd, IDC_STATIC_CACHE_MEMORY, szTemp);

			StringCchPrintf(szTemp, MAX_PATH, TEXT("ProgramPath : %s"), GetProgramFolder().c_str());
			SetDlgItemText(hWnd, IDC_STATIC_PROGRAM_PATH, szTemp);

			StringCchPrintf(szTemp, MAX_PATH, TEXT("Library Version : %s"), ZImage::GetLibraryVersion().c_str());
			SetDlgItemText(hWnd, IDC_STATIC_LIBRARY_VERSION, szTemp);
			*/
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDOK:
				EndDialog(hWnd, 0);
				break;
			}
		}
		break;

	}
	return FALSE;
}

void ExifDlg::DoResource(HWND hParentHWND)
{
	DialogBox(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDD_EXIF_DLG), hParentHWND, (DLGPROC)m_wndProc);
}


void ExifDlg::MakeExifMap(ZImage & image)
{
	image.GetExifMap(m_exifMap);
}
