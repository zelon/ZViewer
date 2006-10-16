/* ------------------------------------------------------------------------
 *
 * Copyright 2006
 *
 * MessageManager.h
 *
 * 2006.9.23 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#pragma once

#include <string>
#include <map>

#include "OptionFile.h"

enum eLanguage
{
	eLanguage_ENGLISH,
	eLanguage_KOREAN,
};

/// 언어별로의 메시지를 가지고 있다가 필요할 때 반환하는 클래스
class CMessageManager
{
public:
	~CMessageManager();

	static CMessageManager & getInstance();

	/// 어떤 언어를 쓸건지 선택한다.
	void SetLanguage(eLanguage lang);

	/// 이 메시지를 얻는다.
	const tstring & GetMessage(const tstring & key) const;

protected:

	CMessageManager();

	/// 해당 언어에 맞는 파일을 읽어들여 메시지 맵을 구성한다.
	void _LoadLanguage(eLanguage lang);

	iniMap m_messageMap;

	mutable tstring m_errorMSg;
};

inline const TCHAR * GetMessage(const TCHAR * key)
{
	return CMessageManager::getInstance().GetMessage(key).c_str();
}