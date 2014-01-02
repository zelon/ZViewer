/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * ShortTimeMessage.h
 *
 * 2006. 3. 13 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#pragma once

/// 잠시 보여주는 메시지를 위한 싱글턴 클래스
class CShortTimeMessage
{
    CShortTimeMessage();

public:
    static CShortTimeMessage & getInstance();
    ~CShortTimeMessage();

	void setMessage(const std::string & strMsg, DWORD dwShowTimeInMilliSec);

	/// 타이머가 호출되었을 때
	void onTimer();

protected:
	/// 보여줄 메시지
	std::string m_strMessage;
	
	/// 보여줄 메시지가 사라질 시각
	std::chrono::system_clock::time_point m_hideTick;
};
