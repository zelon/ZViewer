/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	CommonFunc.cpp
*
*                                       http://www.wimy.com
*********************************************************************/
#include "stdafx.h"
#include "CommonFunc.h"
#include <shlobj.h>
#include <io.h>
#include "LogManager.h"
#include <strsafe.h>

#ifdef _DEBUG
void DebugPrintf( const TCHAR *fmt, ... )
{
//	return;

	va_list v;
	TCHAR buf[1024*4];
	int len;

	va_start( v, fmt );
	len = StringCbVPrintf( buf, sizeof(buf) / sizeof(TCHAR), fmt, v );
	va_end( v );

	if ( S_OK != len )
	{
		assert(false);
	}

	OutputDebugString( buf );
	OutputDebugString( TEXT("\r\n") );

	CLogManager::getInstance().Output(buf);
}
#else
void DebugPrintf( const TCHAR * , ...){}
#endif


const tstring GetOnlyFileNameWithoutExt(const tstring & strFullFileName)
{
	TCHAR szFile[FILENAME_MAX] = { 0 };
	SplitPath(strFullFileName.c_str(), NULL,0 , NULL, 0, szFile,FILENAME_MAX, NULL, 0);

	return szFile;
}

bool IsPressedVirtualKey(int vk)
{
	if ( GetKeyState(vk) < 0 ) return true;
	return false;
}

bool SetRegistryValue(HKEY hOpenKey, const tstring & strKey,LPCTSTR szValue, const tstring & strData)
{
	if( !hOpenKey || strKey.empty() || !szValue)
	{
		assert(!"SetRegistryValue invalid arg");
		return false;
	}

	bool bRetVal = false;
	DWORD dwDisposition;
	HKEY hTempKey = NULL;

	if( ERROR_SUCCESS == ::RegCreateKeyEx(hOpenKey, strKey.c_str(), NULL,
		NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hTempKey, &dwDisposition) )
	{
		// 마지막 \0 까지 포함해야한다던데;;
		DWORD	dwBufferLength = (DWORD)(strData.size() + 1) * sizeof(TCHAR);

		if( ERROR_SUCCESS == ::RegSetValueEx(hTempKey, (LPTSTR)szValue,
			NULL, REG_SZ, (const BYTE *)strData.c_str(), dwBufferLength) )
		{
			bRetVal = true;
		}
		::RegCloseKey(hTempKey);
	}

	return bRetVal;
}

// 최대 크기를 넘지 않는 적당한 리사이즈 크기를 돌려준다.
RECT GetResizedRectForBigToSmall(const RECT & MaximumSize, const RECT & originalSize)
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


	assert(ret.right <= MaximumSize.right);
	assert(ret.bottom <= MaximumSize.bottom);

	return ret;
}

RECT GetResizedRectForSmallToBig(const RECT & MaximumSize, const RECT & originalSize)
{
	if ( originalSize.right > MaximumSize.right && originalSize.bottom > MaximumSize.bottom )
	{
		return GetResizedRectForBigToSmall(MaximumSize, originalSize);
	}

	// 가로 세로 크기 중 큰 값을 찾는다.
	bool bSetWidth = true;		// 가로 크기를 기준으로 맞출 것인가?

	double dWidthRate = (double)originalSize.right / (double)MaximumSize.right;
	double dHeightRate = (double)originalSize.bottom / (double)MaximumSize.bottom;

	if ( dHeightRate <=  dWidthRate)
	{
		bSetWidth = true;
	}
	else
	{
		bSetWidth = false;
	}

	RECT ret;

	if ( bSetWidth == true )
	{
		// 가로 크기가 기준이다.
		SetRect(&ret, 0, 0, (int)(MaximumSize.right), (int)(MaximumSize.right * originalSize.bottom/originalSize.right));
	}
	else
	{
		// 세로 크기가 기준이다.
		SetRect(&ret, 0, 0, (int)(originalSize.right * MaximumSize.bottom / originalSize.bottom), (int)(MaximumSize.bottom));
	}


	assert(ret.right <= MaximumSize.right);
	assert(ret.bottom <= MaximumSize.bottom);

	return ret;
}

tstring toString(int i)
{
	const int szSize = 20;
	TCHAR szTemp[szSize];
	SPrintf(szTemp, szSize, TEXT("%d"), i);

	return tstring(szTemp);
}

/// 폴더를 선택하는 다이얼로그를 띄운다.
bool SelectFolder(HWND hWnd, TCHAR * szFolder)
{
	LPITEMIDLIST pidl;
	BROWSEINFO bi;

	bi.hwndOwner = hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = TEXT("Select folder");
	bi.ulFlags = 0;
	bi.lpfn = 0;
	bi.lParam = 0;

	pidl = SHBrowseForFolder(&bi);

	if ( pidl == NULL )
	{
		return false;
	}

	SHGetPathFromIDList(pidl, szFolder);

	return true;
}

/// 드라이브와 폴더명과 파일명으로 이루어진 문자열을 주면, 드라이브와 폴더명까지만 반환한다.
tstring GetFolderNameFromFullFileName(const tstring & strFullFilename)
{
	TCHAR szDrive[_MAX_DRIVE] = { 0 };
	TCHAR szDir[_MAX_DIR] = { 0 };
	SplitPath(strFullFilename.c_str(), szDrive,_MAX_DRIVE, szDir,_MAX_DIR, NULL,0, NULL,0);

	tstring strFolder = szDrive;
	strFolder += szDir;

	return strFolder;
}

/// 드라이브와 폴더명과 파일명으로 된 문자열을 주면, 파일명만 준다.
tstring GetFileNameFromFullFileName(const tstring & strFullFilename)
{
	TCHAR szFileName[FILENAME_MAX] = { 0 };
	TCHAR szFileExt[MAX_PATH] = { 0 };
	SplitPath(strFullFilename.c_str(), NULL,0, NULL,0, szFileName,FILENAME_MAX, szFileExt,MAX_PATH);

	tstring strFilename = szFileName;
	strFilename += szFileExt;

	return strFilename;
}

/// 현재 실행 중인 OS 의 버젼을 반환한다.
eOSKind getOSVersion()
{
	const int BUFSIZE = 1024;

	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi);
	if( !bOsVersionInfoEx )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return eOSKind_UNKNOWN;
	}

	eOSKind retKind = eOSKind_UNKNOWN;

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:

		// Test for the specific product family.
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			retKind = eOSKind_XP;
			printf ("Microsoft Windows Server&nbsp;2003 family, ");
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			retKind = eOSKind_XP;
			printf ("Microsoft Windows XP ");
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			retKind = eOSKind_2000;
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
			TCHAR szProductType[BUFSIZE];
			DWORD dwBufLen=BUFSIZE;
			LONG lRet;

			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				TEXT("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet != ERROR_SUCCESS )
				return eOSKind_UNKNOWN;

			lRet = RegQueryValueEx( hKey, TEXT("ProductType"), NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
				return eOSKind_UNKNOWN;

			RegCloseKey( hKey );

			/*
			if ( lstrcmpi( TEXT("WINNT"), szProductType) == 0 )
				printf( "Workstation " );
			if ( lstrcmpi( TEXT("LANMANNT"), szProductType) == 0 )
				printf( "Server " );
			if ( lstrcmpi( TEXT("SERVERNT"), szProductType) == 0 )
				printf( "Advanced Server " );

			printf( "%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion );
			*/
		}

		// Display service pack (if any) and build number.

		if( osvi.dwMajorVersion == 4 )//&& lstrcmpi( osvi.szCSDVersion, TEXT("Service Pack 6") ) == 0 )
		{
			HKEY hKey;
			LONG lRet;

			// Test for SP6 versus SP6a.
			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"),
				0, KEY_QUERY_VALUE, &hKey );
			if (lRet == ERROR_SUCCESS)
			{
				printf("Service Pack 6a (Build %u)\n", osvi.dwBuildNumber & 0xFFFF);
				RegCloseKey(hKey);
			}
			else // Windows NT 4.0 prior to SP6a
			{
				//printf( "%s (Build %d)\n", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
			}

		}
		else // Windows NT 3.51 and earlier or Windows 2000 and later
		{
			//printf( "%s (Build %d)\n", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
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
			retKind = eOSKind_98;
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
	return retKind; 
}


/// string 을 wstring 으로 변환
std::wstring getWStringFromString(const std::string & str)
{
	WCHAR buff[256] = { 0 };
	if ( 0 == MultiByteToWideChar(CP_THREAD_ACP, MB_PRECOMPOSED, str.c_str(), (int)str.size(), buff, 256) )
	{
		return L"failedConvert";
	}
	return std::wstring(buff);
}

/// Make dump file name
tstring GetDumpFilename()
{
	TCHAR szFilename[_MAX_PATH];

	for ( int i=0; i<100; ++i )
	{
		SPrintf(szFilename, _MAX_PATH, TEXT("C:\\ZViewer%s_%d.dmp"), g_strVersion.c_str(), i);

		if ( 0 != _taccess(szFilename, 00) )	///< file not exist
		{
			break;
		}
	}
	tstring strDumpFilename = szFilename;
	return strDumpFilename;
}


/// 현재 실행 파일이 있는 폴더를 얻는다.
tstring GetProgramFolder()
{
	tstring retString;

	TCHAR szGetFileName[FILENAME_MAX] = { 0 };

	HMODULE hModule = GetModuleHandle(
		
#ifdef _DEBUG
		TEXT("FreeImaged.dll")
#else
		TEXT("FreeImage.dll")
#endif
		);

	/// ZViewer, ZViewerAgent 둘다 FreeImage.dll 을 쓰므로 이 dll 이 있는 폴더를 찾는다.
	DWORD ret = GetModuleFileName(hModule, szGetFileName, FILENAME_MAX);

	if ( ret == 0 )
	{
		assert(!"Can't get module folder");
	}
	TCHAR szDrive[_MAX_DRIVE] = { 0 };
	TCHAR szDir[_MAX_DIR] = { 0 };
	SplitPath(szGetFileName, szDrive,_MAX_DRIVE, szDir,_MAX_DIR, NULL,0, NULL,0);

	retString = szDrive;
	retString += szDir;

	return retString;
}

/// 컴파일러 버젼에 맞게 함수 정의
void SplitPath(const TCHAR * path, TCHAR * drive, size_t driveNumberOfElements, TCHAR * dir, size_t dirNumberOfElements,
				TCHAR * fname, size_t nameNumberOfElements, TCHAR * ext, size_t extNumberOfElements)
{
#if _MSC_VER >= 1400
	_tsplitpath_s(path, drive, driveNumberOfElements, dir, dirNumberOfElements, fname, nameNumberOfElements, ext, extNumberOfElements);
#else
	_tsplitpath(path, drive, dir, fname, ext);
#endif
	
}

/// 파일 열기 다이얼로그는 항상 부모의 가운데에 띄운다.
UINT_PTR CenterOFNHookProc(HWND hdlg, UINT uiMsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    if ( uiMsg == WM_INITDIALOG)
	{
		HWND hwndParent = GetParent(hdlg);

		if ( hwndParent != INVALID_HANDLE_VALUE )
		{
			RECT windowRect;

			if ( TRUE == GetWindowRect(hwndParent, &windowRect) )
			{
				int width, height;
				width = windowRect.right - windowRect.left;
				height = windowRect.bottom - windowRect.top;

				RECT viewerArea = { 0, };
				
				if ( TRUE == GetWindowRect(GetParent(hwndParent), &viewerArea) )
				{
					int newX = ((viewerArea.right-viewerArea.left)/2)-(width/2);
					int newY = ((viewerArea.bottom-viewerArea.top)/2)-(height/2);

					newX += viewerArea.left;
					newY += viewerArea.top;

					MoveWindow(hwndParent, newX, newY, width, height, FALSE);
				}
			}
		}
    }
    return 0;
}


