/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZMain.h
*
*                                       http://www.wimy.com
*********************************************************************/
#pragma once

#include "src/ZHistory.h"
#include "src/ZOption.h"
#include "../lib/DesktopWallPaper.h"
#include "../lib/ZImage.h"
#include "src/ZCacheImage.h"


enum eFileSortOrder
{
	eFileSortOrder_FILENAME,
	eFileSortOrder_LAST_MODIFY_TIME,
	eFileSortOrder_FILESIZE,
};


class ZMain
{
public:
	static ZMain & GetInstance();
	~ZMain(void);

	void setInitArg(const std::string & strArg) { m_strInitArg = strArg; }
	void SetProgramFolder();
	void RescanFolder();			// 파일목록을 다시 읽어들인다.
	void OnInit();
	void Draw(bool bEraseBg = true);

	// 현재보고 있는 이미지를 윈도우 바탕화면의 배경으로 지정한다.
	void SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style);
	
	// 바탕화면 이미지를 없앤다.
	void ClearDesktopWallPaper();

	bool GetNeighborFolders(std::vector < std::string > & vFolders);
	void NextFolder();
	void PrevFolder();

	// 특정 위치의 이미지 파일로 건너뛴다.
	bool MoveIndex(int iIndex);

	// 현재 위치에서 파일을 이동한다.
	bool MoveRelateIndex(int iRelateIndex)
	{
		return MoveIndex(m_iCurretFileIndex + iRelateIndex);
	}

	bool NextImage()
	{
		ZCacheImage::GetInstance().SetLastActionDirection(eLastActionDirection_FORWARD);
		return MoveRelateIndex(+1);
	}

	bool PrevImage()
	{
		ZCacheImage::GetInstance().SetLastActionDirection(eLastActionDirection_BACKWARD);
		return MoveRelateIndex(-1);
	}

	bool FirstImage();
	bool LastImage();

	void Rotate(bool bClockWise);


	HWND GetHWND() const { return m_hMainDlg; }
	void SetHWND(HWND hWnd);
	void SetMainMenu(HMENU hMenu)
	{
		m_hMainMenu = hMenu;
	}
	void SetPopupMenu(HMENU hMenu)
	{
		m_hPopupMenu = hMenu;
	}

	long GetCachedKByte();

	void SetStatusHandle(HWND hWnd) { m_hStatus = hWnd; }
	HWND GetStatusHandle() const { return m_hStatus; }

	void ToggleFullScreen();

	void ToggleBigToScreenStretch();

	int GetLogCacheHitRate();

	void DeleteThisFile();
	void MoveThisFile();

	const std::string & GetProgramFolder() const	// 프로그램 실행 파일이 있는 폴더를 가져온다.
	{
		return m_strProgramFolder;
	}

	/// 적절한 언어 ini 를 읽어와서 화면을 세팅한다.
	//void LoadLanguage();

	void SetInstance(HINSTANCE hInstance) { m_hMainInstance = hInstance; }
	HINSTANCE GetHInstance() const { return m_hMainInstance; }
	void OnChangeCurrentSize(int iWidth, int iHeight);
	void OnDrag(int x, int y);

	void OnFocusLose();
	void OnFocusGet();

	void OnRightTopFirstDraw();

	inline bool IsFullScreen() const { return m_option.m_bFullScreen; }

	/// 특정 파일을 연다.
	void OpenFile(const std::string & strFilename);

	/// 특정 폴더의 첫번째 파일을 연다.
	void OpenFolder(const std::string strFolder);
	void OpenFileDialog();

	void Undo();
	void Redo();

	bool m_bHandCursor;

	void ShowFileExtDlg();

	void ShellTrayShow();

	void ChangeFilrSort(eFileSortOrder sortOrder);
	void ReLoadFileList();

	void LoadCurrent();

private:
	ZMain(void);

	void SetStatusBarText();
	void SetTitle();

	void InitOpenFileDialog();

	void ShellTrayHide();

	// 현재 파일이 지워졌을 때 후의 처리. 파일 삭제, 이동 후에 불리는 함수이다.
	void _ProcAfterRemoveThisFile();

	void FormShow();
	void FormHide();

	std::string m_strInitArg;		// 프로그램 시작 인자.
	std::string m_strProgramFolder;	// 프로그램 실행 파일이 있는 폴더

	std::string m_strCurrentFolder;	// 현재 폴더
	std::string m_strCurrentFilename;	// 현재 보여주고 있는 파일이름
	
	//std::vector<std::string> m_vFile;
	std::vector< FileData > m_vFile;
	eFileSortOrder m_sortOrder;

	int m_iCurretFileIndex;

	int m_iCurrentScreenWidth;
	int m_iCurrentScreenHeight;

	/// 현재 보여주고 있는 이미지
	ZImage m_currentImage;
	DWORD m_dwLoadingTime;				// 현재 이미지를 로딩하는 데 걸린 시간

	HWND m_hMainDlg;
	HWND m_hStatus;
	HINSTANCE m_hMainInstance;

	HMENU m_hMainMenu;
	HMENU m_hPopupMenu;

	ZOption m_option;

	int m_iShowingX;			// 그림 중 어디를 찍기 시작하나.
	int m_iShowingY;			// 그림 중 어디를 찍기 시작하나.

	void ZFindFile(const char *path, std::vector< FileData > & foundStorage, bool bFindRecursive);
	void ZFindFolders(const char *path, std::vector<std::string> & foundStorage, bool bFindRecursive = false);

	// For Open File Dialog
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[MAX_PATH];       // buffer for file name
	//bool m_bOpeningFileDialog;	// 전체화면일 때 파일 다이얼로그를 열면 깜빡거림을 막기 위해

	// For Undo/Redo
	ZHistory m_history;

};
