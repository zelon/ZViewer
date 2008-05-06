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
#include "ZMain.h"


ZOption & ZOption::GetInstance()
{
	static ZOption inst;
	return inst;
}


ZOption::ZOption()
{
	TCHAR buffer[256];
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

	/// 불러온 최종 옵션을 점검하여 메뉴 중 체크표시할 것들을 표시한다.
	ZMain::GetInstance().SetCheckMenus();
}

void ZOption::SetSaveOptions()
{
	_InsertSaveOptionSetting(L"maximumcachememoryMB", &m_iMaximumCacheMemoryMB);
	_InsertSaveOptionSetting(L"maximumcachefilenum", &m_iMaxCacheImageNum);

	_InsertSaveOptionSetting(L"use_cache", &m_bUseCache);
	_InsertSaveOptionSetting(L"loop_view", &m_bLoopImages);

	_InsertSaveOptionSetting(L"stretch_small_to_big", &m_bSmallToBigStretchImage);
	_InsertSaveOptionSetting(L"stretch_big_to_small", &m_bBigToSmallStretchImage);
}

/// 기본적인 옵션을 설정해둔다.
void ZOption::SetDefaultOption()
{
	m_bAlwaysOnTop = false;
	m_bSlideMode = false;
	m_iSlideModePeriodMiliSeconds = 5000;	///< Default slide mode period is 5 seconds
	m_bLoopImages = false;
	m_bUseCache = true;
	m_bFullScreen = false;
	m_bBigToSmallStretchImage = false;
	m_bSmallToBigStretchImage = false;
	m_iMaximumCacheMemoryMB = 50;

	m_iMaxCacheImageNum = 50;

#ifdef _DEBUG
	m_iMaxCacheImageNum = 10;
	m_iSlideModePeriodMiliSeconds = 1000;
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