/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * ZWindow.cpp
 *
 * 2006.11.3 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "ZWindow.h"
#include "resource.h"
#include "src/ZMain.h"

#include "../commonSrc/MessageManager.h"

#include "src/ShortCut.h"
#include "src/ZResourceManager.h"
#include "../commonSrc/ZOption.h"

ZWindow::ZWindow()
{
	m_wndProc = NULL;
}


ZWindow::~ZWindow()
{
}


HWND ZWindow::Create(HINSTANCE hInstance, HWND hParentHWND, int nCmdShow, const tstring & dlgName, HMENU hMenu)
{
	SetWndProc();
	if ( NULL == m_wndProc)
	{
		assert(!"before create, set WndProc");
		return (HWND)INVALID_HANDLE_VALUE;
	}

	m_hParentWindow = hParentHWND;

	WNDCLASS WndClass;
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BIG_MAIN));
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=m_wndProc;
	WndClass.lpszClassName = dlgName.c_str();
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	RegisterClass(&WndClass);

	int iWidth = (int)(GetSystemMetrics(SM_CXSCREEN) * 0.8);
	int iHeight = (int)(GetSystemMetrics(SM_CYSCREEN) * 0.8);
	int iXPosition = (GetSystemMetrics(SM_CXSCREEN)/2) - (iWidth/2);
	int iYPosition = (GetSystemMetrics(SM_CYSCREEN)/2) - ( iHeight/2) ;

	//HMENU hMenu = (HMENU)LoadMenu(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDR_MAIN_MENU));

	//ZMain::GetInstance().SetMainMenu(hMenu);

	m_hWindow = ::CreateWindow(
		dlgName.c_str(),
		dlgName.c_str(),		///< Window Title
		WS_OVERLAPPEDWINDOW,///< | WS_EX_ACCEPTFILES,
		iXPosition,		///< Default position X
		iYPosition,		///< Default position Y
		iWidth,			///< width
		iHeight,		///< height
		hParentHWND,
		hMenu,			///< MainMenu
		hInstance,NULL);

	// Enable Drag&Drop from explorer
	//DragAcceptFiles(m_hWindow, TRUE);

	ShowWindow(m_hWindow, nCmdShow);

	m_hAccel = ShortCut::GetInstance().MakeAccelTable();

	DialogBox(hInstance, dlgName.c_str(), hParentHWND, (DLGPROC)m_wndProc);

	return m_hWindow;
}

WPARAM ZWindow::MsgProc()
{
	MSG Message;

	while(GetMessage(&Message,0,0,0))
	{
		if (!TranslateAccelerator(m_hWindow, m_hAccel, &Message))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}

	return Message.wParam;
}
