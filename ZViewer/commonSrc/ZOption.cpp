/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZOption.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "ZOption.h"


ZOption & ZOption::GetInstance()
{
	static ZOption inst;
	return inst;
}


ZOption::ZOption()
{
	/// 아래 값은 ZViewerAgent 에서만 true 이다
	m_bDontSaveInstance = false;
}

void ZOption::LoadOption()
{
	TCHAR buffer[256];

	/// C:\Documents and Settings\USERID\Local Settings\Application Data 의 위치를 얻어온다.
	if ( S_OK != SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer) )
	{
		m_strOptionFilename = GetProgramFolder();
	}
	else
	{
		m_strOptionFilename = buffer;
	}

	m_strOptionFilename += TEXT("\\zviewer.ini");
	m_bOptionChanged = false;

	/// 기본적인 옵션은 설정해둔다.
	SetDefaultOption();

	SetSaveOptions();

	/// 기본적인 옵션에서 파일에서 불러온 설정을 덮어씌운다.
	LoadFromFile();
}

/// 이 멤버들은 이 이름으로 저장되고, 불려져 온다라고 설정
void ZOption::SetSaveOptions()
{
	_InsertSaveOptionSetting(L"maximumcachememoryMB", &m_iMaximumCacheMemoryMB);
	_InsertSaveOptionSetting(L"maximumcachefilenum", &m_iMaxCacheImageNum);

	_InsertSaveOptionSetting(L"loop_view", &m_bLoopImages);

	_InsertSaveOptionSetting(L"stretch_small_to_big", &m_bSmallToBigStretchImage);
	_InsertSaveOptionSetting(L"stretch_big_to_small", &m_bBigToSmallStretchImage);

	_InsertSaveOptionSetting(L"use_open_cmd_shell", &m_bUseOpenCMDInShell);
	_InsertSaveOptionSetting(L"use_preview_shell", &m_bUsePreviewInShell);

	_InsertSaveOptionSetting(L"use_debug", &m_bUseDebug);

	_InsertSaveOptionSetting(L"use_auto_rotation", &m_bUseAutoRotation);
}

/// 기본적인 옵션을 설정해둔다. 설치 후 처음 실행되었을 때 이 값을 기준으로 zviewer.ini 파일이 만들어진다.
void ZOption::SetDefaultOption()
{
	m_bUsePreviewInShell = true;
	m_bUseOpenCMDInShell = false;
	m_bAlwaysOnTop = false;
	m_bSlideMode = false;
	m_iSlideModePeriodMiliSeconds = 5000;	///< Default slide mode period is 5 seconds
	m_bLoopImages = false;
	m_bUseAutoRotation = true;
	m_bFullScreen = false;
	m_bUseDebug = true;
	m_bBigToSmallStretchImage = false;
	m_bSmallToBigStretchImage = false;
	m_iMaximumCacheMemoryMB = 50;

	m_iMaxCacheImageNum = 50;

#ifdef _DEBUG
	m_iMaxCacheImageNum = 10;
	m_iSlideModePeriodMiliSeconds = 1000;
	m_bUseOpenCMDInShell = true;
	m_bUseDebug = true;
#endif
}

void ZOption::LoadFromFile()
{
	iniMap data;

	/// 파일로부터 설정 불러오기가 성공했을 때만 설정을 한다.
	if ( COptionFile::LoadFromFile(m_strOptionFilename, data) )
	{
		for ( size_t i=0; i<m_saveOptions.size(); ++i )
		{
			if ( data.count(m_saveOptions[i].getString()) > 0)
			{
				m_saveOptions[i].InsertMapToValue(data);
			}
		}
	}
}

void ZOption::SaveToFile()
{
	/// ZViewerAgent 에서 마칠 때는 저장을 하지 않기 위해
	if ( m_bDontSaveInstance ) return;

	iniMap data;

	m_bOptionChanged = true;

	// 저장해야하는 옵션 중 변경된 것이 있으면
	if ( m_bOptionChanged )
	{
		for ( size_t i=0; i<m_saveOptions.size(); ++i )
		{
			m_saveOptions[i].InsertValueToMap(data);
		}
		COptionFile::SaveToFile(m_strOptionFilename, data);
	}
}