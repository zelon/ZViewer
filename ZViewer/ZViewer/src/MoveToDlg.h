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

class CMoveToDlg
{
public:
	CMoveToDlg();
	~CMoveToDlg();

	bool DoModal();

	const std::string & GetMoveToFolder() const { return m_strMoveToFolder; }

	static int CALLBACK MoveToDlgPrc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

	void SetFolder(const std::string & strFolder);

protected:

	HWND m_hWnd;
	void OnBrowserButton();

	std::string m_strMoveToFolder;
};