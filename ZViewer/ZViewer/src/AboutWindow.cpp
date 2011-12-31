/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * AboutWindow.cpp
 *
 * 2006.11.4 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "../res/resource.h"

#include "AboutWindow.h"
#include "src/ZMain.h"
#include "src/ZResourceManager.h"

int CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

CAboutWindow::CAboutWindow()
{
}


CAboutWindow::~CAboutWindow()
{
}


void CAboutWindow::SetWndProc()
{
	m_wndProc = (WNDPROC)AboutWndProc;
}


int CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM /*lParam*/)
{
	switch(iMessage)
	{
		case WM_INITDIALOG:
		{
			SetDlgItemText(hWnd, IDC_STATIC_VERSION, g_strVersion.c_str());

			TCHAR szTemp[MAX_PATH];
			SPrintf(szTemp, MAX_PATH, TEXT("CacheHitRate : %d%%"), ZMain::GetInstance().GetLogCacheHitRate());
			SetDlgItemText(hWnd, IDC_STATIC_HITRATE, szTemp);

			NUMBERFMT nFmt = { 0, 0, 3, TEXT("."), TEXT(","), 1 };

			TCHAR szOUT[20];
			SPrintf(szTemp, MAX_PATH, TEXT("%d"),ZMain::GetInstance().GetCachedKByte());
			::GetNumberFormat((LCID)NULL, (DWORD)NULL, szTemp, &nFmt, szOUT, 20);

			SPrintf(szTemp, MAX_PATH, TEXT("CachedMemory : %sKB, Num of Cached Image : %d"), szOUT, ZCacheImage::GetInstance().GetNumOfCacheImage());
			SetDlgItemText(hWnd, IDC_STATIC_CACHE_MEMORY, szTemp);

			SPrintf(szTemp, MAX_PATH, TEXT("ProgramPath : %s"), GetProgramFolder().c_str());
			SetDlgItemText(hWnd, IDC_STATIC_PROGRAM_PATH, szTemp);

			SPrintf(szTemp, MAX_PATH, TEXT("Library Version : %s"), ZImage::GetLibraryVersion().c_str());
			SetDlgItemText(hWnd, IDC_STATIC_LIBRARY_VERSION, szTemp);
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

		case WM_CLOSE:
		{
			EndDialog(hWnd, 0);
			break;
		}
	}
	return FALSE;
}


void CAboutWindow::DoResource(HWND hParentHWND)
{
	DialogBox(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDD_DIALOGHELP), hParentHWND, (DLGPROC)AboutWndProc);
}
