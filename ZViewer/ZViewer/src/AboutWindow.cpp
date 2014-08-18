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

LRESULT CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

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

void CreateLabel(const HWND hWnd, const TCHAR * msg, int & y)
{
	CreateWindow(TEXT("static"), msg, WS_VISIBLE | WS_CHILD, 20, y, 500, 30,
		hWnd, nullptr, nullptr, nullptr);

	y += 30;
}

LRESULT CALLBACK AboutWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_CREATE:
		{
			int y = 20;
			TCHAR szTemp[MAX_PATH];

			SPrintf(szTemp, MAX_PATH, TEXT("ZViewer Version : %s"), g_strVersion.c_str());
			CreateLabel(hWnd, szTemp, y);

			y += 20;

			SPrintf(szTemp, MAX_PATH, TEXT("CacheHitRate : %d%%"), ZMain::GetInstance().GetLogCacheHitRate());
			CreateLabel(hWnd, szTemp, y);

			NUMBERFMT nFmt = { 0, 0, 3, TEXT("."), TEXT(","), 1 };

			TCHAR szOUT[20];
			SPrintf(szTemp, MAX_PATH, TEXT("%d"),ZMain::GetInstance().GetCachedKByte());
			::GetNumberFormat((LCID)NULL, (DWORD)NULL, szTemp, &nFmt, szOUT, 20);

			SPrintf(szTemp, MAX_PATH, TEXT("CachedMemory : %sKB, Num of Cached Image : %u"), szOUT, static_cast<unsigned int>(ZCacheImage::GetInstance().GetNumOfCacheImage()));
			CreateLabel(hWnd, szTemp, y);

			SPrintf(szTemp, MAX_PATH, TEXT("ProgramPath : %s"), GetProgramFolder().c_str());
			CreateLabel(hWnd, szTemp, y);

			SPrintf(szTemp, MAX_PATH, TEXT("Library Version : %s"), ZImage::GetLibraryVersion().c_str());
			CreateLabel(hWnd, szTemp, y);

			y += 20;

			SPrintf(szTemp, MAX_PATH, TEXT("Homepage : http://zviewer.wimy.com/"));
			CreateLabel(hWnd, szTemp, y);
		}
		return TRUE;

		case WM_DESTROY:
		{
			EnableWindow( ZMain::GetInstance().GetHWND(), TRUE );
			break;
		}
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}


void CAboutWindow::DoResource(HWND hParentHWND)
{
	WNDCLASSEX wc = { 0, };
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.lpfnWndProc		= (WNDPROC)AboutWndProc;
	wc.hInstance		= ZResourceManager::GetInstance().GetHInstance();
	wc.hbrBackground	= GetSysColorBrush(COLOR_3DFACE);
	wc.lpszClassName	= TEXT("AboutWindow");
	RegisterClassEx(&wc);

	HWND hCreatedHandle = CreateWindowEx(
		/* exStyle */		  WS_EX_DLGMODALFRAME
		/* classname */		, TEXT("AboutWindow")
		/* window title */	, TEXT("About")
		/* dwStyle */		, WS_VISIBLE | WS_SYSMENU | WS_CAPTION
		/* x */				, 100
		/* y */				, 100
		/* width */			, 600
		/* height */		, 300
		/* hWndParent */	, hParentHWND
		/* hMenu */			, nullptr
		/* hInstance */		, ZResourceManager::GetInstance().GetHInstance()
		/* lpParam */		, nullptr
		);

	if ( hCreatedHandle == NULL )
	{
		assert(!"Cannot create aboutwindow");
		return;
	}

	// disable parent window to set this dialog modal
	EnableWindow( hParentHWND, FALSE );
}
