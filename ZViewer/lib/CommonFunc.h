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

/// OS 종류를 판단할 때 쓰일 enum
enum eOSKind
{
	eOSKind_UNKNOWN,
	eOSKind_98,
	eOSKind_2000,
	eOSKind_XP,
};

const std::string GetOnlyFileName(const std::string & strFullFileName);

class CStringCompareIgnoreCase
{
public:
	bool operator()(const std::string & a, const std::string & b)
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
		return (strcmp(b.c_str(), a.c_str()) > 0);
	}
};

class CStringCompareIgnoreCase_LengthFirst
{
public:
	bool operator()(const std::string & a, const std::string & b)
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
		return (strcmpi(b.c_str(), a.c_str()) > 0);

	}
};

void DebugPrintf( const char *fmt, ... );

bool SetRegistryValue(HKEY hOpenKey, const std::string & strKey,LPCTSTR szValue, const std::string & strData);

/// 최대 크기를 넘지 않는 적당한 리사이즈 크기를 돌려준다.
RECT GetResizedRectForBigToSmall(const RECT & MaximumSize, const RECT & originalSize);

/// 최대 크기를 넘지 않는 적당한 리사이즈 크기를 돌려준다.
RECT GetResizedRectForSmallToBig(const RECT & MaximumSize, const RECT & originalSize);

std::string toString(int i);

bool SelectFolder(HWND hWnd, char * szFolder);

std::string GetFolderNameFromFullFileName(const std::string & strFullFilename);
std::string GetFileNameFromFullFileName(const std::string & strFullFilename);

eOSKind getOSVersion();
