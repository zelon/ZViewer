
#include "stdafx.h"
#include "ZOption.h"
#include "OptionFile.h"

void ZOption::SetDefaultOption()
{
	m_bFullScreen = false;
	m_bBigToSmallStretchImage = false;
	m_bSmallToBigStretchImage = false;
}

void ZOption::LoadFromFile()
{
	iniMap data;

//	COptionFile::LoadFromFile(m_strOptionFilename, data);
}

void ZOption::SaveToFile()
{
	iniMap data;

	COptionFile::SaveToFile(m_strOptionFilename, data);
}

