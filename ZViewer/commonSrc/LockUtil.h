/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * LockUtil.h
 *
 * 2006. 3. 11 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */


#pragma once

/// 윈도우의 Event 를 쉽게 쓰게 하는 유틸리티 클래스
class CEventObj
{
public:
	CEventObj()
	{
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	~CEventObj()
	{
		CloseHandle(m_hEvent);
	}

	bool setEvent()
	{
		return SetEvent(m_hEvent) == TRUE;
	}

	DWORD wait()
	{
		return WaitForSingleObject(m_hEvent, INFINITE);
	}

protected:
	/// 내부적으로 가지고 있는 이벤트 핸들
	HANDLE m_hEvent;

};

typedef std::recursive_mutex CLockObj;
typedef std::lock_guard<std::recursive_mutex> CLockObjUtil;
