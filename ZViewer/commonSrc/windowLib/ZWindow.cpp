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

#include "src/ZResourceManager.h"
#include "src/ZOption.h"

ZWindow::ZWindow()
{
	m_wndProc = NULL;
}


ZWindow::~ZWindow()
{
}


HWND ZWindow::Create(HINSTANCE hInstance, HWND hParentHWND, int nCmdShow)
{
	if ( NULL == m_wndProc)
	{
		_ASSERTE(!"before create, set WndProc");
		return (HWND)INVALID_HANDLE_VALUE;
	}

	m_hParentWindow = hParentHWND;

	TCHAR lpszClass[256] = TEXT("ZViewer");

	WNDCLASS WndClass;
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BIG_MAIN));
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=m_wndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	RegisterClass(&WndClass);

	int iWidth = 1000;
	int iHeight = 700;
	int iXPosition = (GetSystemMetrics(SM_CXSCREEN)/2) - (iWidth/2);
	int iYPosition = (GetSystemMetrics(SM_CYSCREEN)/2) - ( iHeight/2) ;

	HMENU hMenu = (HMENU)LoadMenu(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDR_MAIN_MENU));

	ZMain::GetInstance().SetMainMenu(hMenu);

	m_hWindow = ::CreateWindow(
		lpszClass,
		lpszClass,		///< Window Title
		WS_OVERLAPPEDWINDOW,///< | WS_EX_ACCEPTFILES,
		iXPosition,		///< �⺻ x ��ġ
		iYPosition,		///< �⺻ y ��ġ
		iWidth,			///< width
		iHeight,		///< height
		NULL,
		hMenu,			///< MainMenu
		hInstance,NULL);

	// Ž���⿡���� Drag&Drop �� �����ϰ� �Ѵ�.
	DragAcceptFiles(m_hWindow, TRUE);

	ShowWindow(m_hWindow, nCmdShow);

	// ����Ű ����. ���⼭ ��ȯ�� �ڵ��� ���α׷��� ����� �� �ڵ������� close ��.
	m_hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN_ACCELERATOR));

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