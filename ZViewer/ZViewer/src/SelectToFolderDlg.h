#pragma once

/// class for selecting a directory
class CSelectToFolderDlg final
{
public:
	CSelectToFolderDlg(const tstring & initaliDir = L"");
	~CSelectToFolderDlg();

	/// 다이얼로그를 화면에 띄운다.
	bool DoModal();

	const tstring & GetSelectedFolder() const { return m_strSelectedFolder; }

	static INT_PTR CALLBACK MoveToDlgPrc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

private:
	HWND m_hWnd;
	void SetFolder(const tstring & strFolder);
	void OnBrowserButton() const;

	tstring m_strSelectedFolder;
};