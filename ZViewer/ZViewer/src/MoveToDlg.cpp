
#include "stdafx.h"
#include "MoveToDlg.h"

#include "resource.h"
#include "ZResourceManager.h"
#include "../ZMain.h"

CMoveToDlg * pThis = NULL;

CMoveToDlg::CMoveToDlg()
{
	pThis = this;
}

CMoveToDlg::~CMoveToDlg()
{
}

bool CMoveToDlg::DoModal()
{
	if ( IDOK == DialogBox(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDD_MOVE_DIALOG), ZMain::GetInstance().GetHWND(), MoveToDlgPrc) )
	{
		return true;
	}
	return false;
}

void CMoveToDlg::OnBrowserButton()
{

}

int CALLBACK CMoveToDlg::MoveToDlgPrc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static std::string strLastFolder = "";

	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			pThis->m_hWnd = hWnd;

			if ( !strLastFolder.empty() )
			{
				pThis->SetFolder(strLastFolder);
			}
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDOK:
				{
					char szTemp[MAX_PATH];
					GetDlgItemText(hWnd, IDC_EDIT_MOVE_TO_FOLDER, szTemp, sizeof(szTemp));

					pThis->m_strMoveToFolder = szTemp;
					strLastFolder = szTemp;

					::EndDialog(hWnd, IDOK);
				}

				break;

			case IDCANCEL:
				::EndDialog(hWnd, IDCANCEL);
				break;

			case IDC_BUTTON_SELECT_FOLDER:
				{
					char szFolder[MAX_PATH] = { 0 };

					if ( SelectFolder(hWnd, szFolder) )
					{
						pThis->SetFolder(szFolder);
					}
				}
				break;
			}
		}
		break;

	case WM_DESTROY:
		::EndDialog(hWnd, 0);
		return 0;
	}
	return FALSE;
}

void CMoveToDlg::SetFolder(const std::string & strFolder)
{
	m_strMoveToFolder = strFolder;

	SetDlgItemText(m_hWnd, IDC_EDIT_MOVE_TO_FOLDER, strFolder.c_str());
}