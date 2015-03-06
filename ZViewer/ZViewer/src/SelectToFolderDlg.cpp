#include "stdafx.h"
#include "SelectToFolderDlg.h"

#include "../../commonSrc/CommonFunc.h"
#include "resource.h"
#include "ZResourceManager.h"
#include "ZMain.h"

CSelectToFolderDlg * pThis = NULL;

CSelectToFolderDlg::CSelectToFolderDlg(const tstring & initaliDir)
{
	pThis = this;

	m_strSelectedFolder = initaliDir;
}

CSelectToFolderDlg::~CSelectToFolderDlg()
{
}

bool CSelectToFolderDlg::DoModal()
{
	if ( IDOK == DialogBox(ZResourceManager::GetInstance().GetHInstance(), MAKEINTRESOURCE(IDD_MOVE_DIALOG), ZMain::GetInstance().GetHWND(), MoveToDlgPrc) )
	{
		return true;
	}
	return false;
}

void CSelectToFolderDlg::OnBrowserButton() const
{

}

INT_PTR CALLBACK CSelectToFolderDlg::MoveToDlgPrc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM /*lParam*/)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			pThis->m_hWnd = hWnd;

			if ( !pThis->m_strSelectedFolder.empty() )
			{
				pThis->SetFolder(pThis->m_strSelectedFolder);
			}
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch ( wParam )
			{
			case IDOK:
				{
					TCHAR szTemp[MAX_PATH];
					GetDlgItemText(hWnd, IDC_EDIT_MOVE_TO_FOLDER, szTemp, sizeof(szTemp) / sizeof(TCHAR) );

					pThis->m_strSelectedFolder = szTemp;

					::EndDialog(hWnd, IDOK);
				}

				break;

			case IDCANCEL:
				::EndDialog(hWnd, IDCANCEL);
				break;

			case IDC_BUTTON_SELECT_FOLDER:
				{
					TCHAR szFolder[MAX_PATH] = { 0 };

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

void CSelectToFolderDlg::SetFolder(const tstring & strFolder)
{
	m_strSelectedFolder = strFolder;

	SetDlgItemText(m_hWnd, IDC_EDIT_MOVE_TO_FOLDER, strFolder.c_str());
}
