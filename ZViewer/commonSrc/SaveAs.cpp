/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * s:\ZViewer\ZViewer\src\SaveAs.cpp
 *
 * 2006. 2. 4 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "SaveAs.h"

#include "ExtInfoManager.h"
#include "CommonFunc.h"

CSaveAs & CSaveAs::getInstance()
{
    static CSaveAs inst;
    return inst;
}

CSaveAs::CSaveAs()
{
	m_hParent = 0;
}

CSaveAs::~CSaveAs()
{
}

/// 파일 열기 다이얼로그는 항상 부모의 가운데에 띄운다.
UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	switch ( uiMsg )
	{
	case WM_INITDIALOG:
		{
			HWND hFileOpenHWND = ::GetParent(hdlg);
			HWND hParentHWND = ::GetParent(hFileOpenHWND);
			RECT thisRect, parentRect;
			if ( ::GetWindowRect(hParentHWND, &parentRect) )
			{
				if ( ::GetWindowRect(hFileOpenHWND, &thisRect) )
				{
					int centerX = (parentRect.right - parentRect.left) / 2;
					int centerY = (parentRect.bottom - parentRect.top ) / 2;

					int toX = centerX - (thisRect.right - thisRect.left ) / 2;
					int toY = centerY - (thisRect.bottom - thisRect.top ) / 2;

					::MoveWindow(hFileOpenHWND, toX, toY, thisRect.right - thisRect.left, thisRect.bottom - thisRect.top, TRUE);
					return 1;
				}
				else
				{
					assert(false);
				}
			}
			else
			{
				assert(false);
			}
		}
		break;
	}
	return 0L;
}

/// 다른 이름으로 저장창을 띄운다. 반환값이 false 이면 저장하지 않는다.
bool CSaveAs::showDialog()
{
	// Initialize OPENFILENAME
	ZeroMemory(&m_ofn, sizeof(m_ofn));
	m_ofn.lStructSize = sizeof(m_ofn);
	m_ofn.hwndOwner = m_hParent;
	SPrintf(m_szFilenamebuf, FILENAME_MAX, m_strSaveFileName.c_str());
	m_ofn.lpstrFile = m_szFilenamebuf;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
//	m_ofn.lpstrFile[0] = '\0';
	m_ofn.nMaxFile = FILENAME_MAX;
	m_ofn.lpstrFilter = ExtInfoManager::GetInstance().GetFileDlgFilter();
	m_ofn.nFilterIndex = 1;
	m_ofn.lpstrFileTitle = NULL;
	m_ofn.nMaxFileTitle = 0;
	m_ofn.lpstrInitialDir = m_strInitialiFolder.c_str();
	
	bool bUseHook = false;

	if ( bUseHook )
	{
		m_ofn.Flags = OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK | OFN_EXPLORER | OFN_ENABLESIZING | OFN_SHOWHELP;
		m_ofn.lpfnHook = OFNHookProc;
	}
	else
	{
		m_ofn.Flags = OFN_OVERWRITEPROMPT;// | OFN_ENABLEHOOK;
		//m_ofn.lpfnHook = OFNHookProc;
	}

	if ( FALSE == GetSaveFileName(&m_ofn) )
	{
		return false;
	}

	return true;
}
