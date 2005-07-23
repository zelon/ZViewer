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
#include "../lib/DesktopWallPaper.h"
#include "src/MoveToDlg.h"

#include <ShlObj.h>
#include <cstdio>

#include "resource.h"

using namespace std;

ZMain & ZMain::GetInstance()
{
	static ZMain aInstance;
	return aInstance;
}

BOOL ZMain::_SetOSVersion()
{
	const int BUFSIZE = 1024;

	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return FALSE;
	}

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:

		// Test for the specific product family.
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			m_osKind = eOSKind_XP;
			printf ("Microsoft Windows Server&nbsp;2003 family, ");
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			m_osKind = eOSKind_XP;
			printf ("Microsoft Windows XP ");
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			m_osKind = eOSKind_2000;
			printf ("Microsoft Windows 2000 ");
		}

		if ( osvi.dwMajorVersion <= 4 )
			printf("Microsoft Windows NT ");

		// Test for specific product on Windows NT 4.0 SP6 and later.
		if( bOsVersionInfoEx )
		{
			// Test for the workstation type.
			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				if( osvi.dwMajorVersion == 4 )
					printf ( "Workstation 4.0 " );
				else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
					printf ( "Home Edition " );
				else
					printf ( "Professional " );
			}

			// Test for the server type.
			else if ( osvi.wProductType == VER_NT_SERVER )
			{
				if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						printf ( "Datacenter Edition " );
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						printf ( "Enterprise Edition " );
					else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
						printf ( "Web Edition " );
					else
						printf ( "Standard Edition " );
				}

				else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						printf ( "Datacenter Server " );
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						printf ( "Advanced Server " );
					else
						printf ( "Server " );
				}

				else  // Windows NT 4.0 
				{
					if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						printf ("Server 4.0, Enterprise Edition " );
					else
						printf ( "Server 4.0 " );
				}
			}
		}
		else  // Test for specific product on Windows NT 4.0 SP5 and earlier
		{
			HKEY hKey;
			char szProductType[BUFSIZE];
			DWORD dwBufLen=BUFSIZE;
			LONG lRet;

			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				"SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet != ERROR_SUCCESS )
				return FALSE;

			lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
				return FALSE;

			RegCloseKey( hKey );

			if ( lstrcmpi( "WINNT", szProductType) == 0 )
				printf( "Workstation " );
			if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
				printf( "Server " );
			if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
				printf( "Advanced Server " );

			printf( "%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion );
		}

		// Display service pack (if any) and build number.

		if( osvi.dwMajorVersion == 4 && 
			lstrcmpi( osvi.szCSDVersion, "Service Pack 6" ) == 0 )
		{
			HKEY hKey;
			LONG lRet;

			// Test for SP6 versus SP6a.
			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009",
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet == ERROR_SUCCESS )
				printf( "Service Pack 6a (Build %d)\n", osvi.dwBuildNumber & 0xFFFF );         
			else // Windows NT 4.0 prior to SP6a
			{
				printf( "%s (Build %d)\n",
					osvi.szCSDVersion,
					osvi.dwBuildNumber & 0xFFFF);
			}

			RegCloseKey( hKey );
		}
		else // Windows NT 3.51 and earlier or Windows 2000 and later
		{
			printf( "%s (Build %d)\n",
				osvi.szCSDVersion,
				osvi.dwBuildNumber & 0xFFFF);
		}


		break;

		// Test for the Windows 95 product family.
	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			printf ("Microsoft Windows 95 ");
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				printf("OSR2 " );
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			m_osKind = eOSKind_98;
			printf ("Microsoft Windows 98 ");
			if ( osvi.szCSDVersion[1] == 'A' )
				printf("SE " );
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			printf ("Microsoft Windows Millennium Edition\n");
		} 
		break;

	case VER_PLATFORM_WIN32s:

		printf ("Microsoft Win32s\n");
		break;
	}
	return TRUE; 
}


ZMain::ZMain(void)
:	m_hMainDlg(NULL)
,	m_sortOrder(eFileSortOrder_FILENAME)
,	m_osKind(eOSKind_UNKNOWN)
{
	SetProgramFolder();

	_SetOSVersion();

	_ASSERTE(m_osKind != eOSKind_UNKNOWN);
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

void ZMain::Draw(bool bEraseBg)
{
	if ( m_currentImage.IsValid() == FALSE ) return;
	if ( m_hMainDlg == NULL ) return;

	RECT currentScreenRect;
	GetClientRect(m_hMainDlg, &currentScreenRect);
	if ( m_option.m_bFullScreen == false ) currentScreenRect.bottom -= STATUSBAR_HEIGHT;

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

	if ( m_currentImage.GetWidth() < currentScreenRect.right )
	{
		// 가로 길이가 화면보다 작을 때는 중앙에 오게
		iDrawX = (currentScreenRect.right/2) - (m_currentImage.GetWidth()/2);
	}
	else
	{
		// 가로 길이가 화면보다 크면 클리핑
		iDrawPartX = m_iShowingX;
		bNeedClipping = true;
	}

	if ( m_currentImage.GetHeight() < currentScreenRect.bottom )
	{
		// 세로 길이가 화면보다 작을 때는 중앙에 오게
		iDrawY = (currentScreenRect.bottom/2) - (m_currentImage.GetHeight()/2);
	}
	else
	{
		// 세로 길이가 화면보다 크면 클리핑
		iDrawPartY = m_iShowingY;
		bNeedClipping = true;
	}


	/// 그림이 화면보다 큰가 확인
	if ( bNeedClipping )
	{
		/// 그림이 화면보다 크면...

		if ( m_option.m_bBigToSmallStretchImage )
		{
			/// 큰 그림을 축소시켜서 그린다.

			RECT toRect;
			SetRect(&toRect, 0, 0, m_currentImage.GetWidth(), m_currentImage.GetHeight());
			toRect = GetResizedRectForBigToSmall(currentScreenRect, toRect);

			ZImage stretchedImage(m_currentImage);

			if ( stretchedImage.GetBPP() == 8 )
			{
				stretchedImage.ConvertTo32Bit();
			}
			stretchedImage.Resize((WORD)toRect.right, (WORD)toRect.bottom);
			
			iDrawX = (currentScreenRect.right - toRect.right) / 2;
			iDrawY = (currentScreenRect.bottom - toRect.bottom) / 2;

			if ( bEraseBg )	// 배경을 지워야 하면 지운다.
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
				Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);
			}

			// 메모리것을 화면에 그린다.
			BOOL b = BitBlt(mainDC, 
				iDrawX, iDrawY,			// 그릴 화면의 x, y 좌표. 화면에 꽉 찰 때는 0, 0 이어야한다.
				m_currentImage.GetWidth(), currentScreenRect.bottom,		// 그려질 화면의 가로, 세로 길이. 
				memDC, 
				iDrawPartX, iDrawPartY,			// 그려질 이미지 원본의 시작 x,y 좌표
				SRCCOPY);

			DebugPrintf("rt.bottom : %d, PartX : %d, iDrawPartY : %d", currentScreenRect.bottom, iDrawPartX, iDrawPartY);

			DeleteObject(hbmScreen);
			DeleteDC(memDC);
		}
	}
	else	/// 화면이 그림보다 작으면...
	{
		if ( bEraseBg )	// 배경을 지워야 하면 지운다.
		{
			SelectObject(mainDC, GetStockObject(BLACK_BRUSH));
			Rectangle(mainDC, 0, 0, currentScreenRect.right, currentScreenRect.bottom);
		}

		if ( m_option.m_bSmallToBigStretchImage )
		{
			/// 작은 그림을 화면에 맞게 확대해서 그린다.
			RECT originalImageRect;
			SetRect(&originalImageRect, 0, 0, m_currentImage.GetWidth(), m_currentImage.GetHeight());
			RECT toRect = GetResizedRectForBigToSmall(currentScreenRect, originalImageRect);

			ZImage stretchedImage(m_currentImage);

			if ( stretchedImage.GetBPP() == 8 )
			{
				stretchedImage.ConvertTo32Bit();
			}
			stretchedImage.Resize((WORD)toRect.right, (WORD)toRect.bottom);
			
			_ASSERTE(toRect.right == stretchedImage.GetWidth());
			_ASSERTE(toRect.bottom == stretchedImage.GetHeight());

			int iDrawPointX = (currentScreenRect.right - toRect.right) / 2;
			int iDrawPointY = (currentScreenRect.bottom - toRect.bottom) / 2;
			
			int r = StretchDIBits(mainDC,
				iDrawPointX, iDrawPointY, 
				toRect.right, toRect.bottom,
				0, 0,
				toRect.right, toRect.bottom,
				stretchedImage.GetData(),
				stretchedImage.GetBitmapInfo(),
				DIB_RGB_COLORS, SRCCOPY);
		}
		else
		{
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
	if ( m_currentImage.GetWidth() > currentScreenRect.right ||
		m_currentImage.GetHeight() > currentScreenRect.bottom
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

void ZMain::ZFindFile(const char *path, std::vector< FileData > & foundStorage, bool bFindRecursive)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;

	char fname[_MAX_FNAME] = { 0 };
	BOOL bResult=TRUE;
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char newpath[MAX_PATH] = { 0 };

	hSrch=FindFirstFile(path,&wfd);
	while (bResult)
	{
		_splitpath(path,drive,dir,NULL,NULL);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wfd.cFileName[0]!='.' && bFindRecursive == true)
			{
				_snprintf(newpath, sizeof(newpath), "%s%s%s\\*.*",drive,dir,wfd.cFileName);
				ZFindFile(newpath, foundStorage, bFindRecursive);
			}
		}
		else
		{
			_snprintf(fname, sizeof(fname), "%s%s%s",drive,dir,wfd.cFileName);

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

void ZMain::ZFindFolders(const char *path, std::vector<std::string> & foundStorage, bool bFindRecursive)
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
			if (wfd.cFileName[0]!='.' )
			{
				_snprintf(fname, sizeof(fname), "%s%s%s",drive,dir,wfd.cFileName);

				foundStorage.push_back(fname);

				if ( bFindRecursive == true )
				{
					_snprintf(newpath, sizeof(newpath), "%s%s%s\\*.*",drive,dir,wfd.cFileName);
					ZFindFolders(newpath, foundStorage, bFindRecursive);
				}
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

bool ZMain::GetNeighborFolders(std::vector < std::string > & vFolders)
{
	std::string strParentFolder;

	// 현재 폴더의 상위 폴더를 검색한다.
	{
		// 현재 폴더에서 오른쪽부터 2번째의 \ 를 얻는다.
		size_t pos = m_strCurrentFolder.find_last_of("\\");

		if ( pos == m_strCurrentFolder.npos )
		{
			MessageBox(m_hMainDlg, "Can't find parent folder.", "ZViewer", MB_OK);
			return false;
		}

		std::string strParentFolder = m_strCurrentFolder.substr(0, pos);

		pos = strParentFolder.find_last_of("\\");

		if ( pos == strParentFolder.npos )
		{
			MessageBox(m_hMainDlg, "Can't find parent folder.", "ZViewer", MB_OK);
			return false;
		}

		strParentFolder = strParentFolder.substr(0, pos);

		strParentFolder += "\\*.*";
		// 상위 폴더의 하위 폴더들을 얻는다.
		ZFindFolders(strParentFolder.c_str(), vFolders, false);

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
	std::vector < std::string > vFolders;

	if ( !GetNeighborFolders(vFolders) ) return;

	// 상위 폴더의 하위 폴더 중 현재 폴더의 다음 폴더가 있으면 다음 폴더를 검색한 후, 처음 이미지를 연다.
	{
		int iFoundIndex = -1;
		// 현재 폴더의 index 를 찿는다.
		for (unsigned int i=0; i<vFolders.size(); ++i)
		{
			if ( vFolders[i] + "\\" == m_strCurrentFolder )
			{
				iFoundIndex = i;
				break;
			}
		}

		_ASSERTE(iFoundIndex != -1);

		if ( (iFoundIndex + 1) >= (int)vFolders.size() )
		{
			// 마지막 폴더이다.
			MessageBox(m_hMainDlg, "Here is the last folder.", "ZViewer", MB_OK);
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
	std::vector < std::string > vFolders;

	if ( !GetNeighborFolders(vFolders) ) return;

	// 상위 폴더의 하위 폴더 중 현재 폴더의 다음 폴더가 있으면 다음 폴더를 검색한 후, 처음 이미지를 연다.
	{
		int iFoundIndex = -1;
		// 현재 폴더의 index 를 찿는다.
		for (unsigned int i=0; i<vFolders.size(); ++i)
		{
			if ( vFolders[i] + "\\" == m_strCurrentFolder )
			{
				iFoundIndex = i;
				break;
			}
		}

		_ASSERTE(iFoundIndex != -1);

		if ( (iFoundIndex-1 < 0 ) )
		{
			// 마지막 폴더이다.
			MessageBox(m_hMainDlg, "Here is the first folder.", "ZViewer", MB_OK);
			return;
		}
		else
		{
			OpenFolder(vFolders[iFoundIndex-1]);
		}
	}
}

void ZMain::_GetFileListAndSort(const std::string & strFolderPathAndWildCard, FileListVector & vFileList)
{
	vFileList.clear();
	ZFindFile(strFolderPathAndWildCard.c_str(), vFileList, false);

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

void ZMain::OpenFolder(const std::string strFolder)
{
	// 특정 폴더의 하위 파일들을 검색해서 정렬 후 첫번째 파일을 연다.

	std::string strTemp = strFolder;
	strTemp += "\\*.*";

	vector < FileData > vFiles;

	_GetFileListAndSort(strTemp, vFiles);

	if ( vFiles.size() == 0 )
	{
		std::string strMsg = strFolder;
		strMsg += " folder has no image file.";
		MessageBox(m_hMainDlg, strMsg.c_str(), "ZViewer", MB_OK);
		return;
	}
	else
	{
		OpenFile(vFiles[0].m_strFileName);
	}
}


void ZMain::OpenFile(const string & strFilename)
{
	m_strCurrentFolder = GetFolderNameFromFullFileName(strFilename);
	RescanFolder();

	// 스캔한 파일 중 현재 파일을 찾는다.
	std::vector< FileData >::iterator it, endit = m_vFile.end();

	for ( it = m_vFile.begin(); it != endit; ++it)
	{
		if ( it->m_strFileName == strFilename )
		{
			break;
		}
	}

	if ( m_vFile.size() <= 0 )
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

	if ( iIndex < 0 ) iIndex = 0;
	if ( iIndex >= (int)m_vFile.size() ) iIndex = (int)m_vFile.size() - 1;

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

	m_iShowingX = 0;
	m_iShowingY = 0;
}


void ZMain::ToggleSmallToScreenStretch()
{
	m_option.m_bSmallToBigStretchImage = ! m_option.m_bSmallToBigStretchImage;

	CheckMenuItem(m_hMainMenu, ID_VIEW_SMALLTOSCREENSTRETCH, m_option.m_bSmallToBigStretchImage ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hPopupMenu, ID_POPUPMENU_SMALLTOSCREENSTRETCH, m_option.m_bSmallToBigStretchImage ? MF_CHECKED : MF_UNCHECKED);

	Draw();
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

	if ( m_vFile.size() == 0 || m_strCurrentFilename.empty() ) // 보고 있는 파일이 없으면
	{
		// File Index
		_snprintf(szTemp, sizeof(szTemp), "No File");
		SendMessage(m_hStatus, SB_SETTEXT, 0, (LPARAM)szTemp);

		// 해상도 정보
		_snprintf(szTemp, sizeof(szTemp), "");
		SendMessage(m_hStatus, SB_SETTEXT, 1, (LPARAM)szTemp);

		// 이미지 사이즈
		_snprintf(szTemp, sizeof(szTemp), "");
		SendMessage(m_hStatus, SB_SETTEXT, 2, (LPARAM)szTemp);

		// 임시로 http://wimy.com
		_snprintf(szTemp, sizeof(szTemp), "http://wimy.com");
		SendMessage(m_hStatus, SB_SETTEXT, 3, (LPARAM)szTemp);

		// 로딩시간
		_snprintf(szTemp, sizeof(szTemp), "");
		SendMessage(m_hStatus, SB_SETTEXT, 4, (LPARAM)szTemp);

		// 파일명
		_snprintf(szTemp, sizeof(szTemp), "No File");
		SendMessage(m_hStatus, SB_SETTEXT, 5, (LPARAM)szTemp);
	}
	else
	{
		// File Index
		_snprintf(szTemp, sizeof(szTemp), "%d/%d", m_iCurretFileIndex+1, m_vFile.size());
		SendMessage(m_hStatus, SB_SETTEXT, 0, (LPARAM)szTemp);

		// 해상도 정보
		_snprintf(szTemp, sizeof(szTemp), "%dx%dx%dbpp", m_currentImage.GetWidth(), m_currentImage.GetHeight(), m_currentImage.GetBPP());
		SendMessage(m_hStatus, SB_SETTEXT, 1, (LPARAM)szTemp);

		// image size
		long imageSize = m_vFile[m_iCurretFileIndex].m_nFileSize;

		if ( imageSize > 1024 )
		{
			if ( imageSize/1024 > 1024 )
			{
				_snprintf(szTemp, sizeof(szTemp), "%.2fMByte", imageSize/1024/1024.0f);
			}
			else
			{
				_snprintf(szTemp, sizeof(szTemp), "%dKByte", imageSize/1024);
			}
		}
		else
		{
			_snprintf(szTemp, sizeof(szTemp), "%dByte", imageSize);
		}
		SendMessage(m_hStatus, SB_SETTEXT, 2, (LPARAM)szTemp);



		// 임시로 http://wimy.com
		_snprintf(szTemp, sizeof(szTemp), "http://wimy.com");
		SendMessage(m_hStatus, SB_SETTEXT, 3, (LPARAM)szTemp);

		// 로딩시간
		_snprintf(szTemp, sizeof(szTemp), "%.3fsec", (float)(m_dwLoadingTime / 1000.0));
		SendMessage(m_hStatus, SB_SETTEXT, 4, (LPARAM)szTemp);

		// 파일명
		char szFilename[MAX_PATH], szFileExt[MAX_PATH];
		_splitpath(m_strCurrentFilename.c_str(), NULL, NULL, szFilename, szFileExt);

		_snprintf(szTemp, sizeof(szTemp), "%s%s", szFilename, szFileExt);
		SendMessage(m_hStatus, SB_SETTEXT, 5, (LPARAM)szTemp);

	}
}

void ZMain::SetTitle()
{
	char szTemp[MAX_PATH+256];

	if ( m_strCurrentFilename.empty() )	// 현재보고 있는 파일명이 없으면
	{
		_snprintf(szTemp, sizeof(szTemp), "ZViewer v%s", g_strVersion.c_str());
	}
	else // 현재보고 있는 파일명이 있으면
	{
		char szFileName[MAX_PATH] = { 0 };
		char szFileExt[MAX_PATH] = { 0 };
		_splitpath(m_strCurrentFilename.c_str(), NULL, NULL, szFileName, szFileExt);

		//_snprintf(szTemp, sizeof(szTemp), "%s%s - ZViewer for rubi v%s", szFileName, szFileExt, g_strVersion.c_str() );
		_snprintf(szTemp, sizeof(szTemp), "%s%s - %s [for rubi :D]", szFileName, szFileExt, m_strCurrentFilename.c_str());
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

		DebugPrintf("Cache Hit!!!!!!!!!!!!!\r\n");

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

			DebugPrintf("Direct Load failed. sleep");

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
			
			DebugPrintf("Cache miss. Add to cache.");
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
	if ( !m_currentImage.IsValid()) return;

	if ( m_option.m_bBigToSmallStretchImage )
	{
		/// 큰 그림을 화면에 맞게 스트레칭할 때는 드래그는 하지 않아도 된다.
		return;
	}

	RECT rt;
	GetClientRect(m_hMainDlg, &rt);

	if ( m_option.m_bFullScreen == false ) rt.bottom -= STATUSBAR_HEIGHT;

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

void ZMain::ShellTrayShow()
{
	// 작업 표시줄을 보이게 해준다.
	HWND h = FindWindow("Shell_TrayWnd", "");

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
	std::string strFileName = m_strCurrentFilename;
	RescanFolder();

	OpenFile(strFileName);
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


void ZMain::_ProcAfterRemoveThisFile()
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
	DebugPrintf("OnFocusLose()");

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
	DebugPrintf("OnFocusGet()");
	if ( m_option.m_bFullScreen )
	{
		DebugPrintf("OnFocusGet() at fullscreen");
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
			_ProcAfterRemoveThisFile();
		}
		else
		{
			MessageBox(m_hMainDlg, "Can't delete this file!", "ZViewer", MB_OK);
		}
	}
}


void ZMain::MoveThisFile()
{

	CMoveToDlg aDlg;
	
	if ( !aDlg.DoModal() )
	{
		return;
	}

	std::string strFolder = aDlg.GetMoveToFolder();

	std::string filename = GetFileNameFromFullFileName(m_strCurrentFilename);
	std::string strToFileName = aDlg.GetMoveToFolder();
	strToFileName += "\\";
	strToFileName += filename;

	// 옮겨갈 폴더에 같은 파일이 있는지 확인한다.
	if ( 0 != _access(aDlg.GetMoveToFolder().c_str(), 00) )
	{
		MessageBox(m_hMainDlg, "Wrong folder name", "ZViewer", MB_OK);
		return;
	}

	// 같은 파일이 존재하는지 확인한다.
	if ( 0 == _access(strToFileName.c_str(), 00) )
	{
		// 이미 존재하면
		if ( IDNO == MessageBox(m_hMainDlg, "There is a file same name. Overwrite?", "ZViewer", MB_YESNO) )
		{
			return;
		}

	}
	
	MoveFileEx(m_strCurrentFilename.c_str(), strToFileName.c_str(), MOVEFILE_REPLACE_EXISTING);
	_ProcAfterRemoveThisFile();
}

/*
void ZMain::LoadLanguage()
{
	ZINIOption opt;
	opt.LoadFromFile("language/korean.ini");

	// 메뉴를 세팅한다.
	ModifyMenu(m_hPopupMenu, ID_DELETETHISFILE, MF_BYCOMMAND, ID_DELETETHISFILE, opt.GetValue("MenuFileOpen").c_str());
	//SetDlgItemText(m_hPopupMenu, ID_DELETETHISFILE, opt.GetValue("MenuDeleteThisFile").c_str());
}
*/

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
		Draw(true);
	}
}

void ZMain::SetDesktopWallPaper(CDesktopWallPaper::eDesktopWallPaperStyle style)
{
	// 현재보고 있는 파일을 윈도우 폴더에 저장한다.
	char szSystemFolder[_MAX_PATH] = { 0 };

	if ( E_FAIL == SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, SHGFP_TYPE_CURRENT, szSystemFolder) )
	{
		_ASSERTE(false);
		return;
	}

	char szFileName[MAX_PATH] = { 0 };
	_splitpath(m_vFile[m_iCurretFileIndex].m_strFileName.c_str(), 0, 0, szFileName, 0);

	std::string strSaveFileName = szSystemFolder;
	strSaveFileName += "\\RUBI_bg_";
	strSaveFileName += szFileName;
	strSaveFileName += ".bmp";

	if ( FALSE == m_currentImage.SaveToFile(strSaveFileName, BMP_DEFAULT) )
	{
		_ASSERTE(false);
		return;
	}


	CDesktopWallPaper wallPaper(strSaveFileName);
	wallPaper.SetWallPaperStyle(style);
	wallPaper.SetDesktopWallPaper();
}

void ZMain::ClearDesktopWallPaper()
{
	CDesktopWallPaper::ClearDesktopWallPaper();
}