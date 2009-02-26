/********************************************************************
*
* Created by zelon(Kim, Jinwook - Korea)
*
*   2005. 5. 7
*	ZMain.h
*
*                                       http://www.wimy.com
*********************************************************************/
#pragma once

#include "src/ZHistory.h"
#include "../commonSrc/DesktopWallPaper.h"
#include "../commonSrc/ZImage.h"
#include "src/ZCacheImage.h"

/// 파일을 보여줄 때의 정렬 순서
enum eFileSortOrder
{
	eFileSortOrder_FILENAME,
	eFileSortOrder_LAST_MODIFY_TIME,
	eFileSortOrder_FILESIZE,
};

/// 대부분의 작업을 처리하는 메인 클래스
class ZMain
{
	ZMain(void);
public:
	static ZMain & GetInstance();
	~ZMain(void);

	void StartSlideMode();

	/// Timer 를 받았을 때
	void onTimer();

	/// ZViewer 전용 메시지 박스
	int ShowMessageBox(const TCHAR * msg, UINT button = MB_OK);

	/// 현재 화면의 그릴 수 있는 영역의 크기를 받아온다.
	bool getCurrentScreenRect(RECT & rect);

	/// 현재 크기에 맞는 ShowWindow 크기를 정한다.
	void AdjustShowWindowScreen();

	/// On Window is resized
	void OnWindowResized();

	bool IsHandCursor() const { return m_bHandCursor; }
	void SetHandCursor(bool bHandCursor) { m_bHandCursor = bHandCursor; }

	void SetInitArg(const tstring & strArg) { m_strInitArg = strArg; }
	void RescanFolder();			///< 파일목록을 다시 읽어들인다.

	void OnInit();

	/// 현재 보고 있는 파일의 Exif 정보를 보여준다.
	void ShowExif();

	void Draw(HDC toDrawDC = NULL, bool bEraseBg = true);

	/// 현재보고 있는 이미지를 윈도우 바탕화면의 배경으로 지정한다.
	void SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style);

	/// 현재 위치의 폴더 이웃 폴더 - 상위 폴더의 하위 폴더들 - 를 얻어온다.
	bool GetNeighborFolders(std::vector < tstring > & vFolders);

	/// 다음 폴더로 이동
	void NextFolder();

	/// 이전 폴더로 이동
	void PrevFolder();

	/// 특정 위치의 이미지 파일로 건너뛴다.
	bool MoveIndex(int iIndex);

	int GetCalculatedMovedIndex(int iIndex);	///< 새로 이동할 인덱스 번호를 주면 지금 상황에 맞는 인덱스 번호를 돌려준다.

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

	void SetStatusHandle(HWND hWnd) { m_hStatusBar = hWnd; }
	HWND GetStatusHandle() const { return m_hStatusBar; }

	void StartTimer();
	void StopTimer();

	/// Cache status 를 상태 표시줄에 표시한다.
	void ShowCacheStatus();

	void ToggleAutoRotation();
	void ToggleAlwaysOnTop();

	/// 전체화면 모드를 토글한다.
	void ToggleFullScreen();

	void ToggleBigToScreenStretch();
	void ToggleSmallToScreenStretch();

	/// Loop Image 를 토글한다.
	void ToggleLoopImage();

	int GetLogCacheHitRate() const;

	/// 현재 파일을 휴지통으로 삭제한다.
	void DeleteThisFileToRecycleBin();

	/// 현재 파일을 삭제한다.
	void DeleteThisFile();

	/// 현재 파일을 이동한다.
	void MoveThisFile();

	/// 현재 파일을 복사한다.
	void CopyThisFile();

	void SetInstance(HINSTANCE hInstance) { m_hMainInstance = hInstance; }
	HINSTANCE GetHInstance() const { return m_hMainInstance; }
	void OnChangeCurrentSize(int iWidth, int iHeight);
	void OnDrag(int x, int y);

	void OnFocusLose();
	void OnFocusGet();

	void OnRightTopFirstDraw();

	/// 특정 파일을 연다.
	void OpenFile(const tstring & strFilename);

	/// 특정 폴더의 첫번째 파일을 연다.
	void OpenFolder(const tstring & strFolder);
	void OpenFileDialog();

	/// 현재 파일을 다른 형식으로 저장하는 파일 다이얼로그를 연다.
	void SaveFileDialog();

	void Undo();
	void Redo();

	void ShowFileExtDlg();

	void ChangeFileSort(eFileSortOrder sortOrder);
	void ReLoadFileList();

	void LoadCurrent();

	/// 그림을 보여줄 윈도우를 만든다.
	void CreateShowWindow();

	/// 상태 표시 윈도우를 만든다.
	void CreateStatusBar();

	/// 메뉴 중 체크표시 되는 것을 확인하여 설정해준다.
	void SetCheckMenus();

	void ZoomIn();
	void ZoomOut();

	void ZoomNone();

	HWND GetShowWindow() const { return m_hShowWindow; }

	/// 현재보는 이미지를 클립보드에 복사한다.
	void CopyToClipboard();
private:

	/// 현재 이미지를 드래그할 수 있어서, 손모양의 커서인가
	bool m_bHandCursor;

	void SetStatusBarText();
	void SetTitle();

	eOSKind m_osKind;

	void InitOpenFileDialog();

	typedef std::vector< FileData > FileListVector;

	void _GetFileListAndSort(const tstring & strFolderPathAndWildCard, FileListVector & vFileList);

	/// 현재 파일이 지워졌을 때 후의 처리. 파일 삭제, 이동 후에 불리는 함수이다.
	void _ProcAfterRemoveThisFile();

	/// 메뉴, 상태 표시줄등을 보여준다.
	void FormShow();

	/// 메뉴, 상태 표시줄등을 숨긴다.
	void FormHide();

	tstring m_strInitArg;			///< 프로그램 시작 인자.

	tstring m_strCurrentFolder;		///< 현재 폴더
	tstring m_strCurrentFilename;	///< 현재 보여주고 있는 파일이름

	/// TODO: 이 파일을 따로 클래스로 빼내어야 함. 왜냐하면 이 클래스 변동에 따라서 ZCacheImage 에 전달해줘야하기 때문임.
	FileListVector m_vFile;			///< 현재 폴더의 파일들 목록

	eFileSortOrder m_sortOrder;

	int m_iCurretFileIndex;

	/// 현재 보는 화면 배율. 1 이면 100%, 2 이면 200%, 0.1 이면 10%의 크기로 본다. 그림을 m_currentImage 로 했을 때 옵션에 따라서 배율을 결정한다.
	double m_fCurrentZoomRate;

	/// 현재 보여주고 있는 이미지
	ZImage * m_pCurrentImage;

	///< 현재 이미지를 로딩하는 데 걸린 시간
	DWORD m_dwLoadingTime;

	/// 메인 윈도우에 대한 핸들
	HWND m_hMainDlg;

	/// 창 아래의 상태 표시줄에 대한 핸들
	HWND m_hStatusBar;

	HINSTANCE m_hMainInstance;			///< 메인 인스턴스 핸들

	HMENU m_hMainMenu;					///< 메인 메뉴에 대한 핸들
	HMENU m_hPopupMenu;					///< 팝업 메뉴에 대한 핸들

	int m_iShowingX;					///< 그림 중 어디를 찍기 시작하나.
	int m_iShowingY;					///< 그림 중 어디를 찍기 시작하나.

	RECT m_lastPosition;				///< Full Screen 을 toggle 할 때 full screen 전의 창 위치

	static void FindFile(const TCHAR *path, std::vector< FileData > & foundStorage, bool bFindRecursive);
	static void FindFolders(const TCHAR *path, std::vector<tstring> & foundStorage, bool bFindRecursive = false);

	/// For Open File Dialog
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];       // buffer for file name
	//bool m_bOpeningFileDialog;	// 전체화면일 때 파일 다이얼로그를 열면 깜빡거림을 막기 위해

	// For Undo/Redo
	ZHistory m_history;

	/// For Cache DC
	HDC m_hBufferDC;

	UINT_PTR m_timerPtr;		///< for timer

	/// 버퍼로 쓰이는 DC 를 릴리즈한다.
	void _releaseBufferDC();

	/// Brush for Background
	HBRUSH m_bgBrush;

	/// 배경을 지운다.
	void _eraseBackground(HDC mainDC, LONG right, LONG bottom);

	/// 실제로 그림을 그릴 화면창
	HWND m_hShowWindow;

	/// 배경을 지울 때 쓰일 Region
	HRGN m_hEraseRgb;

	/// 현재보고 있는 화면의 센터는 그림의 몇 % 지점인가 기록
	float m_fCenterX;
	float m_fCenterY;

	/// Zoom in & out 전의 센터 위치로 그림을 드래그함. ZoomOut, ZoomIn 중에 호출됨
	void _PositionPreviousCenter();

	/// 현재 이미지가 로딩되었나
	bool m_bCurrentImageLoaded;
};
