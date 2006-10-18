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
#include "OptionFile.h"
#include "CommonFunc.h"
#include "ZMain.h"


ZOption & ZOption::GetInstance()
{
	static ZOption inst;
	return inst;
}


ZOption::ZOption()
{
	m_bLoopImages = true;
	m_bUseCache = true;
	
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

	/// 기본적인 옵션에서 파일에서 불러온 설정을 덮어씌운다.
	LoadFromFile();
}

void ZOption::SetDefaultOption()
{
	m_bFullScreen = false;
	m_bBigToSmallStretchImage = false;
	m_bSmallToBigStretchImage = false;
	m_iMaximumCacheMemoryMB = 50;

	m_iMaxCacheImageNum = 50;

#ifdef _DEBUG
	m_iMaxCacheImageNum = 10;
#endif
}

void ZOption::LoadFromFile()
{
	/*
	iniMap data;

	COptionFile::LoadFromFile(m_strOptionFilename, data);

	m_iMaximumCacheMemoryMB = _tstoi(data[TEXT("maximumcachememory")].c_str());
	m_iMaxCacheImageNum = _tstoi(data[TEXT("maximumcachefilenum")].c_str());
	*/
}

void ZOption::SaveToFile()
{
	/*
	iniMap data;

	m_bOptionChanged = true;

	// 저장해야하는 옵션 중 변경된 것이 있으면
	if ( m_bOptionChanged )
	{
		data[TEXT("maximumcachememory")] = toString(m_iMaximumCacheMemoryMB);
		data[TEXT("maximumcachefilenum")] = toString(m_iMaxCacheImageNum);
	}

	COptionFile::SaveToFile(m_strOptionFilename, data);
	*/
}