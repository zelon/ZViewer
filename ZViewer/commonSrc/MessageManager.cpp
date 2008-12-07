/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * MessageManager.cpp
 *
 * 2006.9.23 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "MessageManager.h"
#include "UnicodeFile.h"
#include "CommonFunc.h"

CMessageManager & CMessageManager::getInstance()
{
	static CMessageManager inst;
	return inst;
}

CMessageManager::CMessageManager()
{
	_LoadLanguage(eLanguage_ENGLISH);

	m_errorMSg = TEXT("(null)");
}


CMessageManager::~CMessageManager()
{
}


/// 해당 언어에 맞는 파일을 읽어들여 메시지 맵을 구성한다.
void CMessageManager::_LoadLanguage(eLanguage lang)
{
	tstring strFileName = GetProgramFolder();
	strFileName += TEXT("\\language\\");

	switch ( lang )
	{
	case eLanguage_KOREAN:
		strFileName += TEXT("korean.txt");
		break;

	default:
		strFileName += TEXT("english.txt");
	}

	COptionFile::LoadFromFile(strFileName, m_messageMap);
}

/// 어떤 언어를 쓸건지 선택한다.
void CMessageManager::SetLanguage(eLanguage lang)
{
	_LoadLanguage(lang);
}



/// 이 메시지를 얻는다.
const tstring & CMessageManager::GetMessage(const tstring & key) const
{
	iniMap::const_iterator it = m_messageMap.find(key);

	if ( it == m_messageMap.end() )
	{
		assert(false);
		DebugPrintf(TEXT("Can't find message from messagemap"));
		m_errorMSg = key;
		return m_errorMSg;
	}

	return it->second;
}