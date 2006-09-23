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

/// 언어별로의 메시지를 가지고 있다가 필요할 때 반환하는 클래스
class CMessageManager
{
public:
	CMessageManager();
	~CMessageManager();

	/// 어떤 언어를 쓸건지 선택한다.
	void SetLanguage(const std::string & lang);

protected:

	std::map < tstring, tstring > m_messageMap;
};