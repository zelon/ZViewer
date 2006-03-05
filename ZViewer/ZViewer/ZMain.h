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
#include "../lib/DesktopWallPaper.h"
#include "../lib/ZImage.h"
#include "src/ZCacheImage.h"

/// 파일을 보여줄 때의 정렬 순서
enum eFileSortOrder
{
	eFileSortOrder_FILENAME,
	eFileSortOrder_LAST_MODIFY_TIME,
	eFileSortOrder_FILESIZE,
};

enum eOSKind
{
	eOSKind_UNKNOWN,
	eOSKind_98,
	eOSKind_2000,
	eOSKind_XP,
};

class ZMain
{
	ZMain(void);
public:
	static ZMain & GetInstance();
	~ZMain(void);

	bool IsHandCursor() const { return m_bHandCursor; }
	void SetHandCursor(bool bHandCursor) { m_bHandCursor = bHandCursor; }

	void SetInitArg(const std::string & strArg) { m_strInitArg = strArg; }
	void SetProgramFolder();
	void RescanFolder();			///< 파일목록을 다시 읽어들인다.
	void OnInit();
	void Draw(bool bEraseBg = true);

	/// 현재보고 있는 이미지를 윈도우 바탕화면의 배경으로 지정한다.
	void SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style);
	
	/// 바탕화면 이미지를 없앤다.
	void ClearDesktopWallPaper();

	/// 현재 위치의 폴더 이웃 폴더 - 상위 폴더의 하위 폴더들 - 를 얻어온다.
	bool GetNeighborFolders(std::vector < std::string > & vFolders);

	/// 다음 폴더로 이동
	void NextFolder();

	/// 이전 폴더로 이동
	void PrevFolder();

	/// 특정 위치의 이미지 파일로 건너뛴다.
	bool MoveIndex(int iIndex);

	/// 현재 위치에서 파일을 이동한다.
	bool MoveRelateIndex(int iRelateIndex)
	{
		return MoveIndex(m_iCurretFileIndex + iRelateIndex);
	}

	/// 다음 이미지 파일로 이동
	bool NextImage()
	{
		ZCacheImage::GetInstance().SetLastActionDirection(eLastActionDirection_FORWARD);
		return MoveRelateIndex(+1);
	}

	/// 이전 이미지 파일로 이동
	bool PrevImage()
	{
		ZCacheImage::GetInstance().SetLastActionDirection(eLastActionDirection_BACKWARD);
		return MoveRelateIndex(-1);
	}

	/// 첫번째 이미지 파일로 이동
	bool FirstImage();

	/// 마지막 이미지 파일로 이동
	bool LastImage();

	/// 현재보고 있는 이미지 파일을 90도 회전시킴
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

	long GetCachedKByte() const;

	void SetStatusHandle(HWND hWnd) { m_hStatus = hWnd; }
	HWND GetStatusHandle() const { return m_hStatus; }

	void ToggleFullScreen();

	void ToggleBigToScreenStretch();
	void ToggleSmallToScreenStretch();

	int GetLogCacheHitRate() const;

	void DeleteThisFile();
	void MoveThisFile();

	const std::string & GetProgramFolder() const	// 프로그램 실행 파일이 있는 폴더를 가져온다.
	{
		return m_strProgramFolder;
	}

	void SetInstance(HINSTANCE hInstance) { m_hMainInstance = hInstance; }
	HINSTANCE GetHInstance() const { return m_hMainInstance; }
	void OnChangeCurrentSize(int iWidth, int iHeight);
	void OnDrag(int x, int y);

	void OnFocusLose();
	void OnFocusGet();

	void OnRightTopFirstDraw();

	/// 특정 파일을 연다.
	void OpenFile(const std::string & strFilename);

	/// 특정 폴더의 첫번째 파일을 연다.
	void OpenFolder(const std::string strFolder);
	void OpenFileDialog();

	/// 현재 파일을 다른 형식으로 저장하는 파일 다이얼로그를 연다.
	void SaveFileDialog();

	void Undo();
	void Redo();

	void ShowFileExtDlg();

	/// 윈도우의 작업 표시줄을 보이게 해준다.
	void ShellTrayShow();

	void ChangeFileSort(eFileSortOrder sortOrder);
	void ReLoadFileList();

	void LoadCurrent();

private:

	/// 현재 이미지를 드래그할 수 있어서, 손모양의 커서인가
	bool m_bHandCursor;

	void SetStatusBarText();
	void SetTitle();

	BOOL _SetOSVersion();
	eOSKind m_osKind;

	void InitOpenFileDialog();

	typedef std::vector< FileData > FileListVector;

	void _GetFileListAndSort(const std::string & strFolderPathAndWildCard, FileListVector & vFileList);

	/// 윈도우의 작업 표시줄을 숨긴다.
	void ShellTrayHide();

	// 현재 파일이 지워졌을 때 후의 처리. 파일 삭제, 이동 후에 불리는 함수이다.
	void _ProcAfterRemoveThisFile();

	void FormShow();
	void FormHide();

	std::string m_strInitArg;			///< 프로그램 시작 인자.
	std::string m_strProgramFolder;		///< 프로그램 실행 파일이 있는 폴더

	std::string m_strCurrentFolder;		///< 현재 폴더
	std::string m_strCurrentFilename;	///< 현재 보여주고 있는 파일이름
	
	FileListVector m_vFile;
	eFileSortOrder m_sortOrder;

	int m_iCurretFileIndex;

	int m_iCurrentScreenWidth;
	int m_iCurrentScreenHeight;

	/// 현재 보여주고 있는 이미지
	ZImage m_currentImage;
	DWORD m_dwLoadingTime;				///< 현재 이미지를 로딩하는 데 걸린 시간

	HWND m_hMainDlg;
	HWND m_hStatus;
	HINSTANCE m_hMainInstance;

	HMENU m_hMainMenu;
	HMENU m_hPopupMenu;

	int m_iShowingX;					///< 그림 중 어디를 찍기 시작하나.
	int m_iShowingY;					///< 그림 중 어디를 찍기 시작하나.

	void ZFindFile(const char *path, std::vector< FileData > & foundStorage, bool bFindRecursive);
	void ZFindFolders(const char *path, std::vector<std::string> & foundStorage, bool bFindRecursive = false);

	// For Open File Dialog
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[MAX_PATH];       // buffer for file name
	//bool m_bOpeningFileDialog;	// 전체화면일 때 파일 다이얼로그를 열면 깜빡거림을 막기 위해

	// For Undo/Redo
	ZHistory m_history;

};
