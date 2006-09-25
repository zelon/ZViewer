/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	CommonFunc.h
*
*                                       http://www.wimy.com
*********************************************************************/
#pragma once

#include "CommonDefine.h"

/// OS 종류를 판단할 때 쓰일 enum
enum eOSKind
{
	eOSKind_UNKNOWN,
	eOSKind_98,
	eOSKind_2000,
	eOSKind_XP,
};

const tstring GetOnlyFileName(const tstring & strFullFileName);

class CStringCompareIgnoreCase
{
public:
	bool operator()(const tstring & a, const tstring & b)
	{
		/*
		// 문자열 비교를 할 때 모두 소문자로 바꿔서 비교한다.
		char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
		_snprintf(szTempA, sizeof(szTempA), a.c_str());
		_snprintf(szTempB, sizeof(szTempB), b.c_str());
		strlwr(szTempA);
		strlwr(szTempB);

		return (strcmp(szTempB, szTempA) > 0);
		*/
		return (_tcscmp(b.c_str(), a.c_str()) > 0);
	}
};

class CStringCompareIgnoreCase_LengthFirst
{
public:
	bool operator()(const tstring & a, tstring & b)
	{
		// 길이를 우선.
		if ( a.size() < b.size() )
		{
			return true;
		}

		if ( a.size() > b.size() )
		{
			return false;
		}

		/*
		// 문자열 비교를 할 때 모두 소문자로 바꿔서 비교한다.
		char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
		_snprintf(szTempA, sizeof(szTempA), a.c_str());
		_snprintf(szTempB, sizeof(szTempB), b.c_str());
		strlwr(szTempA);
		strlwr(szTempB);

		return (strcmp(szTempB, szTempA) > 0);
		*/
		return (_tcscmp(b.c_str(), a.c_str()) > 0);

	}
};

void DebugPrintf( const TCHAR *fmt, ... );

bool SetRegistryValue(HKEY hOpenKey, const tstring & strKey,LPCTSTR szValue, const tstring & strData);

/// 최대 크기를 넘지 않는 적당한 리사이즈 크기를 돌려준다.
RECT GetResizedRectForBigToSmall(const RECT & MaximumSize, const RECT & originalSize);

/// 최대 크기를 넘지 않는 적당한 리사이즈 크기를 돌려준다.
RECT GetResizedRectForSmallToBig(const RECT & MaximumSize, const RECT & originalSize);

tstring toString(int i);

bool SelectFolder(HWND hWnd, TCHAR * szFolder);

tstring GetFolderNameFromFullFileName(const tstring & strFullFilename);
tstring GetFileNameFromFullFileName(const tstring & strFullFilename);

eOSKind getOSVersion();

/// string 을 wstring 으로 변환
std::wstring getWStringFromString(const std::string & str);

/// 현재 실행 파일이 있는 폴더를 얻는다.
tstring GetProgramFolder();