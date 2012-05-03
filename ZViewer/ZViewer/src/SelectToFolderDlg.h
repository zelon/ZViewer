/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	MoveToDlg.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

/// class for selecting a directory
class CSelectToFolderDlg
{
public:
	CSelectToFolderDlg(const tstring & initaliDir = L"");
	~CSelectToFolderDlg();

	/// 다이얼로그를 화면에 띄운다.
	bool DoModal();

	const tstring & GetSelectedFolder() const { return m_strSelectedFolder; }

	static INT_PTR CALLBACK MoveToDlgPrc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

protected:

	HWND m_hWnd;
	void SetFolder(const tstring & strFolder);
	void OnBrowserButton() const;

	tstring m_strSelectedFolder;
};