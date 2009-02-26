/********************************************************************
*
* Created by zelon(Kim, Jinwook - Korea)
* 
*   2008. 6. 2 created
*	FileName : TaskBar.cpp
*
*                                       http://zviewer.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "TaskBar.h"

/// 작업 표시줄을 보이게 해준다.
void TaskBar::ShellTrayShow()
{
	/// 작업 표시줄을 보이게 해준다.
	HWND h = FindWindow(TEXT("Shell_TrayWnd"), TEXT(""));

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_SHOW);
	}
}

void TaskBar::ShellTrayHide()
{
	// 작업 표시줄을 보이게 해준다.
	HWND h = FindWindow(TEXT("Shell_TrayWnd"), TEXT(""));

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_HIDE);
	}
}

