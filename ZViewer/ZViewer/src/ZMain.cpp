/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZMain.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "StdAfx.h"
#include ".\zmain.h"
#include "src/ZFileExtDlg.h"
#include "src/ZResourceManager.h"
#include "../commonSrc/DesktopWallPaper.h"
#include "src/SelectToFolderDlg.h"
#include "../commonSrc/SaveAs.h"
#include "src/ZOption.h"
#include "MessageManager.h"

#include <ShlObj.h>
#include <cstdio>
#include <strsafe.h>

#include "resource.h"

using namespace std;

ZMain & ZMain::GetInstance()
{
	static ZMain aInstance;
	return aInstance;
}


ZMain::ZMain(void)
:	m_hMainDlg(NULL)
,	m_sortOrder(eFileSortOrder_FILENAME)
,	m_osKind(eOSKind_UNKNOWN)
{
	m_hBufferDC = NULL;
	m_bLastCacheHit = false;
	SetProgramFolder();
}

ZMain::~ZMain(void)
{
	if ( NULL != m_hBufferDC )
	{
		DebugPrintf(TEXT("Before delete bufferDC"));
		BOOL bRet = DeleteDC(m_hBufferDC);
		DebugPrintf(TEXT("after delete bufferDC"));
	
		_ASSERTE(bRet);

		m_hBufferDC = NULL;
	}
}


/// 버퍼로 쓰이는 DC 를 릴리즈한다.
void ZMain::_releaseBufferDC()
{
	if ( m_hBufferDC != NULL )
	{
		BOOL bRet = DeleteDC(m_hBufferDC);
		_ASSERTE(bRet);
		m_hBufferDC = NULL;
	}
}

/// Timer 를 받았을 때
void ZMain::onTimer()
{
	if ( ZCacheImage::GetInstance().isCachingNow() )
	{
		//DebugPrintf("now cache status...");
	}
	showCacheStatus();
}


/// ZViewer 전용 메시지 박스
int ZMain::MessageBox(const TCHAR * msg, UINT button)
{
	return ::MessageBox(m_hMainDlg, GetMessage(msg), TEXT("ZViewer"), button);
}

int ZMain::GetLogCacheHitRate() const
{
	return ZCacheImage::GetInstance().GetLogCacheHitRate();
}

long ZMain::GetCachedKByte() const
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
	ofn.lpstrFilter = TEXT("ImageFiles(jpg,jpeg2000,gif,png,bmp,psd,tga,tif,ico)\0*.jpg;*.jpeg;*.jp2;*.j2k;*.gif;*.png;*.bmp;*.psd,*.tga;*.tif;*.ico\0All(*.*)\0*.*\0");
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
		OpenFile(ofn.lpstrFile);
	}
}

/// 현재 파일을 다른 형식으로 저장하는 파일 다이얼로그를 연다.
void ZMain::SaveFileDialog()
{
	CSaveAs::getInstance().setParentHWND(m_hMainDlg);
	CSaveAs::getInstance().setDefaultSaveFilename(m_strCurrentFolder, m_strCurrentFilename);
	
	if ( CSaveAs::getInstance().showDialog() )
	{
		tstring strSaveFilename = CSaveAs::getInstance().getSaveFileName();

		if ( false == m_currentImage.SaveToFile(strSaveFilename, 0) )
		{
			MessageBox(TEXT("CANNOT_SAVE_AS_FILE"));
		}
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

	/// 여러 컨트롤들을 초기화시켜준다.
	_InitControls();
}


/// 여러 컨트롤들을 초기화시켜준다.
void ZMain::_InitControls()
{
	CheckMenuItem(m_hMainMenu, ID_OPTION_VIEWLOOP, ZOption::GetInstance().IsLoopImages() ? MF_CHECKED : MF_UNCHECKED);
}


void ZMain::Draw(bool bEraseBg)
{
	if ( m_currentImage.IsValid() == FALSE ) return;
	if ( m_hMainDlg == NULL ) return;

	RECT currentScreenRect;
	getCurrentScreenRect(currentScreenRect);

	HDC mainDC = GetDC(m_hMainDlg);

	if ( m_vFile.size() <= 0 )
	{
		SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
		Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);

		ReleaseDC(m_hMainDlg, mainDC);
		return;
	}

	// 그림이 그려지기 시작할 화면의 위치
	int iDrawX = 0;			
	int iDrawY = 0;

	// 그림이 그려지기 시작하는 그림의 위치
	int iDrawPartX = 0;
	int iDrawPartY = 0;

	bool bNeedClipping = false;

	const WORD currentImageWidth = m_currentImage.GetWidth();
	const WORD currentImageHeight = m_currentImage.GetHeight();

	if ( currentImageWidth <= currentScreenRect.right )
	{
		// 가로 길이가 화면보다 작을 때는 중앙에 오게
		iDrawX = (currentScreenRect.right/2) - (currentImageWidth/2);
	}
	else
	{
		// 가로 길이가 화면보다 크면 클리핑
		iDrawPartX = m_iShowingX;
		bNeedClipping = true;
	}

	if ( currentImageHeight <= currentScreenRect.bottom )
	{
		// 세로 길이가 화면보다 작을 때는 중앙에 오게
		iDrawY = (currentScreenRect.bottom/2) - (currentImageHeight/2);
	}
	else
	{
		// 세로 길이가 화면보다 크면 클리핑
		iDrawPartY = m_iShowingY;
		bNeedClipping = true;
	}

#pragma message("TODO : 화면을 지워야할 때 Rectangle 로 지우지 않고, 화면 크기만큼 memoryDC 를 잡은 후 검게 칠하고, 그림을 거기 그리고, 그 memDC 를 그리자")
#pragma message("TODO : memDC 가 다시 세팅되어야하는지를 flag 로 관리해서 화면을 스크롤할 때, 좀더 부드럽게 하자")

	/// 그림이 화면보다 큰가 확인
	if ( bNeedClipping )
	{
		DebugPrintf(TEXT("!!!!!!!!! clipping on draw......"));

		/// 그림이 화면보다 크면...

		if ( ZOption::GetInstance().IsBigToSmallStretchImage() )
		{
			/// 큰 그림을 축소시켜서 그린다.

			RECT toRect;
			SetRect(&toRect, 0, 0, currentImageWidth, currentImageHeight);
			toRect = GetResizedRectForBigToSmall(currentScreenRect, toRect);

			ZImage stretchedImage(m_currentImage);

			/*
			if ( stretchedImage.GetBPP() == 8 )
			{
				stretchedImage.ConvertTo32Bit();
			}
			*/

			stretchedImage.Resize((WORD)toRect.right, (WORD)toRect.bottom);
			
			iDrawX = (currentScreenRect.right - toRect.right) / 2;
			iDrawY = (currentScreenRect.bottom - toRect.bottom) / 2;

			if ( bEraseBg )	// 배경을 지워야 하면 지운다. 새로운 그림을 그리기 직전에 그려야 깜빡임이 적다.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);
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
			/// 버퍼의 DC 가 NULL 이란 것은 이전 캐시가 없거나, 무효화되었다는 것이다.
			if ( m_hBufferDC == NULL )
			{
				DebugPrintf(TEXT("!!!!! no DC cache clipping..."));
				/// 큰 그림을 스크롤이 가능하게 클리핑해서 그린다.
				m_hBufferDC = CreateCompatibleDC(mainDC);

				HBITMAP hbmScreen = CreateCompatibleBitmap(mainDC, currentImageWidth, currentImageHeight); 

				SelectObject(m_hBufferDC, hbmScreen);

				// 메모리에 전체 그림을 그린다.
				if ( m_currentImage.isTransparent() )
				{
					DebugPrintf(TEXT("Drawing transparent image..."));
					fipWinImage tempImage;
					tempImage = m_currentImage.getFipImage();

					tempImage.rescale(currentImageWidth, currentImageHeight, FILTER_BOX);
					RECT rt;
					rt.left = 0;
					rt.top = 0;
					rt.right = currentImageWidth;
					rt.bottom = currentImageHeight;

					tempImage.drawEx(m_hBufferDC, rt, TRUE);
				}
				else
				{
					int r = StretchDIBits(m_hBufferDC,
						0, 0, 
						currentImageWidth, currentImageHeight,
						0, 0,
						currentImageWidth, currentImageHeight,
						m_currentImage.GetData(),
						m_currentImage.GetBitmapInfo(),
						DIB_RGB_COLORS, SRCCOPY);
				}
				DeleteObject(hbmScreen);
			}
			else
			{
				DebugPrintf(TEXT("DC cache clipping"));
			}

			if ( bEraseBg )	// 배경을 지워야 하면 지운다. 새로운 그림을 그리기 직전에 그려야 깜빡임이 적다.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);
			}

			// 메모리것을 화면에 그린다.
			BOOL b = BitBlt(mainDC, 
				iDrawX, iDrawY,			// 그릴 화면의 x, y 좌표. 화면에 꽉 찰 때는 0, 0 이어야한다.
				m_currentImage.GetWidth(), currentScreenRect.bottom,		// 그려질 화면의 가로, 세로 길이. 
				m_hBufferDC, 
				iDrawPartX, iDrawPartY,			// 그려질 이미지 원본의 시작 x,y 좌표
				SRCCOPY);

			//DebugPrintf("rt.bottom : %d, PartX : %d, iDrawPartY : %d", currentScreenRect.bottom, iDrawPartX, iDrawPartY);

//				DeleteDC(memDC);
		}
	}
	else	/// image is smaller than screen
	{
		if ( currentImageWidth < currentScreenRect.right && currentImageHeight < currentScreenRect.bottom 
			&& ZOption::GetInstance().IsSmallToBigStretchImage() ) ///< if option is on
		{
			DebugPrintf(TEXT("!!!!!!!!!!!! stretching on draw..."));

			/// 작은 그림을 화면에 맞게 확대해서 그린다.
			RECT originalImageRect;
			SetRect(&originalImageRect, 0, 0, currentImageWidth, currentImageHeight);
			RECT toRect = GetResizedRectForSmallToBig(currentScreenRect, originalImageRect);

			ZImage stretchedImage(m_currentImage);

			/*
			if ( stretchedImage.GetBPP() == 8 )
			{
				stretchedImage.ConvertTo32Bit();
			}
			*/

			stretchedImage.Resize((WORD)toRect.right, (WORD)toRect.bottom);
			
			_ASSERTE(toRect.right == stretchedImage.GetWidth());
			_ASSERTE(toRect.bottom == stretchedImage.GetHeight());

			int iDrawPointX = (currentScreenRect.right - toRect.right) / 2;
			int iDrawPointY = (currentScreenRect.bottom - toRect.bottom) / 2;
			
			if ( bEraseBg )	// 배경을 지워야 하면 지운다. 새로운 그림을 그리기 직전에 그려야 깜빡임이 적다.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);
			}

			if ( stretchedImage.isTransparent() )
			{
				DebugPrintf(TEXT("drawing trans....."));
//				stretchedImage.Resize(currentImageWidth, currentImageHeight);

				fipWinImage temp;
				temp = stretchedImage.getFipImage();

				RECT rt;
				rt.left = iDrawPointX;
				rt.top = iDrawPointY;
				rt.right = toRect.right;
				rt.bottom = toRect.bottom;

				temp.drawEx(mainDC, rt, TRUE);
			}
			else
			{
				int r = StretchDIBits(mainDC,
					iDrawPointX, iDrawPointY, 
					toRect.right, toRect.bottom,
					0, 0,
					toRect.right, toRect.bottom,
					stretchedImage.GetData(),
					stretchedImage.GetBitmapInfo(),
					DIB_RGB_COLORS, SRCCOPY);
			}

		}
		else
		{
			if ( bEraseBg )	// 배경을 지워야 하면 지운다. 새로운 그림을 그리기 직전에 그려야 깜빡임이 적다.
			{
				SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
				Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);
			}

			if ( m_currentImage.isTransparent() )
			{
				DebugPrintf(TEXT("Drawing transparent image..."));
				fipWinImage tempImage;
				tempImage = m_currentImage.getFipImage();
				
				/// rescale 을 하면, 투명한 배경에 체크무늬가 그려진다... 왜 이럴까;;;
				tempImage.rescale(currentImageWidth, currentImageHeight, FILTER_BOX);
				RECT rt;
				rt.left = iDrawX;
				rt.top = iDrawY;
				rt.right = rt.left + currentImageWidth;
				rt.bottom = rt.top + currentImageHeight;
				tempImage.drawEx(mainDC, rt, TRUE);
			}
			else
			{
				/// 작은 그림을 화면 가운데에 그린다.
				int r = StretchDIBits(mainDC,
					iDrawX, iDrawY, 
					currentImageWidth, currentImageHeight,
					0, 0,
					currentImageWidth, currentImageHeight,
					m_currentImage.GetData(),
					m_currentImage.GetBitmapInfo(),
					DIB_RGB_COLORS, SRCCOPY);
			}

		}
	}
	ReleaseDC(m_hMainDlg, mainDC);

	// 마우스 커서 모양
	if ( currentImageWidth > currentScreenRect.right ||
		currentImageHeight > currentScreenRect.bottom
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

	if ( false == ZOption::GetInstance().IsFullScreen() )
	{
		PostMessage(m_hStatusBar, WM_PAINT, 0, 0);
	}
}

void ZMain::FindFile(const TCHAR * path, std::vector< FileData > & foundStorage, bool bFindRecursive)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;

	TCHAR fname[_MAX_FNAME] = { 0 };
	BOOL bResult=TRUE;
	TCHAR drive[_MAX_DRIVE] = { 0 };
	TCHAR dir[_MAX_DIR] = { 0 };
	TCHAR newpath[MAX_PATH] = { 0 };

	hSrch=FindFirstFile(path,&wfd);
	while (bResult)
	{
		_tsplitpath(path,drive,dir,NULL,NULL);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0]!='.' && bFindRecursive == true)
			{
				StringCchPrintf(newpath, sizeof(newpath), TEXT("%s%s%s\\*.*"),drive,dir,wfd.cFileName);
				FindFile(newpath, foundStorage, bFindRecursive);
			}
		}
		else
		{
			StringCchPrintf(fname, sizeof(fname), TEXT("%s%s%s"),drive,dir,wfd.cFileName);

			if ( ZImage::IsValidImageFileExt(wfd.cFileName) )
			{
				FileData aData;
				aData.m_timeModified = wfd.ftLastWriteTime;
				aData.m_strFileName = fname;
				aData.m_nFileSize = wfd.nFileSizeLow;
				foundStorage.push_back(aData);
			}
		}
		bResult=FindNextFile(hSrch,&wfd);
	}
	FindClose(hSrch);
}

void ZMain::FindFolders(const TCHAR *path, std::vector<tstring> & foundStorage, bool bFindRecursive)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	//memset(&wfd, 0, sizeof(wfd));
	TCHAR fname[MAX_PATH] = { 0 };
	BOOL bResult=TRUE;
	TCHAR drive[_MAX_DRIVE] = { 0 };
	TCHAR dir[MAX_PATH] = { 0 };
	TCHAR newpath[MAX_PATH] = { 0 };

	hSrch=FindFirstFile(path,&wfd);
	while (bResult)
	{
		_tsplitpath(path,drive,dir,NULL,NULL);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0]!='.' )
			{
				StringCchPrintf(fname, sizeof(fname), TEXT("%s%s%s"),drive,dir,wfd.cFileName);

				foundStorage.push_back(fname);

				if ( bFindRecursive == true )
				{
					StringCchPrintf(newpath, sizeof(newpath), TEXT("%s%s%s\\*.*"),drive,dir,wfd.cFileName);
					FindFolders(newpath, foundStorage, bFindRecursive);
				}
			}
		}
		bResult=FindNextFile(hSrch,&wfd);
	}
	FindClose(hSrch);
}


void ZMain::RescanFolder()
{
	tstring strToFindFolder = m_strCurrentFolder;

	strToFindFolder += TEXT("*.*");

	_GetFileListAndSort(strToFindFolder, m_vFile);

	// Cache Thread 에 전달한다.
	ZCacheImage::GetInstance().SetImageVector(m_vFile);

	if ( m_strCurrentFilename.empty() && !m_vFile.empty())
	{
		m_strCurrentFilename = m_vFile[0].m_strFileName;
		m_iCurretFileIndex = 0;

		LoadCurrent();
	}
}

void ZMain::SetProgramFolder()
{
	m_strProgramFolder = GetProgramFolder();
}

bool ZMain::GetNeighborFolders(std::vector < tstring > & vFolders)
{
	tstring strParentFolder;

	// 현재 폴더의 상위 폴더를 검색한다.
	{
		// 현재 폴더에서 오른쪽부터 2번째의 \ 를 얻는다.
		size_t pos = m_strCurrentFolder.find_last_of(TEXT("\\"));

		if ( pos == m_strCurrentFolder.npos )
		{
			MessageBox(TEXT("CANNOT_FIND_PARENT_DIRECTORY"));
			return false;
		}

		tstring strParentFolder = m_strCurrentFolder.substr(0, pos);

		pos = strParentFolder.find_last_of(TEXT("\\"));

		if ( pos == strParentFolder.npos )
		{
			MessageBox(TEXT("CANNOT_FIND_PARENT_DIRECTORY"));
			return false;
		}

		strParentFolder = strParentFolder.substr(0, pos);

		strParentFolder += TEXT("\\*.*");
		// 상위 폴더의 하위 폴더들을 얻는다.
		FindFolders(strParentFolder.c_str(), vFolders, false);

		if ( vFolders.size() <= 0 )
		{
			_ASSERTE(vFolders.size() > 0 );
			return false;
		}
	}

	// 상위 폴더의 하위 폴더들을 정렬한다.
	if ( m_osKind == eOSKind_XP )
	{
		sort(vFolders.begin(), vFolders.end(), CStringCompareIgnoreCase_LengthFirst());
	}
	else
	{
		sort(vFolders.begin(), vFolders.end(), CStringCompareIgnoreCase());
	}

	return true;
}

void ZMain::NextFolder()
{
	std::vector < tstring > vFolders;

	if ( !GetNeighborFolders(vFolders) ) return;

	// 상위 폴더의 하위 폴더 중 현재 폴더의 다음 폴더가 있으면 다음 폴더를 검색한 후, 처음 이미지를 연다.
	{
		int iFoundIndex = -1;
		// 현재 폴더의 index 를 찿는다.
		for (unsigned int i=0; i<vFolders.size(); ++i)
		{
			if ( vFolders[i] + TEXT("\\") == m_strCurrentFolder )
			{
				iFoundIndex = i;
				break;
			}
		}

		_ASSERTE(iFoundIndex != -1);

		if ( (iFoundIndex + 1) >= (int)vFolders.size() )
		{
			// 마지막 폴더이다.
			MessageBox(TEXT("LAST_DIRECTORY"));
			return;
		}
		else
		{
			OpenFolder(vFolders[iFoundIndex+1]);
		}
	}
}


void ZMain::PrevFolder()
{
	std::vector < tstring > vFolders;

	if ( !GetNeighborFolders(vFolders) ) return;

	// 상위 폴더의 하위 폴더 중 현재 폴더의 다음 폴더가 있으면 다음 폴더를 검색한 후, 처음 이미지를 연다.
	{
		int iFoundIndex = -1;
		// 현재 폴더의 index 를 찿는다.
		for (unsigned int i=0; i<vFolders.size(); ++i)
		{
			if ( vFolders[i] + TEXT("\\") == m_strCurrentFolder )
			{
				iFoundIndex = i;
				break;
			}
		}

		_ASSERTE(iFoundIndex != -1);

		if ( (iFoundIndex-1 < 0 ) )
		{
			// 마지막 폴더이다.
			MessageBox(TEXT("FIRST_FOLDER"));
			return;
		}
		else
		{
			OpenFolder(vFolders[iFoundIndex-1]);
		}
	}
}

void ZMain::_GetFileListAndSort(const tstring & strFolderPathAndWildCard, FileListVector & vFileList)
{
	vFileList.resize(0);
	FindFile(strFolderPathAndWildCard.c_str(), vFileList, false);

	// 얻은 파일을 정렬한다.
	switch ( m_sortOrder )
	{
	case eFileSortOrder_FILENAME:

		if ( m_osKind == eOSKind_XP )
		{
			sort(vFileList.begin(), vFileList.end(), CFileDataSort_OnlyFilenameCompare_XP());
		}
		else
		{
			sort(vFileList.begin(), vFileList.end(), CFileDataSort_OnlyFilenameCompare());
		}
		break;

	case eFileSortOrder_FILESIZE:
		sort(vFileList.begin(), vFileList.end(), CFileDataSort_FileSize());
		break;

	case eFileSortOrder_LAST_MODIFY_TIME:
		sort(vFileList.begin(), vFileList.end(), CFileDataSort_LastModifiedTime());
		break;

	default:
		_ASSERTE(false);

	}
}

void ZMain::OpenFolder(const tstring & strFolder)
{
	// 특정 폴더의 하위 파일들을 검색해서 정렬 후 첫번째 파일을 연다.

	tstring strTemp = strFolder;
	strTemp += TEXT("\\*.*");

	vector < FileData > vFiles;

	_GetFileListAndSort(strTemp, vFiles);

	if ( vFiles.size() == 0 )
	{
		TCHAR msg[256];
		StringCchPrintf(msg, sizeof(msg), GetMessage(TEXT("THIS_DIRECTORY_HAS_NO_IMAGE")), strFolder.c_str());
		::MessageBox(m_hMainDlg, msg, TEXT("ZViewer"), MB_OK);
		return;
	}
	else
	{
		OpenFile(vFiles[0].m_strFileName);
	}
}


void ZMain::OpenFile(const tstring & strFilename)
{
	m_strCurrentFolder = GetFolderNameFromFullFileName(strFilename);
	RescanFolder();

	// 스캔한 파일 중 현재 파일을 찾는다.
	std::vector< FileData >::const_iterator it, endit = m_vFile.end();

	for ( it = m_vFile.begin(); it != endit; ++it)
	{
		if ( it->m_strFileName == strFilename )
		{
			break;
		}
	}

	if ( m_vFile.empty() )
	{
		_ASSERTE(!"size of scanned file list is 0. Check folder name or path!!");

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


bool ZMain::MoveIndex(int iIndex)
{
	if ( m_vFile.empty() ) return false;

	if ( iIndex < 0 )
	{
		if ( ZOption::GetInstance().IsLoopImages() )
		{
			iIndex = (int)(m_vFile.size() - ((-1*iIndex) % m_vFile.size()));
		}
		else
		{
			iIndex = 0;
		}
	}

	if ( iIndex >= (int)m_vFile.size() )
	{
		if ( ZOption::GetInstance().IsLoopImages() )
		{
			iIndex = (int)( iIndex % m_vFile.size() );
		}
		else
		{
			iIndex = (int)m_vFile.size() - 1;
		}
	}

	if ( m_iCurretFileIndex == iIndex ) return false;

	m_iCurretFileIndex = iIndex;
	m_strCurrentFilename = m_vFile[m_iCurretFileIndex].m_strFileName;
	LoadCurrent();

	return true;
}

bool ZMain::FirstImage()
{
	// 현재의 위치를 History 에 저장해놓는다.
	m_history.push_lastImageIndex(m_iCurretFileIndex);
	return MoveIndex(0);
}

bool ZMain::LastImage()
{
	// 현재의 위치를 History 에 저장해놓는다.
	m_history.push_lastImageIndex(m_iCurretFileIndex);
	return MoveIndex((int)m_vFile.size() - 1);
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
	ShowWindow(m_hStatusBar, SW_HIDE);
	SetMenu(m_hMainDlg, NULL);
	SetWindowLong(m_hMainDlg, GWL_STYLE, style);
}

void ZMain::FormShow()
{
	LONG style = GetWindowLong(m_hMainDlg, GWL_STYLE);
	style |= WS_CAPTION;
	style |= WS_THICKFRAME;
	ShowWindow(m_hStatusBar, SW_SHOW);
	SetMenu(m_hMainDlg, m_hMainMenu);
	SetWindowLong(m_hMainDlg, GWL_STYLE, style);
}

/// Cache status 를 상태 표시줄에 표시한다.
void ZMain::showCacheStatus()
{
	// 해상도 정보
	//_snprintf(szTemp, sizeof(szTemp), "");
	//SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)szTemp);

	static bool bLastActionIsCache = false;

	bool bNowActionIsCache = ZCacheImage::GetInstance().isCachingNow();

	if ( bLastActionIsCache != bNowActionIsCache )
	{
		bLastActionIsCache = bNowActionIsCache;
		
		if ( bNowActionIsCache )
		{
			PostMessage(m_hStatusBar, SB_SETTEXT, 5, (LPARAM)TEXT("Caching"));
		}
		else
		{
			PostMessage(m_hStatusBar, SB_SETTEXT, 5, (LPARAM)TEXT("Cached"));
		}

		/*
		HDC hDC = ::GetDC(m_hStatusBar);

		enum
		{
			eCacheShowWindowSize = 20
		};
		RECT drawRect;
		drawRect.left  = 503;
		drawRect.right = drawRect.left + eCacheShowWindowSize;
		drawRect.top = 3;
		drawRect.bottom = 19;

		static int percentage = 0;

		if ( ZCacheImage::GetInstance().isCachingNow() )
		{
			++percentage;

			if ( percentage > 100 ) percentage = 0;

			drawRect.right = drawRect.left + (eCacheShowWindowSize * ( percentage / 100.0 ));
		}

		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = RGB(100, 200, 200);
		logBrush.lbHatch = 0;
		HBRUSH hBrush = CreateBrushIndirect(&logBrush);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
		Rectangle(hDC, drawRect.left, drawRect.top, drawRect.right, drawRect.bottom);
		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);
		*/
	}
}


void ZMain::ToggleFullScreen()
{
	static RECT lastPosition;

	if ( ZOption::GetInstance().IsFullScreen() )	// 현재 풀스크린이면 원래 화면으로 돌아간다.
	{
		ZOption::GetInstance().SetFullScreen(!ZOption::GetInstance().IsFullScreen());

		ShellTrayShow();	// 숨겨졌던 작업 표시줄을 보여준다.

		FormShow();	// 메뉴, 상태 표시줄등을 보여준다.

		SetWindowPos(m_hMainDlg, HWND_TOP, lastPosition.left, lastPosition.top, lastPosition.right - lastPosition.left, lastPosition.bottom - lastPosition.top, SWP_SHOWWINDOW);
	}
	else	// 현재 풀스크린이 아니면 풀스크린으로 만든다.
	{
		ZOption::GetInstance().SetFullScreen(!ZOption::GetInstance().IsFullScreen());
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

	CheckMenuItem(m_hMainMenu, ID_VIEW_FULLSCREEN, ZOption::GetInstance().IsFullScreen() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_VIEW_FULLSCREEN, ZOption::GetInstance().IsFullScreen() ? MF_CHECKED : MF_UNCHECKED);

	m_iShowingX = 0;
	m_iShowingY = 0;
}


void ZMain::ToggleSmallToScreenStretch()
{
	ZOption::GetInstance().ToggleSmallToBigStretchImage();

	CheckMenuItem(m_hMainMenu, ID_VIEW_SMALLTOSCREENSTRETCH, ZOption::GetInstance().IsSmallToBigStretchImage() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_SMALLTOSCREENSTRETCH, ZOption::GetInstance().IsSmallToBigStretchImage() ? MF_CHECKED : MF_UNCHECKED);

	ZCacheImage::GetInstance().clearCache();
	ZCacheImage::GetInstance().setCacheEvent();

	LoadCurrent();
	Draw();

}

void ZMain::ToggleBigToScreenStretch()
{
	ZOption::GetInstance().ToggleBigToSmallStretchImage();

	CheckMenuItem(m_hMainMenu, ID_VIEW_BIGTOSCREENSTRETCH , ZOption::GetInstance().IsBigToSmallStretchImage() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_BIGTOSCREENSTRETCH, ZOption::GetInstance().IsBigToSmallStretchImage() ? MF_CHECKED : MF_UNCHECKED);

	ZCacheImage::GetInstance().clearCache();
	ZCacheImage::GetInstance().setCacheEvent();

	LoadCurrent();
	Draw();

}

void ZMain::ToggleLoopImage()
{
	ZOption::GetInstance().ToggleLoopImages();

	CheckMenuItem(m_hMainMenu, ID_OPTION_VIEWLOOP, ZOption::GetInstance().IsLoopImages() ? MF_CHECKED : MF_UNCHECKED);
}

void ZMain::SetStatusBarText()
{
	TCHAR szTemp[256];

	if ( m_vFile.size() == 0 || m_strCurrentFilename.empty() ) // 보고 있는 파일이 없으면
	{
		// File Index
		StringCchPrintf(szTemp, sizeof(szTemp), TEXT("No File"));
		SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)szTemp);

		// 해상도 정보
		StringCchPrintf(szTemp, sizeof(szTemp), TEXT(""));
		SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)szTemp);

		// 이미지 사이즈
		StringCchPrintf(szTemp, sizeof(szTemp), TEXT(""));
		SendMessage(m_hStatusBar, SB_SETTEXT, 2, (LPARAM)szTemp);

		// 임시로 http://wimy.com
		StringCchPrintf(szTemp, sizeof(szTemp), TEXT("http://wimy.com"));
		SendMessage(m_hStatusBar, SB_SETTEXT, 3, (LPARAM)szTemp);

		// 로딩시간
		StringCchPrintf(szTemp, sizeof(szTemp), TEXT(""));
		SendMessage(m_hStatusBar, SB_SETTEXT, 4, (LPARAM)szTemp);

		// 파일명
		StringCchPrintf(szTemp, sizeof(szTemp), TEXT("No File"));

		showCacheStatus(); ///< 5

		SendMessage(m_hStatusBar, SB_SETTEXT, 6, (LPARAM)szTemp);
	}
	else
	{
		// File Index
		StringCchPrintf(szTemp, sizeof(szTemp), TEXT("%d/%d"), m_iCurretFileIndex+1, m_vFile.size());
		SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)szTemp);

		// 해상도 정보
		StringCchPrintf(szTemp, sizeof(szTemp), TEXT("%dx%dx%dbpp"), m_currentImage.GetOriginalWidth(), m_currentImage.GetOriginalHeight(), m_currentImage.GetBPP());
		SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)szTemp);

		// image size
		long imageSize = m_vFile[m_iCurretFileIndex].m_nFileSize;

		if ( imageSize > 1024 )
		{
			if ( imageSize/1024 > 1024 )
			{
				StringCchPrintf(szTemp, sizeof(szTemp), TEXT("%.2fMByte"), imageSize/1024/1024.0f);
			}
			else
			{
				StringCchPrintf(szTemp, sizeof(szTemp), TEXT("%dKByte"), imageSize/1024);
			}
		}
		else
		{
			StringCchPrintf(szTemp, sizeof(szTemp), TEXT("%dByte"), imageSize);
		}
		SendMessage(m_hStatusBar, SB_SETTEXT, 2, (LPARAM)szTemp);

		// 임시로 http://wimy.com
		StringCchPrintf(szTemp, sizeof(szTemp), TEXT("http://wimy.com"));
		SendMessage(m_hStatusBar, SB_SETTEXT, 3, (LPARAM)szTemp);

		// 로딩시간
		if ( m_bLastCacheHit )
		{
			StringCchPrintf(szTemp, sizeof(szTemp), TEXT("%.3fsec [C]"), (float)(m_dwLoadingTime / 1000.0));
		}
		else
		{
			StringCchPrintf(szTemp, sizeof(szTemp), TEXT("%.3fsec [N]"), (float)(m_dwLoadingTime / 1000.0));
		}
		SendMessage(m_hStatusBar, SB_SETTEXT, 4, (LPARAM)szTemp);

		// 파일명
		TCHAR szFilename[MAX_PATH], szFileExt[MAX_PATH];
		_tsplitpath(m_strCurrentFilename.c_str(), NULL, NULL, szFilename, szFileExt);

		showCacheStatus(); ///< 5

		StringCchPrintf(szTemp, sizeof(szTemp), TEXT("%s%s"), szFilename, szFileExt);
		SendMessage(m_hStatusBar, SB_SETTEXT, 6, (LPARAM)szTemp);
	}
}

void ZMain::SetTitle()
{
	TCHAR szTemp[MAX_PATH+256];

	if ( m_strCurrentFilename.empty() )	// 현재보고 있는 파일명이 없으면
	{
		StringCchPrintf(szTemp, sizeof(szTemp), TEXT("ZViewer v%s"), g_strVersion.c_str());
	}
	else // 현재보고 있는 파일명이 있으면
	{
		TCHAR szFileName[MAX_PATH] = { 0 };
		TCHAR szFileExt[MAX_PATH] = { 0 };
		_tsplitpath(m_strCurrentFilename.c_str(), NULL, NULL, szFileName, szFileExt);

		StringCchPrintf(szTemp, sizeof(szTemp), TEXT("%s%s - %s"), szFileName, szFileExt, m_strCurrentFilename.c_str());
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

	_releaseBufferDC();

	if ( ZCacheImage::GetInstance().hasCachedData(m_strCurrentFilename, m_iCurretFileIndex) )
	{
		{
			ZCacheImage::GetInstance().getCachedData(m_strCurrentFilename, m_currentImage);
		}

		DebugPrintf(TEXT("Cache Hit!!!!!!!!!!!!!"));

		m_bLastCacheHit = true;

		ZCacheImage::GetInstance().LogCacheHit();
	}
	else
	{
		// 캐시에서 찾을 수 없으면 지금 읽어들이고, 캐시에 추가한다.
		DebugPrintf(TEXT("Can't find in cache. load now..."));

		bool bLoadOK = false;

		for ( unsigned int i=0; i<10; ++i)
		{
			bLoadOK = m_currentImage.LoadFromFile(m_strCurrentFilename);
			if ( bLoadOK || i >= 5) break;

			DebugPrintf(TEXT("Direct Load failed. sleep"));

			Sleep(10);
		}

		if ( bLoadOK == false )
		{
			_ASSERTE(!"Can't load image");

			tstring strErrorFilename = m_strProgramFolder;
			strErrorFilename += TEXT("LoadError.png");
			if ( !m_currentImage.LoadFromFile(strErrorFilename) )
			{
				// 에러 때 표시하는 파일을 읽어들이지 못 했으면
				MessageBox(TEXT("CANNOT_LOAD_ERROR_IMAGE_FILE"));
			}
		}
		else
		{
			DebugPrintf(TEXT("Cache miss."));
			m_bLastCacheHit = false;
			ZCacheImage::GetInstance().LogCacheMiss();
		}
	}
	m_dwLoadingTime = GetTickCount() - start;
	SetTitle();	// 파일명을 윈도우 타이틀바에 적는다.
	SetStatusBarText();

	m_iShowingX = 0;
	m_iShowingY = 0;

	if ( ZOption::GetInstance().m_bRightTopFirstDraw )	// 우측 상단부터 시작해야하면
	{
		RECT rt;
		getCurrentScreenRect(rt);

		if ( m_currentImage.GetWidth() > rt.right )
		{
			m_iShowingX = m_currentImage.GetWidth() - rt.right;
		}
	}
}

void ZMain::OnDrag(int x, int y)
{
	if ( !m_currentImage.IsValid()) return;

	if ( ZOption::GetInstance().IsBigToSmallStretchImage() )
	{
		/// 큰 그림을 화면에 맞게 스트레칭할 때는 드래그는 하지 않아도 된다.
		return;
	}

	RECT rt;
	getCurrentScreenRect(rt);

	int iNowShowingX = m_iShowingX;
	int iNowShowingY = m_iShowingY;

	if ( (m_iShowingX + x) >= 0 ) 
	{
		if ( m_iShowingX + x + rt.right >= m_currentImage.GetWidth())
		{
			x = m_currentImage.GetWidth() - rt.right - m_iShowingX;// - 1;
		}
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
			y = m_currentImage.GetHeight() - rt.bottom - m_iShowingY;// - 1;
		}
		m_iShowingY += y;
	}
	else
	{
		m_iShowingY = 0;
	}

	if ( m_iShowingX != iNowShowingX || m_iShowingY != iNowShowingY )
	{
		Draw(false);
	}
}

/// 작업 표시줄을 보이게 해준다.
void ZMain::ShellTrayShow()
{
	/// 작업 표시줄을 보이게 해준다.
	HWND h = FindWindow(TEXT("Shell_TrayWnd"), TEXT(""));

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_SHOW);
	}
}


void ZMain::ChangeFileSort(eFileSortOrder sortOrder)
{
	m_sortOrder = sortOrder;
	ReLoadFileList();
}

void ZMain::ReLoadFileList()
{
	tstring strFileName = m_strCurrentFilename;
	RescanFolder();

	OpenFile(strFileName);
}

void ZMain::ShellTrayHide()
{
	// 작업 표시줄을 보이게 해준다.
	HWND h = FindWindow(TEXT("Shell_TrayWnd"), TEXT(""));

	if ( h != INVALID_HANDLE_VALUE )
	{
		ShowWindow(h, SW_HIDE);
	}
}


void ZMain::_ProcAfterRemoveThisFile()
{
	// 현재 파일이 마지막 파일인가?
	if ( m_vFile.size() <= 1 )
	{
		m_iCurretFileIndex = 0;
		m_strCurrentFilename = TEXT("");

		m_vFile.resize(0);

		SetTitle();
		SetStatusBarText();
		Draw(true);
	}
	else
	{
		if ( ((int)m_vFile.size() - 1) > m_iCurretFileIndex )	// 다음 그림이 있다.
		{
			// for 문을 돌면서 지울 것을 찾아놓는다.
			vector< FileData >::iterator it, endit = m_vFile.end();
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
			vector< FileData >::iterator it, endit = m_vFile.end();
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


void ZMain::OnFocusLose()
{
	//DebugPrintf("OnFocusLose()");

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
	//DebugPrintf("OnFocusGet()");
	if ( ZOption::GetInstance().IsFullScreen() )
	{
		DebugPrintf(TEXT("OnFocusGet() at fullscreen"));
		SetWindowPos(m_hMainDlg, HWND_TOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		MoveWindow(m_hMainDlg, 0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), TRUE);
		SetWindowPos(m_hMainDlg, HWND_NOTOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN), SWP_NOMOVE|SWP_NOSIZE);
		ShellTrayHide();
	}
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
		m_strCurrentFilename = m_vFile[m_iCurretFileIndex].m_strFileName;
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
	ZOption::GetInstance().m_bRightTopFirstDraw = !ZOption::GetInstance().m_bRightTopFirstDraw;

	CheckMenuItem(m_hMainMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, ZOption::GetInstance().m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_VIEW_RIGHTTOPFIRSTDRAW, ZOption::GetInstance().m_bRightTopFirstDraw ? MF_CHECKED : MF_UNCHECKED);
}

void ZMain::ShowFileExtDlg()
{
	ZFileExtDlg::GetInstance().ShowDlg();
}

void ZMain::DeleteThisFile()
{
	/// 현재 보고 있는 파일이 없으면 바로 리턴한다.
	if ( m_strCurrentFilename.empty() )
	{
		return;
	}

	TCHAR szDeleteMsg[256];

	StringCchPrintf(szDeleteMsg, sizeof(szDeleteMsg), GetMessage(TEXT("DELETE_THIS_FILE")), GetFileNameFromFullFileName(m_strCurrentFilename).c_str());
	int iRet = ::MessageBox(m_hMainDlg, szDeleteMsg, TEXT("ZViewer"), MB_YESNO);

	if ( iRet == IDYES )
	{
		if ( 0 == _tunlink(m_strCurrentFilename.c_str()) )
		{
			_ProcAfterRemoveThisFile();
		}
		else
		{
			MessageBox(TEXT("CANNOT_DELETE_THIS_FILE"));
		}
	}
}


void ZMain::MoveThisFile()
{
	/// 현재 보고 있는 파일이 없으면 바로 리턴한다.
	if ( m_strCurrentFilename.empty() )
	{
		return;
	}

	CSelectToFolderDlg aDlg;
	
	if ( !aDlg.DoModal() )
	{
		return;
	}

	tstring strFolder = aDlg.GetMoveToFolder();

	tstring filename = GetFileNameFromFullFileName(m_strCurrentFilename);
	tstring strToFileName = aDlg.GetMoveToFolder();
	
	if ( strToFileName.size() <= 2 )
	{
		MessageBox(TEXT("MOVE_DESTINATION_IS_TOO_SHORT"));
		return;
	}

	strToFileName += TEXT("\\");
	strToFileName += filename;

	// 옮겨갈 폴더에 같은 파일이 있는지 확인한다.
	if ( 0 != _taccess(aDlg.GetMoveToFolder().c_str(), 00) )
	{
		MessageBox(TEXT("WRONG_DIRECTORY_NAME"));
		return;
	}

	// 같은 파일이 존재하는지 확인한다.
	if ( 0 == _taccess(strToFileName.c_str(), 00) )
	{
		// 이미 존재하면
		if ( IDNO == MessageBox(TEXT("ASK_OVERWRITE_FILE"), MB_YESNO) )
		{
			return;
		}
	}
	
	if ( FALSE == MoveFileEx(m_strCurrentFilename.c_str(), strToFileName.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING) )
	{
		MessageBox(TEXT("CANNOT_MOVE_FILE"));
	}
	_ProcAfterRemoveThisFile();
}


/// 현재 파일을 복사한다.
void ZMain::CopyThisFile()
{
	/// 현재 보고 있는 파일이 없으면 바로 리턴한다.
	if ( m_strCurrentFilename.empty() )
	{
		return;
	}

	CSelectToFolderDlg aDlg;

	if ( !aDlg.DoModal() )
	{
		return;
	}

	tstring strFolder = aDlg.GetMoveToFolder();

	tstring filename = GetFileNameFromFullFileName(m_strCurrentFilename);
	tstring strToFileName = aDlg.GetMoveToFolder();

	if ( strToFileName.size() <= 2 )
	{
		MessageBox(TEXT("MOVE_DESTINATION_IS_TOO_SHORT"));
		return;
	}

	strToFileName += TEXT("\\");
	strToFileName += filename;

	// 옮겨갈 폴더에 같은 파일이 있는지 확인한다.
	if ( 0 != _taccess(aDlg.GetMoveToFolder().c_str(), 00) )
	{
		MessageBox(TEXT("WRONG_DIRECTORY_NAME"));
		return;
	}

	// 같은 파일이 존재하는지 확인한다.
	if ( 0 == _taccess(strToFileName.c_str(), 00) )
	{
		// 이미 존재하면
		if ( IDNO == MessageBox(TEXT("ASK_OVERWRITE_FILE"), MB_YESNO) )
		{
			return;
		}
	}

	if ( FALSE == CopyFile(m_strCurrentFilename.c_str(), strToFileName.c_str(), FALSE) )
	{
		MessageBox(TEXT("CANNOT_COPY_FILE"));
	}
}


void ZMain::Rotate(bool bClockWise)
{
	if ( m_currentImage.IsValid() )
	{
		if ( bClockWise )
		{
			m_currentImage.Rotate(-90);
		}
		else
		{
			m_currentImage.Rotate(90);
		}

		m_iShowingX = 0;
		m_iShowingY = 0;

		_releaseBufferDC();

		Draw(true);
	}
}

void ZMain::SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style)
{
	// 현재보고 있는 파일을 윈도우 폴더에 저장한다.
	TCHAR szSystemFolder[_MAX_PATH] = { 0 };

	if ( E_FAIL == SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, SHGFP_TYPE_CURRENT, szSystemFolder) )
	{
		_ASSERTE(false);
		return;
	}

	TCHAR szFileName[MAX_PATH] = { 0 };
	_tsplitpath(m_vFile[m_iCurretFileIndex].m_strFileName.c_str(), 0, 0, szFileName, 0);

	tstring strSaveFileName = szSystemFolder;
	strSaveFileName += TEXT("\\zviewer_bg_");
	strSaveFileName += szFileName;
	strSaveFileName += TEXT(".bmp");

	if ( FALSE == m_currentImage.SaveToFile(strSaveFileName, BMP_DEFAULT) )
	{
		_ASSERTE(false);
		return;
	}

	CDesktopWallPaper::SetDesktopWallPaper(strSaveFileName, style);
}


/// 현재 화면의 그릴 수 있는 영역의 크기를 받아온다.
void ZMain::getCurrentScreenRect(RECT & rect)
{
	_ASSERTE(m_hMainDlg);
	GetClientRect(m_hMainDlg, &rect);
	if ( ZOption::GetInstance().IsFullScreen() == false ) rect.bottom -= STATUSBAR_HEIGHT;
}

/// On Window is resized
void ZMain::OnWindowResized()
{
	RECT rt;
	getCurrentScreenRect(rt);

	OnChangeCurrentSize(rt.right, rt.bottom);

	if ( ZOption::GetInstance().IsBigToSmallStretchImage() || ZOption::GetInstance().IsSmallToBigStretchImage() )
	{
		ZCacheImage::GetInstance().clearCache();
		ZCacheImage::GetInstance().setCacheEvent();
		LoadCurrent();
	}
}


/// 상태 표시 윈도우를 만든다.
void ZMain::CreateStatusBar()
{
	// StatusBar 를 생성한다.
	m_hStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, TEXT("Status line"), m_hMainDlg, 0);

	// StatusBar 를 split 한다. 아래의 숫자는 크기가 아니라 절대 위치라는 것을 명심!!!!!!!
	int SBPart[7] =
	{
		70,		/// %d/%d 현재보고 있는 이미지 파일의 index number
		200,	/// %dx%dx%dbpp 해상도와 color depth, image size
		300,	/// image size
		420,	/// temp banner http://www.wimy.com
		500,	/// 파일을 읽어들이는데 걸린 시간
		553,	/// cache status
		1860,	/// 파일명표시
	};
	SendMessage(m_hStatusBar, SB_SETPARTS, 7, (LPARAM)SBPart);
}