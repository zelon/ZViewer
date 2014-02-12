/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * MainWindow.h
 *
 * 2006.11.4 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#pragma once

#include "../commonSrc/windowLib/ZWindow.h"

void HandCursorProc();


class CMainWindow : public ZWindow
{
public:
	CMainWindow();
	~CMainWindow();

	virtual void SetWndProc();

	HWND Create(HINSTANCE hInstance, HWND hParentHWND, int nCmdShow);
	static HMENU CreatePopupMenu();
protected:
};