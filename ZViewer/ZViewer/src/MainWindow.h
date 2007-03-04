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

class CMainWindow : public ZWindow
{
public:
	CMainWindow();
	~CMainWindow();

	virtual void SetWndProc();

protected:
};