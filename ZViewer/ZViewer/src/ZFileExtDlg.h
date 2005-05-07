/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZFileExtDlg.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

enum eFileExt
{
	eFileExt_BMP = 1,
	eFileExt_JPG = 2,
	eFileExt_GIF = 5,
	eFileExt_PNG = 3,
	eFileExt_PSD = 4,
	eFileExt_MAX
};

enum
{
	eFileExt_ALL = 0
};

class ZFileExtDlg
{
	ZFileExtDlg();

public:
	static ZFileExtDlg & GetInstance();

	static int CALLBACK FileExtDlgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

	void ShowDlg();


	/// 현재 시스템에 설정된 파일 확장자 연결 상태를 저장한다. true 가 연결된 것이다.
	bool m_bFileExt[eFileExt_MAX];

	/// 현재 시스템에 설정된 파일 확장자 연결 상태를 얻어온다.
	void LoadExtEnv(HWND hwnd);

	/// 현재 설정값대로 시스템에 쓴다.
	void SaveExtEnv();

private:
	/// 변경된 값이 있나?
	bool m_bChanged;

	/// 실제로 파일 연결을 세팅하는 함수
	bool SetExtWithProgram(const std::string & strProgramName, const std::string & strExt, std::string strFullProgramPath = "", const std::string & strIcon = "", int iIconIndex = 0);

	/// 확장자를 연결
	void ExtMapInit();
	typedef std::map< eFileExt, std::string> extMapType;
	extMapType	m_extMap;
};