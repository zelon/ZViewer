/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * AboutWindow.h
 *
 * 2006.11.4 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#pragma once

#include "../commonSrc/windowLib/ZWindow.h"

class CAboutWindow : public ZWindow
{
public:
	CAboutWindow();
	~CAboutWindow();

	virtual void SetWndProc();

	void DoResource(HWND hParentHWND);

protected:
};