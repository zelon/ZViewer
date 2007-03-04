/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * ZWindow.h
 *
 * 2006.11.3 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#pragma once

class ZWindow
{
public:
	ZWindow();
	~ZWindow();

	virtual void SetWndProc() = 0;

	HWND Create(HINSTANCE hInstance, HWND hParentHWND, int nCmdShow);

	WPARAM MsgProc();

protected:

	HWND m_hParentWindow;
	HWND m_hWindow;
	HACCEL m_hAccel;

	WNDPROC m_wndProc;
};