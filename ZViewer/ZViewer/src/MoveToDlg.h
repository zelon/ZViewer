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

/// 파일 이동을 할 때 보여주는 다이얼로그를 위한 클래스
class CMoveToDlg
{
public:
	CMoveToDlg();
	~CMoveToDlg();

	/// 다이얼로그를 화면에 띄운다.
	bool DoModal();

	const std::string & GetMoveToFolder() const { return m_strMoveToFolder; }

	static int CALLBACK MoveToDlgPrc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

	void SetFolder(const std::string & strFolder);

protected:

	HWND m_hWnd;
	void OnBrowserButton();

	std::string m_strMoveToFolder;
};