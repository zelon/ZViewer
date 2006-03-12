/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	CommonDefine.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include <string>
#include <windows.h>

#include "CommonFunc.h"
#define STATUSBAR_HEIGHT		20

const std::string g_strVersion = "0.4.0";

struct FileData
{
	std::string m_strFileName;	// 파일명
	_FILETIME m_timeModified;	// 최근 수정된 날짜
	DWORD m_nFileSize;			// 파일크기
};

class CFileDataSort_OnlyFilenameCompare
{
public:
	bool operator()(const FileData & a, const FileData & b)
	{
		/*
		// 파일명 비교를 할 때 모두 소문자로 바꿔서 비교한다.
		char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
		_snprintf(szTempA, sizeof(szTempA), GetOnlyFileName(a.m_strFileName).c_str());
		_snprintf(szTempB, sizeof(szTempB), GetOnlyFileName(b.m_strFileName).c_str());
		strlwr(szTempA);
		strlwr(szTempB);

		return (strcmp(szTempB, szTempA) > 0);
		*/
		return (strcmpi(b.m_strFileName.c_str(), a.m_strFileName.c_str()) > 0);
	}
};

class CFileDataSort_OnlyFilenameCompare_XP
{
public:
	bool operator()(const FileData & a, const FileData & b)
	{
		/*
		// 파일명 비교를 할 때 모두 소문자로 바꿔서 비교한다.
		char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
		_snprintf(szTempA, sizeof(szTempA), GetOnlyFileName(a.m_strFileName).c_str());
		_snprintf(szTempB, sizeof(szTempB), GetOnlyFileName(b.m_strFileName).c_str());
		strlwr(szTempA);
		strlwr(szTempB);

		return (StrCmp(szTempB, szTempA) > 0);
		*/

		return ( strcmpi(b.m_strFileName.c_str(), a.m_strFileName.c_str()) > 0);
	}
};

class CFileDataSort_FileSize
{
public:
	bool operator()(const FileData & a, const FileData & b)
	{
		return a.m_nFileSize > b.m_nFileSize;
	}
};

class CFileDataSort_LastModifiedTime
{
public:
	bool operator()(const FileData & a, const FileData & b)
	{
		// 돌려주는 부등호가 > 이면 최근것이 앞에, < 이면 최근것이 뒤에 간다.
		if ( a.m_timeModified.dwHighDateTime == b.m_timeModified.dwHighDateTime)
		{
			return a.m_timeModified.dwLowDateTime >= b.m_timeModified.dwLowDateTime;
		}
		return a.m_timeModified.dwHighDateTime >= b.m_timeModified.dwHighDateTime;
	}
};
