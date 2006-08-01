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

ZOption & ZOption::GetInstance()
{
	static ZOption inst;
	return inst;
}


ZOption::ZOption()
{
	m_bLoopImages = true;
	m_bUseCache = true;
	m_strOptionFilename = "option.ini";
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

	// 저장해야하는 옵션 중 변경된 것이 있으면
	if ( m_bOptionChanged )
	{
		data["maximumcachememory"] = toString(m_iMaximumCacheMemory);
		data["maximumcachefilenum"] = toString(m_iMaximumCacheFileNum);
	}

	COptionFile::SaveToFile(m_strOptionFilename, data);
}

