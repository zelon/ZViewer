#include "StdAfx.h"
#include ".\zmain.h"
#include "src/ZFileExtDlg.h"
#include "src/ZCacheImage.h"
#include "src/ZINIOption.h"
#include "src/ZResourceManager.h"

#include "resource.h"

using namespace std;

// 최대 크기를 넘지 않는 적당한 리사이즈 크기를 돌려준다.
RECT GetResizedRect(const RECT & MaximumSize, const RECT & originalSize)
{
	if ( originalSize.right <= MaximumSize.right && originalSize.bottom <= MaximumSize.bottom )
	{
		RECT ret = originalSize;
		return ret;
	}

	// 가로 세로 크기 중 큰 값을 찾는다.
	bool bSetWidth = true;		// 가로 크기를 기준으로 맞출 것인가?

	double dWidthRate = (double)MaximumSize.right / (double)originalSize.right;
	double dHeightRate = (double)MaximumSize.bottom / (double)originalSize.bottom;

	if ( dHeightRate >=  dWidthRate)
	{
		bSetWidth = true;
	}
	else
	{
		bSetWidth = false;
	}

	// 큰 값이 MaximumSize 가 되게 하는 비례를 찾는다.
	RECT ret;

	double dRate = 1;
	if ( bSetWidth == true )
	{
		// 가로 크기가 기준이다.
		SetRect(&ret, 0, 0, (int)(originalSize.right*dWidthRate), (int)(originalSize.bottom*dWidthRate));
	}
	else
	{
		// 세로 크기가 기준이다.
		SetRect(&ret, 0, 0, (int)(originalSize.right*dHeightRate), (int)(originalSize.bottom*dHeightRate));
	}


	_ASSERTE(ret.right <= MaximumSize.right);
	_ASSERTE(ret.bottom <= MaximumSize.bottom);

	return ret;
}


ZMain & ZMain::GetInstance()
{
	static ZMain aInstance;
	return aInstance;
}

ZMain::ZMain(void)
:	m_hMainDlg(NULL)
	
{
	SetProgramFolder();

	if ( m_bannerImage.load((m_strProgramFolder + "\\banner.jpg").c_str()) == false )
	{
		_ASSERTE(!"can't load banner.");
	}
	
	// 배너 다운로드를 시작한다.
	//StartBannerDownload();

}

ZMain::~ZMain(void)
{
}

int ZMain::GetLogCacheHitRate()
{
	return ZCacheImage::GetInstance().GetLogCacheHitRate();
}

long ZMain::GetCachedKByte()
{
	return ZCacheImage::GetInstance().GetCachedKByte();
}

void ZMain::SetHWND(HWND hWnd)
{
	m_hMainDlg = hWnd;
	InitOpenFileDialog();
}

void ZMain::InitOpenFileDialog()
{
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hMainDlg;
	ofn.lpstrFile = szFile;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "ImageFiles(jpg,gif,png,bmp,psd,tga,tif,ico)\0*.jpg;*.jpeg;*.gif;*.png;*.bmp;*.psd,*.tga;*.tif;*.ico\0All(*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = m_strCurrentFolder.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

void ZMain::OpenFileDialog()
{
	// 현재 폴더를 세팅해 준다.
	ofn.lpstrInitialDir = m_strCurrentFolder.c_str();

	//m_bOpeningFileDialog = true;

	// Display the Open dialog box. 
	if (GetOpenFileName(&ofn)==TRUE) 
	{
		/*
		hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
		0, (LPSECURITY_ATTRIBUTES) NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		(HANDLE) NULL);
		*/
		OpenFile(ofn.lpstrFile);
	}
}

void ZMain::OnInit()
{
	if ( m_strInitArg.empty() )
	{
		// 시작 인자가 없으면 프로그램 폴더가 현재 폴더이다.
		m_strCurrentFolder = m_strProgramFolder;
		RescanFolder();

		//m_currentImage = m_bannerImage;
	}
	else
	{
		// 시작 인자가 있으면 그 파일을 보여준다.
		OpenFile(m_strInitArg);
	}
}

std::string ZMain::GetFolderNameFromFullFileName(const std::string & strFullFilename)
{
	char szDrive[_MAX_DRIVE] = { 0 };
	char szDir[_MAX_DIR] = { 0 };
	_splitpath(strFullFilename.c_str(), szDrive, szDir, 0, 0);

	std::string strFolder = szDrive;
	strFolder += szDir;

	return strFolder;
}

void ZMain::Draw(bool bEraseBg)
{
	//return;
	if ( m_currentImage.IsValid() == FALSE ) return;
	if ( m_hMainDlg == NULL ) return;

	RECT rt;
	GetClientRect(m_hMainDlg, &rt);
	if ( m_option.m_bFullScreen == false ) rt.bottom -= STATUSBAR_HEIGHT;

	HDC mainDC = GetDC(m_hMainDlg);

	if ( m_vFile.size() <= 0 )
	{
		SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
		Rectangle(mainDC, 0, 0, rt.right, rt.bottom);

		ReleaseDC(m_hMainDlg, mainDC);
		return;
	}

	// 그림이 그려지기 시작할 화면의 위치
	int iDrawX = 0;			
	int iDrawY = 0;

	// 그림이 그려지기 시작하는 그림의 위치
	int iDrawPartX = 0;
	int iDrawPartY = 0;

	bool bWidthBig = false;
	bool bHeightBig = false;

	if ( m_currentImage.GetWidth() < rt.right )
	{
		// 가로 길이가 화면보다 작을 때는 중앙에 오게
		iDrawX = (rt.right/2) - (m_currentImage.GetWidth()/2);
	}
	else
	{
		// 가로 길이가 화면보다 크면 클리핑
		iDrawPartX = m_iShowingX;
		bWidthBig = true;
	}

	if ( m_currentImage.GetHeight() < rt.bottom )
	{
		// 세로 길이가 화면보다 작을 때는 중앙에 오게
		iDrawY = (rt.bottom/2) - (m_currentImage.GetHeight()/2);
	}
	else
	{
		// 세로 길이가 화면보다 크면 클리핑
		iDrawPartY = m_iShowingY;
		bHeightBig = true;
	}


	/// 그림이 화면보다 큰가 확인
	if ( bWidthBig || bHeightBig )
	{
		/// 그림이 화면보다 크면...

		if ( m_option.m_bBigToSmallStretchImage )
		{
			/// 큰 그림을 축소시켜서 그린다.

			RECT toRect;
			SetRect(&toRect, 0, 0, m_currentImage.GetWidth(), m_currentImage.GetHeight());
			toRect = GetResizedRect(rt, toRect);

			ZImage stretchedImage(m_currentImage);

			if ( stretchedImage.GetBPP() == 8 )
			{
				stretchedImage.ConvertTo32Bit();
			}
			stretchedImage.Resize((WORD)toRect.right, (WORD)toRect.bottom);
			
			iDrawX = (rt.right/2) - (toRect.right/2);
			iDrawY = (rt.bottom/2) - (toRect.bottom/2);

			if ( bEraseBg )	// 배경을 지워야 하면 지운다.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, rt.right, rt.bottom);
			}

			int r = StretchDIBits(mainDC,
				iDrawX, iDrawY, 
				stretchedImage.GetWidth(), stretchedImage.GetHeight(),
				0, 0,
				stretchedImage.GetWidth(), stretchedImage.GetHeight(),
				stretchedImage.GetData(),
				stretchedImage.GetBitmapInfo(),
				DIB_RGB_COLORS, SRCCOPY);
		}
		else
		{
			/// 큰 그림을 스크롤이 가능하게 클리핑해서 그린다.
			HDC memDC = CreateCompatibleDC(mainDC);

			int x = m_currentImage.GetWidth();
			int y = m_currentImage.GetHeight();
			HBITMAP hbmScreen = CreateCompatibleBitmap(mainDC,
				x, 
				y); 

			SelectObject(memDC, hbmScreen);


			// 메모리에 전체 그림을 그린다.
			int r = StretchDIBits(memDC,
				0, 0, 
				m_currentImage.GetWidth(), m_currentImage.GetHeight(),
				0, 0,
				m_currentImage.GetWidth(), m_currentImage.GetHeight(),
				m_currentImage.GetData(),
				m_currentImage.GetBitmapInfo(),
				DIB_RGB_COLORS, SRCCOPY);

			if ( bEraseBg )	// 배경을 지워야 하면 지운다.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, rt.right, rt.bottom);
			}

			// 메모리것을 화면에 그린다.
			BOOL b = BitBlt(mainDC, 
				iDrawX, iDrawY,			// 그릴 화면의 x, y 좌표. 화면에 꽉 찰 때는 0, 0 이어야한다.
				m_currentImage.GetWidth(), rt.bottom,		// 그려질 화면의 가로, 세로 길이. 
				memDC, 
				iDrawPartX, iDrawPartY,			// 그려질 이미지 원본의 시작 x,y 좌표
				SRCCOPY);

#ifdef _DEBUG
			char szTemp[256];
			sprintf(szTemp, "rt.bottom : %d, PartX : %d, iDrawPartY : %d\n", rt.bottom, iDrawPartX, iDrawPartY);
			//	strstream s;
			//	s.clear();
			//	s << rt.bottom << "\n";
			OutputDebugString(szTemp);
#endif

			DeleteObject(hbmScreen);
			DeleteDC(memDC);
		}
	}
	else	/// 화면이 그림보다 작으면...
	{
		if ( m_option.m_bSmallToBigStretchImage )
		{
			/// 작은 그림을 화면에 맞게 확대해서 그린다.
		}
		else
		{
			if ( bEraseBg )	// 배경을 지워야 하면 지운다.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, rt.right, rt.bottom);
			}

			/// 작은 그림을 화면 가운데에 그린다.
			int r = StretchDIBits(mainDC,
				iDrawX, iDrawY, 
				m_currentImage.GetWidth(), m_currentImage.GetHeight(),
				0, 0,
				m_currentImage.GetWidth(), m_currentImage.GetHeight(),
				m_currentImage.GetData(),
				m_currentImage.GetBitmapInfo(),
				DIB_RGB_COLORS, SRCCOPY);
		}
	}
	ReleaseDC(m_hMainDlg, mainDC);

	// 마우스 커서 모양
	if ( m_currentImage.GetWidth() > rt.right ||
		m_currentImage.GetHeight() > rt.bottom
		)
	{
		// 마우스 커서를 hand 로
		m_bHandCursor = true;
	}
	else
	{
		// 마우스 커서를 원래대로
		m_bHandCursor = false;
	}
	PostMessage(m_hMainDlg, WM_SETCURSOR, 0, 0);

	if ( false == m_option.m_bFullScreen )
	{
		PostMessage(m_hStatus, WM_PAINT, 0, 0);
	}
}

bool StringCompare(const std::string & a, const std::string & b)
{
	return (strcmp(b.c_str(), a.c_str()) > 0);
}

bool ZMain::IsValidImageFileExt(const char * szFilename)
{

	if ( PathMatchSpec ( szFilename, ("*.bmp") ) ||
		PathMatchSpec ( szFilename, ("*.jpg") ) ||
		PathMatchSpec ( szFilename, ("*.jpeg") ) ||
		PathMatchSpec ( szFilename, ("*.gif") ) ||
		PathMatchSpec ( szFilename, ("*.ico") ) ||
		PathMatchSpec ( szFilename, ("*.pcx") ) ||
		PathMatchSpec ( szFilename, ("*.psd") ) ||
		PathMatchSpec ( szFilename, ("*.tif") ) ||
		PathMatchSpec ( szFilename, ("*.tga") ) ||
		PathMatchSpec ( szFilename, ("*.png") ) ||
		PathMatchSpec ( szFilename, ("*.jpg") )
		)
	{
		return true;
	}
	return false;
}

void ZMain::ZFindFile(const char *path, std::vector<std::string> & foundStorage, bool bFindRecursive)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	//memset(&wfd, 0, sizeof(wfd));
	char fname[MAX_PATH] = { 0 };
	BOOL bResult=TRUE;
	char drive[_MAX_DRIVE] = { 0 };
	char dir[MAX_PATH] = { 0 };
	char newpath[MAX_PATH] = { 0 };

	hSrch=FindFirstFile(path,&wfd);
	while (bResult)
	{
		_splitpath(path,drive,dir,NULL,NULL);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0]!='.' && bFindRecursive == true)
			{
				wsprintf(newpath,"%s%s%s\\*.*",drive,dir,wfd.cFileName);
				ZFindFile(newpath, foundStorage, bFindRecursive);
			}
		}
		else
		{
			wsprintf(fname,"%s%s%s",drive,dir,wfd.cFileName);

			if ( IsValidImageFileExt(wfd.cFileName) )
			{
				foundStorage.push_back(fname);
			}
		}
		bResult=FindNextFile(hSrch,&wfd);
	}
	FindClose(hSrch);
}

void ZMain::RescanFolder()
{
	std::string strToFindFolder = m_strCurrentFolder;

	strToFindFolder += "*.*";

	m_vFile.clear();
	ZFindFile(strToFindFolder.c_str(), m_vFile, false);

	// 얻은 파일을 정렬한다.
	sort(m_vFile.begin(), m_vFile.end(), StringCompare);

	// Cache Thread 에 전달한다.
	ZCacheImage::GetInstance().SetImageVector(m_vFile);

	if ( m_strCurrentFilename.empty() && !m_vFile.empty())
	{
		m_strCurrentFilename = m_vFile[0];
		m_iCurretFileIndex = 0;

		LoadCurrent();
	}
}

void ZMain::SetProgramFolder()
{
	char szGetFileName[FILENAME_MAX] = { 0 };
	DWORD ret = GetModuleFileName(GetModuleHandle(NULL), szGetFileName, FILENAME_MAX);

	if ( ret == 0 )
	{
		_ASSERTE(!"Can't get module folder");
	}
	char szDrive[_MAX_DRIVE] = { 0 };
	char szDir[_MAX_DIR] = { 0 };
	_splitpath(szGetFileName, szDrive, szDir, 0, 0);

	m_strProgramFolder = szDrive;
	m_strProgramFolder += szDir;
}

bool ZMain::NextImage()
{
	if ( m_vFile.empty() ) return false;

	unsigned int iNextIndex = m_iCurretFileIndex + 1;

	if ( iNextIndex >= m_vFile.size() )	// 없는 인덱스이다.
	{
		return false;
	}
	m_iCurretFileIndex++;
	m_strCurrentFilename = m_vFile[m_iCurretFileIndex];
	LoadCurrent();
	return true;
}

void ZMain::OpenFile(const string & strFilename)
{
	m_strCurrentFolder = GetFolderNameFromFullFileName(strFilename);
	RescanFolder();

	// 스캔한 파일 중 현재 파일을 찾는다.
	std::vector<std::string>::iterator it = find(m_vFile.begin(), m_vFile.end(), strFilename);

	if ( m_vFile.size() <= 0 )
	{
		_ASSERTE(!"size of scanned file list is 0.");

		return;
	}

	_ASSERTE(it != m_vFile.end());	// 그 파일이 없을리가 없다.

	if ( it != m_vFile.end() )
	{
		m_iCurretFileIndex = (int)(it - m_vFile.begin());//i;
		m_strCurrentFilename = strFilename;

		LoadCurrent();

		Draw();
	}
}

bool ZMain::PrevImage()
{
	if ( m_vFile.empty() ) return false;
	int iPrevIndex = m_iCurretFileIndex - 1;

	if ( iPrevIndex < 0 ) return false;	// 없는 인덱스이다.

	m_iCurretFileIndex--;
	m_strCurrentFilename = m_vFile[m_iCurretFileIndex];
	LoadCurrent();
	return true;
}

bool ZMain::FirstImage()
{
	if ( m_vFile.empty() ) return false;
	if ( m_iCurretFileIndex == 0 ) return false;

	// 현재의 위치를 History 에 저장해놓는다.
	m_history.push_lastImageIndex(m_iCurretFileIndex);

	m_iCurretFileIndex = 0;
	m_strCurrentFilename = m_vFile[0];
	LoadCurrent();
	return true;
}

bool ZMain::LastImage()
{
	if ( m_vFile.empty() ) return false;
	if ( m_iCurretFileIndex == ( m_vFile.size() -1 ) ) return false;	// 이미 마지막 파일이면

	// 현재의 위치를 History 에 저장해놓는다.
	m_history.push_lastImageIndex(m_iCurretFileIndex);

	m_iCurretFileIndex = (int)m_vFile.size() - 1;
	m_strCurrentFilename = m_vFile[m_iCurretFileIndex];
	LoadCurrent();
	return true;
}

void ZMain::OnChangeCurrentSize(int iWidth, int iHeight)
{
	m_iCurrentScreenWidth = iWidth;
	m_iCurrentScreenHeight = iHeight;

	if ( m_iShowingX + iWidth > m_currentImage.GetWidth() )
	{
		m_iShowingX -= (m_iShowingX + iWidth - m_currentImage.GetWidth());

		if ( m_iShowingX < 0 ) m_iShowingX = 0;
	}
	if ( m_iShowingY + iHeight > m_currentImage.GetHeight() )
	{
		m_iShowingY -= (m_iShowingY + iHeight - m_currentImage.GetHeight());
		if ( m_iShowingY < 0 ) m_iShowingY = 0;
	}
}

void ZMain::FormHide()
{
	LONG style = GetWindowLong(m_hMainDlg, GWL_STYLE);
	style &= ~WS_CAPTION;
	style &= ~WS_THICKFRAME;
	ShowWindow(m_hStatus, SW_HIDE);
	SetMenu(m_hMainDlg, NULL);
	SetWindowLong(m_hMainDlg, GWL_STYLE, style);
}

void ZMain::FormShow()
{
	LONG style = GetWindowLong(m_hMainDlg, GWL_STYLE);
	style |= WS_CAPTION;
	style |= WS_THICKFRAME;
	ShowWindow(m_hStatus, SW_SHOW);
	SetMenu(m_hMainDlg, m_hMainMenu);
	SetWindowLong(m_hMainDlg, GWL_STYLE, style);
}

void ZMain::ToggleFullScreen()
{
	static RECT lastPosition;

	if ( m_option.m_bFullScreen )	// 현재 풀스크린이면 원래 화면으로 돌아간다.
	{
		m_option.m_bFullScreen = !m_option.m_bFullScreen;

		ShellTrayShow();	// 숨겨졌던 작업 표시줄을 보여준다.

		FormShow();	// 메뉴, 상태 표시줄등을 보여준다.

		SetWindowPos(m_hMainDlg, HWND_TOP, lastPosition.left, lastPosition.top, lastPosition.right - lastPosition.left, lastPosition.bottom - lastPosition.top, SWP_SHOWWINDOW);
	}
	else	// 현재 풀스크린이 아니면 풀스크린으로 만든다.
	{
		m_option.m_bFullScreen = !m_option.m_bFullScreen;
		// 현재 크기를 기억한다.
#pragma message("TODO: Maximized 되었을 때 처리")
		GetWindowRect(m_hMainDlg, &lastPosition);

		FormHide();// 메뉴, 상태 표시줄등을 숨긴다.

		SetWindowPos(m_hMainDlg, HWND_TOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		MoveWindow(m_hMainDlg, 0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), TRUE);

		// 작업 표시줄을 가려준다.
		ShellTrayHide();

		// 포커스를 잃으면 원래대로 돌아가야하므로 풀어놓는다.
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		//MoveWindow(m_hMainDlg, )
		//m_iRestoreX = 
	}

	

	CheckMenuItem(m_hMainMenu, ID_VIEW_FULLSCREEN, m_option.m_bFullScreen ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_VIEW_FULLSCREEN, m_option.m_bFullScreen ? MF_CHECKED : MF_UNCHECKED);

}

void ZMain::ToggleBigToScreenStretch()
{
	m_option.m_bBigToSmallStretchImage = !m_option.m_bBigToSmallStretchImage;

	CheckMenuItem(m_hMainMenu, ID_VIEW_BIGTOSCREENSTRETCH , m_option.m_bBigToSmallStretchImage ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_BIGTOSCREENSTRETCH, m_option.m_bBigToSmallStretchImage ? MF_CHECKED : MF_UNCHECKED);

	Draw();
}

void ZMain::SetStatusBarText()
{
	char szTemp[256];

	if ( m_vFile.size() == 0 || m_strCurrentFilename.empty() )
	{
		// File Index
		sprintf(szTemp, "No File");
		SendMessage(m_hStatus, SB_SETTEXT, 0, (LPARAM)szTemp);

		// 해상도 정보
		sprintf(szTemp, "");
		SendMessage(m_hStatus, SB_SETTEXT, 1, (LPARAM)szTemp);

		// 이미지 사이즈
		sprintf(szTemp, "");
		SendMessage(m_hStatus, SB_SETTEXT, 2, (LPARAM)szTemp);

		// 임시로 http://wimy.com
		sprintf(szTemp, "http://wimy.com");
		SendMessage(m_hStatus, SB_SETTEXT, 3, (LPARAM)szTemp);

		// 로딩시간
		sprintf(szTemp, "");
		SendMessage(m_hStatus, SB_SETTEXT, 4, (LPARAM)szTemp);

		// 파일명
		sprintf(szTemp, "No File");
		SendMessage(m_hStatus, SB_SETTEXT, 5, (LPARAM)szTemp);
	}
	else
	{
		// File Index
		sprintf(szTemp, "%d/%d", m_iCurretFileIndex+1, m_vFile.size());
		SendMessage(m_hStatus, SB_SETTEXT, 0, (LPARAM)szTemp);

		// 해상도 정보
		sprintf(szTemp, "%dx%dx%dbpp", m_currentImage.GetWidth(), m_currentImage.GetHeight(), m_currentImage.GetBPP());
		SendMessage(m_hStatus, SB_SETTEXT, 1, (LPARAM)szTemp);

		// image size
		long imageSize = m_currentImage.GetImageSize();

		if ( imageSize > 1024 )
		{
			sprintf(szTemp, "%dKByte", m_currentImage.GetImageSize()/1024);
		}
		else
		{
			sprintf(szTemp, "%dByte", m_currentImage.GetImageSize());
		}
		SendMessage(m_hStatus, SB_SETTEXT, 2, (LPARAM)szTemp);



		// 임시로 http://wimy.com
		sprintf(szTemp, "http://wimy.com");
		SendMessage(m_hStatus, SB_SETTEXT, 3, (LPARAM)szTemp);

		// 로딩시간
		sprintf(szTemp, "%.3fsec", (float)(m_dwLoadingTime / 1000.0));
		SendMessage(m_hStatus, SB_SETTEXT, 4, (LPARAM)szTemp);

		// 파일명
		char szFilename[MAX_PATH], szFileExt[MAX_PATH];
		_splitpath(m_strCurrentFilename.c_str(), NULL, NULL, szFilename, szFileExt);

		sprintf(szTemp, "%s%s", szFilename, szFileExt);
		SendMessage(m_hStatus, SB_SETTEXT, 5, (LPARAM)szTemp);

	}

	if ( m_strCurrentFilename.empty() ) return;


}

void ZMain::SetTitle()
{
	char szTemp[MAX_PATH+256];
	if ( m_strCurrentFilename.empty() )
	{
		sprintf(szTemp, "ZViewer v%s", g_strVersion.c_str());
	}
	else
	{
		sprintf(szTemp, "ZViewer v%s - %s", g_strVersion.c_str(), m_strCurrentFilename.c_str());
	}
	SetWindowText(m_hMainDlg, szTemp);
}

void ZMain::LoadCurrent()
{
	static bool bFirst = true;

	if ( bFirst )
	{
		bFirst = false;

		ZCacheImage::GetInstance().SetImageVector(m_vFile);
		ZCacheImage::GetInstance().StartThread();
	}

	DWORD start = GetTickCount();
	
	if ( ZCacheImage::GetInstance().hasCachedData(m_strCurrentFilename, m_iCurretFileIndex) )
	{
		{
			ZCacheImage::GetInstance().getCachedData(m_strCurrentFilename, m_currentImage);
		}
		OutputDebugString("Cache Hit!!!!!!!!!!!!!\r\n");

		ZCacheImage::GetInstance().LogCacheHit();
	}
	else
	{
		// 캐시에서 찾을 수 없으면 지금 읽어들이고, 캐시에 추가한다.

		bool bLoadOK = false;

		for ( int i=0; i<10; ++i)
		{
			bLoadOK = m_currentImage.LoadFromFile(m_strCurrentFilename);
			if ( bLoadOK || i >= 5) break;

			OutputDebugString("Direct Load failed. sleep");
			Sleep(100);
		}

		if ( bLoadOK == false )
		{
			_ASSERTE(!"Can't load image");

			string strErrorFilename = m_strProgramFolder;
			strErrorFilename += "LoadError.png";
			if ( !m_currentImage.LoadFromFile(strErrorFilename) )
			{
				// 에러 때 표시하는 파일을 읽어들이지 못 했으면
				MessageBox(m_hMainDlg, "Can't load Error Image file.", "ZViewer", MB_OK);
			}
		}
		else
		{
			if ( !ZCacheImage::GetInstance().hasCachedData(m_strCurrentFilename, m_iCurretFileIndex))
			{
				ZCacheImage::GetInstance().AddCacheData(m_strCurrentFilename, m_currentImage);
			}
			OutputDebugString("Cache miss. Add to cache.");
			ZCacheImage::GetInstance().LogCacheMiss();
		}


	}
	m_dwLoadingTime = GetTickCount() - start;
	SetTitle();	// 파일명을 윈도우 타이틀바에 적는다.
	SetStatusBarText();

	m_iShowingX = 0;
	m_iShowingY = 0;

	if ( m_option.m_bRightTopFirstDraw )	// 우측 상단부터 시작해야하면
	{
		RECT rt;
		GetClientRect(m_hMainDlg, &rt);

		if ( m_currentImage.GetWidth() > rt.right )
		{
			m_iShowingX = m_currentImage.GetWidth() - rt.right;
		}
	}

}

void ZMain::OnDrag(int x, int y)
{
	if ( m_option.m_bBigToSmallStretchImage )
	{
		/// 큰 그림을 화면에 맞게 스트레칭할 때는 드래그는 하지 않아도 된다.
		return;
	}

	RECT rt;
	GetClientRect(m_hMainDlg, &rt);

	if ( m_option.m_bFullScreen == false ) rt.bottom -= STATUSBAR_HEIGHT;

	bool bDraw = false;
	if ( (m_iShowingX + x) >= 0 ) 
	{
		if ( m_iShowingX + x + rt.right >= m_currentImage.GetWidth())
		{
			x = m_currentImage.GetWidth() - rt.right - m_iShowingX - 1;
		}

		bDraw = true;
		m_iShowingX += x;
	}
	else
	{
		m_iShowingX = 0;
	}

	if ( ( m_iShowingY + y )  >= 0 ) 
	{
		if ( m_iShowingY + y + rt.bottom >= m_currentImage.GetHeight())
		{
			y = m_currentImage.GetHeight() - rt.bottom - m_iShowingY - 1;
		}
		bDraw = true;
		m_iShowingY += y;
	}
	else
	{
		m_iShowingY = 0;
	}

	if ( bDraw ) Draw(false);
}

void ZMain::ShellTrayShow()
{
	// 작업 표시줄을 보이게 해준다.
	HWND h = FindWindow("Shell_TrayWnd", "");

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_SHOW);
	}
}

void ZMain::ShellTrayHide()
{
	// 작업 표시줄을 보이게 해준다.
	HWND h = FindWindow("Shell_TrayWnd", "");

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_HIDE);
	}
}

void ZMain::OnFocusLose()
{
	OutputDebugString("OnFocusLose()\n");

	ShellTrayShow();
	/*
//	if ( m_bOpeningFileDialog )
	{
		m_bOpeningFileDialog = false;
	}
//	else
	{
	}
	*/
}

void ZMain::OnFocusGet()
{
	OutputDebugString("OnFocusGet()\n");
	if ( m_option.m_bFullScreen )
	{
		OutputDebugString("OnFocusGet() at fullscreen\n");
		SetWindowPos(m_hMainDlg, HWND_TOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		MoveWindow(m_hMainDlg, 0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), TRUE);
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		ShellTrayHide();
	}
}

void ZMain::StartBannerDownload()
{
	DWORD dwThreadID;
	HANDLE hThread = CreateThread(0, 0, BannerThreadFunc, this, 0, &dwThreadID);

	if ( hThread == INVALID_HANDLE_VALUE )
	{
		_ASSERTE(!"Can't make thread");
	}
	CloseHandle(hThread);
}

DWORD WINAPI ZMain::BannerThreadFunc(LPVOID p)
{
	/*
	fipInternetIO i;
	if ( i.downloadFile("http://www.wimy.com/images/zviewerbanner.jpg") )
	{
		fipImage k;
		k.loadFromHandle(&i, (fi_handle)&i, 0);
		if ( k.save((ZMain::GetInstance().m_strProgramFolder + "\\banner.jpg").c_str(), JPEG_QUALITYSUPERB) == FALSE )
		{
			_ASSERTE(!"Can't banner save");
		}
	}
	else
	{
//		_ASSERTE(!"Can't download");
	}
	*/

	return 0;
}

void ZMain::Undo()
{
	if ( m_history.CanUndo() )
	{
		int iLast = m_history.Undo();

		// 혹시나 범위를 벗어나면 안됨
		if ( iLast < 0 || iLast >= (int)m_vFile.size() )
		{
			_ASSERTE(!"Over range...");
			return;
		}

		if ( m_vFile.empty() ) return;

		m_iCurretFileIndex = iLast;
		m_strCurrentFilename = m_vFile[m_iCurretFileIndex];
		LoadCurrent();
	}
}

void ZMain::Redo()
{
	if ( m_history.CanRedo() )
	{
		m_history.Redo();
	}
}

void ZMain::OnRightTopFirstDraw()
{
	m_option.m_bRightTopFirstDraw = !m_option.m_bRightTopFirstDraw;

	CheckMenuItem(m_hMainMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, m_option.m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, m_option.m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);
}

void ZMain::ShowFileExtDlg()
{
	ZFileExtDlg::GetInstance().ShowDlg();
}

void ZMain::DeleteThisFile()
{
	/// TODO : LoadString() 을 이용해서 리소스에서 불러오도록 변경해야함.
	int iRet = MessageBox(m_hMainDlg, ZResourceManager::GetInstance().GetString(IDS_DELETE_THIS_FILE).c_str(), "ZViewer", MB_YESNO);

	if ( iRet == IDYES )
	{
		if ( 0 == unlink(m_strCurrentFilename.c_str()) )
		{
			// 현재 파일이 마지막 파일인가?
			if ( m_vFile.size() <= 1 )
			{
				m_iCurretFileIndex = 0;
				m_strCurrentFilename = "";
				
				m_vFile.clear();

				SetTitle();
				SetStatusBarText();
				Draw(true);


			}
			else
			{
				if ( ((int)m_vFile.size() - 1) > m_iCurretFileIndex )	// 다음 그림이 있다.
				{
					// for 문을 돌면서 지울 것을 찾아놓는다.
					vector<std::string>::iterator it, endit = m_vFile.end();
					int i = 0;
					bool bFound = false;
					for ( it = m_vFile.begin(); it != endit; ++it)
					{
						if ( i == m_iCurretFileIndex)
						{
							bFound = true;
							break;
						}
						++i;
					}
					if ( !bFound )
					{
						_ASSERTE(!"Can't find the file");
						return;
					}
					NextImage();

					m_vFile.erase(it);

					// 지웠으므로 현재 인덱스를 1줄인다.
					m_iCurretFileIndex -= 1;

					SetTitle();
					SetStatusBarText();

					Draw(true);
				}
				else
				{
					// 사이즈가 1이 아니고, 다음것이 없었으므로 이전 것은 있다.
					// for 문을 돌면서 지울 것을 찾아놓는다.
					vector<std::string>::iterator it, endit = m_vFile.end();
					int i = 0;

					bool bFound = false;
					for ( it = m_vFile.begin(); it != endit; ++it)
					{
						if ( i == m_iCurretFileIndex)
						{
							bFound = true;
							break;
						}
						++i;
					}

					if ( !bFound )
					{
						_ASSERTE(!"Can't find the file");
						return;
					}
					PrevImage();

					m_vFile.erase(it);
					SetTitle();
					SetStatusBarText();
					Draw(true);
				}

				
			}
		}
		else
		{
			MessageBox(m_hMainDlg, "Can't delete this file!", "ZViewer", MB_OK);
		}
	}
}

void ZMain::LoadLanguage()
{
	ZINIOption opt;
	opt.LoadFromFile("language/korean.ini");

	// 메뉴를 세팅한다.
	ModifyMenu(m_hPopupMenu, ID_DELETETHISFILE, MF_BYCOMMAND, ID_DELETETHISFILE, opt.GetValue("MenuFileOpen").c_str());
	//SetDlgItemText(m_hPopupMenu, ID_DELETETHISFILE, opt.GetValue("MenuDeleteThisFile").c_str());
}