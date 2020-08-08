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