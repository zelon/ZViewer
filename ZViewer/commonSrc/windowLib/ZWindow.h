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
	virtual ~ZWindow();

	virtual void SetWndProc() = 0;

	virtual HWND Create(HINSTANCE hInstance, HWND hParentHWND, int nCmdShow, const tstring & dlgName, HMENU hMenu);

	WPARAM DoModal()
	{
		return MsgProc();
	}

	virtual void SetHWND(HWND hWnd)
	{
		m_hWindow = hWnd;
	}

protected:

	WPARAM MsgProc();
	HWND m_hParentWindow;
	HWND m_hWindow;
	HACCEL m_hAccel;

	WNDPROC m_wndProc;
};