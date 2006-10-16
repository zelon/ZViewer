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
	m_iMaxCacheImageNum = 50;

#ifdef _DEBUG
	m_iMaxCacheImageNum = 10;
#endif
	SetDefaultOption();

	LoadFromFile();
}

void ZOption::SetDefaultOption()
{
	m_bFullScreen = false;
	m_bBigToSmallStretchImage = false;
	m_bSmallToBigStretchImage = false;
}

void ZOption::LoadFromFile()
{
	iniMap data;

	COptionFile::LoadFromFile(m_strOptionFilename, data);
}

void ZOption::SaveToFile()
{
	iniMap data;

	m_bOptionChanged = true;

	// 저장해야하는 옵션 중 변경된 것이 있으면
	if ( m_bOptionChanged )
	{
		data[TEXT("maximumcachememory")] = toString(m_iMaximumCacheMemory);
		data[TEXT("maximumcachefilenum")] = toString(m_iMaximumCacheFileNum);
	}

	COptionFile::SaveToFile(m_strOptionFilename, data);
}

